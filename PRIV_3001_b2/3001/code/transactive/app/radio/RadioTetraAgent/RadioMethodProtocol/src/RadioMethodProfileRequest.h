
#ifndef RADIO_METHOD_PROFILE_REQUEST_H
#define RADIO_METHOD_PROFILE_REQUEST_H

#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethod.h"
#include<string>
#include <time.h>

namespace TA_IRS_App
{
	class RadioMethodProfileRequest : public RadioMethod
	{
	public:
		RadioMethodProfileRequest(const std::string dataMFTUser);
		virtual ~RadioMethodProfileRequest();
		
	};

};
#endif