/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/StationModeManager.cpp $
  * @author:	Raymond Pau
  * @version:	$Revision: #2 $
  *
  * Last modification:	$DateTime: 2016/01/25 10:34:25 $
  * Last modified by:	$Author: qi.huang $
  *
  * Description:
  *
  *
  */
#include "app/bas/BASAgent/src/StationModeManager.h"
#include "core/data_access_interface/bas/src/SubsystemAccessFactory.h"
#include "app/bas/BASAgent/src/BasCachedConfig.h"
#include "bus/modbus_comms/src/DataBlock.h"
#include "bus/scada/ModbusHelper/src/ModbusHelperFactory.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "app/bas/BASAgent/src/ModeExecutionException.h"
#include "app/bas/BASAgent/src/BASAgent.h"
#include "app/bas/BASAgent/src/FireTriggerBasEquipmentManager.h"

namespace TA_IRS_App
{
    StationModeManager::StationModeManager()
		: m_fireTriggerManager(NULL)
	{
		FUNCTION_ENTRY("StationModeManager");

		createModeEquipments();
		
		m_fireTriggerManager = new FireTriggerBasEquipmentManager(this);
		m_fireTriggerManager->initializeManager();

		FUNCTION_EXIT;
	}

	StationModeManager::~StationModeManager()
	{
		FUNCTION_ENTRY("~StationModeManager");

		if(NULL != m_fireTriggerManager)
		{
			m_fireTriggerManager->uninitializeManager();
		}

		if (m_modeEquipments.size() != 0)
		{
			std::map< unsigned long, BasModeEquipment* >::iterator iter = m_modeEquipments.begin();
			for (; iter != m_modeEquipments.end(); ++iter)
			{
				delete iter->second;
			}
		}



		FUNCTION_EXIT;
	}

	void StationModeManager::createModeEquipments()
	{
		FUNCTION_ENTRY("createModeEquipments");
		
        m_modeEquipments = TA_IRS_App::BasModeEquipmentFactory::getInstance().getAllBasModeEquipments();

		//TA_IRS_Core::AllLocationSubsystems allSubsystems;
		//std::vector< SubSystem_SmartPtr > subsystems;
		//if(false != TA_IRS_Core::SubsystemAccessFactory::getInstance().getAllSubsystems(allSubsystems))
		//{
		//	subsystems = allSubsystems[m_locationKey];
		//	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Found %d Subsystems for location %d, contain the system config items", subsystems.size(), m_locationKey);
		//	std::vector< SubSystem_SmartPtr >::iterator subIter = subsystems.begin();

		//	for (; subIter != subsystems.end(); ++subIter)
		//	{
		//		if ((*subIter)->getAssetName() != "" && !(*subIter)->getIsSystem())
		//		{
		//			m_modeEquipments[(*subIter)->getKey()] = new BasModeEquipment(m_stationName + (*subIter)->getAssetName());
		//		}
		//		else
		//		{
		//			LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, 
		//				"can not get mode subsystems Asset name from DB!, BA_SUBSYSTEM PKEY: %lu", (*subIter)->getKey());
		//		}
		//	}
		//}
		//else
		//{
		//	LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal, "can not load mode subsystems from DB!");
		//	TA_ASSERT(false, "BA_SUBSYSTEM can not be loaded!!!");
		//}

		FUNCTION_EXIT;
	}

	void StationModeManager::executeStationMode(CORBA::Long subsystemKey, CORBA::Long stationMode, CORBA::Boolean canOverrideStationModeInProgress, const char* sessionId)
	{
		FUNCTION_ENTRY("executeStationMode");


		TA_THREADGUARD(m_staionModeLock);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"executeStationMode: [subsystemKey:%lu], [stationMode:%lu], [sessionId:%s]", 
			subsystemKey, stationMode, sessionId);

		std::map<unsigned long, BasModeEquipment*>::iterator iter = m_modeEquipments.find(subsystemKey);

		if (iter != m_modeEquipments.end())
		{
			try
			{
				(iter->second)->executeStationMode(stationMode, canOverrideStationModeInProgress, sessionId);
			}
			catch (const ModeExecutionException& e)
			{
				if (e.getFailureType() == ModeExecutionException::MODE_NOT_IN_SINGLE_CONTROL)
				{
					throw(TA_IRS_Bus::IStationBasAgentCorbaDef::StationInLocalMode(e.what()));
				}
				else if (e.getFailureType() == ModeExecutionException::MODE_IN_PROGRESS)
				{
					throw(TA_IRS_Bus::IStationBasAgentCorbaDef::StationModeInProgress(stationMode, e.what()));
				}
			}
			catch (const TA_Base_Core::TransactiveException& e)
			{
				throw (TA_Base_Core::NotAvailableException(e.what()));
			}
			catch (...)
			{
				//other exception throw to client directly
                throw;
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No subsystem found, key: %lu", subsystemKey);
			// TODO throw to client
			throw (TA_Base_Core::NotAvailableException("No subsystem found."));
		}
		FUNCTION_EXIT;
	}


	unsigned long StationModeManager::getCurrentModeValue(unsigned long subsystemKey)
	{
		TA_THREADGUARD(m_staionModeLock);

		std::map<unsigned long, BasModeEquipment*>::iterator iter = m_modeEquipments.find(subsystemKey);
		unsigned long ulModeValue = 0;
		bool bGetModeValue = false;

		if (iter != m_modeEquipments.end())
		{
			try
			{
				ulModeValue = (iter->second)->getCurrentModeValue();
				bGetModeValue = true;
			}
			catch (const TA_Base_Core::TransactiveException& e)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", e.what());
			}
			catch(...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "UnknownException when get Subsystem mode Value.");
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No subsystem found, key: %lu", subsystemKey);
			// TODO throw to client
		}

		if (!bGetModeValue)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Cannot get current mode value, subsystemKey= %lu", subsystemKey);
			TA_THROW(TA_Base_Core::TransactiveException("Cannot get current mode value"));
		}
        
		return ulModeValue;		
	}

    bool StationModeManager::isAgentRunningControlMode()
    {
        return m_controlMode;
    }

    void StationModeManager::setControlMode( bool bControlMode )
    {
        m_controlMode = bControlMode;
    }

}
