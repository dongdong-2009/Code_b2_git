#pragma once

namespace TA_IRS_Bus
{
	class ICallFailureObserver
	{
		public:
			virtual void notifyCallTermination (unsigned long callId) = 0;
	};

}
