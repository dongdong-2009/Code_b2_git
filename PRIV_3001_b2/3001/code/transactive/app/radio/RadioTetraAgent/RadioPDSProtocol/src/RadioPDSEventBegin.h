
#ifndef RadioPDSEventBegin_H
#define RadioPDSEventBegin_H

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEventReader.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEvent.h"

namespace TA_IRS_App
{
	class RadioPDSEventBegin : public RadioPDSEventReader
	{
		public:
			RadioPDSEventBegin(boost::shared_ptr<RadioPDSEvent> event);
			virtual ~RadioPDSEventBegin();

		private:
			static const unsigned long EVENT_MESSAGE_TYPE;
	};
}
#endif