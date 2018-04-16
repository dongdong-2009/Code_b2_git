
#ifndef RADIO_EVENT_PROFILE_REQUEST_H
#define RADIO_EVENT_PROFILE_REQUEST_H

#include <string>
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventReader.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

namespace TA_IRS_App
{
	class RadioEventProfileRequest : public RadioEventReader
	{
	public:
		RadioEventProfileRequest(boost::shared_ptr<RadioEvent> event);
		virtual ~RadioEventProfileRequest();

	public:

		/**
		 * Return the Talkgroup Allias                                                                  
		 **/
		std::vector<std::string> getTalkgroup();
	};
};
#endif // end of RADIO_EVENT_PROFILE_REQUEST_H