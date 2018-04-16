
#ifndef RADIO_EVENT_QUERY_CALL_FORWARDING_H
#define RADIO_EVENT_QUERY_CALL_FORWARDING_H

#include <string>
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventReader.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

namespace TA_IRS_App
{
	class RadioEventQueryCallForwarding : public RadioEventReader
	{
	public:
		RadioEventQueryCallForwarding(boost::shared_ptr<RadioEvent> event);
		virtual ~RadioEventQueryCallForwarding();

	public:

		/**
		 * Return the Condition of the Query Call Forwarding                                                               
		 **/
		unsigned short getCondition();
		
		/**
		 * Return the Forwarding ISSI of the Query Call Forwarding                                                               
		 **/
		unsigned long getForwardingISSI();

		/**
		 * Return the Forwarded ISSI of the Query Call Forwarding                                                               
		 **/
		unsigned long getForwardedISSI();

		/**
		 * Return the converted unsigned long long from the string data                                                              
		 **/
		unsigned long getLong(std::string strData);


	};
};
#endif // end of RADIO_EVENT_QUERY_CALL_FORWARDING_H