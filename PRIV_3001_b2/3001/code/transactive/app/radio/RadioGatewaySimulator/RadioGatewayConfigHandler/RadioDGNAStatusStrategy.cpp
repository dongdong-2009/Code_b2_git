#include "StdAfx.h"
#include "RadioDGNAStatusStrategy.h"

#include <vector>

#define	RADIO_DGNA_SECTION			"DGNA_REQUEST_RESULT"
#define STATUSKEY					"STATUS"


RadioDGNAStatusStrategy::RadioDGNAStatusStrategy(IParser * parsetIF,
													 IIniConfigHandler * configHandler) : 
	AbstractStrat (parsetIF,configHandler,ERadioProfileDBUpdate)
{
	
}

RadioDGNAStatusStrategy::~RadioDGNAStatusStrategy(void)
{
	
}

RadioResultData * RadioDGNAStatusStrategy::executeRadioConfigStrategy (const RadioParamData & radioParamdata)
{
	std::vector <std::string> vctrData = getConfigValue(RADIO_DGNA_SECTION,STATUSKEY, false);
	
	RadioDGNAStatusResultData * resultData = new RadioDGNAStatusResultData ();
	
	resultData->setRadioStatus(vctrData.at (0));

	return resultData;
}