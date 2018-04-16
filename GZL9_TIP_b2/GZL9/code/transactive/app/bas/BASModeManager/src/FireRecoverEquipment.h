#if !defined (APP_BAS_BasManager_FireRecoverEquipment_H)
#define APP_BAS_BasManager_FireRecoverEquipment_H
#pragma once

#include "boost/shared_ptr.hpp"
#include "core/timers/src/TimedWaitSemaphore.h"
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"
#include "bus/scada/proxy_library/src/DataNodeProxySmartPtr.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "app/bas/BASModeManager/src/CommonDef.h"
#include "app/bas/BASModeManager/src/BasManagerSubscriber.h"


namespace TA_IRS_App
{
	class FireRecoverEquipment : public TA_Base_Bus::IEntityUpdateEventProcessor
	{
	public:
		friend class BasManagerSubscriber;
		~FireRecoverEquipment(void);

	public:
		void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);

		void setFireRecoverFlag();

	protected:
		FireRecoverEquipment(const std::string& strDatapointName);
		void initializeEquipment();
	    void uninitEquipment();

	private:
		TA_Base_Bus::DataPointProxySmartPtr m_dpFireRecover;
	    TA_Base_Core::TimedWaitSemaphore m_semInitDone;
	    bool m_bInitDone;
		std::string m_strEquipmentName;
	};

}

#endif //APP_BAS_BasManager_FireRecoverEquipment_H

