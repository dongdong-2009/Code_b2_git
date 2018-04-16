
#ifndef RADIO_METHOD_QUERY_CALL_FORWARDING_H
#define RADIO_METHOD_QUERY_CALL_FORWARDING_H

#include <string>
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethod.h"

namespace TA_IRS_App
{
	class RadioMethodQueryCallFowarding : public RadioMethod
	{
	public:
		RadioMethodQueryCallFowarding(unsigned long issi);
		virtual ~RadioMethodQueryCallFowarding();
	public:
		std::string getISSI(unsigned long issi);
	};
};
#endif // end of RADIO_METHOD_QUERY_CALL_FORWARDING_H