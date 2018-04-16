/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\TrainDataNotification.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for handling the activation and deactivation of Corba Servant 
  */

#include "bus/ats/ats_agent/CommonATS/src/DataSynch.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/exceptions/src/AtsAgentException.h"
#include "core/exceptions/src/FatalConfigurationException.h"
#include "core/utilities/src/RunParams.h"
#include "bus/ats/ats_agent/CommonATS/src/AgentOperationMode.h"
#include "bus/ats/ats_agent/IDL/src/IAtsPasCorbaDef.h"

namespace TA_IRS_Bus
{
	DataSynch::DataSynch(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: m_entityData(EntityData),
		 m_occUpdateProcessor(NULL),
		 m_dataUpdate(NULL)
	{
		FUNCTION_ENTRY("DataSynch");

		initialize();

		FUNCTION_EXIT;
	}

	DataSynch::~DataSynch()
	{
		FUNCTION_ENTRY("~DataSynch");

		deInitialize();

		FUNCTION_EXIT;
	}

	void DataSynch::initialize()
	{
		FUNCTION_ENTRY("initialize");

		//Check for the location type, the source of data synchronization is from Occ to stations
		if (TA_Base_Core::AtsAgentEntityData::Occ == m_entityData->getAgentAtsLocationType())
		{
			m_occUpdateProcessor = new OccUpdateProcessor(m_entityData->getLocation(),m_entityData->getAgentAtsLocationType());
		}
		else
		{
			// Create OCC ATS Agent named object
			try
			{
				//TODO: hard-code OCC location
				TA_Base_Core::CorbaNameList occAtsAgentCorbaName = 
					TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation(
					TA_Base_Core::AtsAgentEntityData::getStaticType(), "OCC",true);

				if (occAtsAgentCorbaName.size() != 1)
				{
					throw TA_Base_Core::AtsAgentException("Can not locate OCC ATS Agent");
				}

				m_occAtsAgent = new OccUpdateProcessor::AtsAgentNamedObject;

				if (m_occAtsAgent != NULL)
				{
					m_occAtsAgent->setCorbaName(occAtsAgentCorbaName[0]);
				}
			}
			catch (...)
			{
				TA_THROW(TA_Base_Core::FatalConfigurationException(
					"Configuration Error: Can not locate OCC ATS Agent"));
			}
		}

		// Register this object with the naming service
		this->activateServantWithName(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME) + TA_IRS_Bus::IAtsPasCorbaDef::PA_CORBA_NAME);

		FUNCTION_EXIT;
	}

	void DataSynch::deInitialize()
	{
		FUNCTION_ENTRY("initialize");

		if (NULL != m_occUpdateProcessor)
		{
			delete m_occUpdateProcessor;
			m_occUpdateProcessor = NULL;
		}

		this->deactivateServant();

		FUNCTION_EXIT;
	}

	void DataSynch::setDataUpdates(IDataUpdate* dataUpdate)
	{
		FUNCTION_ENTRY("setDataUpdates");

		m_dataUpdate = dataUpdate;

		FUNCTION_EXIT
	}
	
}