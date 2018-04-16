
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodCallForwardRequest.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

//Length of All data
#define CALL_FOWARD_REQUEST_LENGTH                                              46

//Character Start Position
#define METHOD_HEADER_CALL_FOWARD_REQUEST_TIMESTAMP_START_POSITION               3
#define METHOD_HEADER_CALL_FOWARD_REQUEST_MESSAGE_ID_START_POSITION             17
#define CALL_FOWARD_REQUEST_MESSAGE_ID                                         "M32"
#define METHOD_HEADER_CALL_FOWARD_REQUEST_DATA_CONDITION_START_POSITION         20
#define METHOD_HEADER_CALL_FOWARD_REQUEST_FORWARDING_ISSI_START_POSITION        21
#define METHOD_HEADER_CALL_FOWARD_REQUEST_FORWARDED_ISSI_START_POSITION         33
#define METHOD_HEADER_CALL_FOWARD_REQUEST_ETX_START_POSITION                    45

//Character Size
#define FORWARDING_ISSI_SIZE    12
#define FORWARDED_ISSI_SIZE     12

using namespace TA_IRS_App;

RadioMethodCallForwardRequest::RadioMethodCallForwardRequest(bool condition, unsigned long forwardingISSI, unsigned long forwardedISSI)
: RadioMethod(CALL_FOWARD_REQUEST_LENGTH,STX_VALUE)
{
	FUNCTION_ENTRY("RadioMethodCallForwardRequest");

	//Set the timestamp
	setParameterString(METHOD_HEADER_CALL_FOWARD_REQUEST_TIMESTAMP_START_POSITION,getTimestamp());

	//Set the Message ID
	setParameterString(METHOD_HEADER_CALL_FOWARD_REQUEST_MESSAGE_ID_START_POSITION,CALL_FOWARD_REQUEST_MESSAGE_ID);

	//Set the Data Condition
	setByte(METHOD_HEADER_CALL_FOWARD_REQUEST_DATA_CONDITION_START_POSITION,condition);

	//Set the Data Forwarding ISSI
	//check if the length of ForwardingISSI is less than 6 characters, If its true then set to 00 based on the document.
	//if(getISSI(forwardingISSI).length() < 6)
	//{
	//	for (int index=0; index<FORWARDING_ISSI_SIZE; index++)
	//		setByte(METHOD_HEADER_CALL_FOWARD_REQUEST_FORWARDING_ISSI_START_POSITION+index,0);
	//	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"ISSI was set to 00 since the inputted value was less than 6 characters");
	//}else{
	setParameterString(METHOD_HEADER_CALL_FOWARD_REQUEST_FORWARDING_ISSI_START_POSITION,getISSI(forwardingISSI));
	//}

	//Set the Data Forwarded ISSI
	//check if the length of ForwardingISSI is less than 6 characters, If its true then set to 00 based on the document.
	//if(getISSI(forwardedISSI).length() < 6)
	//{
	//	for (int index=0; index<FORWARDED_ISSI_SIZE; index++)
	//		setByte(METHOD_HEADER_CALL_FOWARD_REQUEST_FORWARDED_ISSI_START_POSITION+index,0);
	//	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"ISSI was set to 00 since the inputted value was less than 6 characters");
	//}else{
	setParameterString(METHOD_HEADER_CALL_FOWARD_REQUEST_FORWARDED_ISSI_START_POSITION,getISSI(forwardedISSI));
	//}
	
	//Set the ETX
	setByte(METHOD_HEADER_CALL_FOWARD_REQUEST_ETX_START_POSITION,ETX_VALUE);
	FUNCTION_EXIT;
}

RadioMethodCallForwardRequest::~RadioMethodCallForwardRequest()
{
	FUNCTION_ENTRY("~RadioMethodCallForwardRequest");

	FUNCTION_EXIT;
}

std::string RadioMethodCallForwardRequest::getISSI(unsigned long ssi)
{
	FUNCTION_ENTRY("RadioMethodCallForwardRequest");
	
	std::string uString;
	char tempData[1024]={0};
	sprintf(tempData,"%u",ssi);
	
	uString += tempData;
	uString.append(12-strlen(tempData),0);

	FUNCTION_EXIT;
	return uString;
}