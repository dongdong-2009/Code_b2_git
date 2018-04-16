#if !defined (Transactive_3002_GF_App_Bas_BasModeManager_DeviceEquipment)
#define Transactive_3002_GF_App_Bas_BasModeManager_DeviceEquipment
#pragma once
#include "boost/shared_ptr.hpp"
#include "core/data_access_interface/bas/src/SubsystemAccessFactory.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "app/bas/BASModeManager/src/BasManagerDataHelper.h"

namespace TA_IRS_App
{
	class DeviceEquipmentManager;
	class DeviceEquipment : public TA_Base_Bus::IEntityUpdateEventProcessor
	{
	public:
		friend DeviceEquipmentManager;
		~DeviceEquipment(void);
	public:
		//TA_Base_Bus::IEntityUpdateEventProcessor interface implement
		void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

		// The public function
		std::string getValueType();
		unsigned long getDeviceEquipmentKey();
		unsigned long getDeviceKey();
		std::string getCurrentStatusValue();
		std::string getCurrentAutoValue();
		std::string getDeviceEquipmentAutoDpTag();
	protected:
		DeviceEquipment(DeviceEquipmentManager* equipmentManager, 
            const std::string& strEquipmentStatus, const std::string& strEquipmentAuto, unsigned long deviceKey);
		void initializeDeviceEquipment();
		void prepareToExit();
		bool isDatapointObjectValid();
	private:
		bool isMatching(const std::string& datapointName, const std::string& tokenName);
		std::string getCorrectValue(const std::string& value);
		std::string getValueLableString();
	private:
		TA_Base_Bus::ScadaProxyFactory* m_proxyFactory;
		TA_Base_Bus::DataPointProxySmartPtr m_dpAIDatapoint_Status;
		TA_Base_Bus::DataPointProxySmartPtr m_dpAIDatapoint_Auto;
		//TA_Base_Bus::DataPointProxySmartPtr m_dpAODatapoint; // not used

		DeviceEquipmentManager* m_callBack;
		std::string m_DeviceEquipment_Status;
		std::string m_DeviceEquipment_Auto;
		unsigned long m_deviceKey;
	};
	typedef boost::shared_ptr<DeviceEquipment> DeviceEquipment_SmartPtr;
}
#endif //Transactive_3002_GF_App_Bas_BasModeManager_DeviceEquipment
