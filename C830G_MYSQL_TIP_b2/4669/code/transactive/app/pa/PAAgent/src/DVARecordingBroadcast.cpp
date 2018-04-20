#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/4669_T01271350/4669/transactive/app/pa/PAAgent/src/DVARecordingBroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2008/11/28 16:26:01 $
  * Last modified by : $Author: builder $
  *
  *
  */

#include <time.h>
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "app/pa/PAAgent/src/DVARecordingBroadcast.h"
#include "app/pa/PAAgent/src/Table630.h"
#include "app/pa/PAAgent/src/Table590.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/PASConnection.h"


using namespace TA_Base_Core;
using namespace TA_Base_Bus;

namespace TA_IRS_App
{

DVARecordingBroadcast::DVARecordingBroadcast( Table590& table590,
                                              Table630& table630,
                                              TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                                              const std::string& sessionId )
 : AbstractStationBroadcast( broadcastConfigP, sessionId ),
   m_theSpecifics( broadcastConfigP->theBroadcastConfigSpecifics.theStationRecordingConfig() ),
   m_table630(table630),
   m_table590(table590),
   m_isConnected(false)
{
}

DVARecordingBroadcast::~DVARecordingBroadcast() 
{
}

void DVARecordingBroadcast::validateBroadcastConfig() //throw(TA_Base_Core::BadParameterException) 
{
    ThreadGuard guard( m_lock );

    // Source Id needs to be from a paging console:
    if ( !PasHelpers::getInstance()->zoneIsAPagingConsole(m_theSpecifics.sourceId,getSessionId()) ) // TD19669
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[BadParameterException] The specified source is not a paging console");
        throw BadParameterException("The specified source is not a paging console");
    }
    

    // Message can only be from this location 
    try
    {
        unsigned long locationKey = TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageLocationKeyFromKey(m_theSpecifics.messageKey);
        if (locationKey != CachedConfig::getInstance()->getAgentLocationKey())
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[BadParameterException] The requested message is not within this station's scope");
            throw BadParameterException("The requested message is not within this station's scope");
        }
    }
    catch( TA_Base_Core::CachedMappingNotFoundException& )
    {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[BadParameterException] The specified message is not an ad-hoc message");
        throw BadParameterException("The requested message does not exist in the database");
    }

    // Message can only be in the range of adhoc messages
    int adhocOffset = PasHelpers::getInstance()->getAdhocOffset(m_theSpecifics.messageKey);
    
    if (adhocOffset<0 || adhocOffset >= TA_Base_Bus::IPAAgentCorbaDef::PA_NBADHOC)
    {
        // out of range
        throw BadParameterException("The specified message is not an ad-hoc message");
    }
}

void DVARecordingBroadcast::execute() 
{
    if (!executePhaseOneAndReturnIsCompleted())
    {
        // retry next iteration
        return;
    }

    if (!executePhaseTwoAndReturnIsCompleted())
    {
        // retry next iteration
        return;
    }

    // All went well so we move to the runtime state
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Runtime);
}

bool DVARecordingBroadcast::executePhaseOneAndReturnIsCompleted() 
{
    ThreadGuard guard( m_lock );


    ///////////////////////////////////////////////////////////////////////////
    // Phase 1 of execution - PAS connection
    ///////////////////////////////////////////////////////////////////////////

    if (getIsConnected())
    {
        // Already completed
        return true;
    }

    setWasExecutedSuccessfully(false);

    try
    {
        m_table630.setTableDataAndWrite(m_theSpecifics.messageKey, m_theSpecifics.sourceId);
    }
    catch(TA_Base_Core::PasWriteErrorException& e)
    {
        if (e.getErrorValue() == AbstractPASTable::PAS_ERR_BUSY_SOURCE)
        {
            determineWhetherToRetryOrAbort(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT);
            return false;
        }
        else if (e.getErrorValue() == AbstractPASTable::PAS_ERR_BUSY_RECORDING)
        {
            determineWhetherToRetryOrAbort(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_MESSAGE_BUSY_RECORDING);
            return false;
        }
        else if (e.getErrorValue() == AbstractPASTable::PAS_ERR_BUSY_BROADCASTING)
        {
            determineWhetherToRetryOrAbort(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_MESSAGE_BUSY_BROADCASTING);
            return false;
        }
        else 
        {
            // PAS_ERR_GENERIC or anything else

            // Serious error - terminate
            abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT);
            return false;
        }
    }
    catch(TA_Base_Core::PasConnectionException&)
    {
        // Serious error - terminate
        abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT);
        return false;
    }

    setWasExecutedSuccessfully(true);

    // Succeeded so we reset the GlobalState 
    m_progressReportSentry->resetGlobalState();

    // Set this flag to true so we can hang around the execution state 
    // without resending the PAS request until PTT is pressed.
    setIsConnected(true);

    return true;
}

bool DVARecordingBroadcast::executePhaseTwoAndReturnIsCompleted() 
{
    ThreadGuard guard( m_lock );

    ///////////////////////////////////////////////////////////////////////////
    // Phase 2 of execution - awaiting PTT 
    ///////////////////////////////////////////////////////////////////////////

    bool pttState;

    try
    {
        pttState = m_table590.isPttStatusOn(m_theSpecifics.sourceId);
    }
    catch(TA_Base_Core::PasConnectionException&)
    {
        // Serious error - terminate
        abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT);
        return false;
    }

    // Succeeded so we reset the GlobalState 
    m_progressReportSentry->resetGlobalState();

    return pttState;
}

void DVARecordingBroadcast::runtimeSpecifics() 
{
    ThreadGuard guard( m_lock );

    bool pttState;

    try
    {
        pttState = m_table590.isPttStatusOn(m_theSpecifics.sourceId);
    }
    catch(TA_Base_Core::PasConnectionException&)
    {
        // We will be heavy handed on any failures and terminate the broadcast.  
        abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT);
        return;
    }
   
    if (pttState == true)
    {
        // Operator is still talking so we cannot terminate yet.
        return;
    }


    // The operator has released the PTT button so we move to the termination state
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);
    // terminate will be called by the BroadcastWorker().
}


void DVARecordingBroadcast::terminate() 
{
    ThreadGuard guard( m_lock );

    if (getWasExecutedSuccessfully())
    {
        try
        {
            m_table630.setTableDataAndWrite(m_theSpecifics.messageKey, 0);
        }
        catch(TA_Base_Core::PasWriteErrorException&)
        {
            // Not expected to get here 
            m_progressReportSentry->setGlobalState(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT);
            // Carry on regardless
        }
        catch(TA_Base_Core::PasConnectionException&)
        {
            determineWhetherToRetryOrAbort(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT);
            return;
        }
    }

    // Reset the connected flag so a new execution will send a new request to PAS
    setIsConnected(false);

    updateFinalProgressReport();

    // All went well so we move to the pending removal state
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval);

}

} // namespace TA_IRS_App

