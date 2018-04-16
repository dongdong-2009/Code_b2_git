#pragma once
#include "StdAfx.h"
#include "IRadioOperationStrategy.h"
#include "../ConfigReaderLib/Parser/IParser.h"
#include "../ConfigReaderLib/ConfigReader/IIniConfigHandler.h"

class RadioSystemStatusStrategy : public AbstractStrat 
{
	public:
		RadioSystemStatusStrategy(IParser * parsetIF,
									IIniConfigHandler * configHandler);
		~RadioSystemStatusStrategy(void);

		virtual RadioResultData * executeRadioConfigStrategy (const RadioParamData & radioParamdata);
};
