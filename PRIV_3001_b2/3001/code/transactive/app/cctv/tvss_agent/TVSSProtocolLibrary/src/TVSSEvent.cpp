

#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/ITcpEvent.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSEventReader.h"

#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSEvent.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSProtocol.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSCommandResponseEventReader.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSTrainborneEventReader.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSEquipmentResponseEventReader.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/crc16.h"

#include <string>
#include <vector>
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;

const unsigned int EVENT_HEADER_LENGTH = 6;
const unsigned int EVENT_HEADER_LENGTH_START_POSITION = 0;
const unsigned int EVENT_HEADER_TRAIN_ID_START_POSITION = 4;
const unsigned int EVENT_HEADER_TRANSACTION_ID_START_POSITION = 2;
const unsigned int EVENT_LENGTH_POSITION = 5;
const unsigned int EVENT_TYPE_START_POSITION = 1;
const unsigned int EVENT_STATUS_POSITION = 7;

// For method 1-5 response
const unsigned int EVENT_TYPE1_LENGTH = 10;
// Trainborne event trigger
const unsigned int EVENT_TYPE2_LENGTH = 19;
// Equipment alarm query response, OCC
const unsigned int EVENT_TYPE3_LENGTH = 250;
// Equipment alarm query response, Station
const unsigned int EVENT_TYPE4_LENGTH = 13;

TVSSEvent::TVSSEvent(void) :
ITcpEvent(),
m_eventReader(NULL)
{
}

TVSSEvent::~TVSSEvent(void)
{
	if(m_eventReader!=NULL)
	{
		delete m_eventReader;
		m_eventReader = NULL;
	}
}

TVSSEvent::TVSSEvent(const TVSSEvent& tvssEvent): 
ITcpEvent(tvssEvent),
m_eventReader(NULL)
{
}

/**
* Resets the command to be empty.
*/
void TVSSEvent::clear()
{
   ITcpEvent::clear();
   if(m_eventReader)
   {
	   delete m_eventReader;
	   m_eventReader = NULL;
   }
}

/**
* This method is used to determine if the message is complete.  It is used after appending incoming characters to the message.
*/
bool TVSSEvent::isComplete() const
{
	return getEventLength()==getCurrentLength();
}

bool TVSSEvent::isCompleteWithCrc()
{
	if(getBufferLength() == EVENT_TYPE4_LENGTH
		|| getBufferLength() == EVENT_TYPE3_LENGTH
		|| getBufferLength() == EVENT_TYPE2_LENGTH
		|| getBufferLength() == EVENT_TYPE1_LENGTH)
	{
		return isComplete();
	}
	return false;
}

/**
* Returns the transaction Id for the event which will correspond to the Transaction ID of the associated Method called.
*/
TVSSEvent::TransactionIDType TVSSEvent::getTransactionId() const
{
	//return readBufferByte(EVENT_HEADER_TRANSACTION_ID_START_POSITION);
	unsigned short trainId = readBufferShort(EVENT_HEADER_TRANSACTION_ID_START_POSITION);
	return trainId;
}


/**
* Returns the status code for the event
*/
TVSSEvent::StatusType TVSSEvent::getStatus() const
{
	unsigned char status = readBufferByte(EVENT_STATUS_POSITION);

	// Force it back to a long. (Use long rather than type to ensure it will break if StatusType is changed)
	return status;
}

std::string TVSSEvent::getTextStatus() const
{
   FUNCTION_ENTRY("getTextStatus");

	std::string  text;
	switch ( getStatus () ) 
	{
		//TBD
	case (STATUS_OK): 
		text = "Successful status";
		break;
	default:
		text = "Unknown TVSS error";
	}
	return text ;

}

/**
* Returns the radio event type for the event.
*/
TVSSEvent::EventType TVSSEvent::getEventType() const
{
    FUNCTION_ENTRY("getEventType");
    FUNCTION_EXIT;

	return readBufferByte(EVENT_TYPE_START_POSITION );
}

/**
* Return the specified length of the message as defined in the first bytes.
*/
unsigned long TVSSEvent::getEventLength() const
{
    FUNCTION_ENTRY("getEventLength");
    FUNCTION_EXIT;
	return readBufferByte(EVENT_LENGTH_POSITION);
}

/**
* Return the specified length of the message as defined in the first bytes.
*/
unsigned long TVSSEvent::getEventHeaderLength() const
{
    FUNCTION_ENTRY("getEventHeaderLength");
    FUNCTION_EXIT;
    return EVENT_HEADER_LENGTH;
}

TVSSEventReader* TVSSEvent:: getEventReader()
{
   FUNCTION_ENTRY("getEventReader");


	if (m_eventReader==NULL)
	{
		m_eventReader = createEventReader();
	}

    FUNCTION_EXIT;
	return m_eventReader;     
}

TVSSEventReader* TVSSEvent::createEventReader()
{
   	FUNCTION_ENTRY("createEventReader");
	if(!isComplete())
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
		"Mismatch in EventLength and RecievedLength. transaction: %lu, event: %lu",
		getTransactionId(), getEventType());
		FUNCTION_EXIT;
		return NULL;
	}

	TVSSEventReader*  eventReader = NULL;
	boost::shared_ptr<TVSSEvent> data(new TVSSEvent(*this));
	
	switch (getEventType())
	{
		//TBD		
	case EVENT_COMMAND_RESPONSE:  
		eventReader =  new TVSSCommandEventReader(data);
		break;

	case EVENT_TRAINBORNE_EVENT:
		eventReader = new TVSSTrainborneEventReader(data);
		break;

	case EVENT_EQUIPMENT_ALARM_RESPONSE:
		eventReader = new TVSSEquipmentResponseEventReader(data);
		break;
	  
	}

	if(eventReader!=NULL)
	{
		if(!eventReader->isEventReaderValid())
		{
			delete eventReader;
			eventReader=NULL;
		}
	}
	FUNCTION_EXIT;

	return eventReader;
}


void TVSSEvent::peelOffCrc()
{
	if(isCompleteWithCrc())
	{
		m_buffer[EVENT_LENGTH_POSITION] = m_buffer[EVENT_LENGTH_POSITION] - 2;
		m_buffer.resize(m_buffer.size()-2);
	}
}

bool TVSSEvent::crcCheck() const
{
	if(getBufferLength() < 2)
	{
		return false;
	}
	unsigned short crc = ((unsigned short)m_buffer[m_buffer.size()-1] << 8) + m_buffer[m_buffer.size()-2];
	unsigned char buffer[2000];
	std::copy(m_buffer.begin(), m_buffer.end(), buffer);
	return crc16(buffer, getBufferLength()-2) == crc;
}