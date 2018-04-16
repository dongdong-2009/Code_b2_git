#include "StdAfx.h"
#include "app/bas/BasParameterManager/src/Equipment.h"
#include "app/bas/BasParameterManager/src/EquipmentManager.h"
#include "app/bas/BasParameterManager/src/BasParasManagerUtility.h"
namespace TA_IRS_App
{
	Equipment::Equipment(EquipmentManager* equipmentManager, const std::string& strLocation, const TA_IRS_Core::ParameterValueTypeInfo& parasValueType, const std::string& strEquipment)
		:m_callBack(equipmentManager),m_locationName(strLocation),m_parameterValueTypeInfo(parasValueType),m_equipmentName(strEquipment)
	{
		FUNCTION_ENTRY("Equipment");
		FUNCTION_EXIT;
	}

	Equipment::~Equipment(void)
	{
		FUNCTION_ENTRY("~Equipment");
		FUNCTION_EXIT;
	}

	//TA_Base_Bus::IEntityUpdateEventProcessor interface implement
	void Equipment::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY("processEntityUpdateEvent");
		using namespace TA_Base_Core;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "processEntityUpdateEvent entityKey=%lu ScadaEntityUpdateType=%d", entityKey, updateType);
		BAS_TRY
		{
			if (TA_Base_Bus::ValueStateUpdate == updateType)
			{
				if (entityKey == m_dpAIDatapoint->getEntityKey())
				{
					std::string strValue = getCurrentValue();
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
						"process Entity Update Event location=%s Equipment=%s datapointEntityKey=%d ScadaEntityUpdateType=%d NewValue:%s",
						m_locationName.c_str(), m_equipmentName.c_str(), entityKey, updateType, strValue.c_str());

					if (NULL != m_callBack)
					{
						m_callBack->updataDatapointValue(getEquipmentKey(), strValue);
					}
					else
					{
                        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,  "m_callBack is NULL.");
					}					
				}
				else
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,  "Receive the AO datapoint update but we ignore it");
				}
			}
			else if (TA_Base_Bus::ControlStateUpdate == updateType)
			{
				if (entityKey == m_dpAODatapoint->getEntityKey())
				{
					TA_Base_Bus::EDataPointControlState controlStatus = m_dpAODatapoint->getControlState();

					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
						"process Entity Update Event location=%s Equipment=%s datapointEntityKey=%d ScadaEntityUpdateType=%d NewStatus:%d",
						m_locationName.c_str(), m_equipmentName.c_str(), entityKey, updateType, controlStatus);

					switch(controlStatus)
					{
					case TA_Base_Bus::success:
					case TA_Base_Bus::conditionalSuccess:
					case TA_Base_Bus::LCC_failed:
					case TA_Base_Bus::comms_failed:
					case TA_Base_Bus::failed:
						break;
					default:
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Ignore this kind of control status: %d",controlStatus);
						return;
					}

					setExecuteStatus(convertStatusToString(controlStatus));
					if (NULL != m_callBack)
					{
						m_callBack->updataDatapointStatus(getEquipmentKey(), convertStatusToString(controlStatus));
					}
					else
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,  "m_callBack is NULL.");
					}						
				}
				else
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,  "Receive the AO datapoint status update but we ignore it");
				}

			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "processEntityUpdateEvent entityKey=%lu ScadaEntityUpdateType=%d but ignore this update.",
					entityKey, updateType);
			}
		}
		BAS_CATCH;

		FUNCTION_EXIT;
	}

	void Equipment::initializeEquipment()
	{
		FUNCTION_ENTRY("initializeEquipment");

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Equipment [%s] create beginning.", m_equipmentName.c_str());
		// get a handle to the ScadaProxyFactory
		TA_Base_Bus::ScadaProxyFactory& refScadaFactory = TA_Base_Bus::ScadaProxyFactory::getInstance();

		refScadaFactory.setProxiesToControlMode();

        try
        {
			TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_equipmentName);
			TA_Base_Core::DataNodeEntityData* dnEntityData = dynamic_cast<TA_Base_Core::DataNodeEntityData*>(entityData);

			if (NULL != dnEntityData)
			{
				TA_Base_Core::SharedIEntityDataList myDatapoints = dnEntityData->getChildDataPoints();
				TA_Base_Core::SharedIEntityDataList::const_iterator iterPt;

				unsigned int count = 0;
				const std::string strAIDataPointName = constructDataPointString(m_equipmentName, m_parameterValueTypeInfo.aiToken);
				const std::string strAODataPointName = constructDataPointString(m_equipmentName, m_parameterValueTypeInfo.aoToken);
				for ( iterPt = myDatapoints.begin(); iterPt != myDatapoints.end(); ++iterPt)
				{
					TA_Base_Core::DataPointEntityData* dpEntityData = dynamic_cast<TA_Base_Core::DataPointEntityData*>((*iterPt).get());
					//TA_ASSERT(dpEntityData != NULL, "Equipment is not defined as datapoint entity");
					if (NULL == dpEntityData)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Equipment is not defined as datapoint entity,EquipmentName=%s", m_equipmentName.c_str());
						continue;
					}

					TA_Base_Core::DataPointEntityDataPtr dp(dpEntityData);
					std::string strDataPointName = dp->getName();

					if (strAIDataPointName == strDataPointName)
					{
						refScadaFactory.createDataPointProxy(dp, *this, m_dpAIDatapoint);
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Equipment [%s] datapoint [%s] is created.", m_equipmentName.c_str(), strDataPointName.c_str());
					} 
					else if(strAODataPointName == strDataPointName)
					{
						refScadaFactory.createDataPointProxy(dp, *this, m_dpAODatapoint);
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Equipment [%s] datapoint [%s] is created.", m_equipmentName.c_str(), strDataPointName.c_str());
					}
				}
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Equipment is not defined as datanode entity,EquipmentName=%s", m_equipmentName.c_str());
			}
		}
		catch ( const std::exception& expWhat )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, typeid(expWhat).name(), expWhat.what());
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Caught unknown exception when initializeEquipment");
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Equipment [%s] is created.", m_equipmentName.c_str());
		TA_Base_Bus::ScadaProxyFactory::removeInstance();
		FUNCTION_EXIT;
	}

	void Equipment::prepareToExit()
	{
		FUNCTION_ENTRY("prepareToExit");
		FUNCTION_EXIT;
	}

	bool Equipment::isMatching(const std::string& datapointName, const std::string& tokenName)
	{
		FUNCTION_ENTRY("isMatching");
		FUNCTION_EXIT;
		return true;
	}
	
	std::string Equipment::constructDataPointString(const std::string& strEquipment, const std::string& parasTypeToken)
	{
		FUNCTION_ENTRY("constructTokenString");
		size_t pos = parasTypeToken.find_last_of('.');
		FUNCTION_EXIT;
		return strEquipment + parasTypeToken.substr(pos);
	}

	std::string Equipment::getValueType()
	{
		return m_parameterValueTypeInfo.name;
	}

	std::string Equipment::getEquipmentLable()
	{
		return m_equipmentName;
	}

	DWORD Equipment::getEquipmentKey()
	{
		return static_cast<DWORD>(m_dpAIDatapoint->getParentKey());
	}

	std::string Equipment::getLocationName()
	{
		return m_locationName;
	}

	std::string Equipment::getCurrentValue()
	{
		double value = m_dpAIDatapoint->getValueAsDouble();
		
		return BasParasManagerUtility::doubleToString(value);
	}

	void Equipment::setCurrentValue(const std::string& newValue)
	{
		std::stringstream ss;
		double value;
		ss << newValue;
		ss >> value;
		setCurrentValue(value);
	}

	void Equipment::setCurrentValue(double newValue)
	{
		std::stringstream ss;
		ss << newValue;

		std::string session = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		LOG_GENERIC(SourceInfo ,TA_Base_Core::DebugUtil::DebugInfo, "set datapoint new value, datapoint:%s NewValue:%s session:%s",
			m_dpAODatapoint->getEntityName().c_str(), ss.str().c_str(), session.c_str());
		m_dpAODatapoint->setAnalogueValue(newValue, session);
	}

	bool Equipment::getLimitedValue(double& lowValue, double& highValue)
	{
		bool bSuccess = false;
		BAS_TRY
		{
			lowValue = m_dpAODatapoint->getLimit(true);
			highValue = m_dpAODatapoint->getLimit(false);
			bSuccess = true;
		}
		BAS_CATCH;
		return bSuccess;
	}

	void Equipment::setExecuteStatus(const std::string& strStatus)
	{
		m_status = strStatus;
	}

	std::string Equipment::getExecuteStatus()
	{
        return m_status;
	}


	std::string Equipment::convertStatusToString(TA_Base_Bus::EDataPointControlState status)
	{
		std::string strStatus = "";
		switch (status)
		{
		case TA_Base_Bus::success:
		case TA_Base_Bus::conditionalSuccess:
			strStatus = "成功";
			break;
		case TA_Base_Bus::LCC_failed:
			strStatus = "LCC校验失败";
			break;
		case TA_Base_Bus::comms_failed:
			strStatus = "通讯失败";
			break;
		case TA_Base_Bus::failed:
			strStatus = "失败";
			break;
		default:
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,	"Ignore update the status, NewStatus:%d", status);
		}
		return strStatus;
	}
}
