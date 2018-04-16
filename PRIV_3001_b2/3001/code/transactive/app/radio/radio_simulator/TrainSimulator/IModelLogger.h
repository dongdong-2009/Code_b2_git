
#ifndef _I_MODEL_LOGGER_H_
#define _I_MODEL_LOGGER_H_

#include "app/radio/radio_simulator/MotorolaProtocolLib/src/CMTypes.h"


namespace TA_IRS_App
{
	class IModelLogger
	{
	public:
		virtual void setLogView(const std::string& log) = 0;
		virtual void setHeartBeatView(const HeartBeatInfo& info) = 0;
		//virtual void setHeartBeatView(const TrainHeartBeatProcessorInfo& info) = 0;
	};
}
#endif