/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ECSAgent/src/EcsZoneDataSource.cpp $
  * @author:  seanl
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2010/03/24 10:32:30 $
  * Last modified by:  $Author: builder $
  * 
  */

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290 4503)
#endif // defined _MSC_VER

#include <time.h>

#include "app/ecs/ECSAgent/src/EcsZoneDataSource.h"
#include "app/ecs/ECSAgent/src/EcsHelpers.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedConfig.h"
#include "app/ecs/ECSAgent/src/ItaEcsCachedMaps.h"
#include "app/ecs/ECSAgent/src/ItaEcsDatapoints.h"

#include "bus/scada/datapoint_library/src/DataPoint.h"

#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"


EcsZoneDataSource::EcsZoneDataSource(
    ItaEcsDatapoints& theDatapoints,
    ItaEcsCachedMaps& theMaps,
    ItaEcsCachedConfig& theConfig)
: m_theConfig(theConfig),
  m_theMaps(theMaps),
  m_theDatapoints(theDatapoints),
  m_atsAgent(NULL),
  m_spareAtsAgent(NULL)
{
    FUNCTION_ENTRY("Constructor");

    m_trainStatesMap.clear();

    try
    {
        m_atsAgent = new AtsAgentNamedObject();
        m_atsAgent->setEntityName(m_theConfig.getLocalATSAgentName(),true);

        // check if spare ats agent is set to this ecs agent
        if( (m_theConfig.getSpareATSAgentKey() > 0) &&
            (m_theConfig.getSpareATSAgentLocationKey() > 0) &&
            (m_theConfig.getSpareATSAgentName().empty() == false) )
		{
            m_spareAtsAgent = new AtsAgentNamedObject();
            m_spareAtsAgent->setEntityName(m_theConfig.getSpareATSAgentName(),true);
        }
        else
        {
            m_spareAtsAgent = NULL;
        }
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"Failed to resolve the local ATS Agent. Cannot request initial data.");
    }

    FUNCTION_EXIT;
}


// ExceptionChecked
EcsZoneDataSource::~EcsZoneDataSource()
{
    FUNCTION_ENTRY("Destructor");

    // Harmless to unsubscribe if not subscribed
    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

    clearTrainStates();

    if (NULL != m_spareAtsAgent)
    {
        delete m_spareAtsAgent;
        m_spareAtsAgent = NULL;
    }

	if (NULL != m_atsAgent)
	{
		delete m_atsAgent;
		m_atsAgent = NULL;
	}

    FUNCTION_EXIT;
}


void EcsZoneDataSource::clearTrainStates()
{
    TA_Base_Core::ThreadGuard guard( m_trainStatesMapLock );
	//[[//CL-XXXXX wzy, ECSAgent Crash
	/*
    for (ZoneIdToTrainStatesMapIt it = m_trainStatesMap.begin(); it != m_trainStatesMap.end(); ++it)
	{
        delete (it->second);
        m_trainStatesMap.erase(it);
    }*/
	
	ZoneIdToTrainStatesMapIt it = m_trainStatesMap.begin();
	while (it != m_trainStatesMap.end())
	{
        delete it->second;
        m_trainStatesMap.erase(it++);
	}
	//]]
}



void EcsZoneDataSource::setControl(bool isOnStartup)
{
    refreshCompleteAtsData();

    // Subscribe for updates from the local ATS Agent
	TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( 
        TA_Base_Core::ATSAgentComms::ECSUpdate,		//context
        this,										//SpecialisedMessageSubscriber
        m_theConfig.getLocalATSAgentKey(),			//unsigned long entityKey
 		0,		    								//unsigned long subsystemKey
		m_theConfig.getAgentLocationKey() );		//unsigned long regionKey

    if(m_spareAtsAgent)
    {
        // Subscribe for updates from the local ATS Agent
	    TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( 
            TA_Base_Core::ATSAgentComms::ECSUpdate,		//context
            this,										//SpecialisedMessageSubscriber
            m_theConfig.getSpareATSAgentKey(),			//unsigned long entityKey
 		    0,		    								//unsigned long subsystemKey
		    m_theConfig.getSpareATSAgentLocationKey()); //unsigned long regionKey
    }

    AbstractFailoverable::setControl(isOnStartup);
}


void EcsZoneDataSource::setMonitor(bool isOnStartup)
{
    TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

    AbstractFailoverable::setMonitor(isOnStartup);
}

// ExceptionChecked
void EcsZoneDataSource::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY("receiveSpecialisedMessage");

    try
    {
        const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef* atsPacket = 0;

	    if ( (message.messageState >>= atsPacket) == 0 )
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to interpret message");
            return;
        }
        
        processAtsPacket(*atsPacket);
	}
    catch ( TA_Base_Core::InvalidATSPacketException& )
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Ignoring invalid data from ATS");
    }
    catch (...)
	{
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed to process data from ATS");
	}

    FUNCTION_EXIT;
}


void EcsZoneDataSource::refreshCompleteAtsData()
{
    FUNCTION_ENTRY("refreshCompleteAtsData");

    try
    {
        TA_Base_Core::ThreadGuard guard( m_trainStatesMapLock );

        m_trainStatesMap.clear();

        TA_IRS_Bus::IAtsEcsCorbaDef::EcsInfoList_var ecsInfoList;
		CORBA_CALL_RETURN( ecsInfoList,
		                   (*m_atsAgent),
		                   getEcsInformationList,
		                   () );

        for (int index=0; index < ecsInfoList->length(); ++index)
        {
            try
            {
                const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef* atsPacket = &(ecsInfoList[index]);

                processAtsPacket(*atsPacket, false);      // false to postpone datapoint updates
	        }
            catch ( TA_Base_Core::InvalidATSPacketException& )
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Ignoring invalid data from ATS agent");
            }
            catch (...)
	        {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed to process data from ATS agent");
            }
        }

        // if spare ats agnet is set
        if( m_spareAtsAgent)
        {
            TA_IRS_Bus::IAtsEcsCorbaDef::EcsInfoList_var ecsInfoList;
			CORBA_CALL_RETURN( ecsInfoList,
			                   (*m_spareAtsAgent),
			                   getEcsInformationList,
			                   () );

            for (int index=0; index < ecsInfoList->length(); ++index)
            {
                try
                {
                    const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef* atsPacket = &(ecsInfoList[index]);

                    processAtsPacket(*atsPacket, false);      // to postpone datapoint updates
	            }
                catch ( TA_Base_Core::InvalidATSPacketException& )
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Ignoring invalid data from spare ATS agent");
                }
                catch (...)
	            {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Failed to process data from ATS");
                }
            }
        }
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Initial state of ECS Zones may not be correct");
        // No updates possible at this point.  The quality of the datapoints will default
        // to QUALITY_GOOD_NO_SPECIFIC_REASON
    }

    updateAllEcsZoneDataPoints();

    FUNCTION_EXIT;
}


ZoneIdToTrainStatesMapIt EcsZoneDataSource::getRecordOfTrainId(unsigned long trainId)
    // throws TrainIdNotFoundException
{
    TA_Base_Core::ThreadGuard guard( m_trainStatesMapLock );

    for (ZoneIdToTrainStatesMapIt it = m_trainStatesMap.begin(); it != m_trainStatesMap.end(); ++it)
    {
        TA_ASSERT((it->second), "trainState is NULL");

        if ((it->second)->m_trainId == trainId)
        {
            return it;
        }
    }

	// marvin++ 
	//create a new entry and return the iterator to it
	TrainState* trainStatePtr = new TrainState(trainId);
	TA_ASSERT((trainStatePtr!=NULL), "Unable to allocate memory.");
	// ++marvin
	return m_trainStatesMap.insert(zoneId2trainState(TA_IRS_Bus::ECS_NONE_OR_INVALID, trainStatePtr));
}

// ExceptionChecked
void EcsZoneDataSource::processAtsPacket(
    const TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef& packet,
    bool updateAffectedDataPoints)
{
    FUNCTION_ENTRY("processAtsPacket");

	bool isStationAndNotDegraded = (!m_theConfig.getIsMaster() && m_theConfig.getIsOccOnline());

	if (isStationAndNotDegraded)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"EcsZoneDataSource::processAtsPacket: No Ats packet processing for station ECS agent when OccOnline");

	}

    // Never throws
    unsigned long trainId = EcsHelpers::getPhysicalTrainNumber(packet);

    TA_Base_Core::ThreadGuard guard( m_trainStatesMapLock );

    ZoneIdToTrainStatesMapIt it = getRecordOfTrainId(trainId);

	unsigned long oldEcsZoneId = it->first;
	TrainState* trainStatePtr = it->second;
	
	try
	{
		// Determine ECSZone 
		unsigned long newEcsZoneId = EcsHelpers::getEcsZoneId(packet);

		// Validity bit
		trainStatePtr->m_isLocalisationValid = EcsHelpers::getLocalisationValid(packet);
		trainStatePtr->m_isOa1Valid = EcsHelpers::getOa1DataValid(packet);

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"EcsZoneDataSource::processAtsPacket: Validity: Train=%d Localisation=%d oa1=%d", 
			EcsHelpers::getTrainValid(packet), trainStatePtr->m_isLocalisationValid, trainStatePtr->m_isOa1Valid);

		if (trainStatePtr->m_isLocalisationValid == false)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"Localisation validity bit are invalid. ATS Data is not used.");	
			return;
		}

		// Determine Emergency State
		if (trainStatePtr->m_isOa1Valid == true && trainStatePtr->m_isLocalisationValid == true)
		{
			trainStatePtr->m_isInEmergency = EcsHelpers::getIsInEmergency(packet);
		}

		// Set stall time threshold states
		trainStatePtr->m_isStalled = 
			(EcsHelpers::getStallTimeInSeconds(packet) >= 
			 m_theConfig.getThresholdStallTimeInSecs());

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
			"EcsZoneDataSource::processAtsPacket: Train ID=%ld newEcsZoneId=%ld oldEcsZoneId=%ld m_isInEmergency=%d StallTimeInSeconds=%d m_isStalled=%d", 
			trainId, newEcsZoneId, oldEcsZoneId, trainStatePtr->m_isInEmergency, EcsHelpers::getStallTimeInSeconds(packet), trainStatePtr->m_isStalled);

		// Set temperature threshold states
		if (false == trainStatePtr->m_isHot)
		{
			// Not in hot state yet
			trainStatePtr->m_isHot = 
				(EcsHelpers::getMaxExternalTemperatureInCelsius(packet) >= 
				 m_theConfig.getThresholdTemperature());

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"EcsZoneDataSource::processAtsPacket: packet temp=%d, thresholdTemp=%d, m_isHot=%d", 
				EcsHelpers::getMaxExternalTemperatureInCelsius(packet), 
				m_theConfig.getThresholdTemperature(), 
				trainStatePtr->m_isHot);
		}
		else 
		{
			// Consider deadband if it's in hot state
			if ( EcsHelpers::getMaxExternalTemperatureInCelsius(packet) <
				 m_theConfig.getTemperatureDeadbandLowerLimit())
			{
				// Fallen out of the deadband region
				trainStatePtr->m_isHot = false;
			}

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"EcsZoneDataSource::processAtsPacket: packet temp=%d, deadBandLowerLimitTemp=%d, m_isHot=%d", 
				EcsHelpers::getMaxExternalTemperatureInCelsius(packet), 
				m_theConfig.getTemperatureDeadbandLowerLimit(), 
				trainStatePtr->m_isHot);
		}

		// Set congestion state
    
		/*
			Four cases:
			1  UseStall &&  UseTemp => IsCongested = isStalled || isHot
			2  UseStall && !UseTemp => IsCongested = isStalled 
			3 !UseStall &&  UseTemp => IsCongested = isHot
			4 !UseStall && !UseTemp => IsCongested = false
		*/

		trainStatePtr->m_isCongested = false;

		// marvin++ add support for ignoring congestion
		trainStatePtr->m_isIgnoringCongestion = m_theMaps.getIsIgnoringCongestion(newEcsZoneId);
		// ++marvin

		// Ignore congestion for ECS Zones at stations

		if (false == trainStatePtr->m_isIgnoringCongestion)
		{

			if (true == m_theConfig.getCongestionUsesStallTime())
			{
				trainStatePtr->m_isCongested |= trainStatePtr->m_isStalled;
			}

			if (true == m_theConfig.getCongestionUsesTemperature())
			{
				trainStatePtr->m_isCongested |= trainStatePtr->m_isHot;
			}

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"EcsZoneDataSource::processAtsPacket: useStallTime=%d useTemperature=%d m_isCongested=%d",
				m_theConfig.getCongestionUsesStallTime(),
				m_theConfig.getCongestionUsesTemperature(),
				trainStatePtr->m_isCongested);
		
		}
		else
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
				"EcsZoneDataSource::processAtsPacket: m_isIgnoringCongestion is TRUE");
		}

		// Change trainStatePtr ownership
		if (oldEcsZoneId != newEcsZoneId)
		{
			m_trainStatesMap.erase(it);
			m_trainStatesMap.insert(zoneId2trainState(newEcsZoneId, trainStatePtr));

			if (true == updateAffectedDataPoints && false == isStationAndNotDegraded)
			{
				updateEcsZoneDataPoints(newEcsZoneId);
			}
		}
	}
	catch(TA_Base_Core::InvalidATSPacketException&)
	{
		// Move trainStatePtr object to no mans land
		delete (it->second);
		m_trainStatesMap.erase(it);
		trainStatePtr = new TrainState(trainId);
		m_trainStatesMap.insert(zoneId2trainState(TA_IRS_Bus::ECS_NONE_OR_INVALID, trainStatePtr));

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					 "EcsZoneDataSource::processAtsPacket: InvalidATSPacketException");

	}
	catch(TA_Base_Core::CachedMappingNotFoundException&)
	{
		// Ignore unknown ECS Zones for m_theMaps.getIsIgnoringStall(newEcsZoneId)
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
				"EcsZoneDataSource::processAtsPacket: CachedMappingNotFoundException");
	}  

    if (true == updateAffectedDataPoints && false == isStationAndNotDegraded)
    {
        // Regardless of outcome, the datapoints at the oldEcsZoneId should be updated. 
		updateEcsZoneDataPoints(oldEcsZoneId);		
    }

    FUNCTION_EXIT;
}


void EcsZoneDataSource::updateEcsZoneDataPoints(unsigned long ecsZoneId)
    // throw 
{
    FUNCTION_ENTRY("updateEcsZoneDataPoints");

    bool summaryEmergencyState = false;
    unsigned long summaryCongestionState = TA_IRS_Bus::ECS_ZONE_CONGESTION_ENUM_NORMAL;

    TA_Base_Core::ThreadGuard guard( m_trainStatesMapLock );

    // Search for all trainIds at this ECS Zone
    std::pair<ZoneIdToTrainStatesMapIt, ZoneIdToTrainStatesMapIt> range =
        m_trainStatesMap.equal_range (ecsZoneId);


    for (ZoneIdToTrainStatesMapIt it=range.first ; it != range.second; ++it)
    {
        TA_ASSERT((it->second), "trainState is NULL");
        
        summaryEmergencyState |= (it->second)->getEmergencyDataPointState();

        unsigned long newCongestionState = (it->second)->getCongestionDataPointState();
        if (newCongestionState>summaryCongestionState)
        {
            summaryCongestionState = newCongestionState;
        }
    }

    // Updates congestion datapoints
	try
	{
		unsigned long oldCongestionState= m_theDatapoints.getCongestion(ecsZoneId);

		if (oldCongestionState != summaryCongestionState)
		{
            //CongestionApplied datapoint is removed
			//if (oldCongestionState == TA_IRS_Bus::ECS_ZONE_CONGESTION_ENUM_NORMAL)
			//{
			//	// If the transition is from a normal state into congestion, then
			//	// reset the congestion applied datapoint
			//	m_theDatapoints.setCongestionApplied(ecsZoneId, false);
			//}

			m_theDatapoints.setCongestion(ecsZoneId, summaryCongestionState);

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"EcsZoneDataSource::updateEcsZoneDataPoints: setCongestion with ecsZoneId=%d oldCongestionState=%d summaryCongestionState=%d",
				ecsZoneId, oldCongestionState, summaryCongestionState);
		}
	}
	catch(TA_Base_Core::InvalidECSConfigurationException& )
	{
		// Ignore unrecognised datapoint - already logged
	}

	// Updates emergency datapoints
	try
	{
		bool oldEmergencyState = m_theDatapoints.getEmergency(ecsZoneId);

		if (oldEmergencyState != summaryEmergencyState)
		{
			if (false == oldEmergencyState)
			{
				// If the transition is from a normal state into emergency, then
				// reset the emergency applied datapoint
				m_theDatapoints.setEmergencyApplied(ecsZoneId, false);
			}

			m_theDatapoints.setEmergency(ecsZoneId, summaryEmergencyState);

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"EcsZoneDataSource::updateEcsZoneDataPoints: setEmergency with ecsZoneId=%d summaryCongestionState=%d",
				ecsZoneId, summaryEmergencyState);
		}
	}
	catch(TA_Base_Core::InvalidECSConfigurationException& )
	{
		// Ignore unrecognised datapoint - already logged
	}
		
    FUNCTION_EXIT;
}


void EcsZoneDataSource::updateAllEcsZoneDataPoints()
{
    std::vector<unsigned long> zoneIds = m_theMaps.getAllZoneIds();

    for (std::vector<unsigned long>::iterator it = zoneIds.begin(); it!=zoneIds.end(); ++it)
    {
        if(true == m_theConfig.getIsMaster())
        {
            updateEcsZoneDataPoints(*it);
        }
        else // station
        {
            // Unknown ecs zones will be ignored
			if (false == m_theMaps.isZoneIdInScope(*it))
            {
               return; 
            }

			updateEcsZoneDataPoints(*it);
        }
    }
}


void EcsZoneDataSource::removeEmergencyIfAlarmManuallyClosed(unsigned long zoneId)
{
	std::pair<ZoneIdToTrainStatesMapIt, ZoneIdToTrainStatesMapIt> range =
	m_trainStatesMap.equal_range (zoneId);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						"Remove alarm in zone %d", zoneId);

	bool isZoneEcsDatapointsNeedUpdate = false;
	bool isAlarmNeedToRaiseAgain = false;
	std::set<int> trainIdNeedToRaiseAgain;
	ZoneIdToTrainStatesMapIt it;
	for (it=range.first ; it != range.second; ++it)
	{
		TA_ASSERT((it->second), "trainState is NULL");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"Found train state for zone %d: emergency is %d", 
						zoneId, it->second->m_isInEmergency);

		if (it->second->m_isInEmergency == true)
		{
			it->second->m_isInEmergency = false;
			isZoneEcsDatapointsNeedUpdate = true;

			if (true == it->second->m_isOa1Valid && 
				true == it->second->m_isLocalisationValid)
			{
				isAlarmNeedToRaiseAgain = true;
				trainIdNeedToRaiseAgain.insert(it->second->m_trainId);
			}
		}
	}	

	if (true == isZoneEcsDatapointsNeedUpdate)
	{
		updateEcsZoneDataPoints(zoneId);
	}

	if (true == isAlarmNeedToRaiseAgain)
	{
		for (it=range.first ; it != range.second; ++it)
		{
			std::set<int>::iterator iter=trainIdNeedToRaiseAgain.find(it->second->m_trainId);
			
			if (iter==trainIdNeedToRaiseAgain.end())
			{
				continue;
			}

			if (it->second->m_isInEmergency == false)
			{
				it->second->m_isInEmergency = true;
			}
		}
		
		updateEcsZoneDataPoints(zoneId);
	}
}
