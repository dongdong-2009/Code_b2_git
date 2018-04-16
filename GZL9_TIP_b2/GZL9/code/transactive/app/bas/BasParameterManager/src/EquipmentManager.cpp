#include "StdAfx.h"
#include "app/bas/BasParameterManager/src/EquipmentManager.h"
#include "app/bas/BasParameterManager/src/BasParasManagerDataHelper.h"

namespace TA_IRS_App
{
	const std::string EquipmentManager::DataNodeEntityTypeName = "DataNode";
	const std::string EquipmentManager::DataPointEntityTypeName = "DataPoint";
	EquipmentManager::EquipmentManager(IEquipmentUpdateInterface* listener)
		:m_equipmentUpdateListener(listener)
	{
		m_isPrepareToExit = false;
	}

	EquipmentManager::~EquipmentManager(void)
	{
	}

	void EquipmentManager::getLocationEquipmentMap(unsigned long locationKey, EquipmentMap& equipments)
	{
		FUNCTION_ENTRY("getLocationEquipmentMap");
		bool bFound = false;
		{
			TA_THREADGUARD(m_LocationEquipmentLock);
			bFound = (m_locationEquipmentMap.end() != m_locationEquipmentMap.find(locationKey));
		}
		
		// load from data base
		if (!bFound)
		{
			loadLocationEquipments(locationKey);
		}

		// return the equipments
		{
			TA_THREADGUARD(m_LocationEquipmentLock);
			equipments = m_locationEquipmentMap[locationKey];
		}	
		FUNCTION_EXIT;
	}

	void EquipmentManager::loadLocationEquipments(unsigned long locationKey)
	{
		FUNCTION_ENTRY("loadLocationEquipments");
		using namespace TA_Base_Core;
		TA_THREADGUARD(m_LocationEquipmentLock);
		ILocation_SmartPtr location = BasParasManagerDataHelperInstance::instance()->getLocationByKey(locationKey);
		if (NULL == location.get())
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Don't get any location. locationKey=%d", locationKey);
			return;
		}

		TA_IRS_Core::ParameterValueTypeInfoVct paraValueType;
		BasParasManagerDataHelperInstance::instance()->getParameterValueType(paraValueType);
		for (TA_IRS_Core::ParameterValueTypeInfoVct::iterator itPara = paraValueType.begin(); itPara != paraValueType.end(); itPara++)
		{
			unsigned long paramTypeKey = (*itPara).pkey;
			IEntityDataList entityData;
			BAS_TRY
			{
				if (!m_isPrepareToExit)
				{
					std::string strDatapointTokenString = constructTokenString(location->getName(), (*itPara).aiToken);				
					entityData = EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken(DataPointEntityTypeName, strDatapointTokenString);
					IEntityDataList::iterator itEntity;
					std::string strEquipment = "";
					for (itEntity = entityData.begin(); itEntity != entityData.end(); itEntity++)
					{
						if (!m_isPrepareToExit)
						{
							TA_Base_Core::DataPointEntityData* dpEntityData = dynamic_cast<TA_Base_Core::DataPointEntityData*>(*itEntity);
							strEquipment = dpEntityData->getParentName();
							Equipment_SmartPtr equipment_ptr(new Equipment(this, location->getName(), (*itPara), strEquipment));
							m_locationEquipmentMap[locationKey][paramTypeKey].push_back(equipment_ptr);
							equipment_ptr->initializeEquipment();
							// delete the data construct from TA_Base_Core
							delete (*itEntity);
						}
					}
				}
			}
			BAS_CATCH;
			if (BAS_WhetherHaveError)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", BAS_GetExceptionString);
			}
		}
		FUNCTION_EXIT;
	}

	void EquipmentManager::setEquipmentValue(unsigned long locationKey, unsigned long typeKey, unsigned long entityKey, double value)
	{
		FUNCTION_ENTRY("setEquipmentValue");
		LocationEquipmentMapIter it = m_locationEquipmentMap.find(locationKey);
		if (m_locationEquipmentMap.end() != it)
		{
			EquipmentMapIter itEquipment = it->second.find(typeKey);
			if ( it->second.end() != itEquipment)
			{
				EquipmentVct equipment = itEquipment->second;
				EquipmentVct::iterator iter;
				for (iter = equipment.begin(); iter != equipment.end(); iter++)
				{
					if ((*iter)->getEquipmentKey() == entityKey)
					{
						(*iter)->setCurrentValue(value);
						break;
					}
				}

			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There not found the typeKey. locationKey=%d TypeKey=%d", locationKey,typeKey);
				throw TA_Base_Core::TransactiveException("There not found the type Key, can not set datapoint");
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There not found the location Key: %d", locationKey);
			throw TA_Base_Core::TransactiveException("There not found the location Key, can not set datapoint");
		}
		FUNCTION_EXIT;
	}

	void EquipmentManager::setExecuteStatus( unsigned long locationKey,unsigned long typeKey, unsigned long entityKey, const std::string& strStatus)
	{
		FUNCTION_ENTRY("setExecuteStatus");
		LocationEquipmentMapIter it = m_locationEquipmentMap.find(locationKey);
		if (m_locationEquipmentMap.end() != it)
		{
			EquipmentMapIter itEquipment = it->second.find(typeKey);
			if ( it->second.end() != itEquipment)
			{
				EquipmentVct equipment = itEquipment->second;
				EquipmentVct::iterator iter;
				for (iter = equipment.begin(); iter != equipment.end(); iter++)
				{
					if ((*iter)->getEquipmentKey() == entityKey)
					{
						(*iter)->setExecuteStatus(strStatus);
						break;
					}
				}

			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There not found the typeKey, can not set status. locationKey=%d TypeKey=%d", locationKey,typeKey);
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There not found the location Key: %d, can not set status", locationKey);
		}
		FUNCTION_EXIT;
	}

	std::string EquipmentManager::constructTokenString(const std::string& strLocation, const std::string& token)
	{
		FUNCTION_ENTRY("constructTokenString");
		std::string strBAS = "";
		FUNCTION_EXIT;
		return strLocation + token;
	}

	void EquipmentManager::updataDatapointValue(unsigned long datapointKey, const std::string& strValue)
	{
		if ((!m_isPrepareToExit) && (NULL != m_equipmentUpdateListener))
		{
			m_equipmentUpdateListener->UpdateDatapointValue(datapointKey, strValue);
		}		
	}

	void EquipmentManager::updataDatapointStatus(unsigned long datapointKey, const std::string& strStatus)
	{
		if ((!m_isPrepareToExit) && (NULL != m_equipmentUpdateListener))
		{
			m_equipmentUpdateListener->UpdateDatapointStatus(datapointKey, strStatus);
		}		
	}

	void EquipmentManager::prepareToExist()
	{
		m_isPrepareToExit = true;
	}

	bool EquipmentManager::getEquipmentLimitedValue(unsigned long locationKey, unsigned long typeKey, unsigned long entityKey, 
													double& lowValue, double& highValue)
	{
		FUNCTION_ENTRY("getEquipmentLimitedValue");
		bool bSuccess = false;

		LocationEquipmentMapIter it = m_locationEquipmentMap.find(locationKey);
		if (m_locationEquipmentMap.end() != it)
		{
			EquipmentMapIter itEquipment = it->second.find(typeKey);
			if ( it->second.end() != itEquipment)
			{
				EquipmentVct equipment = itEquipment->second;
				EquipmentVct::iterator iter;
				for (iter = equipment.begin(); iter != equipment.end(); iter++)
				{
					if ((*iter)->getEquipmentKey() == entityKey)
					{
						bSuccess = (*iter)->getLimitedValue(lowValue, highValue);
						break;
					}
				}

			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There not found the typeKey. locationKey=%d TypeKey=%d", locationKey,typeKey);
			}
		}
		else
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There not found the location Key: %d", locationKey);
		}

		FUNCTION_EXIT;
		return bSuccess;
	}

}
