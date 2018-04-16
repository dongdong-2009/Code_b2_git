#if !defined(GF_3002_app_bas_BASAgent_FireTriggerBasEquipment_H)
#define GF_3002_app_bas_BASAgent_FireTriggerBasEquipment_H

#include <string>
#include "core/data_access_interface/bas/src/BasFireAccessFactory.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"

namespace TA_Base_Bus
{
	class DataPoint;
    class ScadaProxyFactory;
}

namespace TA_IRS_App
{
	class FireTriggerBasEquipmentManager;
	class FireTriggerBasEquipment : public TA_Base_Bus::IEntityUpdateEventProcessor
	{
	public:
		FireTriggerBasEquipment(const TA_IRS_Core::BasFireEquipmentPtr& basFireEquipmentPtr, FireTriggerBasEquipmentManager* fireTriggerManager);
		virtual ~FireTriggerBasEquipment(void);

	public:
		//TA_Base_Bus::IEntityUpdateEventProcessor interface implement
		void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);
		void notifyQuit();

	protected:
		void initializeFireEquipment();
		TA_IRS_Core::BasStationFireStatus getCurrentDpValueToStationFireStatus();

	private:
		TA_Base_Bus::ScadaProxyFactory* m_proxyFactory;
		TA_Base_Bus::DataPointProxySmartPtr m_dpDatapoint_FireTrigger;
		TA_IRS_Core::BasFireEquipmentPtr m_basFireEquipmentPtr;
		FireTriggerBasEquipmentManager* m_fireTriggerBasEquipmentManager;
	};
}

#endif //GF_3002_app_bas_BASAgent_FireTriggerBasEquipment_H