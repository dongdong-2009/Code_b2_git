
#ifndef _TRAIN_STATUS6_H_
#define _TRAIN_STATUS6_H_
#include "ITrainStatus.h"

namespace TA_IRS_App
{
	class TrainStatus6 : public ITrainStatus
	{
	public:
		std::vector<std::string>& getStatusData()
		{
			static std::vector<std::string> statusData;
			
			//Use for first initialization of data
			if ( statusData.empty() )
			{
				statusData.push_back("Proceeding");
				statusData.push_back("In use");
				statusData.push_back("Failed");
			}
			
			return statusData;
		}
	};
}
#endif