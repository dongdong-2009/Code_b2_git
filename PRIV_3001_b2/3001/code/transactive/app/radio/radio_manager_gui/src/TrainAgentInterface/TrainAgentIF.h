#pragma once

#include "bus/trains/TrainCorbaProxy/src/TrainDriverCallCorbaProxy.h"
#include "app/radio/radio_manager_gui/src/TrainAgentInterface/ITrainAgentIF.h"

namespace TA_IRS_App
{
	class TrainAgentIF : public ITrainAgentIF
	{
		public:
			TrainAgentIF(void);
			~TrainAgentIF(void);

			virtual bool answerDriverCall (unsigned long lTrainId);

		private:
			TA_IRS_Bus::TrainDriverCallCorbaProxyPtr m_trainDriverCallCorbaProxy;
	};
	
}
