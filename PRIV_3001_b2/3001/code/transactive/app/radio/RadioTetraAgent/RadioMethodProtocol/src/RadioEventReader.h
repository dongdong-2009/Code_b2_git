#pragma once

//#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"

#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEvent.h"
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace TA_IRS_App
{
class RadioEvent ;

	typedef unsigned char   EventType;
	typedef unsigned long   EventLengthType;


class RadioEventReader
{
   public:
	   RadioEventReader(boost::shared_ptr<RadioEvent> event, EventType eventMessageId,EventLengthType parameterLength);

            /**
             * Constructor
             *
             * Used to construct a RadioEventReader where the length of the message is variable 
             * (i.e. contains a LONGSTRING)
             */
	   RadioEventReader(boost::shared_ptr<RadioEvent> event, EventType eventType);
			virtual ~RadioEventReader();

			const boost::shared_ptr<RadioEvent> getEvent() const { return m_event.lock(); };
            bool isStatusOK();
			bool isEventReaderValid();
		protected:

			short getParameterShort(unsigned int position) const;
			unsigned long getParameterLong(unsigned int position) const;
			std::string getParameterShortString(unsigned int position, unsigned int eventLength) const;
			unsigned char getParameterChar(unsigned int position) const;

		private:
			bool is_valid;
			boost::weak_ptr<RadioEvent>   m_event;

			//Create a temporary shared_ptr from m_event to avoid dangling pointer
			boost::shared_ptr<RadioEvent> m_eventTemp;

	};
};
