#pragma once

#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace TA_IRS_App
{
class TVSSEvent ;

	typedef unsigned char   EventType;
	typedef unsigned long   EventLengthType;

class TVSSEventReader
{
   public:
	   TVSSEventReader(boost::shared_ptr<TVSSEvent> event, EventType eventType,EventLengthType parameterLength);

	   TVSSEventReader(boost::shared_ptr<TVSSEvent> event, EventType eventType,EventLengthType parameterLength,EventLengthType parameterLength_OCC);

            /**
             * Constructor
             *
             * Used to construct a RadioEventReader where the length of the message is variable 
             * (i.e. contains a LONGSTRING)
             */
	   TVSSEventReader(boost::shared_ptr<TVSSEvent> event, EventType eventType);
			virtual ~TVSSEventReader();

			const boost::shared_ptr<TVSSEvent> getEvent() const { return m_event.lock(); };
            bool isStatusOK();
			bool isEventReaderValid();
		protected:

			short getParameterShort(unsigned int position) const;
			unsigned long getParameterLong(unsigned int position) const;
			std::string getParameterShortString(unsigned int position) const;
			unsigned char getParameterChar(unsigned int position) const;

		private:
			bool is_valid;
			boost::weak_ptr<TVSSEvent>   m_event;

			//Create a temporary shared_ptr from m_event to avoid dangling pointer
			boost::shared_ptr<TVSSEvent> m_eventTemp;

	};
};
