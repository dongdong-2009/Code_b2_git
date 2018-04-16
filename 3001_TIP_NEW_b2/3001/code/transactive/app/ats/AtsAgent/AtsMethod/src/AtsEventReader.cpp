
#include "app/ats/AtsAgent/AtsMethod/src/AtsEventReader.h"
#include "app/ats/AtsAgent/AtsMethod/src/AtsEvent.h"
#include "core/utilities/src/TAAssert.h"

using namespace TA_IRS_App;

AtsEventReader::AtsEventReader(boost::shared_ptr<AtsEvent> event,EventType commandType,EventLengthType parameterLength)
				 : m_event(event), is_valid(false)
{
	m_eventTemp = m_event.lock();
	// Check the type and size matches the expected event!
	//TA_ASSERT(eventType==event.getEventType(),"Reader type does not match event type.");
	//TA_ASSERT(parameterLength==event.getParameterLength(),"Reader parameter length does not match event parameter length.");
	if(commandType==event->getCommandType() && event->getParameterLength()==parameterLength)
		is_valid=true;
	else
	{
		std::string error_msg;
		if(commandType!=event->getCommandType())
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

AtsEventReader::AtsEventReader(boost::shared_ptr<AtsEvent> event,EventType commandType)
				 : m_event(event), is_valid(true)
{
	m_eventTemp = m_event.lock();
	// Check the type and size matches the expected event!
	TA_ASSERT(commandType==event->getCommandType(),"Reader type does not match event type.");
}

AtsEventReader::~AtsEventReader()
{
}

short AtsEventReader::getParameterShort(unsigned int position) const
{
	return m_event.lock()->getParameterShort(position);
}

   //Note: To be removed.
//unsigned long AtsEventReader::getParameterLong(unsigned int position) const
//{
//	return m_event.lock()->getParameterLong(position);
//}
   //Note: To be removed.
//std::string AtsEventReader::getParameterShortString(unsigned int position) const
//{
//	std::string   value;
//
//	for (int index=0;index<SHORTSTRING_LENGTH;index++)
//	{
//		char   nextChar = m_event.lock()->getParameterChar(index+position);
//		if ((nextChar=='\0')||(nextChar=='\n')) break;
//		value += nextChar;
//	}
//	return value;
//}

unsigned char AtsEventReader::getParameterChar(unsigned int position) const
{
	return m_event.lock()->getParameterChar(position);
}

bool AtsEventReader::isEventReaderValid()
{
return is_valid;
}

  //Note: To be removed.
//bool AtsEventReader::isStatusOK()
//{
//    // STATUS_OK declared in RadioProtocol.h
//    return STATUS_OK == m_event.lock()->getStatus();
//}