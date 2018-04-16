#pragma once
#include "StdAfx.h"
#include "IRadioOperationStrategy.h"
#include "../ConfigReaderLib/Parser/IParser.h"
#include "../ConfigReaderLib/ConfigReader/IIniConfigHandler.h"

class RadioSubscriberOperationStrategy : public AbstractStrat 
{
	public:
		RadioSubscriberOperationStrategy(IParser * parsetIF,
										 IIniConfigHandler * configHandler);
		~RadioSubscriberOperationStrategy(void);

		virtual RadioResultData * executeRadioConfigStrategy (const RadioParamData & radioParamdata);		

	private:

		enum ERadioSubsIndex
		{
			ETGSubs = 0,
			EBaseStation
		};

};
