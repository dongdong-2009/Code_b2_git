

#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventCancelCallForwarding.h"
#include "core/utilities/src/DebugUtil.h"

#define CANCEL_CALL_FORWARDING_STATUS_START_POSITION   3

using namespace TA_IRS_App;

RadioEventCancelCallForwarding::RadioEventCancelCallForwarding(boost::shared_ptr<RadioEvent> event)
:RadioEventReader(event,TYPE_CANCEL_CALL_FORWARDING)
{
	FUNCTION_ENTRY("RadioEventCallForwardRequest");
	FUNCTION_EXIT;
}

RadioEventCancelCallForwarding::~RadioEventCancelCallForwarding()
{
	FUNCTION_ENTRY("~RadioEventCallForwardRequest");
	FUNCTION_EXIT;
}

bool RadioEventCancelCallForwarding::getStatus()
{
	FUNCTION_ENTRY("getStatus");

	int status = 0;
	status = getParameterChar(CANCEL_CALL_FORWARDING_STATUS_START_POSITION);
	FUNCTION_EXIT;
	return status;
}