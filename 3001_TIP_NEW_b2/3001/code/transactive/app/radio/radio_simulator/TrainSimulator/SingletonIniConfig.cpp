
#include "SingletonIniConfig.h"
#include "IniConfigHandler.h"
#include "core\utilities\src\DebugUtil.h"

SingletonIniConfig * SingletonIniConfig::m_iniConfInstance = NULL;
TA_Base_Core::ReEntrantThreadLockable SingletonIniConfig::m_threadLock;
IIniConfigHandler * SingletonIniConfig::m_iniConfig = NULL;

SingletonIniConfig::SingletonIniConfig(void)
{
	m_iniConfig = new IniConfigHandler ();
}

SingletonIniConfig::~SingletonIniConfig(void)
{
	if (m_iniConfig)
		delete m_iniConfig;
}

SingletonIniConfig * SingletonIniConfig::getInstance ()
{
	{
		TA_THREADGUARD (m_threadLock);

		if (!m_iniConfInstance)
		{
			m_iniConfInstance = new SingletonIniConfig ();
		}
	}

	return m_iniConfInstance;
}

void SingletonIniConfig::deleteInstace ()
{
	{
		TA_THREADGUARD (m_threadLock);

		if (!m_iniConfInstance)
		{
			delete m_iniConfInstance;
			m_iniConfInstance = NULL;
		}
	}
}

std::string SingletonIniConfig::getValueFromKeyAndSection (std::string strSection,std::string strKey)
{
	return m_iniConfig->getValueFromKeyAndSection(strSection,strKey);
}

std::vector <std::string> SingletonIniConfig::getKeysFromSection (std::string strSection)
{
	return m_iniConfig->getKeysFromSection(strSection);
}

bool SingletonIniConfig::setValueFromKeyAndSection (std::string strSection,
												    std::string strKey,
												    std::string strValue)
{
	return m_iniConfig->setValueFromKeyAndSection(strSection,strKey,strValue);
}

