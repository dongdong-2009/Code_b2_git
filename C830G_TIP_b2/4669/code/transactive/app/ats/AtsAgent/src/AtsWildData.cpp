/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/4669_Review_Branch/4669/transactive/app/ats/AtsAgent/src/AtsWildData.cpp $
  * @author:   Darren Sampson
  * @version:  $Revision: #5 $
  *
  * Last modification: $DateTime: 2011/06/13 12:14:52 $
  * Last mofified by:  $Author: bihui.luo $
  *
  * This represents the ATS Train Record
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <time.h>

#include "core/utilities/src/TAAssert.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"

#include "app/ats/AtsAgent/src/AddressUtil.h"
#include "app/ats/AtsAgent/src/AtsWildData.h"

namespace TA_IRS_App
{
	AtsWildData::AtsWildData(unsigned long entityKey, unsigned long locationKey, unsigned int recordNumber)
	: 
	m_recordNumber (recordNumber),
	m_wildInfoChanged (false),
	m_commsMessageSender (NULL),
	m_entityKey (entityKey), 
	m_locationKey (locationKey)
	{
		m_atsWildMessage.timeStamp = 0;
		m_atsWildMessage.physicalTrainNumber = 0;
		m_atsWildMessage.activeCabNumber = 0;
		m_atsWildMessage.direction = TA_IRS_Bus::IAtsWildCorbaDef::HeadCabDirectionUnknown;

		// get a handle to CommsMessageSender
		m_commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender ( TA_Base_Core::ATSAgentComms::Context );

	};

	AtsWildData::~AtsWildData()
	{
		// clean up comms message sender
		if ( m_commsMessageSender != NULL )
		{
			delete m_commsMessageSender;
			m_commsMessageSender = NULL;
		}
	}

	TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef AtsWildData::buildAtsWildMessage()
	{	
		return m_atsWildMessage;
	}

	void AtsWildData::updateTimestamp(TA_IRS_Bus::TimeStampType timeStamp)
	{
		struct tm when;
		
		when.tm_year= timeStamp.year - 1900;
		when.tm_mon	= timeStamp.month - 1;
		when.tm_mday= timeStamp.day;
		when.tm_hour= timeStamp.hour;
		when.tm_min	= timeStamp.minute;
		when.tm_sec	= timeStamp.second;
		when.tm_isdst = -1 ;  //TD15784 prevent solaris lost 1 hour.

		time_t timetTimeStamp = mktime(&when);

		if (timetTimeStamp != m_atsWildMessage.timeStamp)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"updateTimestamp() - record %d timestamp, was %d now %d", 
						m_recordNumber, m_atsWildMessage.timeStamp, timetTimeStamp);

			m_atsWildMessage.timeStamp = timetTimeStamp;

			m_wildInfoChanged = true;
		}
	}

	void AtsWildData::updatePhysicalTrainNumber(TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber)
	{
		if (physicalTrainNumber != m_atsWildMessage.physicalTrainNumber)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"updateTimestamp() - record %d physicalTrainNumber, was %d now %d", 
						m_recordNumber, m_atsWildMessage.physicalTrainNumber, physicalTrainNumber);

			m_atsWildMessage.physicalTrainNumber = physicalTrainNumber;
			m_wildInfoChanged = true;
		}
	}

	void AtsWildData::updateActiveCabNumber(TA_IRS_Bus::IAtsWildCorbaDef::ActiveCabNumberType activeCabNumber, TA_IRS_Bus::PhysicalTrainNumberType physicalTrainNumber)
	{
		if (activeCabNumber !=m_atsWildMessage.activeCabNumber)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"updateActiveCabNumber() - record %d activeCabNumber, was %d now %d", 
						m_recordNumber, m_atsWildMessage.activeCabNumber, activeCabNumber);
			if ( activeCabNumber >=64 && activeCabNumber <= 252)
			{
				if(activeCabNumber%2 == 1)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
						"updateActiveCabNumber() - activeCabNumber %d does not match the trainId  %d " , activeCabNumber,physicalTrainNumber);
						return;
				}
				else
				{
					if ( (activeCabNumber - 60)/4 != physicalTrainNumber )
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
							"updateActiveCabNumber() - activeCabNumber %d does not match the trainId  %d " , activeCabNumber,physicalTrainNumber);

							return;
					}
				}
				m_atsWildMessage.isRevenue = false;
			}
			else if ( activeCabNumber >= 256 )
			{
				if (activeCabNumber%2 == 1 )
				{
					if (( activeCabNumber- 253 )/4 != physicalTrainNumber)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
							"updateActiveCabNumber() - activeCabNumber %d does not match the trainId  %d " , activeCabNumber,physicalTrainNumber);
							return;
					}
				}
				else
				{
					if ( ( activeCabNumber- 252 )/4 != physicalTrainNumber )
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
							"updateActiveCabNumber() - activeCabNumber %d does not match the trainId  %d " , activeCabNumber,physicalTrainNumber);
							return;
					}
				}
				m_atsWildMessage.isRevenue = true;
			}
			m_atsWildMessage.activeCabNumber = activeCabNumber;
			m_wildInfoChanged = true;
		}
	}

	void AtsWildData::updateHeadCabDirection(unsigned char headCabDirectionByte)
	{
		TA_IRS_Bus::IAtsWildCorbaDef::EHeadCabDirection headCabDirection 
			= TA_IRS_Bus::IAtsWildCorbaDef::EHeadCabDirection(headCabDirectionByte);
		if (headCabDirection > TA_IRS_Bus::IAtsWildCorbaDef::HeadCabDirectionUp)
		{
			headCabDirection = TA_IRS_Bus::IAtsWildCorbaDef::HeadCabDirectionUnknown;
		}
		if (m_atsWildMessage.direction != headCabDirection)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
						"updateHeadCabDirection() - record %d headCabDirection, was %d now %d", 
						m_recordNumber, m_atsWildMessage.direction, headCabDirection);

			m_atsWildMessage.direction = headCabDirection;
		}
	}
	
	void AtsWildData::sendUpdates()
	{
		//
		// Process notifications
		//

		if (m_wildInfoChanged)
		{
			//TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef* wildMessage 
			//	= new TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef(buildAtsWildMessage());

			TA_IRS_Bus::IAtsWildCorbaDef::AtsWildMessageCorbaDef wildMessage(buildAtsWildMessage());

			CORBA::Any data;
			data <<= wildMessage;

			m_commsMessageSender->sendCommsMessage ( TA_Base_Core::ATSAgentComms::WILDTrainUpdate,
													m_entityKey,
													data,
													0,
													m_locationKey);

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"sendUpdates() - Info: AtsWildMessage sent - record=%d, entity=%d, location=%d, train=%d, timestamp=%d",
				m_recordNumber, m_entityKey, m_locationKey,
				m_atsWildMessage.physicalTrainNumber,
				m_atsWildMessage.timeStamp);

			m_wildInfoChanged = false;
		}
	}
}
