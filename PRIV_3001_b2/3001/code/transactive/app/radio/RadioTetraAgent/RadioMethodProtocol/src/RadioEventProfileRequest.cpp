
#include "app/radio/RadioTetraAgent/RadioMethodProtocol/src/RadioEventProfileRequest.h"
#include "core/utilities/src/DebugUtil.h"

#define TALKGROUP_ALIAS_SIZE   12
using namespace TA_IRS_App;

RadioEventProfileRequest::RadioEventProfileRequest(boost::shared_ptr<RadioEvent> event)
:RadioEventReader(event,TYPE_PROFILE_REQUEST)
{
	FUNCTION_ENTRY("RadioEventProfileRequest");
	FUNCTION_EXIT;
}

RadioEventProfileRequest::~RadioEventProfileRequest()
{
	FUNCTION_ENTRY("~RadioEventProfileRequest");
	FUNCTION_EXIT;
}

std::vector<std::string> RadioEventProfileRequest::getTalkgroup()
{
	FUNCTION_ENTRY("getTalkgroup");
	

	std::vector<std::string> TALKGROUP;
	for(int index=0; index<TALKGROUP_ALIAS_SIZE; index++)
		TALKGROUP.push_back(getParameterShortString(20+(index*12),TALKGROUP_ALIAS_SIZE));
	
	FUNCTION_EXIT;
	return TALKGROUP;
	

}