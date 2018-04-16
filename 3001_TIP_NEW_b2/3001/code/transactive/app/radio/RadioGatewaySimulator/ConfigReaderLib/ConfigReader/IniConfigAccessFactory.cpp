
#include "IniConfigAccessFactory.h"
#include "IniConfigHandler.h"


IIniConfigHandler * IniConfigAccessFactory::m_iniConfigHandler = NULL;

IniConfigAccessFactory::IniConfigAccessFactory(void)
{
	
}

IniConfigAccessFactory::~IniConfigAccessFactory(void)
{
}

IIniConfigHandler * IniConfigAccessFactory::getConfigHandler ()
{
	if (m_iniConfigHandler == NULL)
	{
		createConfigHandler ();
	}
	
	return m_iniConfigHandler;
}


void IniConfigAccessFactory::createConfigHandler ()
{
	m_iniConfigHandler = new IniConfigHandler ();
}

void IniConfigAccessFactory::clearConfigHandler ()
{
	if (m_iniConfigHandler)
	{
		delete m_iniConfigHandler;
		m_iniConfigHandler = NULL;
	}
}