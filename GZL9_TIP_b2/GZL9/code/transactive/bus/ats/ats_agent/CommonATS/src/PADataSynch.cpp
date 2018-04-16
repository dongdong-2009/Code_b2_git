/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: \\depot\FZL1_TIP\FZL1\transactive\bus\ats\ats_agent\CommonATS\src\PADataSynch.h $
  * @author:   Lucky Cuizon
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/03/9 11:17:43 $
  * Last modified by:  $Author: Lucky Cuizon $
  *
  * This object is responsible for PA Data Synchronization from OCC to station
  */

#include "bus/ats/ats_agent/CommonATS/src/PADataSynch.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/ats/ats_agent/CommonATS/src/AtsPasData.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/exceptions/src/AtsAgentException.h"
#include "bus/ats/ats_agent/IDL/src/IAtsPasCorbaDef.h"
#include "core/exceptions/IDL/src/CommonExceptionsCorbaDef.h"
#include "bus/ats/ats_agent/CommonATS/src/DataCache.h"
#include "bus/ats/ats_agent/CommonATS/src/ATSDefinitions.h"
#include "bus/ats/ats_agent/CommonATS/src/AgentOperationMode.h"
#include <vector>
#include <string>


namespace TA_IRS_Bus
{
	PADataSynch::PADataSynch(TA_Base_Core::AtsAgentEntityDataPtr EntityData)
		: DataSynch(EntityData)
	{
		FUNCTION_ENTRY("PADataSynch");

		FUNCTION_EXIT;
	}

	PADataSynch::~PADataSynch()
	{
		FUNCTION_ENTRY("~PADataSynch");

		FUNCTION_EXIT;
	}
	void PADataSynch::processDataSync()
	{
		FUNCTION_ENTRY("processDataSync");

		PasDataMap& pasDataMap = DataCache::getInstance()->getPasDataMap();
		PasDataMapIt pasIt = pasDataMap.begin();

		for (pasIt; pasIt != pasDataMap.end(); pasIt++)
		{
			if ((*pasIt).second != NULL)
			{
				TA_Base_Core::ThreadGuard guard(*(*pasIt).second);

				if (m_occUpdateProcessor != NULL &&
					m_entityData->getAgentAtsLocationType() == TA_Base_Core::AtsAgentEntityData::Occ)
				{
					std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef>& pasMessageVector = 
						(*pasIt).second->getPasMessageVector();

					std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef>::iterator pasMessageIt;

					for (pasMessageIt = pasMessageVector.begin(); pasMessageIt != pasMessageVector.end(); pasMessageIt++)
					{
						m_occUpdateProcessor->addPasUpdate(*pasMessageIt);
					}
					(*pasIt).second->clearPasMessageVector();
				}
			}
		}

		m_occUpdateProcessor->sendOccUpdates();

		FUNCTION_EXIT;
	}

	void PADataSynch::processOccPasUpdate(const TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList& pasUpdate)
	{
		FUNCTION_ENTRY("processOccPasUpdate");

		if (false == TA_IRS_Bus::AgentOperationMode::getInstance().isInControlMode())
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Agent is not running in Control mode.");
			throw TA_Base_Core::OperationModeException();
			//return;
		}

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,"Synchronize local data.");
		localProcessOccPasUpdate(pasUpdate);

		//Send local Updates for PA Data. calls to DataProcessorManager.
		m_dataUpdate->processDataDispatcher(ePA);

		FUNCTION_EXIT;
	}

	void PADataSynch::localProcessOccPasUpdate(const TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList& pasUpdate)
	{
		FUNCTION_ENTRY("localProcessOccPasUpdate");

		for(int count=0; count < pasUpdate.length(); count++)
		{
			try
			{
				//Synchronize local data
				TA_IRS_Bus::AtsPasData& pasData = DataCache::getInstance()->getPasData(pasUpdate[count].platformId);
				TA_Base_Core::ThreadGuard guard(pasData);
				/*reset AtsPasMessgae to 0 before importing with the new message update,
				so that, if a messgae is lost, the subsequent message can be broadcasted correctly*/
				pasData.resetAtsPasMessage(pasUpdate[count]);
				pasData.importAtsPasMessage(pasUpdate[count]);
			}
			catch (TA_Base_Core::AtsAgentException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "AtsAgentException", e.what());
			}
		}

		FUNCTION_EXIT;
	}

	 TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList* PADataSynch::getPasInformationList()
	 {
		 FUNCTION_ENTRY("getPasInformationList");

		 if (false == TA_IRS_Bus::AgentOperationMode::getInstance().isInControlMode() )
		{
				std::string message = "Agent in Monitor Mode";
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"getPasInformationList() - %s",
					message.c_str());
				FUNCTION_EXIT;

				throw TA_Base_Core::OperationModeException(message.c_str());
		 }

		 std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef> pasVector;
		 PasDataMap& m_pasDataMap = DataCache::getInstance()->getPasDataMap();
		 PasDataMapIt it;

		 for (it=m_pasDataMap.begin(); it != m_pasDataMap.end(); it++)
		 {
			 TA_Base_Core::ThreadGuard guard(*(*it).second);

			 std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef> pasMessageVector = 
				 (*(*it).second).buildAtsPasMessageState();

			 std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef>::iterator pasMessageIt;

			 for (pasMessageIt = pasMessageVector.begin(); pasMessageIt != pasMessageVector.end(); pasMessageIt++)
			 {
				 pasVector.push_back(*pasMessageIt);
			 }
		 }

		 TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList* pasList = new TA_IRS_Bus::IAtsPasCorbaDef::PasInfoList;
		 pasList->length(pasVector.size());

		 std::vector<TA_IRS_Bus::IAtsPasCorbaDef::AtsPasMessageCorbaDef>::iterator tvIt;
		 unsigned int i = 0;

		 for (tvIt = pasVector.begin(), i = 0; tvIt != pasVector.end(); tvIt++, i++)
		 {
			 (*pasList)[i] = *tvIt;
		 }

		 LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			 "getPasInformationList() - %d AtsPasMessageCorbaDef records retrieved",
			 pasVector.size());

		 FUNCTION_EXIT;
		 return pasList;
	 }
}