
#ifndef _IADD_TRAIN_HEART_BEAT_H_
#define _IADD_TRAIN_HEART_BEAT_H_

#include "app/radio/radio_simulator/TrainSimulator/GlobalDataTypes.h"

namespace TA_IRS_App
{
	class ITrainHeartBeat
	{
	public:
		virtual void addTrainHeartbeat(int trainID, int delay) = 0;
		virtual void sendStatusTrainHeartBeat(TrainStatus stat, int trainID) = 0;
		virtual void addTrainResponseStrategy(const TrainStrategyInfo& info) = 0;
		virtual void removeTrainResponseStrategy(int trainID) = 0;
	};
}
#endif