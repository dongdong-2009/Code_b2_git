/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3002_TIP/3002/transactive/app/ats/AtsAgent/src/AtsBasData.cpp $
  * @author:
  * @version:  $Revision: # $
  *
  * Last modification: $DateTime: $
  * Last modified by:  $Author:  $
  *
  * This represents the ATS BAS Data
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"

#include "bus/ats/ats_agent/CommonATS/src/AtsBasData.h"

namespace TA_IRS_Bus
{
	AtsBasData::AtsBasData(	TA_IRS_Bus::PhysicalTrainNumberType trainId,
                    		unsigned long entityKey,
                    		unsigned long locationKey )
	:
	m_trainId (trainId),
	m_commsMessageSender (NULL),
	m_entityKey (entityKey),
	m_locationKey (locationKey)
	{


		m_CongestionInfo.isInCongestionMode = false;
		m_CongestionInfo.stationId = 0;

		// get a handle to CommsMessageSender
		m_commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender ( TA_Base_Core::ATSAgentComms::Context );

        // send initial invalid data
        m_BasInfoChanged = true;
       

		processUpdates();
	};

	AtsBasData::~AtsBasData()
	{
		// clean up comms message sender
		if ( m_commsMessageSender != NULL )
		{
			delete m_commsMessageSender;
			m_commsMessageSender = NULL;
		}
	}

	TA_IRS_Bus::IAtsBasCorbaDef::AtsBasMessageCorbaDef AtsBasData::buildAtsBasMessage()
	{
		TA_IRS_Bus::IAtsBasCorbaDef::AtsBasMessageCorbaDef message;

		message.trainId = m_trainId;
		message.congestionInformation = m_CongestionInfo;
		return message;
	}

	void AtsBasData::importAtsBasMessage(const TA_IRS_Bus::IAtsBasCorbaDef::AtsBasMessageCorbaDef& basUpdate)
	{

		updateCongestionMode(m_CongestionInfo.isInCongestionMode);
		updateStationId(m_CongestionInfo.stationId);
	}



	void AtsBasData::updateCongestionMode(bool isInCongestion)
	{

		if (m_CongestionInfo.isInCongestionMode != isInCongestion)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "updateCongestionMode() - trainId %d isInCongestion, was %s now %s",
				m_trainId,
				m_CongestionInfo.isInCongestionMode?"set":"not set",
				isInCongestion?"set":"not set");

			m_CongestionInfo.isInCongestionMode = isInCongestion;

			m_BasInfoChanged = true;

		}
	}

	void AtsBasData::updateStationId(unsigned long currentStationId)
	{
		if (m_CongestionInfo.stationId != currentStationId)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                "updateStationId() - trainId %d stationId, was %d now %d",
				m_trainId,
				m_CongestionInfo.stationId,
				currentStationId);

			m_CongestionInfo.stationId = currentStationId;

			m_BasInfoChanged = true;
		}
	}



	bool AtsBasData::isBasInfoUpdated()
	{
		return m_BasInfoChanged;
	}

	void AtsBasData::processUpdates()
	{
        if (m_BasInfoChanged)
		{
			TA_IRS_Bus::IAtsBasCorbaDef::AtsBasMessageCorbaDef basMessage
				= buildAtsBasMessage();

			TA_IRS_Bus::IAtsBasCorbaDef::AtsBasMessageCorbaDef* basMessageCopy
				= new TA_IRS_Bus::IAtsBasCorbaDef::AtsBasMessageCorbaDef(basMessage);

			CORBA::Any data;
				data <<= basMessageCopy;

			m_commsMessageSender->sendCommsMessage ( TA_Base_Core::ATSAgentComms::AtsBasUpdate,
													m_entityKey,
													data,
													0,
													m_locationKey);

			//removed to reduce log
			/*LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
					"sendUpdates() - Info: AtsBasMessage sent - trainId=%d, entity=%d, location=%d, Congestionmode=%s, StationID=%d",
				m_trainId, m_entityKey, m_locationKey,
				(m_CongestionInfo.isInCongestionMode?"SET":"NOT SET"),
				(m_CongestionInfo.stationId));*/
			m_BasInfoChanged = false;
		}

	}





}
