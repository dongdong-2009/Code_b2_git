/**
  * The source code in this file is the property of
  * MI Services Group and is not for redistribution
  * in any form.
  *
  * Source : $File: //depot/GZL6_TIP/3002/transactive/app/pa/PAAgent/src/StationLiveBroadcast.cpp $
  * @author  A. Parker
  * @version $Revision: #1 $
  * Last modification : $DateTime: 2012/06/12 13:35:44 $
  * Last modified by : $Author: builder $
  *
  *
  */
  
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <time.h>
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "app/pa/PAAgent/src/StationLiveBroadcast.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/AbstractBroadcastCircularList.h"


using namespace TA_Base_Core;
using namespace TA_Base_Bus;


namespace TA_IRS_App
{

StationLiveBroadcast::StationLiveBroadcast( 
    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfigP, const std::string& sessionId) : 
AbstractStationBroadcast( broadcastConfigP, sessionId ),
m_theSpecifics( broadcastConfigP->theBroadcastConfigSpecifics.theStationLiveConfig() ),
m_isConnected(false)
{
    TA_ASSERT(!CachedConfig::getInstance()->getIsMaster(), "StationLiveBroadcast is not available for the Master PA Agent");

    // Set the initial effective coverage to be the entire list of zones
    // The false flag prevents the sending of the synchronisation message
    setEffectiveCoverageIndices(
        calculateEffectiveCoverageIndices(m_theSpecifics.zones, m_theSpecifics.zones), 
        false);
}

StationLiveBroadcast::~StationLiveBroadcast() 
{
}

void StationLiveBroadcast::validateBroadcastConfig() //throw(TA_Base_Core::BadParameterException) 
{
    ThreadGuard guard( m_lock );

    // At least one zone must be specified
    if (m_theSpecifics.zones.length() == 0)
    {
        throw BadParameterException("No zones requested");
    }

    // Test validity of zone keys in database
    for (int index = 0; index< m_theSpecifics.zones.length(); ++index)
    {
        try
        {
            unsigned int zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(m_theSpecifics.zones[index]);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException&)
        {
            throw BadParameterException("A requested zone does not exist in the database" );
        }
    }

    // Zones can only be from this location
    unsigned long myLocationKey = CachedConfig::getInstance()->getAgentLocationKey();
    for (int i=0; i<m_theSpecifics.zones.length(); ++i)
    {
        unsigned long locationKey = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneLocationKeyFromKey(m_theSpecifics.zones[i]);
        if (locationKey != myLocationKey)
        {
            throw BadParameterException("A requested zone is not within this station's scope");
        }
    }
}

void StationLiveBroadcast::execute() 
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

bool StationLiveBroadcast::executePhaseOneAndReturnIsCompleted() 
{
    ThreadGuard guard( m_lock );
    
    ///////////////////////////////////////////////////////////////////////////
    // Phase 1 of execution - PAS connection
    ///////////////////////////////////////////////////////////////////////////

    if (getIsConnected())
    {
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
		if (isAllFailureZones())
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "noneValidZoneTermination error %d", e.getErrorValue());
			noneValidZoneTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT);
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "abnormalTermination error %d", e.getErrorValue());
			abnormalTermination(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_INVALID_PROTOCOL_FAILURE_PA_AGENT);
		}
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

bool StationLiveBroadcast::executePhaseTwoAndReturnIsCompleted() 
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
        updateProgressReport(false); //isRuntime
    }

    return pttState;
}


void StationLiveBroadcast::runtimeSpecifics() 
{
    ThreadGuard guard( m_lock );

    updateProgressReport(true); //isRuntime


    bool pttState;

    try
    {
        // TBD
    }
    catch(TA_Base_Core::PasConnectionException&)
    {
        determineWhetherToRetryOrAbort(TA_Base_Bus::IPAAgentCorbaDef::GLOBAL_STATE_HARDWARE_FAILURE_PA_AGENT);
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



void StationLiveBroadcast::updateProgressReport(bool isRuntime)
{
    ThreadGuard guard( m_lock );
       
    TA_Base_Bus::IPAAgentCorbaDef::CoverageIndices effectiveCoverageIndices = 
        getEffectiveCoverageIndices();

    // update progress report structure for each zone
    for (int indexIndex = 0; indexIndex< effectiveCoverageIndices.length(); ++indexIndex)
    {
        unsigned long zoneKey = m_theSpecifics.zones[effectiveCoverageIndices[indexIndex]];

        // CachedMappingNotFoundException will never happen because it has been tested
        // in validateBroadcastConfig() 
        int zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(zoneKey);

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



void StationLiveBroadcast::terminate() 
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

// TD17456
bool StationLiveBroadcast::isAllFailureZones()
{
	
	bool isNonValidZone = true;
	
    for (int i=0; i<m_theSpecifics.zones.length(); ++i)
    {
		// get the zone id and status for every selected zone
		unsigned int zoneId = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneIdFromKey(m_theSpecifics.zones[i]);
		
        ZoneStatus zoneSta;
		// TBD
		
		// for zone status, if in normal status, value is 0, and in failure status, value is 1
		isNonValidZone = isNonValidZone && zoneSta.m_status;
	}
	
	return isNonValidZone;
}

} // namespace TA_IRS_App

