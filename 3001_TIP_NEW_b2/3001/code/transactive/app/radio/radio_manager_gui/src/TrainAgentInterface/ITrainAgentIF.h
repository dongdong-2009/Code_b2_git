

#pragma once

namespace TA_IRS_App
{
	class ITrainAgentIF
	{
		public:
			virtual bool answerDriverCall (unsigned long lTrainId) = 0;
	};
}