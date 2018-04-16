#pragma once

#include "core\threads\src\thread.h"
#include "bus\trains\TrainSelectionComponent\src\TrainSelector.h"
#include "core\timers\src\TimedWaitSemaphore.h"

namespace TA_IRS_App
{
	class TrainListRecoveryThread : public TA_Base_Core::Thread
	{
		public:
			TrainListRecoveryThread(TA_IRS_Bus::TrainSelector * trainSelector);
		public:
			~TrainListRecoveryThread(void);

		private:
			void run();
			void terminate();
		private:
			bool m_bIsTerminate;
			long m_trainlistRecoveryDelay;
			TA_IRS_Bus::TrainSelector * m_trainSelector;
			TA_Base_Core::TimedWaitSemaphore m_semaphore;
	};
}
