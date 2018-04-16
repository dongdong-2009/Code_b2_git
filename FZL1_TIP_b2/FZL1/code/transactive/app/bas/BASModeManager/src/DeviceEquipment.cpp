#include "StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/bas/BASModeManager/src/DeviceEquipment.h"
#include "app/bas/BASModeManager/src/DeviceEquipmentManager.h"

namespace TA_IRS_App
{
	DeviceEquipment::DeviceEquipment(DeviceEquipmentManager* DeviceEquipmentManager, const std::string& strEquipmentStatus, const std::string& strEquipmentAuto, unsigned long deviceKey) :
    m_callBack(DeviceEquipmentManager),
    m_DeviceEquipment_Status(strEquipmentStatus),
    m_DeviceEquipment_Auto(strEquipmentAuto),
    m_deviceKey(deviceKey)
	{
		FUNCTION_ENTRY("DeviceEquipment");
		FUNCTION_EXIT;
	}

	DeviceEquipment::~DeviceEquipment(void)
	{
		FUNCTION_ENTRY("~DeviceEquipment");
		FUNCTION_EXIT;
	}

	//TA_Base_Bus::IEntityUpdateEventProcessor interface implement
	void DeviceEquipment::processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType)
	{
		FUNCTION_ENTRY("processEntityUpdateEvent");
		using namespace TA_Base_Core;

		switch( updateType )
		{
		case TA_Base_Bus::ValueStateUpdate:
			break;
		default:
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Receive datapoint update entitykey=%lu ScadaEntityUpdateType=%d", entityKey, updateType);
		}

		std::string strValue = "";
		bool bStatus = true;
		if(entityKey == m_dpAIDatapoint_Status->getEntityKey())
		{
			strValue = getCurrentStatusValue();
			bStatus = true;
		}
		else if (entityKey == m_dpAIDatapoint_Auto->getEntityKey())
		{
			strValue = getCurrentAutoValue();
			bStatus = false;
		}

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "process Entity Update Event. DatapointEntityKey=%d ScadaEntityUpdateType=%d NewValue:%s",entityKey, updateType, strValue.c_str());

		if (NULL != m_callBack)
		{
			m_callBack->updataDatapointValue(m_deviceKey, strValue, bStatus);
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "m_callBack pointer is NULL.");
		}
		FUNCTION_EXIT;
	}

	void DeviceEquipment::initializeDeviceEquipment()
	{
		FUNCTION_ENTRY("initializeDeviceEquipment");
        // get a handle to the ScadaProxyFactory
        TA_Base_Bus::ScadaProxyFactory& refScadaFactory = TA_Base_Bus::ScadaProxyFactory::getInstance();

        refScadaFactory.setProxiesToControlMode();

        try
        {
			// Create data point proxy for device status.
			TA_Base_Core::IEntityData* entityDataStatus = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_DeviceEquipment_Status);
			TA_Base_Core::DataPointEntityData* dpEntityDataStatus = dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDataStatus);

			if (NULL != dpEntityDataStatus)
			{
				TA_Base_Core::DataPointEntityDataPtr dp(dpEntityDataStatus);
				refScadaFactory.createDataPointProxy(dp, *this, m_dpAIDatapoint_Status);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DeviceEquipment [%s] is created.", m_DeviceEquipment_Status.c_str());
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DeviceEquipment is not defined as datapoint entity(Status),DataPointName=%s", m_DeviceEquipment_Status.c_str());
			}

			// Create data point proxy for device auto.
			TA_Base_Core::IEntityData* entityDataAuto = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_DeviceEquipment_Auto);
			TA_Base_Core::DataPointEntityData* dpEntityDataAuto = dynamic_cast<TA_Base_Core::DataPointEntityData*>(entityDataAuto);

			if (NULL != dpEntityDataAuto)
			{
				TA_Base_Core::DataPointEntityDataPtr dp(dpEntityDataAuto);
				refScadaFactory.createDataPointProxy(dp, *this, m_dpAIDatapoint_Auto);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "DeviceEquipment [%s] is created.", m_DeviceEquipment_Auto.c_str());
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DeviceEquipment is not defined as datapoint entity(Auto),DataPointName=%s", m_DeviceEquipment_Auto.c_str());
			}
        }
        catch ( const std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, typeid(expWhat).name(), expWhat.what());
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Caught unknown exception when initializeDeviceEquipment");
        }

        TA_Base_Bus::ScadaProxyFactory::removeInstance();
		FUNCTION_EXIT;
	}

	void DeviceEquipment::prepareToExit()
	{
        FUNCTION_ENTRY("prepareToExit");
        m_dpAIDatapoint_Status.reset();
        m_dpAIDatapoint_Auto.reset();
		FUNCTION_EXIT;
	}

	bool DeviceEquipment::isMatching(const std::string& datapointName, const std::string& tokenName)
	{
		FUNCTION_ENTRY("isMatching");
		FUNCTION_EXIT;
		return true;
	}

	std::string DeviceEquipment::getValueType()
	{
		return "";
	}

	unsigned long DeviceEquipment::getDeviceEquipmentKey()
	{
		return m_dpAIDatapoint_Auto->getParentKey();
	}

	std::string DeviceEquipment::getCurrentStatusValue()
	{
        std::string strValue = "";

        try
        {
            strValue = m_dpAIDatapoint_Status->getValueAsString();
        }
        catch (...)
        {
            strValue = "异常";
        }

		return getCorrectValue(strValue);
	}

	std::string DeviceEquipment::getCurrentAutoValue()
    {
        std::string strValue = "";

        try
        {
            strValue = m_dpAIDatapoint_Auto->getValueAsString();
        }
        catch (...)
        {
            strValue = "异常";
        }

		return getCorrectValue( strValue );
	}

	std::string DeviceEquipment::getCorrectValue(const std::string& value)
	{
		std::string strNewValue = value;
		if( strNewValue.empty() || ( strNewValue.compare("#####") == 0 ))
		{
			strNewValue = "未定义";
		}

		return strNewValue;
	}

	unsigned long DeviceEquipment::getDeviceKey()
	{
		return m_deviceKey;
	}

	std::string DeviceEquipment::getDeviceEquipmentAutoDpTag()
	{
        return m_DeviceEquipment_Auto;
	}
}
