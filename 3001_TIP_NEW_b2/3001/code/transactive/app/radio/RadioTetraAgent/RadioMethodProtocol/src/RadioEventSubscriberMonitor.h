

#ifndef RADIO_EVENT_SUBSCRIBER_MONITOR_H
#define RADIO_EVENT_SUBSCRIBER_MONITOR_H

#include <string>
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventReader.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

namespace TA_IRS_App
{
	class RadioEventSubscriberMonitor : public RadioEventReader
	{
	public:
		RadioEventSubscriberMonitor(boost::shared_ptr<RadioEvent> event);
		virtual ~RadioEventSubscriberMonitor();

	public:

		/**
		 * Return the Talkgroup Alias                                                                  
		 **/
		std::string getTalkgroupAlias();

		/**
		 * Return the Base Station Alias                                                                  
		 **/
		std::string getBaseStaionAlias();
	};
};
#endif // end of RADIO_EVENT_SUBSCRIBER_MONITOR_H