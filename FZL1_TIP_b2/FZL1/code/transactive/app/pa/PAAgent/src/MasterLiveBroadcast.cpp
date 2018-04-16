#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAAgent/src/MasterLiveBroadcast.cpp $
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
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAAgent/src/MasterLiveBroadcast.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"

using namespace TA_Base_Core;
using namespace TA_Base_Bus;

namespace TA_IRS_App
{

MasterLiveBroadcast::MasterLiveBroadcast( 
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId ) : 
AbstractStationBroadcast( broadcastConfigP, sessionId ),
m_theSpecifics( broadcastConfigP->theBroadcastConfigSpecifics.theStationLiveConfig() ),
m_isConnected(false)
{
    TA_ASSERT(CachedConfig::getInstance()->getIsMaster(), "MasterLiveBroadcast is not available for the Station PA Agent");

    // Set the initial effective coverage to be the entire list of zones
    // The false flag prevents the sending of the synchronisation message
    setEffectiveCoverageIndices(
        calculateEffectiveCoverageIndices(m_theSpecifics.zones, m_theSpecifics.zones), 
        false);
}

MasterLiveBroadcast::~MasterLiveBroadcast() 
{
}

void MasterLiveBroadcast::validateBroadcastConfig() //throw(TA_Base_Core::BadParameterException) 
{
    ThreadGuard guard( m_lock );

    // At least one zone must be specified
    if (m_theSpecifics.zones.length() == 0)
    {
		LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugDebug, "No zones requested");
        throw BadParameterException("No zones requested");
    }

    // Test validity of zone keys
    for (int index = 0; index< m_theSpecifics.zones.length(); ++index)
    {
       
        unsigned long zoneKey;
        unsigned int zoneId;
        unsigned long zoneLocationKey;
        unsigned int stationId;

        try
        {
            zoneKey = m_theSpecifics.zones[index];
            zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(zoneKey);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
			LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugDebug, "A requested zone does not exist in the database");
            throw BadParameterException("A requested zone does not exist in the database");
        }

        try
        {
            zoneLocationKey = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneLocationKeyFromKey(zoneKey);
            stationId = TA_IRS_Bus::CachedMaps::getInstance()->getPasStationIdFromLocationKey(zoneLocationKey);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "[BadParameterException] An ISCS station location could not be resolved to a PAS location");
            throw BadParameterException("An ISCS station location could not be resolved to a PAS location");
        }
    }
}

void MasterLiveBroadcast::execute() 
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

bool MasterLiveBroadcast::executePhaseOneAndReturnIsCompleted() 
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
        setAnnounceId(AbstractBroadcastCircularList::getInstance()->getNextAvailableStationAnnounceId());
    }
    catch(TA_Base_Core::NoFreeAnnounceIdException&)
    {
        // Serious error - terminate
        abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_NO_FREE_ANNOUNCE_ID);
        return false;
    }

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

    // Flag to try to cleanup the announceId in terminate 
    setWasExecutedSuccessfully(true);

    // Succeeded so we reset the GlobalState 
    m_progressReportSentry->resetGlobalState();

    // Set this flag to true so we can hang around the execution state 
    // without resending the PAS request until PTT is pressed.
    setIsConnected(true);

    return true;
}

bool MasterLiveBroadcast::executePhaseTwoAndReturnIsCompleted() 
{
    ThreadGuard guard( m_lock );

    ///////////////////////////////////////////////////////////////////////////
    // Phase 2 of execution - awaiting PTT 
    ///////////////////////////////////////////////////////////////////////////

    bool pttState(false);

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

    // if the button is pressed, that means the switching of zones is complete
    if (pttState)
    {
        // Sleep for a poll cycle to ensure table 1200 is processed
        updateProgressReport(false); //isRuntime
    }

    return pttState;
}


void MasterLiveBroadcast::runtimeSpecifics() 
{
    ThreadGuard guard( m_lock );

    updateProgressReport(true); //isRuntime

    bool pttState = false;

    try
    {
        // TBD
    }
    catch(TA_Base_Core::PasConnectionException&)
    {
        determineWhetherToRetryOrAbort(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT);
        return;
    }
    
    if (pttState)
    {
        // Operator is still talking so we cannot terminate yet.
        return;
    }

    // The operator has released the PTT button so we move to the termination state
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::Termination);
    // terminate will be called by the BroadcastWorker().
}

void MasterLiveBroadcast::updateProgressReport(bool isRuntime)
{
    ThreadGuard guard( m_lock );

    TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = getEffectiveCoverageIndices();

    // update progress report structure for each zone
    for (int indexIndex = 0; indexIndex< effectiveCoverageIndices.length(); ++indexIndex)
    {
        unsigned long zoneKey = m_theSpecifics.zones[effectiveCoverageIndices[indexIndex]];

        // CachedMappingNotFoundException will never happen because it has been tested
        // in validateBroadcastConfig() 
        unsigned int zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(zoneKey);
        unsigned long zoneLocationKey = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneLocationKeyFromKey(zoneKey);
        unsigned int stationId = TA_IRS_Bus::CachedMaps::getInstance()->getPasStationIdFromLocationKey(zoneLocationKey);

        ZoneStatus zoneStatus;

        try
        {
            // TBD
        }
        catch(TA_Base_Core::PasConnectionException&)
        {
            if (isRuntime)
            {
                determineWhetherToRetryOrAbort(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT);
                return;
            }
            else
            {
                // Serious error - terminate
                abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT);
                return;
            }
        }


        if (zoneStatus.m_status)
        {
            // Faulty zone
            m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_ZONE);
        }
        else
        {
            // Zone functional
            if (zoneStatus.m_sourceId != 0 &&
				zoneStatus.m_sourceId != m_theSpecifics.sourceId)
            {
                // Overridden by another source
                m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_FAILED_OVERRIDDEN);
                m_progressReportSentry->setOverridingSourcePriority( effectiveCoverageIndices[indexIndex], zoneStatus.m_priority);
            }
            else
            {
                // All ok
                m_progressReportSentry->setLocalState( effectiveCoverageIndices[indexIndex], TA_Base_Bus::IPAAgentCorbaDef::LOCAL_STATE_IN_PROGRESS);
                m_progressReportSentry->setOverridingSourcePriority( effectiveCoverageIndices[indexIndex], 0 );
            }
        }
    } // for loop
}



void MasterLiveBroadcast::terminate() 
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

    // Reset the announce id so that it can be used by new broadcasts 
    setAnnounceId(0);

    updateFinalProgressReport();
    
    // All went well so we move to the pending removal state
    setBroadcastState(TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval);
}

bool MasterLiveBroadcast::isAllFailureZones()
{
	bool isNonValidZone = true;
	
    for (int i=0; i<m_theSpecifics.zones.length(); ++i)
    {
		// get the zone id and status for every selected zone
		unsigned int zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(m_theSpecifics.zones[i]);
		unsigned long zoneLocationKey = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneLocationKeyFromKey(m_theSpecifics.zones[i]);
		unsigned int stationId = TA_IRS_Bus::CachedMaps::getInstance()->getPasStationIdFromLocationKey(zoneLocationKey);
		
		ZoneStatus zoneSta;
        // TBD
		
		// for zone status, if in normal status, value is 0, and in failure status, value is 1
		isNonValidZone = isNonValidZone && zoneSta.m_status;
	}
	
	return isNonValidZone;
	
}

} // namespace TA_IRS_App

