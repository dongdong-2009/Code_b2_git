/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_Review_Branch/4669/transactive/bus/ats/ats_agent/CommonATS/src/AtsPasData.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #7 $
  *
  * Last modification: $DateTime: 2011/06/13 12:14:52 $
  * Last modified by:  $Author: bihui.luo $
  *
  * This represents the ATS PAS Data
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"

#include "bus/ats/ats_agent/CommonATS/src/AtsPasData.h"

namespace TA_IRS_Bus
{
	AtsPasData::AtsPasData(	TA_IRS_Bus::PlatformIdType platformId, unsigned long stationId,
											unsigned long entityKey, unsigned long locationKey,unsigned char  tisPlatformId)
	:
	m_preArrival (false), 
	m_arrival (false),
	m_preDeparture (false),
	m_departure (false),
	m_skip (false),
	m_hold (false),
	m_evacuation (false),
	m_trainOverrun  (false),
	m_inDepot (false),
	m_commsMessageSender (NULL),
	m_entityKey (entityKey), 
	m_locationKey (locationKey)
	{
		m_atsPasMessage.platformId = platformId;
		m_atsPasMessage.stationId = stationId;
		m_atsPasMessage.trainInfo.valid = false;
		m_atsPasMessage.trainInfo.destinationStationId = 0;
		m_atsPasMessage.trainInfo.lastTrain = false;
		m_atsPasMessage.platformDirection = TA_IRS_Bus::Unknown;
		switch(tisPlatformId)
		{
		case TA_IRS_Bus::down:
			m_atsPasMessage.platformDirection = TA_IRS_Bus::down;
			break;
		case TA_IRS_Bus::up:
			m_atsPasMessage.platformDirection = TA_IRS_Bus::up;
			break;
		}

		m_pasEvents.clear();

		// get a handle to CommsMessageSender
		m_commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender ( TA_Base_Core::ATSAgentComms::Context );
	};

	AtsPasData::~AtsPasData()
	{
		// clean up comms message sender
		if ( m_commsMessageSender != NULL )
		{
			delete m_commsMessageSender;
			m_commsMessageSender = NULL;
		}
	}

	TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef AtsPasData::buildAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::EEventType event)
	{
		TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef message = m_atsPasMessage;

		message.atsEventType = event;

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Built Ats Pas Message for Platform id %d, Event Type %i",
			message.platformId,(int)event);	
		return message;
	}

	std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef> AtsPasData::buildAtsPasMessageVector()
	{
		std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef> messageVector;
		std::vector<TA_IRS_Bus::IAtsPasCorbaDef::EEventType>::iterator it;
			
		for (it = m_pasEvents.begin(); it != m_pasEvents.end(); it++)
		{
			messageVector.push_back(buildAtsPasMessage(*it));
		}

		return messageVector;
	}

	std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef> AtsPasData::buildAtsPasMessageState()
	{
		std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef> messageVector;

		if (m_preArrival)
		{
			messageVector.push_back(buildAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::PreArrival));
		}

		if (m_arrival)
		{
			messageVector.push_back(buildAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::Arrival));
		}

		if (m_preDeparture)
		{
			messageVector.push_back(buildAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::PreDeparture));
		}

		if (m_departure)
		{
			messageVector.push_back(buildAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::Departure));
		}

		if (m_skip)
		{
			messageVector.push_back(buildAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::Skip));
		}

		if (m_hold)
		{
			messageVector.push_back(buildAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::Hold));
		}
		
		if (m_evacuation)
		{
			messageVector.push_back(buildAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::Evacuation));
		}
		
		if (m_trainOverrun)
		{
			messageVector.push_back(buildAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::TrainOverrun));
		}

		if (m_inDepot)
		{
			messageVector.push_back(buildAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::InDepot));
		}

		return messageVector;
	}
	
	void AtsPasData::resetAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef pasUpdate)
	{
		//Reset the flags based on message type before setting the flags to 1, so that 
		//AtsPasMessage can be broadcasted properly when there was a message lost before
		switch(pasUpdate.atsEventType)
		{
		case TA_IRS_Bus::IAtsPasCorbaDef::PreArrival:			
			updatePreArrival(false);
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::Arrival:
			updateArrival(false);
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::PreDeparture:
			updatePreDeparture(false);
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::Departure:
			updateDeparture(false);
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::Skip:
			updateSkip(false);
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::Hold:
			updateHold(false);
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::Evacuation:
			updateEvacuation(false);
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::TrainOverrun:
			updateTrainOverrun(false);
			break;
		case TA_IRS_Bus::IAtsPasCorbaDef::InDepot:
			updateInDepot(false);
			break;
		}
		
		return;
	}

	void AtsPasData::importAtsPasMessage(TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef pasUpdate)
	{
		switch(pasUpdate.atsEventType)
		{
		case TA_IRS_Bus::IAtsPasCorbaDef::PreArrival:
			updatePreArrival(true);
			updateArrival(false);
			updatePreDeparture(false);
			updateDeparture(false);
			updateSkip(false);
			updateHold(false);
			updateEvacuation(false);
			updateTrainOverrun(false);
			updateInDepot(false);
			break;

		case TA_IRS_Bus::IAtsPasCorbaDef::Arrival:
			updatePreArrival(false);		
			updateArrival(true);
			updatePreDeparture(false);
			updateDeparture(false);
			updateSkip(false);
			updateHold(false);
			updateEvacuation(false);
			updateTrainOverrun(false);
			updateInDepot(false);
			break;

		case TA_IRS_Bus::IAtsPasCorbaDef::PreDeparture:
			updatePreArrival(false);
			updateArrival(false);			
			updatePreDeparture(true);
			updateDeparture(false);
			updateSkip(false);
			updateHold(false);
			updateEvacuation(false);
			updateTrainOverrun(false);
			updateInDepot(false);
			break;

		case TA_IRS_Bus::IAtsPasCorbaDef::Departure:
			updatePreArrival(false);
			updateArrival(false);
			updatePreDeparture(false);			
			updateDeparture(true);
			updateSkip(false);
			updateHold(false);
			updateEvacuation(false);
			updateTrainOverrun(false);
			updateInDepot(false);
			break;

		case TA_IRS_Bus::IAtsPasCorbaDef::Skip:
			updatePreArrival(false);
			updateArrival(false);
			updatePreDeparture(false);
			updateDeparture(false);		
			updateSkip(true);
			updateHold(false);
			updateEvacuation(false);
			updateTrainOverrun(false);
			updateInDepot(false);
			break;

		case TA_IRS_Bus::IAtsPasCorbaDef::Hold:
			updatePreArrival(false);
			updateArrival(false);
			updatePreDeparture(false);
			updateDeparture(false);
			updateSkip(false);		
			updateHold(true);
			updateEvacuation(false);
			updateTrainOverrun(false);
			updateInDepot(false);
			break;

		case TA_IRS_Bus::IAtsPasCorbaDef::Evacuation:
			updatePreArrival(false);
			updateArrival(false);
			updatePreDeparture(false);
			updateDeparture(false);
			updateSkip(false);
			updateHold(false);		
			updateEvacuation(true);
			updateTrainOverrun(false);
			updateInDepot(false);
			break;

		case TA_IRS_Bus::IAtsPasCorbaDef::TrainOverrun:
			updatePreArrival(false);
			updateArrival(false);
			updatePreDeparture(false);
			updateDeparture(false);
			updateSkip(false);
			updateHold(false);
			updateEvacuation(false);		
			updateTrainOverrun(true);
			updateInDepot(false);
			break;

		case TA_IRS_Bus::IAtsPasCorbaDef::InDepot:
			updatePreArrival(false);
			updateArrival(false);
			updatePreDeparture(false);
			updateDeparture(false);
			updateSkip(false);
			updateHold(false);
			updateEvacuation(false);
			updateTrainOverrun(false);		
			updateInDepot(true);
			break;
		}

		updateTrainInfoValidity(pasUpdate.trainInfo.valid);		
		updateDestinationStationId(pasUpdate.trainInfo.destinationStationId);
		updateLastTrainOfOperatingDay(pasUpdate.trainInfo.lastTrain);
		updateTisPlatformId(pasUpdate.platformDirection);
	}

	void AtsPasData::updatePreArrival(bool preArrival)
	{
		if (m_preArrival != preArrival)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updatePreArrival() - platform %d PreArrival, was %s now %s, Skip is %d", 
				m_atsPasMessage.platformId, 
				m_preArrival ? "set" : "not set", 
				preArrival ? "set" : "not set",
				m_skip);

            m_preArrival = preArrival;

            if (m_preArrival)
            {
                //one event for a platform at a time
				m_pasEvents.clear();
				m_pasEvents.push_back(TA_IRS_Bus::IAtsPasCorbaDef::PreArrival);
            }
        }
    }

	void AtsPasData::updateArrival(bool arrival)
	{
		if (m_arrival != arrival)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateArrival() - platform %d arrival, was %s now %s", 
				m_atsPasMessage.platformId, 
				m_arrival?"set":"not set",
				arrival?"set":"not set");

			m_arrival = arrival;

			if (m_arrival)
			{
				//one event for a platform at a time
				m_pasEvents.clear();
				m_pasEvents.push_back(TA_IRS_Bus::IAtsPasCorbaDef::Arrival);
			}
		}
	}

	void AtsPasData::updatePreDeparture(bool preDeparture)
	{
		if (m_preDeparture != preDeparture)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updatePreDeparture() - platform %d preDeparture, was %s now %s", 
				m_atsPasMessage.platformId, 
				m_preDeparture?"set":"not set", 
				preDeparture?"set":"not set");

			m_preDeparture = preDeparture;

			if (m_preDeparture)
			{
				//one event for a platform at a time
				m_pasEvents.clear();
				m_pasEvents.push_back(TA_IRS_Bus::IAtsPasCorbaDef::PreDeparture);
			}
		}
	}

	void AtsPasData::updateDeparture(bool departure)
	{
		if (m_departure != departure)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateDeparture() - platform %d departure, was %s now %s", 
				m_atsPasMessage.platformId, 
				m_departure?"set":"not set", 
				departure?"set":"not set");

			m_departure = !m_departure;

			if (m_departure)
			{
				//one event for a platform at a time
				m_pasEvents.clear();
				m_pasEvents.push_back(TA_IRS_Bus::IAtsPasCorbaDef::Departure);
			}
		}
	}

	void AtsPasData::updateSkip(bool skip)
	{
		if (m_skip != skip)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateSkip() - platform %d skip, was %s now %s", 
				m_atsPasMessage.platformId, 
				m_skip?"set":"not set", 
				skip?"set":"not set");

			m_skip = skip;
			
			if (m_skip)
			{
				//one event for a platform at a time
				m_pasEvents.clear();
				m_pasEvents.push_back(TA_IRS_Bus::IAtsPasCorbaDef::Skip);
			}
		}
	}

	void AtsPasData::updateHold(bool hold)
	{
		if (m_hold != hold)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateHold() - platform %d hold, was %s now %s", 
				m_atsPasMessage.platformId, 
				m_hold?"set":"not set", 
				hold?"set":"not set");

			m_hold = hold;
			
			if (m_hold)
			{
				//one event for a platform at a time
				m_pasEvents.clear();
				m_pasEvents.push_back(TA_IRS_Bus::IAtsPasCorbaDef::Hold);
			}
		}
	}

	void AtsPasData::updateEvacuation(bool evacuation)
	{
		if (m_evacuation != evacuation)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateEvacuation() - platform %d evacuation, was %s now %s", 
				m_atsPasMessage.platformId, 
				m_evacuation?"set":"not set", 
				evacuation?"set":"not set");

			m_evacuation = evacuation;
			
			if (m_evacuation)
			{
				//one event for a platform at a time
				m_pasEvents.clear();
				m_pasEvents.push_back(TA_IRS_Bus::IAtsPasCorbaDef::Evacuation);
			}
		}
	}

	void AtsPasData::updateTrainOverrun(bool trainOverrun)
	{
		if (m_trainOverrun != trainOverrun)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateTrainOverrun() - platform %d trainOverrun, was %d now %d", 
				m_atsPasMessage.platformId, 
				m_trainOverrun?"set":"not set", 
				trainOverrun?"set":"not set");

			m_trainOverrun = trainOverrun;
			
			if (m_trainOverrun)
			{
				//one event for a platform at a time
				m_pasEvents.clear();
				m_pasEvents.push_back(TA_IRS_Bus::IAtsPasCorbaDef::TrainOverrun);
			}
		}
	}

	void AtsPasData::updateTrainInfoValidity(bool valid)
	{
		if (valid != m_atsPasMessage.trainInfo.valid)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateTrainInfoValidity() - platform %d validity, was %s now %s", 
				m_atsPasMessage.platformId, (m_atsPasMessage.trainInfo.valid?"valid":"invalid"), (valid?"valid":"invalid"));

			m_atsPasMessage.trainInfo.valid = valid;
		}
	}
		
	void AtsPasData::updateDestinationStationId(TA_IRS_Bus::StationIdType destinationStationId)
	{
		if (destinationStationId != m_atsPasMessage.trainInfo.destinationStationId)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateDestinationStationId() - platform %d destinationStationId, was %d now %d", 
				m_atsPasMessage.platformId, m_atsPasMessage.trainInfo.destinationStationId, destinationStationId);

			m_atsPasMessage.trainInfo.destinationStationId = destinationStationId;
		}
	}
	
	void AtsPasData::updateLastTrainOfOperatingDay(bool lastTrainOfOperatingDay)
	{
		if (m_atsPasMessage.trainInfo.lastTrain != lastTrainOfOperatingDay)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateLastTrainOfOperatingDay() - platform %d lastTrainOfOperatingDay, was %s now %s", 
				m_atsPasMessage.platformId, 
				m_atsPasMessage.trainInfo.lastTrain?"set":"not set", 
				lastTrainOfOperatingDay?"set":"not set");

			m_atsPasMessage.trainInfo.lastTrain = lastTrainOfOperatingDay;
		}
	}

	TA_IRS_Bus::PlatformIdType AtsPasData::getPlatformId()
	{
		return m_atsPasMessage.platformId;
	}

	void AtsPasData::processUpdates()
	{
		if (m_pasEvents.size() > 0)
		{			
			m_pasMessageVector.clear();
			m_pasMessageVector = buildAtsPasMessageVector();

			std::vector<TA_IRS_Bus::IAtsPasCorbaDef::EEventType>::iterator it;			
			for (it = m_pasEvents.begin(); it != m_pasEvents.end(); it++)
			{
				TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef pasMessage 
					= buildAtsPasMessage(*it);

				TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef* pasMessageCopy
					= new TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef (pasMessage);

				CORBA::Any data;
				data <<= pasMessageCopy;

				m_commsMessageSender->sendCommsMessage ( TA_Base_Core::ATSAgentComms::AtsPasUpdate,
														m_entityKey,
														data,
														0,
														m_locationKey);


				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"\n\tsendUpdates() -  Info: AtsPasUpdate sent - platform=%d, entity=%d, location=%d, destinationStationId=%d, "
					"PreArrival=%d, Arrival=%d, Departure=%d, Skip=%d, Hold=%d, LastTrain=%d, platformDirection=%d, InDepot=%d", 
					m_atsPasMessage.platformId, m_entityKey, m_locationKey,
					m_atsPasMessage.trainInfo.destinationStationId,
					m_preArrival, m_arrival, m_departure, m_skip, m_hold, m_atsPasMessage.trainInfo.lastTrain, m_atsPasMessage.platformDirection, m_inDepot);
			}			
			m_pasEvents.clear();
		}
		else
		{
			m_pasMessageVector.clear();
		}

	}

	void AtsPasData::updateTisPlatformId(TA_IRS_Bus::EDirection platformDirection)
	{
		FUNCTION_ENTRY("updateTisPlatformId");

		if (platformDirection != m_atsPasMessage.platformDirection )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"updateTisPlatformId() - PlatformDirection, was %d now %d", 
				 m_atsPasMessage.platformDirection, platformDirection);

			m_atsPasMessage.platformDirection = platformDirection;
		}

		FUNCTION_EXIT;
	}

	void AtsPasData::updateInDepot(bool inDepot)
	{
		if (m_inDepot != inDepot)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
				"updateInDepot() - platform %d InDepot, was %s now %s", 
				m_atsPasMessage.platformId, 
				m_inDepot?"set":"not set", 
				inDepot?"set":"not set");

			m_inDepot = inDepot;

			if (m_inDepot)
			{
				m_pasEvents.push_back(TA_IRS_Bus::IAtsPasCorbaDef::InDepot);
			}
		}
	}

	std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef>& AtsPasData::getPasMessageVector()
	{
		return m_pasMessageVector;
	}

	void  AtsPasData::clearPasMessageVector()
	{
		m_pasMessageVector.clear();
	}
	 
}
