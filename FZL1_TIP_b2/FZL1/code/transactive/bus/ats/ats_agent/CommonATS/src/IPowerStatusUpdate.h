
#ifndef _I_POWER_STATUS_UPDATE_H_
#define _I_POWER_STATUS_UPDATE_H_

namespace TA_IRS_Bus
{
	class IPowerStatusUpdate
	{
	public:
		virtual void powerStatusUpdated(unsigned long pkey,bool dpQuality,bool dpValue) = 0;
	};
}
#endif