
#ifndef RADIO_METHOD_CALL_FORWARD_REQUEST_H
#define RADIO_METHOD_CALL_FORWARD_REQUEST_H

#include <string>
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethod.h"

namespace TA_IRS_App
{
	class RadioMethodCallForwardRequest : public RadioMethod
	{
	public:
		RadioMethodCallForwardRequest(bool condition, unsigned long forwardingISSI, unsigned long forwardedISSI);
		virtual ~RadioMethodCallForwardRequest();
	public:
		std::string getISSI(unsigned long ssi);
	};
};
#endif // end of RadioMethodCallForwardRequest