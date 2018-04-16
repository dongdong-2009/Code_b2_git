
#ifndef _ITRAIN_STATUS_H_
#define _ITRAIN_STATUS_H_
#include <vector>
#include <string>

namespace TA_IRS_App
{
	class ITrainStatus
	{
	public:
		virtual std::vector<std::string>& getStatusData() = 0;
	};
}
#endif