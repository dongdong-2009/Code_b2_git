

#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodCancelCallForwarding.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

//Length of All data
#define CANCEL_CALL_FOWARDING_LENGTH                                            33

//Character Start Position
#define METHOD_HEADER_CANCEL_CALL_FOWARDING_TIMESTAMP_START_POSITION             3
#define METHOD_HEADER_CANCEL_CALL_FOWARDING_MESSAGE_ID_START_POSITION           17
#define CANCEL_CALL_FOWARDING_MESSAGE_ID                                       "M33"
#define METHOD_HEADER_CANCEL_CALL_FOWARDING_DATA_ISSI_START_POSITION            20
#define METHOD_HEADER_CANCEL_CALL_FOWARDING_ETX_START_POSITION                  32          

//Character Size
#define CANCEL_FORWARDING_ISSI_SIZE    12


using namespace TA_IRS_App;

RadioMethodCancelCallFowarding::RadioMethodCancelCallFowarding(unsigned long issi)
: RadioMethod(CANCEL_CALL_FOWARDING_LENGTH,STX_VALUE)
{
	FUNCTION_ENTRY("RadioMethodCancelCallFowarding");

	//Set the timestamp
	setParameterString(METHOD_HEADER_CANCEL_CALL_FOWARDING_TIMESTAMP_START_POSITION,getTimestamp());

	//Set the Message ID
	setParameterString(METHOD_HEADER_CANCEL_CALL_FOWARDING_MESSAGE_ID_START_POSITION,CANCEL_CALL_FOWARDING_MESSAGE_ID);

	//Set the Data ISSI
	//check if the length of ForwardingISSI is less than 6 characters, If its true then set to 00 based on the document.
	//if(getISSI(issi).length() < 6)
	//{
	//	for (int index=0; index<CANCEL_FORWARDING_ISSI_SIZE; index++)
	//		setByte(METHOD_HEADER_CANCEL_CALL_FOWARDING_DATA_ISSI_START_POSITION+index,0);
	//	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"ISSI was set to 00 since the inputted value was less than 6 characters");
	//}else{
	setParameterString(METHOD_HEADER_CANCEL_CALL_FOWARDING_DATA_ISSI_START_POSITION,getISSI(issi));
	//}

	//Set the ETX
	setByte(METHOD_HEADER_CANCEL_CALL_FOWARDING_ETX_START_POSITION,ETX_VALUE);
	FUNCTION_EXIT;
}

RadioMethodCancelCallFowarding::~RadioMethodCancelCallFowarding()
{
	FUNCTION_ENTRY("~RadioMethodCallForwardRequest");

	FUNCTION_EXIT;
}

std::string RadioMethodCancelCallFowarding::getISSI(unsigned long ssi)
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