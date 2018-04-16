#include "StdAfx.h"
#include "RadioMonitorProfileRequestStrategy.h"

#include <vector>

#define	RA_PROFILE_REQUEST_SECTION	"RADIO_PROFILE_SYNC_DEF_MEMBERS"
#define ALIAS_REQUIRED_LEN			12


RadioMonitorProfileRequestStrategy::RadioMonitorProfileRequestStrategy(IParser * parsetIF,
														 IIniConfigHandler * configHandler) : 
	AbstractStrat (parsetIF,configHandler,ERadioSubsc)
{
	
}

RadioMonitorProfileRequestStrategy::~RadioMonitorProfileRequestStrategy(void)
{
	
}

RadioResultData * RadioMonitorProfileRequestStrategy::executeRadioConfigStrategy (const RadioParamData & radioParamdata)
{
	
	RadioMonitorProfileRequestParamData radioProfileRequest = (RadioMonitorProfileRequestParamData &)radioParamdata;

	std::string strProfile = radioProfileRequest.getProfile();

	std::vector <std::string> vctrSeperatedVal = getConfigValue(RA_PROFILE_REQUEST_SECTION,strProfile);
	
	RadioProfileRequestResultData * resultData = new RadioProfileRequestResultData ();

	resultData->setProfileTalkGroup(vctrSeperatedVal);

	return resultData;
}