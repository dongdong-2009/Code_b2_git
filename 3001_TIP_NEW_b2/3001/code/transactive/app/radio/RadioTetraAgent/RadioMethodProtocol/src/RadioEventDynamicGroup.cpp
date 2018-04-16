

#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventDynamicGroup.h"
#include "core/utilities/src/DebugUtil.h"

#define DYNAMIC_GROUP_STATUS_START_POSITION   27

using namespace TA_IRS_App;

RadioEventDynamicGroup::RadioEventDynamicGroup(boost::shared_ptr<RadioEvent> event)
:RadioEventReader(event,TYPE_DGNA_REQUEST)
{
	FUNCTION_ENTRY("RadioEventDynamicGroup");
	FUNCTION_EXIT;
}

RadioEventDynamicGroup::~RadioEventDynamicGroup()
{
	FUNCTION_ENTRY("~RadioEventDynamicGroup");
	FUNCTION_EXIT;
}

char RadioEventDynamicGroup::getStatus()
{
	FUNCTION_ENTRY("getStatus");

	char status = 0;
	status = getParameterChar(DYNAMIC_GROUP_STATUS_START_POSITION);
	FUNCTION_EXIT;
	return status;
}