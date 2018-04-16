#include "StdAfx.h"
#include "app/bas/BASModeManager/src/CommonDef.h"
#include "app/bas/BasModeManager/src/DeviceEquipmentManager.h"
#include "app/bas/BasModeManager/src/BasManagerDataHelper.h"

namespace TA_IRS_App
{
	const std::string DeviceEquipmentManager::DataNodeEntityTypeName = "DataNode";
	const std::string DeviceEquipmentManager::DataPointEntityTypeName = "DataPoint";
	DeviceEquipmentManager::DeviceEquipmentManager()
		:m_equipmentUpdateListener(NULL)
	{
	}

	DeviceEquipmentManager::~DeviceEquipmentManager(void)
	{
	}

	void DeviceEquipmentManager::getLocationDeviceEquipments(unsigned long locationKey, unsigned long modeKey, EquipmentVct& equipments)
	{
		FUNCTION_ENTRY("getLocationEquipmentMap");
		m_isStopLoading = false;
		bool bFound = false;
		{
			TA_THREADGUARD(m_LocationEquipmentLock);
			LocationDeviceEquipmentMapIter locationDeviceIter = m_locationEquipmentMap.find(locationKey);

			if(m_locationEquipmentMap.end() != locationDeviceIter)
			{
				bFound = (locationDeviceIter->second.end() != locationDeviceIter->second.find(modeKey)) ? true : false;
			}
		}

		// load from data base
		if (!bFound)
		{
			loadLocationDeviceEquipments(locationKey, modeKey);
		}

		// return the equipments
		{
			TA_THREADGUARD(m_LocationEquipmentLock);
			equipments.clear();
			if (!m_isStopLoading) // if stop the loading action, the data should reload next time
			{
				equipments = m_locationEquipmentMap[locationKey][modeKey];
			}			
		}		
		FUNCTION_EXIT;
	}

	void DeviceEquipmentManager::loadLocationDeviceEquipments(unsigned long locationKey, unsigned long modeKey)
	{
		FUNCTION_ENTRY("loadLocationEquipments");
		using namespace TA_Base_Core;

		EquipmentVct equipmentVct;
		TA_IRS_Core::ModeDevices modeDevices = BasManagerDataHelperInstance::instance()->getModeDevicesByModeKey(locationKey, modeKey);
		for (ModeDevicesIter itPara = modeDevices.begin(); itPara != modeDevices.end(); itPara++)
		{
			if (m_isStopLoading)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, " now, stop loading the location equipment, exit the loop.");
				break;
			}
			BAS_TRY
			{
				DeviceEquipment_SmartPtr equipment_ptr(new DeviceEquipment(this, itPara->datapointTag_Status, itPara->datapointTag_Auto, itPara->pkey));
				equipment_ptr->initializeDeviceEquipment();
				equipmentVct.push_back(equipment_ptr);
			}
			BAS_CATCH;
			if (BAS_WhetherHaveError)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", BAS_GetExceptionString);
			}
		}
		if (!m_isStopLoading)
		{
			TA_THREADGUARD(m_LocationEquipmentLock);
			m_locationEquipmentMap[locationKey][modeKey] = equipmentVct;
		}
		
		FUNCTION_EXIT;
	}

	bool DeviceEquipmentManager::stopLoadDeviceEquipments()
	{
		bool oldValue = m_isStopLoading;
		m_isStopLoading = true;
		return oldValue;
	}

	void DeviceEquipmentManager::prepareClose()
	{
		TA_THREADGUARD(m_LocationEquipmentLock);
		for (LocationDeviceEquipmentMapIter itLocation = m_locationEquipmentMap.begin(); itLocation != m_locationEquipmentMap.end(); itLocation++)
		{
			for (DeviceEquipmentMapIter itDevice = itLocation->second.begin(); itDevice != itLocation->second.end(); itDevice++)
			{
				for (EquipmentVct::iterator it = itDevice->second.begin(); it != itDevice->second.end(); it++)
				{
					(*it)->prepareToExit();
				}
			}
		}
		m_locationEquipmentMap.clear();
	}

	void DeviceEquipmentManager::updataDatapointValue(unsigned long deviceKey, const std::string& strValue, bool status)
	{
		TA_THREADGUARD(m_ListenerLock);//GF-78. like++
		if (NULL != m_equipmentUpdateListener)
		{
			m_equipmentUpdateListener->UpdateDatapointValue(deviceKey, strValue, status);
		}		
	}

	void DeviceEquipmentManager::resetListener(IEquipmentUpdateInterface* listener)
	{
		TA_THREADGUARD(m_ListenerLock);//GF-78. like++
		m_equipmentUpdateListener = listener;
	}
}
