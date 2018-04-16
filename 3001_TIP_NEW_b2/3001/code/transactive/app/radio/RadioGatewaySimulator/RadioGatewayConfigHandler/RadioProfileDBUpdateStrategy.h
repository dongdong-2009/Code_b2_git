#pragma once
#include "StdAfx.h"
#include "IRadioOperationStrategy.h"
#include "../ConfigReaderLib/Parser/IParser.h"
#include "../ConfigReaderLib/ConfigReader/IIniConfigHandler.h"

class RadioProfileDBUpdateStrategy : public AbstractStrat 
{
	public:
		RadioProfileDBUpdateStrategy(IParser * parsetIF,
									 IIniConfigHandler * configHandler);
		~RadioProfileDBUpdateStrategy(void);

		virtual RadioResultData * executeRadioConfigStrategy (const RadioParamData & radioParamdata);		

};
