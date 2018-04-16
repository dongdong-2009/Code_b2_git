#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSEventReader.h"
#include "app/cctv/tvss_agent/TVSSProtocolLibrary/src/TVSSEvent.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

using namespace TA_IRS_App;

TVSSEventReader::TVSSEventReader(boost::shared_ptr<TVSSEvent> event,EventType eventType,EventLengthType parameterLength)
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

TVSSEventReader::TVSSEventReader(boost::shared_ptr<TVSSEvent> event,EventType eventType,EventLengthType parameterLength,EventLengthType parameterLength_OCC)
: m_event(event), is_valid(false)
{
	m_eventTemp = m_event.lock();
	// Check the type and size matches the expected event!
	//TA_ASSERT(eventType==event.getEventType(),"Reader type does not match event type.");
	//TA_ASSERT(parameterLength==event.getParameterLength(),"Reader parameter length does not match event parameter length.");
	unsigned long locationKey = strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str() , NULL, 10);
	TA_Base_Core::ILocation* location =
		TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationKey);
	if (location->getLocationType() == TA_Base_Core::ILocation::OCC)
	{
		parameterLength = parameterLength_OCC;
	} 

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

TVSSEventReader::TVSSEventReader(boost::shared_ptr<TVSSEvent> event,EventType eventType)
				 : m_event(event), is_valid(true)
{
	m_eventTemp = m_event.lock();
	// Check the type and size matches the expected event!
	TA_ASSERT(eventType==event->getEventType(),"Reader type does not match event type.");
}

TVSSEventReader::~TVSSEventReader()
{
}

short TVSSEventReader::getParameterShort(unsigned int position) const
{
	return m_event.lock()->getParameterShort(position);
}


unsigned long TVSSEventReader::getParameterLong(unsigned int position) const
{
	return m_event.lock()->getParameterLong(position);
}


std::string TVSSEventReader::getParameterShortString(unsigned int position) const
{
	std::string   value;

	for (int index=0;index<SHORTSTRING_LENGTH;index++)
	{
		char   nextChar = m_event.lock()->getParameterChar(index+position);
		if ((nextChar=='\0')||(nextChar=='\n')) break;
		value += nextChar;
	}
	return value;
}

unsigned char TVSSEventReader::getParameterChar(unsigned int position) const
{
	return m_event.lock()->getParameterChar(position);
}
bool TVSSEventReader::isEventReaderValid()
{
return is_valid;
}
bool TVSSEventReader::isStatusOK()
{
    // STATUS_OK declared in RadioProtocol.h
    return STATUS_OK == m_event.lock()->getStatus();
}
