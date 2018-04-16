

#include "app//radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventReader.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEvent.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

//Event
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventProfileRequest.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventSubscriberMonitor.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventDynamicGroup.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventCallFowardRequest.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventCancelCallForwarding.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventQueryCallForwarding.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventProfileUpdate.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventSystemStatus.h"


#include <string>
#include <vector>
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;

//TODO : clean up all the unused definition
#define EVENT_HEADER_LENGTH                          5
#define EVENT_HEADER_LENGTH_START_POSITION           0
#define EVENT_HEADER_TRAIN_ID_START_POSITION         3
#define EVENT_HEADER_TRANSACTION_ID_START_POSITION   1
#define EVENT_LENGTH_POSITION                        4
#define EVENT_TYPE_START_POSITION                    1
#define EVENT_HEADER_LENGTH_FLAG_POSITION            0
#define EVENT_STATUS_POSITION                        6  


RadioEvent::RadioEvent(void) :
ITcpEvent(),
m_eventReader(NULL),
eventLength(0)
{
}

RadioEvent::~RadioEvent(void)
{
	if(m_eventReader!=NULL){
		delete m_eventReader;
		m_eventReader = NULL;
	}
}

RadioEvent::RadioEvent(const RadioEvent& radioEvent): 
ITcpEvent(radioEvent),
m_eventReader(NULL)
{
}

/**
* Resets the command to be empty.
*/
void RadioEvent::clear()
{
   ITcpEvent::clear();
   if(m_eventReader){
	   delete m_eventReader;
	   m_eventReader = NULL;
   }
}

/**
* This method is used to determine if the message is complete.  It is used after appending incoming characters to the message.
*/
bool RadioEvent::isComplete() const
{
	// Check message size is complete
	if (m_buffer.size()<getEventLength()) 
		return false;
	else
		return true;
}

/**
* Returns the transaction Id for the event which will correspond to the Transaction ID of the associated Method called.
*/
RadioEvent::TransactionIDType RadioEvent::getTransactionId() const
{
	

	return readBufferShort(METHOD_HEADER_TRANSACTION_ID_START_POSITION);
}


/**
* Returns the status code for the event
*/
RadioEvent::StatusType RadioEvent::getStatus() const
{
	unsigned char status = readBufferByte(EVENT_STATUS_POSITION);

	// Force it back to a long. (Use long rather than type to ensure it will break if StatusType is changed)
	return status;
}

std::string RadioEvent::getTextStatus() const
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
		text = "Unknown Radio error";
	}
	return text ;

}


/**
* Returns the radio event type for the event.
*/
RadioEvent::EventType RadioEvent::getEventType() const
{
    FUNCTION_ENTRY("getEventType");
    FUNCTION_EXIT;
	
	EventType result = 0;
	unsigned long len = getEventLength();
	switch(len)
	{
	case EVENT_LENGTH_PROFILE_REQUEST: //165 bytes
		if( checkMessageID(getMessageID(17),MESSAGE_ID_PROFILE_REQUEST) ) result = TYPE_PROFILE_REQUEST;
		break;
	case EVENT_LENGTH_SUBSCRIBER_MONITORING: //58 bytes
		if( checkMessageID(getMessageID(54),MESSAGE_ID_SUBSCRIBER_MONITORING) ) result = TYPE_SUBSCRIBER_MONITOR;
		break;
	case EVENT_LENGTH_DGNA_QUERY: //46 bytes
		if( checkMessageID(getMessageID(42),MESSAGE_ID_DGNA_REQUEST) ) result = TYPE_DGNA_REQUEST;
		else if( checkMessageID(getMessageID(17),MESSAGE_ID_QUERY_CALL_FORWARDING) )result = TYPE_QUERY_CALL_FORWARDING;
		break;
	case EVENT_LENGTH_CALL_CANCEL_REQUEST: //22 bytes
		if( checkMessageID(getMessageID(18),MESSAGE_ID_CALL_FORWARDING_REQUEST) ) result = TYPE_CALL_FORWARDING_REQUEST;
		else if( checkMessageID(getMessageID(18),MESSAGE_ID_CANCEL_CALL_FORWARDING) ) result = TYPE_CANCEL_CALL_FORWARDING;
		break;
	case EVENT_LENGTH_RADIO_SYSTEM_STATUS: // 8 bytes
		if (checkMessageID(getMessageID(4), MESSAGE_ID_OCC_RADIO_SYSTEM_STATUS) 
			|| checkMessageID(getMessageID(4), MESSAGE_ID_STATION_RADIO_SYSTEM_STATUS)) result = TYPE_RADIO_SYSTEM_STATUS;
		break;
	default:
		// check for non-fixed length types
		if ((len - EVENT_LENGTH_BASE_PROFILE_UPDATE) % EVENT_LENGTH_VARIABLE_PROFILE_UPDATE == 0) {
			if (checkMessageID(getMessageID(17), MESSAGE_ID_PROFILE_UPDATE)) result = TYPE_PROFILE_UPDATE;
		}
	}
	return result;
}
/**
 * Return the Message ID of the specified position
 */
std::string RadioEvent::getMessageID(int startPosition) const
{
	FUNCTION_ENTRY("getMessageID");
	FUNCTION_EXIT;
	std::string messageId;
	for(int index=0;index<3;index++)
		messageId += m_buffer[startPosition+index];
	return messageId;
}
/**
 * Check if the message ID is correct
 */
bool RadioEvent::checkMessageID(std::string eventMessageID, std::string checkMessage) const
{

	FUNCTION_ENTRY("checkMessageID");
	FUNCTION_EXIT;
	if(!(eventMessageID.compare(checkMessage)))
		return true;
	else
		return false;
}
/**
* Return the specified length of the message as defined in the first bytes.
*/
unsigned long RadioEvent::getEventLength() const
{
    FUNCTION_ENTRY("getEventLength");
    FUNCTION_EXIT;

	//return readBufferByte(EVENT_LENGTH_POSITION);
	//return getBufferLength();
	return eventLength;
}

void RadioEvent::setEventLength(unsigned int evntLength) 
{
	FUNCTION_ENTRY("setEventLength");
	FUNCTION_EXIT;
	eventLength = evntLength;
}
/**
* Return the specified length of the message as defined in the first bytes.
*/
unsigned long RadioEvent::getEventHeaderLength() const
{
    FUNCTION_ENTRY("getEventHeaderLength");
    FUNCTION_EXIT;
    return EVENT_HEADER_LENGTH;
}

RadioEventReader* RadioEvent:: getEventReader()
{
   FUNCTION_ENTRY("getEventReader");


	if (m_eventReader==NULL)
	{
		m_eventReader = createEventReader();
	}

    FUNCTION_EXIT;
	return m_eventReader;     
}

RadioEventReader* RadioEvent::createEventReader()
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

	RadioEventReader*  eventReader = NULL;
	boost::shared_ptr<RadioEvent> data(new RadioEvent(*this));
	
	switch (getEventType())
	{
		//TBD		
	case TYPE_PROFILE_REQUEST:  
		eventReader =  new RadioEventProfileRequest(data);
		break;

	case TYPE_SUBSCRIBER_MONITOR:
		eventReader = new RadioEventSubscriberMonitor(data);
		break;
	case TYPE_DGNA_REQUEST:
		eventReader = new RadioEventDynamicGroup(data);
		break;
	case TYPE_CALL_FORWARDING_REQUEST:
		eventReader = new RadioEventCallForwardRequest(data);
		break;
	case TYPE_CANCEL_CALL_FORWARDING:
		eventReader = new RadioEventCancelCallForwarding(data);
		break;
	case TYPE_QUERY_CALL_FORWARDING:
		eventReader = new RadioEventQueryCallForwarding(data);
		break;	  
	case TYPE_PROFILE_UPDATE:
		eventReader = new RadioEventProfileUpdate(data);
		break;
	case TYPE_RADIO_SYSTEM_STATUS:
		eventReader = new RadioEventSystemStatus(data);
		break;
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