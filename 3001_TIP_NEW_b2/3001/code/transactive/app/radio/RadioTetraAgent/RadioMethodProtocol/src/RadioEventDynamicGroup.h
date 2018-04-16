
#ifndef RADIO_EVENT_DYNAMIC_GROUP_H
#define RADIO_EVENT_DYNAMIC_GROUP_H

#include <string>
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventReader.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

namespace TA_IRS_App
{
	class RadioEventDynamicGroup : public RadioEventReader
	{
	public:
		RadioEventDynamicGroup(boost::shared_ptr<RadioEvent> event);
		virtual ~RadioEventDynamicGroup();

	public:

		/**
		 * Return the DGNA Status if it is either successful/unsuccessful                                                                 
		 **/
		char getStatus();
	};
};
#endif // end of RADIO_EVENT_DYNAMIC_GROUP_H