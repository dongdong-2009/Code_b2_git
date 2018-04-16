

#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioMethodDynamicGroup.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioProtocol.h"

//Length of All data
#define DYNAMIC_GROUP_LENGTH                                                    46

//Character Start Position
#define METHOD_HEADER_DYNAMIC_GROUP_ISSI_START_POSITION                          3
#define METHOD_HEADER_DYNAMIC_GROUP_DGNA_ACTION_START_POSITION                  15
#define METHOD_HEADER_DYNAMIC_GROUP_TARGET_TALKGROUP_ALIAS_START_POSITION       16
#define METHOD_HEADER_DYNAMIC_GROUP_TIMESTAMP__START_POSITION                   28
#define METHOD_HEADER_DYNAMIC_GROUP_MESSAGE_ID__START_POSITION                  42
#define DYNAMIC_GROUP_MONITOR_MESSAGE_ID                                       "M41"
#define METHOD_HEADER_DYNAMIC_GROUP_ETX__START_POSITION                         45

//Character Size
#define DYNAMIC_GROUP_ISSI_SIZE                   12
#define DYNAMIC_GROUP_TARGET_GROUP_ALIAS_SIZE     12
using namespace TA_IRS_App;

RadioMethodDynamicGroup::RadioMethodDynamicGroup(unsigned long issi, std::string talkGroupAlias, bool dgnaAction)
: RadioMethod(DYNAMIC_GROUP_LENGTH,STX_VALUE)
{
	FUNCTION_ENTRY("RadioMethodDynamicGroup");

	//Set the ISSI
	//check if the length of ssi is less than 6 characters
	//if(getISSI(issi).length() < 6)
	//{
	//	for (int index=0; index<DYNAMIC_GROUP_ISSI_SIZE; index++)
	//		setByte(METHOD_HEADER_DYNAMIC_GROUP_ISSI_START_POSITION+index,0);
	//	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"ISSI was set to 00 since the inputted value was less than 6 characters");
	//}else{
	setParameterString(METHOD_HEADER_DYNAMIC_GROUP_ISSI_START_POSITION,getISSI(issi));
	//}

	//Set the DGNA Action 
	//setByte(METHOD_HEADER_DYNAMIC_GROUP_DGNA_ACTION_START_POSITION,dgnaAction);
	if (dgnaAction) {
		setParameterString(METHOD_HEADER_DYNAMIC_GROUP_DGNA_ACTION_START_POSITION, "1");
	} else {
		setParameterString(METHOD_HEADER_DYNAMIC_GROUP_DGNA_ACTION_START_POSITION, "0");
	}

	//Set Target Talkgroup Alias
	//if(talkGroupAlias.length()<6)
	//{
	//	for(int index=0;index<DYNAMIC_GROUP_TARGET_GROUP_ALIAS_SIZE;index++)
	//		setByte(METHOD_HEADER_DYNAMIC_GROUP_TARGET_TALKGROUP_ALIAS_START_POSITION+index,0);
	//	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"MFT User Login was set to 00 since the inputted value was less than 6 characters");
	//}else{
	setParameterString(METHOD_HEADER_DYNAMIC_GROUP_TARGET_TALKGROUP_ALIAS_START_POSITION,talkGroupAlias);
	//}

	//Set the timestamp
	setParameterString(METHOD_HEADER_DYNAMIC_GROUP_TIMESTAMP__START_POSITION,getTimestamp());
	
	//Set the MessageID
	setParameterString(METHOD_HEADER_DYNAMIC_GROUP_MESSAGE_ID__START_POSITION,DYNAMIC_GROUP_MONITOR_MESSAGE_ID);

	//Set the ETX
	setByte(METHOD_HEADER_DYNAMIC_GROUP_ETX__START_POSITION,ETX_VALUE);

	FUNCTION_EXIT;
}

RadioMethodDynamicGroup::~RadioMethodDynamicGroup()
{
	FUNCTION_ENTRY("~RadioMethodDynamicGroup");
	FUNCTION_EXIT;
}
std::string RadioMethodDynamicGroup::getISSI(unsigned long ssi)
{
	FUNCTION_ENTRY("getISSI");

	std::string uString;
	char tempData[1024]={0};
	sprintf(tempData,"%u",ssi);
	
	uString += tempData;
	uString.append(12-strlen(tempData),0);

	FUNCTION_EXIT;
	return uString;
}