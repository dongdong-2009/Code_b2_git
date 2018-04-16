
#ifndef RADIO_METHOD_SYSTEM_STATUS_H
#define RADIO_METHOD_SYSTEM_STATUS_H

#include <string>
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethod.h"

namespace TA_IRS_App
{
	class RadioMethodSystemStatus : public RadioMethod
	{
	public:
		RadioMethodSystemStatus(bool occ);
		virtual ~RadioMethodSystemStatus();
	private:
		bool isOcc;
	};
};
#endif // end of RadioMethodSystemStatus