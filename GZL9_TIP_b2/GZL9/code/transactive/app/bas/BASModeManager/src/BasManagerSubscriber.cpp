#include "StdAfx.h"
#include "app/bas/BASModeManager/src/BasManagerSubscriber.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/StationBASAgentEntityData.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/bas/BASModeManager/src/ModeStatusListenerManager.h"
#include "app/bas/BASModeManager/src/BASAgentHelper.h"
#include "app/bas/BASModeManager/src/BasManagerDataHelper.h"
#include "app/bas/BASModeManager/src/SubSystemStatus.h"
#include "app/bas/BASModeManager/src/Equipment.h"

namespace TA_IRS_App
{
	BasManagerSubscriber::BasManagerSubscriber(void)
	{
		FUNCTION_ENTRY("BasManagerSubscriber");
		FUNCTION_EXIT;
	}

	BasManagerSubscriber::~BasManagerSubscriber(void)
	{
		FUNCTION_ENTRY("~BasManagerSubscriber");
		FUNCTION_EXIT;
	}

	void BasManagerSubscriber::startSubscriber(unsigned long locationKey)
	{
		FUNCTION_ENTRY("startSubscriber");
		using namespace TA_Base_Core;
		using namespace TA_IRS_Core;	
	
		if (m_environments.end() == m_environments.find(locationKey))
		{
			// get the StationBASAgent subsystems and location info to construct the datanode Name
			MapOrderToSubsystem subsystems = BasManagerDataHelperInstance::instance()->getAllSubsystems(locationKey, false);
			std::string locationName = BASAgentHelperInstance::getInstance().getStationName(locationKey);

			LocationEquipmentVct locationEquipmentVct;
			for (MapOrderToSubsystem::iterator it = subsystems.begin(); it != subsystems.end(); it++)
			{
				if (!it->second->getIsMerge())
				{
					std::string strEquipmentName = locationName + it->second->getAssetName();
					Equipment_SmartPtr pEquipment = Equipment_SmartPtr(new Equipment(locationKey, this, it->second, strEquipmentName));
					BAS_TRY
					{
						pEquipment->initializeEquipment();
						locationEquipmentVct.push_back(pEquipment);
					}
					BAS_CATCH;				
				}
			}

			m_environments[locationKey] = locationEquipmentVct;
		}

		FUNCTION_EXIT;
	}

	// unsubscribe the message
	void BasManagerSubscriber::stopSubscriber()
	{
		FUNCTION_ENTRY("stopSubscriber");

		for (LocationEquipmentVctMap::iterator it = m_environments.begin(); it != m_environments.end(); it++)
		{
			LocationEquipmentVct locationEvct = it->second;
			for (LocationEquipmentVct::iterator iter = locationEvct.begin(); iter != locationEvct.end(); iter++)
	 		{
	 			(*iter)->prepareToExit();
	 		}
		}

	    for (LocationFireRecoverEquipmentMap::iterator it = m_fireRecoverEquipment.begin(); 
	        it != m_fireRecoverEquipment.end(); it++)
	    {
	        it->second->uninitEquipment();
	    }

		m_environments.clear();
	    m_fireRecoverEquipment.clear();
		FUNCTION_EXIT;
	}

	void BasManagerSubscriber::updateModeStatus(unsigned long locationKey, 
												unsigned long modeKey, 
												unsigned long subsystemKey, 
												EquipmentValueType valueType, 
												double value, 
												int status)
	{
		FUNCTION_ENTRY("updateModeStatus");
		ModeStatusListenerManagerInstance::instance()->updateModeStatus(modeKey, status);	
		FUNCTION_EXIT;
	}

	int BasManagerSubscriber::getSubsystemAutoCfgValue(unsigned long locationKey, unsigned long subsystemKey)
	{
		FUNCTION_ENTRY("getSubsystemAutoCfgValue");
		int retValue = 0;
		bool bFound = false;
		if (m_environments.end() != m_environments.find(locationKey))
		{
			for(LocationEquipmentVct::iterator it = m_environments[locationKey].begin(); it != m_environments[locationKey].end(); it++)
			{
				if ((*it)->getSubsystem()->getKey() == subsystemKey)
				{
					bFound = true;
					retValue = (*it)->getEquipmentValue(EVT_CurrentExcuteModeBit);
					break;
				}
			}
		}

		if (!bFound)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "can not found the subsystemkey:%d,  now locationkey is %d", subsystemKey, locationKey);
			TA_THROW(TA_Base_Core::TransactiveException("can not found the subsystem, so could not get the auto config value"));
		}

		FUNCTION_EXIT;
		return retValue;
	}

	void BasManagerSubscriber::setSubsystemAutoCfgValue(unsigned long locationKey, unsigned long subsystemKey, const int nValue)
	{
		FUNCTION_ENTRY("setSubsystemAutoCfgValue");
		bool bFound = false;
		if (m_environments.end() != m_environments.find(locationKey))
		{
			for(LocationEquipmentVct::iterator it = m_environments[locationKey].begin(); it != m_environments[locationKey].end(); it++)
			{
				if ((*it)->getSubsystem()->getKey() == subsystemKey)
				{
					(*it)->setAutoCfgValue(nValue);
					bFound = true;
					break;
				}
			}
		}

		if (bFound == false)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "can not found the subsystemkey:%d,  now locationkey is %d", subsystemKey, locationKey);
			TA_THROW(TA_Base_Core::TransactiveException("can not found the subsystem, so could not set the auto config value"));
		}
		FUNCTION_EXIT;
	}

	void BasManagerSubscriber::setRecoverFromFireMode(unsigned long locationKey)
	{
		FUNCTION_ENTRY("setRecoverFromFireMode");
		std::string strFireRecoverDpToken = BasManagerDataHelperInstance::instance()->getFireModeRecoverDataPointName();
	    strFireRecoverDpToken = BASAgentHelperInstance::getInstance().getStationName(locationKey) + strFireRecoverDpToken;

		if (m_fireRecoverEquipment.end() ==  m_fireRecoverEquipment.find(locationKey))
		{
			FireRecoverEquipment_SmartPtr firePtr = FireRecoverEquipment_SmartPtr(new FireRecoverEquipment(strFireRecoverDpToken));
			m_fireRecoverEquipment[locationKey] = firePtr;
			firePtr->initializeEquipment();
		}
		m_fireRecoverEquipment[locationKey]->setFireRecoverFlag();
		FUNCTION_EXIT;
	}
}

