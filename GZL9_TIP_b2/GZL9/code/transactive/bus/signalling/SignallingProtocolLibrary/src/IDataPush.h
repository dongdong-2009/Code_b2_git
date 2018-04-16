#pragma once

#include <vector>

namespace TA_IRS_Bus
{
	class IDataPush
	{
		public:
			// interface called by the Telegram Filter to notify 
			// the class processor that the data is already finished 
			// filtering the multi flag
			virtual void pushData (std::vector <unsigned char> & vctrData) = 0;

	};

}
