
#ifndef RADIO_EVENT_SYSTEM_STATUS_H
#define RADIO_EVENT_SYSTEM_STATUS_H

#include <string>
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventReader.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

namespace TA_IRS_App
{
	class RadioEventSystemStatus : public RadioEventReader
	{
	public:
		RadioEventSystemStatus(boost::shared_ptr<RadioEvent> event);
		virtual ~RadioEventSystemStatus();

	public:

		char getStatus();
	};
};
#endif // end of RADIO_EVENT_SYSTEM_STATUS_H