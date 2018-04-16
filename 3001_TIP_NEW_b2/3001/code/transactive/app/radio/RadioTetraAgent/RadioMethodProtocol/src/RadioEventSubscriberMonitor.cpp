
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventSubscriberMonitor.h"
#include "core/utilities/src/DebugUtil.h"

#define TALKGROUP_ALIAS_SIZE                       12
#define BASE_STATION_ALIAS_SIZSE                   12
#define SUBSCRIBER_TALKGROUP_START_POSITION        16
#define SUBSCRIBTER_BASE_STATION_START_POSITION    28

using namespace TA_IRS_App;

RadioEventSubscriberMonitor::RadioEventSubscriberMonitor(boost::shared_ptr<RadioEvent> event)
:RadioEventReader(event,TYPE_SUBSCRIBER_MONITOR)
{
	FUNCTION_ENTRY("RadioEventProfileRequest");
	FUNCTION_EXIT;
}

RadioEventSubscriberMonitor::~RadioEventSubscriberMonitor()
{
	FUNCTION_ENTRY("~RadioEventSubscriberMonitor");
	FUNCTION_EXIT;
}

std::string RadioEventSubscriberMonitor::getTalkgroupAlias()
{
	FUNCTION_ENTRY("getTalkgroupAlias");

	std::string talkGroup;
	talkGroup = getParameterShortString(SUBSCRIBER_TALKGROUP_START_POSITION,TALKGROUP_ALIAS_SIZE);

	FUNCTION_EXIT;
	return talkGroup;
}

std::string RadioEventSubscriberMonitor::getBaseStaionAlias()
{
	FUNCTION_ENTRY("getBaseStaionAlias");

	std::string baseStationAlias;
	baseStationAlias = getParameterShortString(SUBSCRIBTER_BASE_STATION_START_POSITION,BASE_STATION_ALIAS_SIZSE);
	
	FUNCTION_EXIT;
	return baseStationAlias;
}