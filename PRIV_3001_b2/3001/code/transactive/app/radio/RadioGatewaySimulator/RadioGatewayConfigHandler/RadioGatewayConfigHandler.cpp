#include "StdAfx.h"
#include "RadioGatewayConfigHandler.h"


#include <vector>
#define	RA_SUBSCRIBER_SECTION		"SUBSCRIBER"


CRadioGatewayConfigHandler::CRadioGatewayConfigHandler(void)
{
	
	m_radioStrategyFactory = new RadioStratFactory ();

	// prepare strategy
	m_mapStrategyCollection[ERadioSubsc] = m_radioStrategyFactory->createRadioStrategy(ERadioSubsc);
	m_mapStrategyCollection[ERadioProfileRequest] = m_radioStrategyFactory->createRadioStrategy(ERadioProfileRequest);
	m_mapStrategyCollection[ERadioProfileDBUpdate] = m_radioStrategyFactory->createRadioStrategy(ERadioProfileDBUpdate);
	m_mapStrategyCollection[ERadioSystemStatus] = m_radioStrategyFactory->createRadioStrategy(ERadioSystemStatus);
	m_mapStrategyCollection[ERadioDGNAStatus] = m_radioStrategyFactory->createRadioStrategy(ERadioDGNAStatus);
	m_mapStrategyCollection[ERadioQueryCallForwarding] = m_radioStrategyFactory->createRadioStrategy(ERadioQueryCallForwarding);
}

CRadioGatewayConfigHandler::~CRadioGatewayConfigHandler(void)
{
	if (m_radioStrategyFactory)
	{
		delete m_radioStrategyFactory;
		m_radioStrategyFactory = NULL;
	}

	if (FALSE == m_mapStrategyCollection.empty())
	{
		StrategyCollectionType::iterator iterItems = m_mapStrategyCollection.begin();

		for (; iterItems!= m_mapStrategyCollection.end(); iterItems++)
		{
			delete iterItems->second;
			iterItems->second = NULL;
		}

		m_mapStrategyCollection.clear();
	}
}


RadioSubscriberData CRadioGatewayConfigHandler::getRadioSubscriberData (unsigned long resourceId)
{
	RadioSubscriberData radioData;
	RadioSubscriberResultData * resultData;
	StrategyCollectionType::iterator iterFind = m_mapStrategyCollection.find(ERadioSubsc);
	
	if (iterFind != m_mapStrategyCollection.end())
	{
		RadioSubscriberParamData paramData (resourceId);
		
		resultData = (RadioSubscriberResultData *)iterFind->second->executeRadioConfigStrategy(paramData);	
		
		radioData = resultData->getRadioSubscriberData();

		iterFind->second->clearResultData(resultData);
	}

	return radioData;
}

ProfileTalkGroupListType CRadioGatewayConfigHandler::getTalkGroupsFromProfile (std::string strProfile)
{
	RadioProfileRequestResultData * resultData;
	ProfileTalkGroupListType profileForTG;
	StrategyCollectionType::iterator iterFind = m_mapStrategyCollection.find(ERadioProfileRequest);

	if (iterFind != m_mapStrategyCollection.end())
	{
		RadioMonitorProfileRequestParamData paramData (strProfile);
		
		resultData = (RadioProfileRequestResultData *)iterFind->second->executeRadioConfigStrategy(paramData);	
		profileForTG = resultData->getProfileTalkGroup();

		iterFind->second->clearResultData(resultData);
	}

	return profileForTG;
}

ProfileDBUpdateCollectionType CRadioGatewayConfigHandler::getProfileDBUpdate ()
{
	
	RadioProfileDBUpdateResultData * resultData;
	ProfileDBUpdateCollectionType resultItem;

	StrategyCollectionType::iterator iterFind = m_mapStrategyCollection.find(ERadioProfileDBUpdate);
	

	if (iterFind != m_mapStrategyCollection.end())
	{
		RadioMonitorProfileRequestParamData paramData ("SO");

		resultData = (RadioProfileDBUpdateResultData *)iterFind->second->executeRadioConfigStrategy(paramData);	

		resultItem = resultData->getProfileDBUpdate();

		iterFind->second->clearResultData(resultData);
	}

	return resultItem;
}

unsigned long CRadioGatewayConfigHandler::getRadioSystemStatus ()
{
	RadioSystemStatusResultData * resultData;
	unsigned long resultItem;
	StrategyCollectionType::iterator iterFind = m_mapStrategyCollection.find(ERadioSystemStatus);


	if (iterFind != m_mapStrategyCollection.end())
	{
		RadioMonitorProfileRequestParamData paramData ("SO");

		resultData = (RadioSystemStatusResultData *)iterFind->second->executeRadioConfigStrategy(paramData);	

		resultItem = resultData->getRadioStatus();

		iterFind->second->clearResultData(resultData);
	} 

	return resultItem;
}

unsigned long CRadioGatewayConfigHandler::getDGNARequestStatus ()
{
	RadioDGNAStatusResultData * resultData;
	unsigned long resultItem;
	
	
	StrategyCollectionType::iterator iterFind = m_mapStrategyCollection.find(ERadioDGNAStatus);
	if (iterFind != m_mapStrategyCollection.end())
	{
		RadioMonitorProfileRequestParamData paramData ("SO");

		resultData = (RadioDGNAStatusResultData *)iterFind->second->executeRadioConfigStrategy(paramData);	

		resultItem = resultData->getDGNAStatus();

		iterFind->second->clearResultData(resultData);
	} 

	return resultItem;
}

RadioCallForwardingQueryData CRadioGatewayConfigHandler::getQueryCallForwardingData ()
{
	RadioQueryCallForwadingResultData * resultData;
	RadioCallForwardingQueryData resultItem;


	StrategyCollectionType::iterator iterFind = m_mapStrategyCollection.find(ERadioQueryCallForwarding);
	if (iterFind != m_mapStrategyCollection.end())
	{
		RadioMonitorProfileRequestParamData paramData ("SO");

		resultData = (RadioQueryCallForwadingResultData *)iterFind->second->executeRadioConfigStrategy(paramData);	

		resultItem = resultData->getRadioCallForwardingQueryData ();

		iterFind->second->clearResultData(resultData);
	} 

	return resultItem;
}