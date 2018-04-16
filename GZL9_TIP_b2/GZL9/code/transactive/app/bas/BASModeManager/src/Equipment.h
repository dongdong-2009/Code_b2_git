
#if !defined (APP_BAS_BasManager_Equipment_H)
#define APP_BAS_BasManager_Equipment_H

#pragma once
#include "boost/shared_ptr.hpp"
#include "core/data_access_interface/bas/src/SubsystemAccessFactory.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "app/bas/BASModeManager/src/BasManagerSubscriber.h"


namespace TA_IRS_App
{
	class BasManagerSubscriber;
	class Equipment : public TA_Base_Bus::IEntityUpdateEventProcessor
	{
	public:
		friend class BasManagerSubscriber;
		Equipment(unsigned long locationKey, 
				BasManagerSubscriber* basSubscriberPtr, 
				TA_IRS_Core::SubSystem_SmartPtr subsystem, 
				std::string equipmentName);
		~Equipment(void);

	public:
		//TA_Base_Bus::IEntityUpdateEventProcessor interface implement
		void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

	public:
		unsigned long getEquipmentValue(EquipmentValueType);
		TA_IRS_Core::SubSystem_SmartPtr getSubsystem();
		void setAutoCfgValue(const int nValue);

	protected:
		void initializeEquipment();
		void prepareToExit();

	private:
		bool getDatapointNewValue(unsigned long datapointKey, double& value, EquipmentValueType& type);
		unsigned long getModeKeyByValue(double value);
		std::string getDatapointStringByEntityKey(unsigned long entitykey);
		bool isModeValid(unsigned long modeValue);

	private:
		Equipment(void);

	private:
		BasManagerSubscriber* 				m_basSubscriberPtr; // for call back

		TA_Base_Bus::DataPointProxySmartPtr m_dpCurrentExcuteMode;
		TA_Base_Bus::DataPointProxySmartPtr m_dpCurrentExcuteModeBit;
		TA_Base_Bus::DataPointProxySmartPtr m_dpCurrentExcuteModeSetBit;
		TA_Base_Bus::DataPointProxySmartPtr m_dpCurrentExcuteModeStatus;
		
		// The data node name
		std::string 						m_equipmentName;
		TA_IRS_Core::SubSystem_SmartPtr 	m_subsystem;
		unsigned long 						m_locationKey;
	};
	
	typedef boost::shared_ptr<Equipment> 	Equipment_SmartPtr;
}
#endif //APP_BAS_BasManager_Equipment_H

