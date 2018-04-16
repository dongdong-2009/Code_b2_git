

#include "app/cctv/video_switch_agent/TVSSProtocolLibrary/src/ITcpEvent.h"
#include "app/cctv/video_switch_agent/TVSSProtocolLibrary/src/TVSSEventReader.h"

#include "TVSSEvent.h"

#include <string>
#include <vector>
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;

#define EVENT_HEADER_LENGTH                          5
#define EVENT_HEADER_LENGTH_START_POSITION           0
#define EVENT_HEADER_TRAIN_ID_START_POSITION         3
#define EVENT_HEADER_TRANSACTION_ID_START_POSITION   2
#define EVENT_LENGTH_POSITION                        4
#define EVENT_TYPE_START_POSITION                   1
#define EVENT_HEADER_LENGTH_FLAG_POSITION            0
#define EVENT_STATUS_POSITION                        6  

TVSSEvent::TVSSEvent(void) :ITcpEvent () 
{
}

TVSSEvent::~TVSSEvent(void)
{
}

TVSSEvent::TVSSEvent(const TVSSEvent& tvssEvent): ITcpEvent ( tvssEvent) 
{
}

/**
* Resets the command to be empty.
*/
void TVSSEvent::clear()
{
   ITcpEvent::clear();
   //delete m_eventReader;
  // m_eventReader = NULL;
}

/**
* This method is used to determine if the message is complete.  It is used after appending incoming characters to the message.
*/
bool TVSSEvent::isComplete() const
{
	// Check message size is complete
	if (m_buffer.size()<EVENT_HEADER_LENGTH_START_POSITION+4) return false;

	// Ensure the buffer size is as expected.
	return (getEventLength()==getCurrentLength());
}

/**
* Returns the transaction Id for the event which will correspond to the Transaction ID of the associated Method called.
*/
TVSSEvent::TransactionIDType TVSSEvent::getTransactionId() const
{
	

	return readBufferByte(EVENT_HEADER_TRANSACTION_ID_START_POSITION);
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

TVSSEventReader* TVSSEvent:: createEventReader()
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

	switch (getEventType())
	{
		//TBD 
	}
		if(eventReader!=NULL)
	if(!eventReader->isEventReaderValid())
	{
		delete eventReader;
		eventReader=NULL;
	}
	FUNCTION_EXIT;

	return eventReader;


}
