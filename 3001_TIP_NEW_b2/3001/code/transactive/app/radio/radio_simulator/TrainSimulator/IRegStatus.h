
#ifndef _IREG_STATUS_H_
#define _IREG_STATUS_H_

namespace TA_IRS_App
{
	class IRegStatus
	{
	public:
		//Use to update if registration is true or false
		virtual void updateRegStatus(bool status) = 0;
	};
}

#endif