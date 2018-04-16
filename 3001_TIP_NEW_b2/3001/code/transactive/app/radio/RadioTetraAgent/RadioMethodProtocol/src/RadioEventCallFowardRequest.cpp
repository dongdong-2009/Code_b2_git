
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventCallFowardRequest.h"
#include "core/utilities/src/DebugUtil.h"

#define CALL_FORWARD_REQUEST_STATUS_START_POSITION   3

using namespace TA_IRS_App;

RadioEventCallForwardRequest::RadioEventCallForwardRequest(boost::shared_ptr<RadioEvent> event)
:RadioEventReader(event,TYPE_CALL_FORWARDING_REQUEST)
{
	FUNCTION_ENTRY("RadioEventCallForwardRequest");
	FUNCTION_EXIT;
}

RadioEventCallForwardRequest::~RadioEventCallForwardRequest()
{
	FUNCTION_ENTRY("~RadioEventCallForwardRequest");
	FUNCTION_EXIT;
}

bool RadioEventCallForwardRequest::getStatus()
{
	FUNCTION_ENTRY("getStatus");

	int status = 0;
	status = getParameterChar(CALL_FORWARD_REQUEST_STATUS_START_POSITION);
	FUNCTION_EXIT;
	return status;
}