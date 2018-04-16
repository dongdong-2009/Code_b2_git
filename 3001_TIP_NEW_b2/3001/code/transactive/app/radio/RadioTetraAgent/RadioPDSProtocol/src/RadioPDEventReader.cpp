
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEventReader.h"
#include "app/radio/RadioTetraAgent/RadioPDSProtocol/src/RadioPDSEvent.h"


#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{
	
	const unsigned long RadioPDSEventReader::EVENT_DATA_START_POSITION = 6;
	const unsigned long RadioPDSEventReader::EVENT_START_POSITION = 0;
	const unsigned long RadioPDSEventReader::EVENT_CRC_SIZE = 2;

	RadioPDSEventReader::RadioPDSEventReader( boost::shared_ptr<RadioPDSEvent> event, 
											  EventType eventType)
											  : m_IsValid(false),
												m_event(event)
	{
		FUNCTION_ENTRY( "RadioPDSEventReader" ); 

		m_eventTemp = m_event.lock();
		// Check the type and size matches the expected event!
		//TA_ASSERT(eventType==event.getEventType(),"Reader type does not match event type.");
		if(eventType==event->getEventType())
			m_IsValid=true;
		else
		{
			std::string error_msg;
			if(eventType!=event->getEventType())
			{
				error_msg="Mismatch in EventType";
			}
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
				"%s",error_msg.c_str());
		}

		FUNCTION_EXIT;
	}

	RadioPDSEventReader::~RadioPDSEventReader()
	{
		FUNCTION_ENTRY( "~RadioPDSEventReader" );
		FUNCTION_EXIT;
	}


	bool RadioPDSEventReader::isEventReaderValid()
	{
		FUNCTION_ENTRY( "isEventReaderValid" );
		FUNCTION_EXIT;
		return m_IsValid;
	}

	EventStatus RadioPDSEventReader::getStatus() const
	{
		FUNCTION_ENTRY( "getStatus" );
		FUNCTION_EXIT;
		return m_event.lock()->getStatus();
	}

	std::vector<unsigned char> RadioPDSEventReader::getDataHeader() const
	{
		FUNCTION_ENTRY( "getDataHeader" );
		FUNCTION_EXIT;
		std::vector<unsigned char> dataHeader;
		for (int position = EVENT_DATA_START_POSITION;
			 position != ( m_event.lock()->getRawData().size() - EVENT_CRC_SIZE ); position++ )
		{
			dataHeader.push_back(m_event.lock()->getRawData().at(position));
		}
		return dataHeader;
	}

	std::vector<unsigned char> RadioPDSEventReader::getProtocolHeader() const
	{
		FUNCTION_ENTRY( "getProtocolHeader" );
		FUNCTION_EXIT;
		std::vector<unsigned char> dataProtocolHeader;
		for (int position = EVENT_START_POSITION; position != EVENT_DATA_START_POSITION; position++)
		{
			dataProtocolHeader.push_back(m_event.lock()->getRawData().at(position));
		}
		return dataProtocolHeader;
	}
}