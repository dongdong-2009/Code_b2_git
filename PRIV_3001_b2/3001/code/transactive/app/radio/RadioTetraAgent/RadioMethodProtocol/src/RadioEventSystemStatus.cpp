
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventSystemStatus.h"
#include "core/utilities/src/DebugUtil.h"

#define EVENT_HEADER_SYSTEM_STATUS_POSITION 3

using namespace TA_IRS_App;

RadioEventSystemStatus::RadioEventSystemStatus(boost::shared_ptr<RadioEvent> event)
:RadioEventReader(event,TYPE_RADIO_SYSTEM_STATUS)
{
	FUNCTION_ENTRY("RadioEventSystemStatus");
	FUNCTION_EXIT;
}

RadioEventSystemStatus::~RadioEventSystemStatus()
{
	FUNCTION_ENTRY("~RadioEventSystemStatus");
	FUNCTION_EXIT;
}

char RadioEventSystemStatus::getStatus()
{
	FUNCTION_ENTRY("getStatus");

	FUNCTION_EXIT;

	return getParameterChar(EVENT_HEADER_SYSTEM_STATUS_POSITION);
}