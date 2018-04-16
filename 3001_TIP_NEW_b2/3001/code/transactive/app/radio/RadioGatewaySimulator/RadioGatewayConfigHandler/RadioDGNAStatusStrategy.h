#pragma once
#include "StdAfx.h"
#include "IRadioOperationStrategy.h"
#include "../ConfigReaderLib/Parser/IParser.h"
#include "../ConfigReaderLib/ConfigReader/IIniConfigHandler.h"

class RadioDGNAStatusStrategy : public AbstractStrat 
{
	public:
		RadioDGNAStatusStrategy(IParser * parsetIF,
									IIniConfigHandler * configHandler);
		~RadioDGNAStatusStrategy(void);

		virtual RadioResultData * executeRadioConfigStrategy (const RadioParamData & radioParamdata);
};
