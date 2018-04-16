
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventQueryCallForwarding.h"
#include "core/utilities/src/DebugUtil.h"

//Character start position
#define QUERY_CALL_FORWARDING_CONDITION_START_POSITION        20
#define QUERY_CALL_FORWARDING_FORWARDING_ISS_START_POSITION   21
#define QUERY_CALL_FORWARDING_FORWARDED_ISS_START_POSITION    33

//Character size
#define QUERY_CALL_FORWARDING_ISSI_SIZE  12
#define QUERY_CALL_FORWARDED_ISSI_SIZE   12

using namespace TA_IRS_App;

RadioEventQueryCallForwarding::RadioEventQueryCallForwarding(boost::shared_ptr<RadioEvent> event)
:RadioEventReader(event,TYPE_QUERY_CALL_FORWARDING)
{
	FUNCTION_ENTRY("RadioEventQueryCallForwarding");
	FUNCTION_EXIT;
}

RadioEventQueryCallForwarding::~RadioEventQueryCallForwarding()
{
	FUNCTION_ENTRY("~RadioEventQueryCallForwarding");
	FUNCTION_EXIT;
}

unsigned short RadioEventQueryCallForwarding::getCondition()
{
	FUNCTION_ENTRY("getCondition");

	unsigned short condition = getParameterChar(QUERY_CALL_FORWARDING_CONDITION_START_POSITION);

	FUNCTION_EXIT;
	return condition;
}

unsigned long RadioEventQueryCallForwarding::getForwardingISSI()
{
	FUNCTION_ENTRY("getForwardingISSI");

	std::string valueISSI = getParameterShortString(QUERY_CALL_FORWARDING_FORWARDING_ISS_START_POSITION,QUERY_CALL_FORWARDING_ISSI_SIZE);
	unsigned long forwardingISSI = getLong(valueISSI);
	
	FUNCTION_EXIT;
	return forwardingISSI;
}

unsigned long RadioEventQueryCallForwarding::getForwardedISSI()
{
	FUNCTION_ENTRY("getForwardedISSI");
	
	std::string valueISSI = getParameterShortString(QUERY_CALL_FORWARDING_FORWARDED_ISS_START_POSITION,QUERY_CALL_FORWARDED_ISSI_SIZE);
	unsigned long forwardedISSI = getLong(valueISSI);
	FUNCTION_EXIT;
	return forwardedISSI;
}

unsigned long RadioEventQueryCallForwarding::getLong(std::string strData)
{
	FUNCTION_ENTRY("getLong");
	unsigned long longData= atol(strData.c_str());
	FUNCTION_EXIT;
	return longData;

}