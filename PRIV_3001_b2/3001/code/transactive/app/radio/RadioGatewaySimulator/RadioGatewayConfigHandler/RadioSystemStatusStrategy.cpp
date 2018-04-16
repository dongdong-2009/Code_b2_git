#include "StdAfx.h"
#include "RadioSystemStatusStrategy.h"


#include <vector>

#define	RADIO_SYSTEM_STATUS_SECTION			"RADIO_SYSTEM_STATUS"
#define STATUSKEY							"STATUS"


RadioSystemStatusStrategy::RadioSystemStatusStrategy(IParser * parsetIF,
													 IIniConfigHandler * configHandler) : 
	AbstractStrat (parsetIF,configHandler,ERadioProfileDBUpdate)
{
	
}

RadioSystemStatusStrategy::~RadioSystemStatusStrategy(void)
{
	
}

RadioResultData * RadioSystemStatusStrategy::executeRadioConfigStrategy (const RadioParamData & radioParamdata)
{
	std::vector <std::string> vctrData = getConfigValue(RADIO_SYSTEM_STATUS_SECTION,STATUSKEY, false);
	
	RadioSystemStatusResultData * resultData = new RadioSystemStatusResultData ();
	
	
	resultData->setRadioStatus(vctrData.at (0));

	return resultData;
}