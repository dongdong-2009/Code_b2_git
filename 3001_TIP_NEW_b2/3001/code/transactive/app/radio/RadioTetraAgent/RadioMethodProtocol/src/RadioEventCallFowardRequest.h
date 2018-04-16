
#ifndef RADIO_EVENT_CALL_FORWARD_REQUEST_H
#define RADIO_EVENT_CALL_FORWARD_REQUEST_H

#include <string>
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventReader.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

namespace TA_IRS_App
{
	class RadioEventCallForwardRequest : public RadioEventReader
	{
	public:
		RadioEventCallForwardRequest(boost::shared_ptr<RadioEvent> event);
		virtual ~RadioEventCallForwardRequest();

	public:

		/**
		 * Return the Status of the Call Forward Request, NOK = 0, OK = 1                                                                 
		 **/
		bool getStatus();
	};
};
#endif // end of RADIO_EVENT_CALL_FORWARD_REQUEST_H