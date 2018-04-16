/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution in any form.
*
* Source : $File: //depot/3002_TIP/3002/transactive/app/ats/AtsAgent/src/AtsPidsData.cpp $
* @author: shankri
* @version:  $Revision: #3 $
*
* Last modification: $DateTime: $
* Last modified by:  $Author: shankri $
*
* This represents the ATS PIDS Data
*/

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"

#include "bus/ats/ats_agent/CommonATS/src/AtsPidsData.h"

namespace TA_IRS_Bus
{
	AtsPidsData::AtsPidsData( TA_IRS_Bus::PlatformIdType platformId,unsigned long stationId,
		unsigned long entityKey,unsigned long locationKey )
		:
	m_platformId (platformId),
		m_stationId(stationId),
		m_entityKey (entityKey),
		m_locationKey (locationKey)
	{
		m_platformInfo.nTimeToNextTrain = 0;
		m_platformInfo.nDeptTime = 0;
		m_platformInfo.nDwellTime = 0;
		m_platformInfo.nDestStationId = 0;
		m_platformInfo.bSkip = 1; // skip =1, arrival = 0
		m_platformInfo.nTimeTo2NextTrain = 0;
		m_platformInfo.nDeptTime2 = 0;
		m_platformInfo.nDwellTime2 = 0;
		m_platformInfo.nDestStationId2 = 0;
		m_platformInfo.bSkip2 = 1; // skip =1, arrival = 0

		// get a handle to CommsMessageSender
		m_commsMessageSender =
			TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender ( TA_Base_Core::ATSAgentComms::Context );

		// send initial invalid data
		m_pidsInfoChanged = true;       

		processUpdates();
	};

	AtsPidsData::~AtsPidsData()
	{
		// clean up comms message sender
		if ( m_commsMessageSender != NULL )
		{
		delete m_commsMessageSender;
		m_commsMessageSender = NULL;
		}
	}

	TA_IRS_Bus::IAtsPidsCorbaDef::AtsPidsMessageCorbaDef AtsPidsData::buildAtsPidsMessage()
	{
		TA_IRS_Bus::IAtsPidsCorbaDef::AtsPidsMessageCorbaDef message;

		message.platformId = m_platformId;
		message.stationId = m_stationId;
		message.platformInformation = m_platformInfo;
		return message;
	}

	void AtsPidsData::importAtsPidsMessage(const TA_IRS_Bus::IAtsPidsCorbaDef::AtsPidsMessageCorbaDef& pidsUpdate)
	{
		updateTimeNxtTrain(m_platformInfo.nTimeToNextTrain);
		updateDeptTime(m_platformInfo.nDeptTime);
		updateDwellTime(m_platformInfo.nDwellTime);
		updateDestStationId(m_platformInfo.nDestStationId);
		updateSkip(m_platformInfo.bSkip);
		updateTime2NxtTrain(m_platformInfo.nTimeTo2NextTrain);
		update2DeptTime(m_platformInfo.nDeptTime2);
		update2DwellTime(m_platformInfo.nDwellTime2);
		update2DestStationId(m_platformInfo.nDestStationId2);
		update2Skip(m_platformInfo.bSkip2);

	}

	void AtsPidsData::updateTimeNxtTrain(TA_IRS_Bus::TimeTTimeStampType time2NxtTrain)
	{

		if (m_platformInfo.nTimeToNextTrain != time2NxtTrain)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"updateTimeNxtTrain() - platformId %d stationId %d nTimeToNextTrain, was %lu now %lu",
				m_platformId,
				m_stationId,
				m_platformInfo.nTimeToNextTrain,
				time2NxtTrain);

			m_platformInfo.nTimeToNextTrain = time2NxtTrain;

			m_pidsInfoChanged = true;

		}
	}

	void AtsPidsData::updateDeptTime(TA_IRS_Bus::TimeTTimeStampType nDeptTime)
	{
		if (m_platformInfo.nDeptTime != nDeptTime)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"updateDeptTime() - platformId %d stationId %d nDeptTime, was %lu now %lu",
				m_platformId,
				m_stationId,
				m_platformInfo.nDeptTime,
				nDeptTime);

			m_platformInfo.nDeptTime = nDeptTime;

			m_pidsInfoChanged = true;

		}
	}

	void AtsPidsData::updateDwellTime(TA_IRS_Bus::TimeTTimeStampType dwelltime)
	{

		if (m_platformInfo.nDwellTime != dwelltime)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"updateDwellTime() - platformId %d stationId %d nDwellTime, was %lu now %lu",
				m_platformId,
				m_stationId,
				m_platformInfo.nDwellTime,
				dwelltime);

			m_platformInfo.nDwellTime = dwelltime;

			m_pidsInfoChanged = true;

		}
	}

	void AtsPidsData::updateDestStationId(unsigned long destId)
	{

		if (m_platformInfo.nDestStationId != destId)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"updateDestStationId() - platformId %d stationId %d nDestStationId, was %lu now %lu",
				m_platformId,
				m_stationId,
				m_platformInfo.nDestStationId,
				destId);

			m_platformInfo.nDestStationId = destId;

			m_pidsInfoChanged = true;

		}
	}

	void AtsPidsData::updateSkip(bool bSkip )
	{
		if (m_platformInfo.bSkip != bSkip)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"updateSkipInfo() - platformId %d stationId %d Skip Information, was %lu now %lu",
				m_platformId,
				m_stationId,
				m_platformInfo.bSkip,
				bSkip);

			m_platformInfo.bSkip = bSkip;

			m_pidsInfoChanged = true;

		}

	}


	void AtsPidsData::updateTime2NxtTrain(TA_IRS_Bus::TimeTTimeStampType time2NxtTrain)
	{

		if (m_platformInfo.nTimeTo2NextTrain != time2NxtTrain)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"updateTime2NxtTrain() - platformId %d stationId %d nTimeToNext2Train, was %lu now %lu",
				m_platformId,
				m_stationId,
				m_platformInfo.nTimeTo2NextTrain,
				time2NxtTrain);

			m_platformInfo.nTimeTo2NextTrain = time2NxtTrain;

			m_pidsInfoChanged = true;

		}
	}

	void AtsPidsData::update2DeptTime(TA_IRS_Bus::TimeTTimeStampType nDeptTime2)
	{
		if (m_platformInfo.nDeptTime2 != nDeptTime2)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"update2DeptTime() - platformId %d stationId %d nDeptTime2, was %lu now %lu",
				m_platformId,
				m_stationId,
				m_platformInfo.nDeptTime2,
				nDeptTime2);

			m_platformInfo.nDeptTime2 = nDeptTime2;

			m_pidsInfoChanged = true;

		}
	}

	void AtsPidsData::update2DwellTime(TA_IRS_Bus::TimeTTimeStampType nDwelltime2)
	{

		if (m_platformInfo.nDwellTime2 != nDwelltime2)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"update2DwellTime() - platformId %d stationId %d nDwelltime2, was %lu now %lu",
				m_platformId,
				m_stationId,
				m_platformInfo.nDwellTime2,
				nDwelltime2);

			m_platformInfo.nDwellTime2 = nDwelltime2;

			m_pidsInfoChanged = true;

		}
	}

	void AtsPidsData::update2DestStationId(unsigned long nDestId2)
	{

		if (m_platformInfo.nDestStationId2 != nDestId2)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"update2DestStationId() - platformId %d stationId %d nDestStationId2, was %lu now %lu",
				m_platformId,
				m_stationId,
				m_platformInfo.nDestStationId2,
				nDestId2);

			m_platformInfo.nDestStationId2 = nDestId2;

			m_pidsInfoChanged = true;

		}
	}

	void AtsPidsData::update2Skip(bool bSkip2 )
	{
		if (m_platformInfo.bSkip2 != bSkip2)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"update2Skip() - platformId %d stationId %d Skip Information for second next train, was %lu now %lu",
				m_platformId,
				m_stationId,
				m_platformInfo.bSkip2,
				bSkip2);

			m_platformInfo.bSkip2 = bSkip2;

			m_pidsInfoChanged = true;

		}

	}


	bool AtsPidsData::isPidsInfoUpdated()
	{
		return m_pidsInfoChanged;
	}

	void AtsPidsData::processUpdates()
	{
		if (m_pidsInfoChanged)
		{
		TA_IRS_Bus::IAtsPidsCorbaDef::AtsPidsMessageCorbaDef pidsMessage
		= buildAtsPidsMessage();

		TA_IRS_Bus::IAtsPidsCorbaDef::AtsPidsMessageCorbaDef* pidsMessageCopy
		= new TA_IRS_Bus::IAtsPidsCorbaDef::AtsPidsMessageCorbaDef(pidsMessage);

		CORBA::Any data;
		data <<= pidsMessageCopy;

		m_commsMessageSender->sendCommsMessage ( TA_Base_Core::ATSAgentComms::AtsPidsUpdate,
		m_entityKey,
		data,
		0,
		m_locationKey);

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
		"sendUpdates() - Info: AtsPidsMessage sent - PlatformId=%d, StationId=%d, entity=%d, location=%d, nTimeToNextTrain=%lu, nDwellTime=%d,nDestStationId=%d",
		m_platformId,m_stationId, m_entityKey, m_locationKey,
		m_platformInfo.nTimeToNextTrain,
		m_platformInfo.nDwellTime,
		m_platformInfo.nDestStationId);
		m_pidsInfoChanged = false;
		}

	}

}
