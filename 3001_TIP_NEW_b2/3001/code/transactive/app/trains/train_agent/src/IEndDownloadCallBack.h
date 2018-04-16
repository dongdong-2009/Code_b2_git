
#pragma once 

#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"

namespace TA_IRS_App
{
	class IEndDownloadCallBack
	{
		public:
			virtual void processEndDownload (TA_IRS_Bus::CommonTypes::TrainIdType trainId) = 0;
	};
}
