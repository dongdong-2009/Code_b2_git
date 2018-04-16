/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File$
  * @author:   Darren Sampson
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last mofified by:  $Author$
  *
  * This represents the ATS TIS Data
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"

#include "app/ats/AtsAgent/src/AtsTisData.h"

namespace TA_IRS_App
{
	const time_t PLATFORM_INFO_UNCHANGED_MAX_INTERVAL = 12;			//seconds
	AtsTisData::AtsTisData(	TA_IRS_Bus::PlatformIdType platformId,
                            unsigned long stationId,
                            TA_IRS_Bus::TisPlatformIdType tisPlatformId,
							unsigned long entityKey,
                            unsigned long locationKey )
	:
	m_platformId (platformId),
	m_stationId (stationId),
    m_tisPlatformId (tisPlatformId),
	m_commsMessageSender (NULL),
	m_entityKey (entityKey), 
	m_locationKey (locationKey)
	{

		m_platformInfo.preArrival = false;
		m_platformInfo.arrival = false;
		m_platformInfo.preDeparture = false;
		m_platformInfo.departure = false;
		m_platformInfo.skip = false;
		m_platformInfo.hold = false;
		m_platformInfo.evacuation = false;
		m_platformInfo.trainOverrun = false;
		m_platformInfo.validity = false;

		for (int i = 0; i < 2; i++)
		{
			m_trainInfo[i].valid = false;
			m_trainInfo[i].physicalTrainNumber = 0;
			m_trainInfo[i].serviceNumber = 0;	
			m_trainInfo[i].destinationStationId = 0;	
			m_trainInfo[i].lastTrain = 0;
			m_trainInfo[i].predictedTime.year = 0;
			m_trainInfo[i].predictedTime.month = 0;
			m_trainInfo[i].predictedTime.day = 0;
			m_trainInfo[i].predictedTime.hour = 0;
			m_trainInfo[i].predictedTime.minute = 0;
			m_trainInfo[i].predictedTime.second = 0;
		}

		// get a handle to CommsMessageSender
		m_commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender ( TA_Base_Core::ATSAgentComms::Context );

        // send initial invalid data
        m_trainInfoChanged = true;
        m_platformInfoChanged = true;
		m_lastTimePlatformInfoSend = 0;
        
		processUpdates();
	};

	AtsTisData::~AtsTisData()
	{
		// clean up comms message sender
		if ( m_commsMessageSender != NULL )
		{
			delete m_commsMessageSender;
			m_commsMessageSender = NULL;
		}
	}

	TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef AtsTisData::buildAtsTisMessage(bool platformInfo)
	{	
		TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef message;

		message.stationId = m_stationId;
		message.platformId = m_platformId;
        message.tisPlatformId = m_tisPlatformId;
			
		if (platformInfo)
		{
			message.message.platformInformation(m_platformInfo);
		}
		else
		{
			TA_IRS_Bus::IAtsTisCorbaDef::TrainInfoPair trainInformation;

			trainInformation.firstTrain = m_trainInfo[0];
			trainInformation.secondTrain = m_trainInfo[1];

			message.message.trainInformation(trainInformation);
		}

		return message;
	}

	void AtsTisData::importAtsTisMessage(const TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef& tisUpdate)
	{
		switch (tisUpdate.message._d())
		{
		case TA_IRS_Bus::IAtsTisCorbaDef::PLATFORM_INFORMATION:
			{
				const TA_IRS_Bus::IAtsTisCorbaDef::PlatformInfo& platformInfo
					= tisUpdate.message.platformInformation();
					
				if (platformInfo.validity)
				{
					updatePreArrival(platformInfo.preArrival);
					updateArrival(platformInfo.arrival);
					updatePreDeparture(platformInfo.preDeparture);
					updateDeparture(platformInfo.departure);
					updateSkip(platformInfo.skip);
					updateHold(platformInfo.hold);
					updateEvacuation(platformInfo.evacuation); //limin--, ECP-657 CL-21010 //bihui-- CL-22084
					updateTrainOverrun(platformInfo.trainOverrun);
				}
			}
			break;
		
		case TA_IRS_Bus::IAtsTisCorbaDef::TRAIN_INFORMATION:
			{
				unsigned int i;

				for (i = 0; i < 2; i++)
				{
					const TA_IRS_Bus::IAtsTisCorbaDef::TrainInfo* trainInfo;

					if (i == 0)
					{
						trainInfo = &tisUpdate.message.trainInformation().firstTrain;
					}
					else
					{
						trainInfo = &tisUpdate.message.trainInformation().secondTrain;
					}
				
					if (trainInfo->valid)
					{
						updatePhysicalTrainNumber(i, trainInfo->physicalTrainNumber);
						updateServiceNumber(i, trainInfo->serviceNumber);
						updateDestinationStationId(i, trainInfo->destinationStationId);
						updateLastTrainOfOperatingDay(i, trainInfo->lastTrain);
						updateScheduledArrival(i, trainInfo->predictedTime);
						updateTrainInfoValidity(i, true);
					}
					else
					{
						updateTrainInfoValidity(i, false);
					}
				}
			}
			break;
		}
	}

	bool AtsTisData::getStationType()
	{
		// Need to work out how to determine this
		return false;
	};

	void AtsTisData::updatePreArrival(bool preArrival)
	{
		setValid();
		if (m_platformInfo.preArrival != preArrival)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updatePreArrival() - platform %d PreArrival, was %s now %s", 
				m_platformId, 
				m_platformInfo.preArrival?"set":"not set", 
				preArrival?"set":"not set");

			m_platformInfo.preArrival = preArrival;

			m_platformInfoChanged = true;

		}
	}

	void AtsTisData::updateArrival(bool arrival)
	{
		setValid();
		if (m_platformInfo.arrival != arrival)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateArrival() - platform %d arrival, was %s now %s", 
				m_platformId, 
				m_platformInfo.arrival?"set":"not set", 
				arrival?"set":"not set");

			m_platformInfo.arrival = arrival;

			m_platformInfoChanged = true;
		}
	}

	void AtsTisData::updatePreDeparture(bool preDeparture)
	{
		setValid();
		if (m_platformInfo.preDeparture != preDeparture)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updatePreDeparture() - platform %d preDeparture, was %s now %s", 
				m_platformId, 
				m_platformInfo.preDeparture?"set":"not set", 
				preDeparture?"set":"not set");

			m_platformInfo.preDeparture = preDeparture;

			m_platformInfoChanged = true;
		}
	}

	void AtsTisData::updateDeparture(bool departure)
	{
		setValid();
		if (m_platformInfo.departure != departure)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateDeparture() - platform %d departure, was %s now %s", 
				m_platformId, 
				m_platformInfo.departure?"set":"not set", 
				departure?"set":"not set");

			m_platformInfo.departure = departure;

			m_platformInfoChanged = true;
		}
	}

	void AtsTisData::updateSkip(bool skip)
	{
		setValid();
		if (m_platformInfo.skip != skip)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateSkip() - platform %d skip, was %s now %s", 
				m_platformId, 
				m_platformInfo.skip?"set":"not set", 
				skip?"set":"not set");

			m_platformInfo.skip = skip;
			
			m_platformInfoChanged = true;
		}
	}

	void AtsTisData::updateHold(bool hold)
	{
		setValid();
		if (m_platformInfo.hold != hold)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateHold() - platform %d hold, was %s now %s", 
				m_platformId, 
				m_platformInfo.hold?"set":"not set", 
				hold?"set":"not set");

			m_platformInfo.hold = hold;
			
			m_platformInfoChanged = true;
		}
	}

    //limin--, ECP-657 CL-21010
	//bihui - CL-22084
     void AtsTisData::updateEvacuation(bool evacuation)
     {
         setValid();
         if (m_platformInfo.evacuation != evacuation)
         {
             LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                 "updateEvacuation() - platform %d evacuation, was %s now %s", 
                 m_platformId, 
                 m_platformInfo.evacuation?"set":"not set", 
                 evacuation?"set":"not set");
             
             m_platformInfo.evacuation = evacuation;
             
             m_platformInfoChanged = true;
         }
     }
    //--limin, ECP-657 CL-21010

	void AtsTisData::updateTrainOverrun(bool trainOverrun)
	{
		setValid();
		if (m_platformInfo.trainOverrun != trainOverrun)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateTrainOverrun() - platform %d trainOverrun, was %s now %s", 
				m_platformId, 
				m_platformInfo.trainOverrun?"set":"not set", 
				trainOverrun?"set":"not set");

			m_platformInfo.trainOverrun = trainOverrun;
			
			m_platformInfoChanged = true;
		}
	}

	void AtsTisData::updateTrainInfoValidity(unsigned int trainIndex, bool valid)
	{
		setValid();
		if (valid != m_trainInfo[trainIndex].valid)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateTrainInfoValidity() - platform %d train %d validity, was %s now %s", 
				m_platformId, trainIndex, 
				m_trainInfo[trainIndex].valid?"valid":"invalid", 
				valid?"valid":"invalid");

			m_trainInfo[trainIndex].valid = valid;

			m_trainInfoChanged = true;
		}
	}

	void AtsTisData::updatePlatformType(unsigned int trainIndex, bool platformType)
	{
		setValid();
		if (platformType != m_trainInfo[trainIndex].stationType)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updatePlatformType() - platform %d train %d arrival, was %d now %d", 
				m_platformId, trainIndex, m_trainInfo[trainIndex].stationType, platformType);

			m_trainInfo[trainIndex].stationType = platformType;

			m_trainInfoChanged = true;
		}
	}

	void AtsTisData::updatePhysicalTrainNumber(unsigned int trainIndex, TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber)
	{
		setValid();
		if (physicalTrainNumber != m_trainInfo[trainIndex].physicalTrainNumber)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updatePhysicalTrainNumber() - platform %d train %d physicalTrainNumber, was %d now %d", 
				m_platformId, trainIndex, m_trainInfo[trainIndex].physicalTrainNumber, physicalTrainNumber);

			m_trainInfo[trainIndex].physicalTrainNumber = physicalTrainNumber;

			m_trainInfoChanged = true;
		}
	}

	void AtsTisData::updateServiceNumber(unsigned int trainIndex, TA_IRS_Bus::ServiceNumberType serviceNumber)
	{
		setValid();
		if (serviceNumber != m_trainInfo[trainIndex].serviceNumber)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateServiceNumber() - platform %d train %d serviceNumber, was %d now %d", 
				m_platformId, trainIndex, m_trainInfo[trainIndex].serviceNumber, serviceNumber);

			m_trainInfo[trainIndex].serviceNumber = serviceNumber;

			m_trainInfoChanged = true;
		}
	}
			
	void AtsTisData::updateDestinationStationId(unsigned int trainIndex, TA_IRS_Bus::StationIdType destinationStationId)
	{
		setValid();
		if (destinationStationId != m_trainInfo[trainIndex].destinationStationId)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateDestinationStationId() - platform %d train %d destinationStationId, was %d now %d", 
				m_platformId, trainIndex, m_trainInfo[trainIndex].destinationStationId, destinationStationId);

			m_trainInfo[trainIndex].destinationStationId = destinationStationId;

			m_trainInfoChanged = true;
		}
	}
	
	void AtsTisData::updateLastTrainOfOperatingDay(unsigned int trainIndex, bool lastTrainOfOperatingDay)
	{
		setValid();
		if (m_trainInfo[trainIndex].lastTrain != lastTrainOfOperatingDay)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateLastTrainOfOperatingDay() - platform %d train %d lastTrainOfOperatingDay, was %s now %s", 
				m_platformId, trainIndex, 
				m_trainInfo[trainIndex].lastTrain?"set":"not set", 
				lastTrainOfOperatingDay?"set":"not set");

			m_trainInfo[trainIndex].lastTrain = !m_trainInfo[trainIndex].lastTrain;

			m_trainInfoChanged = true;
		}
	}

	void AtsTisData::updateScheduledArrival(unsigned int trainIndex, TA_IRS_Bus::TimeStampType scheduledArrival)
	{
		setValid();
		bool scheduledArrivalChanged = false;

		scheduledArrivalChanged |= (scheduledArrival.year != m_trainInfo[trainIndex].predictedTime.year);
		scheduledArrivalChanged |= (scheduledArrival.month != m_trainInfo[trainIndex].predictedTime.month);
		scheduledArrivalChanged |= (scheduledArrival.day != m_trainInfo[trainIndex].predictedTime.day);
		scheduledArrivalChanged |= (scheduledArrival.hour != m_trainInfo[trainIndex].predictedTime.hour);
		scheduledArrivalChanged |= (scheduledArrival.minute != m_trainInfo[trainIndex].predictedTime.minute);
		scheduledArrivalChanged |= (scheduledArrival.second != m_trainInfo[trainIndex].predictedTime.second);
		
		if (scheduledArrivalChanged)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
                "updateScheduledArrival() - platform %d train %d scheduledArrival, was %d now %d", 
				m_platformId, trainIndex, m_trainInfo[trainIndex].predictedTime, scheduledArrival);

			m_trainInfo[trainIndex].predictedTime = scheduledArrival;

			m_trainInfoChanged = true;
		}
	}

	bool AtsTisData::isPlatformUpdated()
	{
		return m_platformInfoChanged;
	}

	bool AtsTisData::isTrainUpdated()
	{
		return m_trainInfoChanged;
	}

	void AtsTisData::processUpdates()
	{
        if (m_trainInfoChanged)
		{
			TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef tisMessage 
				= buildAtsTisMessage(false);

			//TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef* tisMessageCopy
			//	= new TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef(tisMessage);

			TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef tisMessageCopy(tisMessage);

			CORBA::Any data;
				data <<= tisMessageCopy;

			m_commsMessageSender->sendCommsMessage ( TA_Base_Core::ATSAgentComms::AtsTisUpdate,
													m_entityKey,
													data,
													0,
													m_locationKey);

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"sendUpdates() - Info: AtsTisMessage train type sent - platform=%d, entity=%d, location=%d, train1=%s, train2=%s",
				m_platformId, m_entityKey, m_locationKey,
				(m_trainInfo[0].valid?"VALID":"INVALID"),
				(m_trainInfo[1].valid?"VALID":"INVALID"));

			m_trainInfoChanged = false;
	
		}
		//CL20557, ATS Agent needs to be modify for platform TIS message. hongzhi modified
		//if no update is received for 3 cycles, we should attempt to send the information to the STIS if ATS data is valid
		if (m_platformInfoChanged || ((time(NULL)-m_lastTimePlatformInfoSend) >= PLATFORM_INFO_UNCHANGED_MAX_INTERVAL))
		{
			TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef tisMessage 
				= buildAtsTisMessage(true);

			//TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef* tisMessageCopy
			//	= new TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef(tisMessage);

			TA_IRS_Bus::IAtsTisCorbaDef::AtsTisMessageCorbaDef tisMessageCopy(tisMessage);

			CORBA::Any data;
			data <<= tisMessageCopy;

			m_commsMessageSender->sendCommsMessage ( TA_Base_Core::ATSAgentComms::AtsTisUpdate,
													m_entityKey,
													data,
													0,
													m_locationKey);

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"sendUpdates() - Info: AtsTisMessage platform type sent - platform=%d, entity=%d, location=%d",
				m_platformId, m_entityKey, m_locationKey);

			m_platformInfoChanged = false;
			m_lastTimePlatformInfoSend = time(NULL);				//record the current time
		}
	}

	//TD 16335
	//zhou yuan++
	void AtsTisData::invalidPlatformData()
	{
		m_platformInfo.validity = 0;//it is CORBA::Boolean, better do like this
		m_platformInfoChanged =true;
	}
	//++zhou yuan


	void AtsTisData::setValid()
	{
		if ( false == m_platformInfo.validity )
		{
			m_platformInfo.validity = true;
			m_platformInfoChanged = true;
		}
	}

}
