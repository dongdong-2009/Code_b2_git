#ifndef APP_BAS_BASAgent_StationFireMonitorThread_H
#define APP_BAS_BASAgent_StationFireMonitorThread_H

#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/Semaphore.h"

namespace TA_IRS_App
{
	class FireTriggerBasEquipmentManager;
	class StationFireMonitorThread : public TA_Base_Core::Thread
	{
	public:
		friend class FireTriggerBasEquipmentManager;
		StationFireMonitorThread(FireTriggerBasEquipmentManager* fireManager);

		virtual ~StationFireMonitorThread();

		virtual void run();

		virtual void terminate();
	protected:
		void notifyStationFire();
		void notifyToQuit();
	private:
		bool m_stopMonitor;
		FireTriggerBasEquipmentManager* m_fireTriggerManager;
		TA_Base_Core::Semaphore m_semaphore;
	};
} //end TA_IRS_App

#endif  // APP_BAS_BASAgent_PeriodicThread_H
