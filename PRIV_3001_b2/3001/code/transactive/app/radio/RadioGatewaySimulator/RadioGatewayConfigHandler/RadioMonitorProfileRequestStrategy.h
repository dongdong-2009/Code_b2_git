#pragma once
#include "StdAfx.h"
#include "IRadioOperationStrategy.h"
#include "../ConfigReaderLib/Parser/IParser.h"
#include "../ConfigReaderLib/ConfigReader/IIniConfigHandler.h"

class RadioMonitorProfileRequestStrategy : public AbstractStrat 
{
	public:
		RadioMonitorProfileRequestStrategy(IParser * parsetIF,
										   IIniConfigHandler * configHandler);

		~RadioMonitorProfileRequestStrategy(void);

		virtual RadioResultData * executeRadioConfigStrategy (const RadioParamData & radioParamdata);		
};
