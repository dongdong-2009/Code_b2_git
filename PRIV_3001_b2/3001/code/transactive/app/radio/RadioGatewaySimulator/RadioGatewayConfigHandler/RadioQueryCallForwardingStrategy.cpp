#include "StdAfx.h"
#include "RadioQueryCallForwardingStrategy.h"


#include <vector>

#define	RADIO_QUERY_SECTION			"QUERY_CALL_FORWARDING_RESULT"
#define CONDITION_KEY				"CONDITION"
#define SOURCE_SSI_KEY				"SOURCE_SSI"
#define DESTINATION_SSI_KEY			"DESTINATION_SSI"


RadioQueryCallForwardingStrategy::RadioQueryCallForwardingStrategy(IParser * parsetIF,
																  IIniConfigHandler * configHandler) : 
	AbstractStrat (parsetIF,configHandler,ERadioProfileDBUpdate)
{
	
}

RadioQueryCallForwardingStrategy::~RadioQueryCallForwardingStrategy(void)
{
	
}

RadioResultData * RadioQueryCallForwardingStrategy::executeRadioConfigStrategy (const RadioParamData & radioParamdata)
{
	std::string strCondition,strForwardTsi,strDestionationTsi;
	
	std::vector <std::string> vctrData = getConfigValue(RADIO_QUERY_SECTION,CONDITION_KEY, false);
	strCondition = vctrData.at (0);

	vctrData = getConfigValue(RADIO_QUERY_SECTION,SOURCE_SSI_KEY, false);
	strForwardTsi = vctrData.at (0);

	vctrData = getConfigValue(RADIO_QUERY_SECTION,DESTINATION_SSI_KEY, false);
	strDestionationTsi = vctrData.at (0);
	
	RadioQueryCallForwadingResultData * resultData = new RadioQueryCallForwadingResultData ();
	
	
	resultData->setRadioCallForwardingQueryData(strCondition,strForwardTsi,strDestionationTsi);

	return resultData;
}