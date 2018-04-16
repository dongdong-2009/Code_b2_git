
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodSubscriberMonitor.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

#define SUBSCRIBER_MONITORING_LENGTH                          33
#define SUBSCRIBER_MONITOR_MESSAGE_ID                        "M40"
#define METHOD_HEADER_SUBSCRIBER_ISSI_START_POSITION           3
#define METHOD_HEADER_SUBSCRIBER_TIMESTAMP_START_POSITION     15
#define METHOD_HEADER_SUBSCRIBER_MESSAGE_ID_START_POSITION    29
#define METHOD_HEADER_SUBSCRIBER_ETX_START_POSITION           32

using namespace TA_IRS_App;

RadioMethodSubscriberMonitor::RadioMethodSubscriberMonitor(unsigned long ssi)
: RadioMethod(SUBSCRIBER_MONITORING_LENGTH,STX_VALUE)
{
	FUNCTION_ENTRY("RadioMethodSubscriberMonitor");
	//check if the length of ssi is less than 6 characters
	//if(getISSI(ssi).length() < 6)
	//{
	//	for (int index=0; index<12; index++)
	//		setByte(METHOD_HEADER_SUBSCRIBER_ISSI_START_POSITION+index,0);
	//	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"ISSI was set to 00 since the inputted value was less than 6 characters");
	//}else{
	setParameterString(METHOD_HEADER_SUBSCRIBER_ISSI_START_POSITION,getISSI(ssi));
	//}

	//Set the timestamp
	setParameterString(METHOD_HEADER_SUBSCRIBER_TIMESTAMP_START_POSITION,getTimestamp());
	//Set the MessageID
	setParameterString(METHOD_HEADER_SUBSCRIBER_MESSAGE_ID_START_POSITION,SUBSCRIBER_MONITOR_MESSAGE_ID);
	//Set the ETX
	setByte(METHOD_HEADER_SUBSCRIBER_ETX_START_POSITION,ETX_VALUE);

	FUNCTION_EXIT;
}

RadioMethodSubscriberMonitor::~RadioMethodSubscriberMonitor()
{
	FUNCTION_ENTRY("~RadioMethodSubscriberMonitor");
	FUNCTION_EXIT;
}
std::string RadioMethodSubscriberMonitor::getISSI(unsigned long ssi)
{
	FUNCTION_ENTRY("uLongTostring");

	std::string uString;
	char tempData[1024]={0};
	sprintf(tempData,"%u",ssi);
	
	uString += tempData;
	uString.append(12-strlen(tempData),0);

	FUNCTION_EXIT;
	return uString;
}