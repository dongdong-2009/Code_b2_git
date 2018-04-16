#if !defined(FIRETRIGGERBROADCAST__INCLUDED_)
#define FIRETRIGGERBROADCAST__INCLUDED_

#include <string>
#include "bus/scada/proxy_library/src/IEntityUpdateEventProcessor.h"
#include "bus/scada/proxy_library/src/IDataPointProxy.h"
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"
#include "bus/scada/proxy_library/src/DataPointProxySmartPtr.h"

namespace TA_Base_Bus
{
	class DataPoint;
}

namespace TA_IRS_App
{
    class TriggeredBroadcastWorker;

	class FireAlarmSubscriber : public TA_Base_Bus::IEntityUpdateEventProcessor
	{
	public:
		FireAlarmSubscriber( /*TriggeredBroadcastWorker* triggeredBroadcastWork*/ );
		virtual ~FireAlarmSubscriber();

	public:
		//TA_Base_Bus::IEntityUpdateEventProcessor interface implement
		void processEntityUpdateEvent(unsigned long entityKey, TA_Base_Bus::ScadaEntityUpdateType updateType);
		void notifyQuit();
        void enable(const bool bEnable = true);

	protected:
		void initializeFireTrigger();
		std::string scadaEntityUpdateTypeToString(TA_Base_Bus::ScadaEntityUpdateType updateType);
        std::string fireStatueToString(bool bInFire);

	private:
		void executeFasTriggeredBroadcast( bool bFire );

    private:
		TA_Base_Bus::ScadaProxyFactory* m_proxyFactory;
        TA_Base_Bus::DataPointProxySmartPtr m_dpDatapoint_FireTrigger;
        TriggeredBroadcastWorker* m_pTriggeredBroadcastWorker;
        bool m_bInFire;
        bool m_bEnable;
	};
}

#endif //FIRETRIGGERBROADCAST__INCLUDED_