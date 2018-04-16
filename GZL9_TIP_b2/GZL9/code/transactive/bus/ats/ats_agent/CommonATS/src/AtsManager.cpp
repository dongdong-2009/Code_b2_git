/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\AtsManager.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object handles and manages the initialization of protocol handlers.
  * It is abstract class that must be implemented by the protocol specific. 
  * Example: ModBusProtocol, SignallingProtocol, SOAPProtocol
  */

#include "bus/ats/ats_agent/CommonATS/src/AtsManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/generic_agent/src/IEntity.h"
#include "core/exceptions/src/AtsAgentException.h"
#include "core/exceptions/src/FatalConfigurationException.h"

#include "bus/ats/ats_agent/CommonATS/src/IProtocolManager.h"
#include "bus/ats/ats_agent/CommonATS/src/IAtsDataNotification.h"
#include "bus/ats/ats_agent/CommonATS/src/IDataProcessor.h"

#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/TrainDataLocation.h"
#include "bus/ats/ats_agent/IDL/src/IAtsTrainCorbaDef.h"
#include "core/utilities/src/RunParams.h"

namespace TA_IRS_Bus
{
	AtsManager::AtsManager()
		: m_protocolManager(NULL),
		  m_dataNotification(NULL),
		  m_DataProcessorManager(NULL)
	{
		FUNCTION_ENTRY("AtsManager");

		FUNCTION_EXIT;
	}

	AtsManager::~AtsManager()
	{
		FUNCTION_ENTRY("~AtsManager");

		if (NULL != m_protocolManager)
		{
			delete m_protocolManager;
			m_protocolManager = NULL;
		}

		if (NULL != m_dataNotification )
		{
			delete m_dataNotification;
			m_dataNotification = NULL;
		}

		if (NULL != m_DataProcessorManager)
		{
			delete m_DataProcessorManager;
			m_DataProcessorManager = NULL;
		}

		deactivateAndDeleteServant();
		FUNCTION_EXIT;
	}

	void AtsManager::initializeAtsEntity(TA_Base_Core::IEntityDataPtr entityData)
	{
		FUNCTION_ENTRY("setAtsEntity");

		try
		{
			m_atsEntityData = boost::dynamic_pointer_cast<TA_Base_Core::AtsAgentEntityData>(entityData);
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Ats Agent Entity Data can not be retrieved");
		}

		if (m_atsEntityData.get() == NULL)
		{
			TA_THROW(TA_Base_Core::AtsAgentException("Error: Ats Agent Entity Data can not be retrieved"));
		}

		// Find the ATS location type
		TA_Base_Core::AtsAgentEntityData::EAtsLocationType locationType = m_atsEntityData->getAgentAtsLocationType();

		if (locationType != TA_Base_Core::AtsAgentEntityData::Occ && 
			locationType == TA_Base_Core::AtsAgentEntityData::Depot &&
			locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation1 && 
			locationType == TA_Base_Core::AtsAgentEntityData::SecondaryStation2 &&
			locationType == TA_Base_Core::AtsAgentEntityData::MainStation)
		{
			TA_THROW(TA_Base_Core::FatalConfigurationException(
				"Configuration Error: Not a known location"));
		}

		m_DataProcessorManager = createDataProcessor();
		m_dataNotification = createDataNotification();
		m_protocolManager = createProtocolManager();
		initializeData();

		activateServantWithName(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME));
		FUNCTION_EXIT;
	}

	TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList* AtsManager::getTrainInformationList()
	{
		FUNCTION_ENTRY("AtsManager::getTrainInformationList");
		TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList* pRetVal = new TA_IRS_Bus::IAtsTrainCorbaDef::TrainInfoList();
		std::vector<TrainInfo*> pTrainDataList = TA_IRS_Bus::DataCache::getInstance()->getTrainData()->getTrainAtsInfoCacheList();

		pRetVal->length(pTrainDataList.size());
		for (int idx=0; idx<pTrainDataList.size(); idx++)
		{
			TrainInfo* objTemp = pTrainDataList[idx];
			TA_IRS_Bus::IAtsTrainCorbaDef::AtsTrainMessageCorbaDef newData;
			newData.physicalTrainNumber = CORBA::string_dup(objTemp->TrainID.c_str());
			newData.trainValid = true;
			newData.localisation.localisationValid;
			newData.localisation.physicalTrainService = CORBA::string_dup(objTemp->TrainServiceNumber.c_str());
			newData.localisation.currentLocation = 0;
			newData.localisation.lastLocation = 0;		
			newData.localisation.currentCctvZone = 0;	
			
			// Use genaTrainDirection for GZL9 project
			if (objTemp->genaTrainDirection == 0)		//UPTRACK
				newData.localisation.upDirection = TA_IRS_Bus::up;
			else if (objTemp->genaTrainDirection == 1)	//DOWNTRACK
				newData.localisation.upDirection = TA_IRS_Bus::down;
			else if (objTemp->genaTrainDirection == 2)	//UNKNOWN
				newData.localisation.upDirection = TA_IRS_Bus::Unknown;

			newData.localisation.track = TA_IRS_Bus::TrackUnknown;				
			newData.localisation.isMute = false;				
			newData.localisation.TrainTrackLocation = CORBA::string_dup(objTemp->TrainTrackLocation.c_str());
			newData.localisation.TrainCongestion = objTemp->TrainCongestion;     
			
			(*pRetVal)[idx] =newData;
		}

		FUNCTION_EXIT;
		return pRetVal;
	}	
}
