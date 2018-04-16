#pragma once
#include "StdAfx.h"
#include "IRadioOperationStrategy.h"
#include "../ConfigReaderLib/Parser/IParser.h"
#include "../ConfigReaderLib/ConfigReader/IIniConfigHandler.h"

class RadioQueryCallForwardingStrategy : public AbstractStrat 
{
	public:
		RadioQueryCallForwardingStrategy(IParser * parsetIF,
									IIniConfigHandler * configHandler);
		~RadioQueryCallForwardingStrategy(void);

		virtual RadioResultData * executeRadioConfigStrategy (const RadioParamData & radioParamdata);
};
