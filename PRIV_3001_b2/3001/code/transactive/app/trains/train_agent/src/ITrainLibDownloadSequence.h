#pragma once

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TtisTypes.h"
#include "app/trains/train_agent/src/ITimsTisManagerTaskCallback.h"

namespace TA_IRS_App
{

	struct TrainLibStackInfo
	{
		std::string strSessionId;
		unsigned long lTrainversion;
		unsigned char ucOrigin;
		TA_IRS_Bus::TtisTypes::LibraryDataType libaryData;
		ITimsTisManagerTaskCallback * callback;
	};

	struct TrainDownloadInfo
	{
		TrainDownloadInfo ()
		{

		}
		
		~TrainDownloadInfo ()
		{
			if (stackInfo)
			{
				delete stackInfo;
				stackInfo = NULL;
			}
		}
		
		TrainLibStackInfo * stackInfo;
		TA_IRS_Bus::CommonTypes::TrainIdList trainList;

	};

	
	class ITrainLibDownloadSequence
	{
		public:
			virtual void setWaitTrain (TA_IRS_Bus::CommonTypes::TrainIdType trainId) = 0;
			virtual void initiateWait () = 0;
			virtual void processSequenceDownload (TA_IRS_Bus::CommonTypes::TrainIdList trainIdList, TrainLibStackInfo * trainDownloadInfo) = 0;
	};

}
