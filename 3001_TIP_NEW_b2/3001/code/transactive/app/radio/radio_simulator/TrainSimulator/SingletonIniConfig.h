#pragma once

#include "core\synchronisation\src\ReEntrantThreadLockable.h"
#include "IIniConfigHandler.h"

class SingletonIniConfig
{
	public:
		SingletonIniConfig(void);
		~SingletonIniConfig(void);

		static SingletonIniConfig * getInstance ();
		static void deleteInstace ();

		std::string getValueFromKeyAndSection (std::string strSection,std::string strKey);
		std::vector <std::string> getKeysFromSection (std::string strSection);

		bool setValueFromKeyAndSection (std::string strSection,
										std::string strKey,
										std::string strValue);

	private:
		static SingletonIniConfig * m_iniConfInstance;
		static TA_Base_Core::ReEntrantThreadLockable m_threadLock;
		static IIniConfigHandler * m_iniConfig;
};
