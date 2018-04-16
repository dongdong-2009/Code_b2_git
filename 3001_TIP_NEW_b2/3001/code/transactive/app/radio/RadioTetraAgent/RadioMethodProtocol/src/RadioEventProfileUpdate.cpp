
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventProfileUpdate.h"
#include "core/utilities/src/DebugUtil.h"

#define USER_ALIAS_SIZE   12
#define EVENT_HEADER_PROFILE_UPDATE_NUM_POSITION 20
#define EVENT_HEADER_PROFILE_UPDATE_START_DATA_SET_POSITION 22

#define PROFILE_UPDATE_DELETED_STATUS 00 
#define PROFILE_UPDATE_INSERTED_STATUS 01
#define PROFILE_UPDATE_UPDATED_STATUS 02
using namespace TA_IRS_App;

const unsigned short RadioEventProfileUpdate::STATUS_TYPE_LENGTH = 1;
const unsigned short RadioEventProfileUpdate::EVENT_HEADER_PROFILE_STATUS_NUM_POSITION = 34;
const unsigned short RadioEventProfileUpdate::EVENT_HEADER_PROFILE_STATUS_INTERVAL = 13;

RadioEventProfileUpdate::RadioEventProfileUpdate(boost::shared_ptr<RadioEvent> event)
:RadioEventReader(event,TYPE_PROFILE_UPDATE)
{
	FUNCTION_ENTRY("RadioEventProfileUpdate");
	FUNCTION_EXIT;
}

RadioEventProfileUpdate::~RadioEventProfileUpdate()
{
	FUNCTION_ENTRY("~RadioEventProfileUpdate");
	FUNCTION_EXIT;
}


int RadioEventProfileUpdate::getNumDataSet()
{
	FUNCTION_ENTRY("getNumDataSet");

	numDataSet = getParameterShort(EVENT_HEADER_PROFILE_UPDATE_NUM_POSITION);

	FUNCTION_EXIT;
	return numDataSet;
}


std::vector<std::pair<std::string, short> > RadioEventProfileUpdate::getDataSet()
{
	FUNCTION_ENTRY("getDataSet");
	std::vector<std::pair<std::string, short> > set;

	int index = 0;
	getNumDataSet();
	std::string userAlias;
	short statusFlag = -1;
	char tempStat;

	for (index = 0; index < numDataSet; index++) {
		tempStat = getParameterChar(EVENT_HEADER_PROFILE_STATUS_NUM_POSITION + index*EVENT_HEADER_PROFILE_STATUS_INTERVAL);
		if (tempStat ==PROFILE_UPDATE_DELETED_STATUS) {
			statusFlag = 0;
		} else if (tempStat == PROFILE_UPDATE_INSERTED_STATUS) {
			statusFlag = 1;
		} else if (tempStat == PROFILE_UPDATE_UPDATED_STATUS){
			statusFlag = 2;
		} else {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Radio Profile Update: Undefined status code received = %u", tempStat);
		}
		userAlias = getParameterShortString(EVENT_HEADER_PROFILE_UPDATE_START_DATA_SET_POSITION+((USER_ALIAS_SIZE+STATUS_TYPE_LENGTH)*index), USER_ALIAS_SIZE);
		set.push_back(make_pair(userAlias, statusFlag));
	}
	FUNCTION_EXIT;
	return set;
}




