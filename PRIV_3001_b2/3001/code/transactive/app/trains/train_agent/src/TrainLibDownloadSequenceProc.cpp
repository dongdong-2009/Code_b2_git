
#include "app/trains/train_agent/src/TrainLibDownloadSequenceProc.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/trains/train_agent/src/TisTasks.h"

namespace TA_IRS_App
{
	TrainLibDownloadSequenceProc::TrainLibDownloadSequenceProc(void) : m_semaphore (0),
																	   m_lTrainId(0),
																	   m_threadPool(*TA_Base_Core::ThreadPoolSingletonManager::getInstance())
	{
		FUNCTION_ENTRY ("TrainLibDownloadSequenceProc");
		start ();
		FUNCTION_EXIT;
	}

	TrainLibDownloadSequenceProc::~TrainLibDownloadSequenceProc(void)
	{
		FUNCTION_ENTRY ("~TrainLibDownloadSequenceProc");
		terminateAndWait();
		FUNCTION_EXIT;
	}

	void TrainLibDownloadSequenceProc::processEndDownload (TA_IRS_Bus::CommonTypes::TrainIdType trainId)
	{
		FUNCTION_ENTRY ("processEndDownload");

		if (m_lTrainId == trainId)
		{
			m_semaphore.post();
		}

		FUNCTION_EXIT;
	}

	void TrainLibDownloadSequenceProc::setWaitTrain (TA_IRS_Bus::CommonTypes::TrainIdType trainId)
	{
		FUNCTION_ENTRY ("setWaitTrain");
		
		m_lTrainId = trainId;

		FUNCTION_EXIT;
	}

	void TrainLibDownloadSequenceProc::initiateWait ()
	{
		FUNCTION_ENTRY ("setWaitTrain");

		m_semaphore.wait();

		FUNCTION_EXIT;
	}

	void TrainLibDownloadSequenceProc::processSequenceDownload (TA_IRS_Bus::CommonTypes::TrainIdList trainIdList,TrainLibStackInfo * trainDownloadInfo)
	{
		FUNCTION_ENTRY ("processSequenceDownload");
		
		TrainDownloadInfo * trainDnwInfo =  new TrainDownloadInfo ();

		trainDnwInfo->stackInfo = trainDownloadInfo;
		trainDnwInfo->trainList = trainIdList;

		boost::shared_ptr <TrainDownloadInfo> sharedPtrDwnload (trainDnwInfo);
		

		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
					"processSequenceDownload inserting train list, LibDataSize : %d",sharedPtrDwnload->stackInfo->libaryData.size());

		insert (sharedPtrDwnload);

		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
					"inserteed trains for download");


		FUNCTION_EXIT;
	}

	void TrainLibDownloadSequenceProc::processEvent (boost::shared_ptr <TrainDownloadInfo> sharedPtrData)
	{
		FUNCTION_ENTRY ("processEvent");
		
		TA_IRS_Bus::CommonTypes::TrainIdList::iterator iterTrainList = sharedPtrData->trainList.begin();
		
		LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
					"processEvent : processing train request for download, data size :%d",sharedPtrData->stackInfo->libaryData.size());

		for (; iterTrainList != sharedPtrData->trainList.end(); iterTrainList++)
		{	
			long lTrainId = *iterTrainList;
				
				setWaitTrain(lTrainId);
				
				TA_Base_Core::IWorkItemPtr newTask(
													new LibraryDownloadTask( *sharedPtrData->stackInfo->callback,
																			 lTrainId,
																			 sharedPtrData->stackInfo->ucOrigin,
																			 sharedPtrData->stackInfo->libaryData,
																			 sharedPtrData->stackInfo->lTrainversion,
																			 sharedPtrData->stackInfo->strSessionId)
												  );

				m_threadPool.queueWorkItem( newTask );

				initiateWait();

				LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,
							"Done Waiting for CallBack to post for the train %d",lTrainId);

		}

		FUNCTION_EXIT;
	}
}
