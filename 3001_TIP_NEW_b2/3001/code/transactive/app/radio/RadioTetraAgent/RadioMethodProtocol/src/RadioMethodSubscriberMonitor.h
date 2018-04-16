
#ifndef RADIO_METHOD_SUBSCRIBER_MONITOR_H
#define RADIO_METHOD_SUBSCRIBER_MONITOR_H

#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethod.h"

namespace TA_IRS_App
{
	class RadioMethodSubscriberMonitor : public RadioMethod
	{
	public:
		RadioMethodSubscriberMonitor(unsigned long ssi);
		virtual ~RadioMethodSubscriberMonitor();
	public:
		std::string getISSI(unsigned long ssi);
	};
};
#endif