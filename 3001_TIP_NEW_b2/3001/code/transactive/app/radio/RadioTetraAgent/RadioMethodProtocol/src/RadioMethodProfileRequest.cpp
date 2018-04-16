
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodProfileRequest.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

#define PROFILE_REQUEST_METHOD_LENGTH      33
#define METHOD_TIMESTAMP_START_POSITION     3
#define METHOD_MESSAGE_ID_START_POSITION   17
#define METHOD_MFT_USER_START_POSITION     20
#define METHOD_ETX_START_POSITION          32

#define PROFILE_REQUEST_MESSAGE_ID        "M39"

using namespace TA_IRS_App;

RadioMethodProfileRequest::RadioMethodProfileRequest(const std::string dataMFTUser)
:RadioMethod(PROFILE_REQUEST_METHOD_LENGTH, STX_VALUE)
{
	FUNCTION_ENTRY("RadioProfileRequest");

	//Set the Timestamp
	setParameterString(METHOD_TIMESTAMP_START_POSITION,getTimestamp());
	//Set the Message ID
	setParameterString(METHOD_MESSAGE_ID_START_POSITION,PROFILE_REQUEST_MESSAGE_ID);
	//Set the MFT User Login
	//check if the MFT User Login contains less than 6 characters
	//if(dataMFTUser.length()<6)
	//{
	//	for(int index=0;index<12;index++)
	//		setByte(METHOD_MFT_USER_START_POSITION+index,0);
	//	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"MFT User Login was set to 00 since the inputted value was less than 6 characters");
	//}else{
	setParameterString(METHOD_MFT_USER_START_POSITION,dataMFTUser);
	//}
	//Set the ETX
	setByte(METHOD_ETX_START_POSITION,ETX_VALUE);
	

	FUNCTION_EXIT;
}

RadioMethodProfileRequest::~RadioMethodProfileRequest()
{
	FUNCTION_ENTRY("~RadioProfileRequest");

	FUNCTION_EXIT;
}