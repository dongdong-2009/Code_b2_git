
#ifndef RADIO_METHOD_CANCEL_CALL_FORWARDING_H
#define RADIO_METHOD_CANCEL_CALL_FORWARDING_H

#include <string>
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethod.h"

namespace TA_IRS_App
{
	class RadioMethodCancelCallFowarding : public RadioMethod
	{
	public:
		RadioMethodCancelCallFowarding(unsigned long issi);
		virtual ~RadioMethodCancelCallFowarding();
	public:
		std::string getISSI(unsigned long issi);
	};
};
#endif // end of RADIO_METHOD_CANCEL_CALL_FORWARDING_H