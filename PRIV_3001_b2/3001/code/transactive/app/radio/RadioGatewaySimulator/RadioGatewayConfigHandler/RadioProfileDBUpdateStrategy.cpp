#include "StdAfx.h"
#include "RadioProfileDBUpdateStrategy.h"


#include <vector>

#define	PROFILE_DB_UPDATE_SECTION	"RADIO_PROFILE_DB_UPDATE"
#define PROFILE_DB_UPDATE_KEY		"UPDATE"


RadioProfileDBUpdateStrategy::RadioProfileDBUpdateStrategy(IParser * parsetIF,
														   IIniConfigHandler * configHandler) : 
	AbstractStrat (parsetIF,configHandler,ERadioProfileDBUpdate)
{
	
}

RadioProfileDBUpdateStrategy::~RadioProfileDBUpdateStrategy(void)
{
	
}

RadioResultData * RadioProfileDBUpdateStrategy::executeRadioConfigStrategy (const RadioParamData & radioParamdata)
{
	std::vector <std::string> vctrData = getConfigValue(PROFILE_DB_UPDATE_SECTION,PROFILE_DB_UPDATE_KEY);
	
	RadioProfileDBUpdateResultData * resultData = new RadioProfileDBUpdateResultData ();
	
	resultData->setProfileDBUpdate(vctrData);

	return resultData;
}