#include "stdafx.h"
#include "RadioStratFactory.h"

#include "ConfigReader/IniConfigAccessFactory.h"
#include "Parser/ParserDataAccessFactory.h"

#include "RadioSubscriberOperationStrategy.h"
#include "RadioMonitorProfileRequestStrategy.h"
#include "RadioProfileDBUpdateStrategy.h"
#include "RadioSystemStatusStrategy.h"
#include "RadioDGNAStatusStrategy.h"
#include "RadioQueryCallForwardingStrategy.h"

RadioStratFactory::RadioStratFactory()
{
	ParserDataAccessFactory::createParser();
	m_parserInterface = ParserDataAccessFactory::getParser();
	m_configHandler = IniConfigAccessFactory::getConfigHandler();
}

RadioStratFactory::~RadioStratFactory(void)
{
	IniConfigAccessFactory::clearConfigHandler();
	ParserDataAccessFactory::clearParser();
}

IRadioOperationStrategy * RadioStratFactory::createRadioStrategy (EStratType stratType)
{
	IRadioOperationStrategy * radioStrategy;

	switch (stratType)
	{
		case ERadioSubsc:
			radioStrategy = new RadioSubscriberOperationStrategy (m_parserInterface,m_configHandler);
			break;
		case ERadioProfileRequest:
			radioStrategy = new RadioMonitorProfileRequestStrategy (m_parserInterface,m_configHandler);
			break;
		case ERadioProfileDBUpdate:
			radioStrategy = new RadioProfileDBUpdateStrategy (m_parserInterface,m_configHandler);
			break;
		case ERadioSystemStatus:
			radioStrategy = new RadioSystemStatusStrategy (m_parserInterface,m_configHandler);
			break;
		case ERadioDGNAStatus:
			radioStrategy = new RadioDGNAStatusStrategy (m_parserInterface,m_configHandler);
			break;
		case ERadioQueryCallForwarding:
			radioStrategy = new RadioQueryCallForwardingStrategy (m_parserInterface,m_configHandler);
			break;
	}

	return radioStrategy;
}