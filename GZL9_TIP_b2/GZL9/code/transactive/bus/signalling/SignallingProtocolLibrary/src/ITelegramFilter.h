#pragma once

#include "bus/signalling/SignallingProtocolLibrary/src/IDataPush.h"
#include <vector>

namespace TA_IRS_Bus
{
	class ITelegramFilter
	{
		public:
			virtual void filterTelegram (std::vector <unsigned char> & vctrData) = 0;
	};

}
