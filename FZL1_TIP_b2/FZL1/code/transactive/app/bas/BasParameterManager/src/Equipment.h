#if !defined (Transactive_3002_GF_App_Bas_TimeTableManager_Equipment)
#define Transactive_3002_GF_App_Bas_TimeTableManager_Equipment
#pragma once
#include "boost/smart_ptr.hpp"
#include "core/data_access_interface/bas/src/SubsystemAccessFactory.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "app/bas/BasParameterManager/src/CommonDefine.h"
#include "app/bas/BasParameterManager/src/BasParasManagerDataHelper.h"

namespace TA_IRS_App
{
	class EquipmentManager;
	class Equipment : public TA_Base_Bus::IEntityUpdateEventProcessor
	{
	public:
		friend EquipmentManager;
		~Equipment(void);
	public:
		//TA_Base_Bus::IEntityUpdateEventProcessor interface implement
		void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

		// The public function
		std::string getValueType();
		std::string getEquipmentLable();
		DWORD getEquipmentKey();
		std::string getLocationName();
		std::string getCurrentValue();

		void setCurrentValue(const std::string& newValue);
		void setCurrentValue(double newValue);

		bool getLimitedValue(double& lowValue, double& highValue);

		void setExecuteStatus(const std::string& strStatus);
		std::string getExecuteStatus();

		std::string convertStatusToString(TA_Base_Bus::EDataPointControlState status);
	protected:
		Equipment(EquipmentManager* equipmentManager, const std::string& strLocation, const TA_IRS_Core::ParameterValueTypeInfo& parasValueTypeconst, const std::string& strEquipment);
		void initializeEquipment();
		void prepareToExit();
	private:
		std::string constructDataPointString(const std::string& strEquipment, const std::string& parasTypeToken);
		bool isMatching(const std::string& datapointName, const std::string& tokenName);
	private:
		TA_Base_Bus::DataPointProxySmartPtr m_dpAIDatapoint;
		TA_Base_Bus::DataPointProxySmartPtr m_dpAODatapoint;

		EquipmentManager* m_callBack;
		TA_IRS_Core::ParameterValueTypeInfo m_parameterValueTypeInfo;
		std::string m_equipmentName;
		std::string m_locationName;
		std::string m_status;
	};
	typedef boost::shared_ptr<Equipment> Equipment_SmartPtr;
}
#endif //Transactive_3002_GF_App_Bas_TimeTableManager_Equipment
