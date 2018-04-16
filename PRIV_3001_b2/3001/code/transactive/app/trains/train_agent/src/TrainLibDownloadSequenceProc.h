#pragma once

#include "app/trains/train_agent/src/IEndDownloadCallBack.h"
#include "app/trains/train_agent/src/ITrainLibDownloadSequence.h"
#include "core/synchronisation/src/Semaphore.h"
#include "core/threads/src/QueueProcessor.h"

#include "app/trains/train_agent/src/ITimsTisManagerTaskCallback.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"

#include <string>

namespace TA_IRS_App
{
	
	class TrainLibDownloadSequenceProc : public IEndDownloadCallBack,
										 public ITrainLibDownloadSequence,
										 public TA_Base_Core::QueueProcessor <TrainDownloadInfo>
	{
		public:
			TrainLibDownloadSequenceProc(void);
			~TrainLibDownloadSequenceProc(void);
			
			virtual void processSequenceDownload (TA_IRS_Bus::CommonTypes::TrainIdList trainIdList,TrainLibStackInfo * trainDownloadInfo);

			virtual void processEndDownload (TA_IRS_Bus::CommonTypes::TrainIdType trainId);
			virtual void setWaitTrain (TA_IRS_Bus::CommonTypes::TrainIdType trainId);
			virtual void initiateWait ();
			
			void processEvent (boost::shared_ptr <TrainDownloadInfo> sharedPtrData);
	
	private:
			TA_Base_Core::Semaphore m_semaphore;
			long m_lTrainId;
			TA_Base_Core::ThreadPoolSingletonManager& m_threadPool;
	};

}
