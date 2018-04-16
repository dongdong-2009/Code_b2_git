
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventReader.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEvent.h"
#include "core/utilities/src/TAAssert.h"

#define STATUS_OK 1
using namespace TA_IRS_App;

RadioEventReader::RadioEventReader(boost::shared_ptr<RadioEvent> event,EventType eventType,EventLengthType parameterLength)
				 : m_event(event), is_valid(false)
{
	m_eventTemp = m_event.lock();
	// Check the type and size matches the expected event!
	//TA_ASSERT(eventType==event.getEventType(),"Reader type does not match event type.");
	//TA_ASSERT(parameterLength==event.getParameterLength(),"Reader parameter length does not match event parameter length.");
	if(eventType==event->getEventType() && event->getParameterLength()==parameterLength)
		is_valid=true;
	else
	{
		std::string error_msg;
		if(eventType!=event->getEventType())
		{
			error_msg="Mismatch in EventType";
		}
		if(event->getParameterLength()!=parameterLength)
		{
			if(error_msg.length()>0)
				error_msg+=" and ParameterLength";
			else
				error_msg="Mismatch in ParameterLength";
		}
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
			"%s",error_msg.c_str());
	}
}
RadioEventReader::RadioEventReader(boost::shared_ptr<RadioEvent> event,EventType eventType)
				 : m_event(event), is_valid(true)
{
	m_eventTemp = m_event.lock();
	// Check the type and size matches the expected event!
	//TA_ASSERT(eventType==event->getEventType(),"Reader type does not match event type.");
}

RadioEventReader::~RadioEventReader()
{
}

short RadioEventReader::getParameterShort(unsigned int position) const
{
	return m_event.lock()->getParameterShort(position);
}


unsigned long RadioEventReader::getParameterLong(unsigned int position) const
{
	return m_event.lock()->getParameterLong(position);
}


std::string RadioEventReader::getParameterShortString(unsigned int position, unsigned int eventLength) const
{
	std::string   value;

	//TODO
	for (unsigned int index=0;index<eventLength;index++)
	{
		char   nextChar = m_event.lock()->getParameterChar(position+index);
		if ((nextChar=='\0')||(nextChar=='\n')) break;
		value += nextChar;
	}
	return value;
}

unsigned char RadioEventReader::getParameterChar(unsigned int position) const
{
	return m_event.lock()->getParameterChar(position);
}
bool RadioEventReader::isEventReaderValid()
{
return is_valid;
}
bool RadioEventReader::isStatusOK()
{
    // STATUS_OK declared in RadioProtocol.h
    return STATUS_OK == m_event.lock()->getStatus();
}
