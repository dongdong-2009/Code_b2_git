

#include "app/ats/AtsAgent/AtsMethod/src/AtsEvent.h"
#include "app/ats/AtsAgent/AtsMethod/src/AtsEventReader.h"
#include "app/ats/AtsAgent/AtsMethod/src/AtsEventOa1Table.h"
#include "app/ats/AtsAgent/AtsMethod/src/AtsEventTrainStatusReport.h"

#include "app/ats/AtsAgent/AtsMethod/src/AtsMethodProtocol.h"
#include "core/utilities/src/TAAssert.h"
#include <string>
#include <vector>

using namespace TA_IRS_App;

AtsEvent::AtsEvent(void) :
AtsITcpEvent(),
m_eventReader(NULL)
{
}

AtsEvent::~AtsEvent(void)
{
	if(m_eventReader!=NULL){
		delete m_eventReader;
		m_eventReader = NULL;
	}
}

AtsEvent::AtsEvent(const AtsEvent& atsEvent): 
AtsITcpEvent(atsEvent),
m_eventReader(NULL)
{
}

/**
* Resets the command to be empty.
*/
void AtsEvent::clear()
{
   AtsITcpEvent::clear();
   if(m_eventReader){
	   delete m_eventReader;
	   m_eventReader = NULL;
   }
}

/**
* This method is used to determine if the message is complete.  It is used after appending incoming characters to the message.
*/
bool AtsEvent::isComplete() const
{
	// Check message size is complete
	if (m_buffer.size()<HEADER_LENGTH_LOCATION) return false;

	// Ensure the buffer size is as expected.
	return (getEventLength()==getCurrentLength());
}

/**
* Returns the Command type for the event.
*/
AtsEvent::EventType AtsEvent::getCommandType() const
{
    FUNCTION_ENTRY("getEventType");
    FUNCTION_EXIT;

	return readBufferByte(EVENT_HEADER_COMMAND);
}

/**
* Returns the Traind ID.
*/

AtsEvent::TrainIDType AtsEvent::getTrainId() const
{
	FUNCTION_ENTRY("getEventType");
    FUNCTION_EXIT;

	return readBufferByte(EVENT_HEADER_TRAIN_ID_NUMBER);
}

/**
* Return the specified length of the message as defined in the first bytes.
*/
unsigned long AtsEvent::getEventLength() const
{
    FUNCTION_ENTRY("getEventLength");
    FUNCTION_EXIT;

	return readBufferByte(EVENT_HEADER_LENGTH);
}

/**
* Return the specified length of the message as defined in the first bytes.
*/
unsigned long AtsEvent::getEventHeaderLength() const
{
    FUNCTION_ENTRY("getEventHeaderLength");
    FUNCTION_EXIT;
    return HEADER_LENGTH;
}

AtsEventReader* AtsEvent::getEventReader()
{
   FUNCTION_ENTRY("getEventReader");


	if (m_eventReader==NULL)
	{
		m_eventReader = createEventReader();
	}

    FUNCTION_EXIT;
	return m_eventReader;     
}


AtsEventReader* AtsEvent::createEventReader()
{
   	FUNCTION_ENTRY("createEventReader");
	if(!isComplete())
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
		"Mismatch in EventLength and RecievedLength. train Id: %lu, Command event: %lu",
		getTrainId(), getCommandType());
		FUNCTION_EXIT;
		return NULL;
	}

	AtsEventReader*  eventReader = NULL;
	boost::shared_ptr<AtsEvent> data(new AtsEvent(*this));
	
	switch (getCommandType())
	{
		//TBD		
	case EVENT_ATS_OA1TABLE:  
		eventReader =  new AtsEventOa1Table(data);
		break;

	case EVENT_ATS_TRAIN_STATUS_REPORT:
		eventReader = new AtsEventTrainStatusReport(data);
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