
#ifndef  RadioPDSEventReader_H
#define  RadioPDSEventReader_H

#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEvent.h"
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include "core/utilities/src/DebugUtil.h"
#include <vector>

namespace TA_IRS_App
{
	class RadioPDSEvent;

	typedef unsigned char   EventType;
	typedef unsigned char   EventStatus;
	typedef unsigned long   EventLengthType;

	class RadioPDSEventReader
	{
	public:

		RadioPDSEventReader( boost::shared_ptr<RadioPDSEvent> event, 
			                 EventType evenMessageID );

		virtual ~RadioPDSEventReader();

		bool isEventReaderValid();
		EventStatus getStatus() const;
		std::vector<unsigned char> getDataHeader() const;
		std::vector<unsigned char> getProtocolHeader() const;

	private:
		bool m_IsValid;
		boost::weak_ptr<RadioPDSEvent>   m_event;

		//Create a temporary shared_ptr from m_event to avoid dangling pointer
		boost::shared_ptr<RadioPDSEvent> m_eventTemp;

		static const unsigned long EVENT_DATA_START_POSITION;
		static const unsigned long EVENT_START_POSITION;
		static const unsigned long EVENT_CRC_SIZE;

	};
}
#endif