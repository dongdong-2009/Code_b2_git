#include "StdAfx.h"
#include "RadioSubscriberOperationStrategy.h"
#include <vector>

#define	RA_SUBSCRIBER_SECTION		"SUBSCRIBER"
#define ALIAS_REQUIRED_LEN			12


RadioSubscriberOperationStrategy::RadioSubscriberOperationStrategy(IParser * parsetIF,
																   IIniConfigHandler * configHandler) : 
	AbstractStrat (parsetIF,configHandler,ERadioSubsc)
{
	
}

RadioSubscriberOperationStrategy::~RadioSubscriberOperationStrategy(void)
{
	
}

RadioResultData * RadioSubscriberOperationStrategy::executeRadioConfigStrategy (const RadioParamData & radioParamdata)
{
	RadioSubscriberParamData * radioSubsparamData = (RadioSubscriberParamData *)&radioParamdata;
	
	RadioSubscriberData radioSubsData;

	unsigned long lResourceId = radioSubsparamData->getResourceId();

	char * pszData = new char [1023];

	sprintf (pszData,"%d",lResourceId);

	std::vector <std::string> vctrData = getConfigValue(RA_SUBSCRIBER_SECTION,pszData);
	
	// validate data before updating predefined structure
	if (TRUE != vctrData.empty() &&
		vctrData[ETGSubs].compare("") != 0 &&
		vctrData[EBaseStation].compare("") != 0)
	{
		radioSubsData.strAffiliatedTG = vctrData[ETGSubs];
		radioSubsData.strBaseStation = vctrData[EBaseStation];
	}


	delete [] pszData;

	RadioSubscriberResultData * resultData = new RadioSubscriberResultData ();
	resultData->setRadioSubscriberData(radioSubsData);

	
	return resultData;
}