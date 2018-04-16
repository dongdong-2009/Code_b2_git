#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAAgent/src/DVARecordingBroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2012/06/12 13:35:44 $
  * Last modified by : $Author: builder $
  *
  *
  */

#include <time.h>
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "app/pa/PAAgent/src/DVARecordingBroadcast.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"


using namespace TA_Base_Core;
using namespace TA_Base_Bus;

namespace TA_IRS_App
{

DVARecordingBroadcast::DVARecordingBroadcast( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP,
                                              const std::string& sessionId )
 : AbstractStationBroadcast( broadcastConfigP, sessionId ),
   m_theSpecifics( broadcastConfigP->theBroadcastConfigSpecifics.theStationRecordingConfig() ),
   m_isConnected(false)
{
}

DVARecordingBroadcast::~DVARecordingBroadcast() 
{
}

void DVARecordingBroadcast::validateBroadcastConfig() //throw(TA_Base_Core::BadParameterException) 
{
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
        // TBD
    }
    catch(TA_Base_Core::PasWriteErrorException& e)
    {
        // Serious error - terminate
        abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT);

        return false;
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
        // TBD
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
        // TBD
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
            // TBD
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

