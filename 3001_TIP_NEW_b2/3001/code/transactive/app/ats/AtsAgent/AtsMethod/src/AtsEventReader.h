
#ifndef ATS_EVENT_READER_H
#define ATS_EVENT_READER_H

#include "app/ats/AtsAgent/AtsMethod/src/AtsMethodProtocol.h"
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace TA_IRS_App
{
	class AtsEvent;

	typedef unsigned char EventType;
	typedef unsigned long EventLengthType;

	class AtsEventReader
	{
	public:
		AtsEventReader(boost::shared_ptr<AtsEvent> event, EventType eventType,EventLengthType parameterLength);

		/**
         * Constructor
         *
         * Used to construct a RadioEventReader where the length of the message is variable 
         * (i.e. contains a LONGSTRING)
         */
	   AtsEventReader(boost::shared_ptr<AtsEvent> event, EventType eventType);
			virtual ~AtsEventReader();

			const boost::shared_ptr<AtsEvent> getEvent() const { return m_event.lock(); };
              //Note: To be removed.
			//bool isStatusOK();
			bool isEventReaderValid();
			
	protected:

			short getParameterShort(unsigned int position) const;
			//unsigned long getParameterLong(unsigned int position) const;
			std::string getParameterShortString(unsigned int position) const;
			unsigned char getParameterChar(unsigned int position) const;

	private:
			bool is_valid;
			boost::weak_ptr<AtsEvent>   m_event;

			//Create a temporary shared_ptr from m_event to avoid dangling pointer
			boost::shared_ptr<AtsEvent> m_eventTemp;
	};
};

#endif // end of ATS_EVNENT_READER_H