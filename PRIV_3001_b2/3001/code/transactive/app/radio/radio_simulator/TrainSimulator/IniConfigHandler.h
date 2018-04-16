#pragma once

#include "IIniConfigHandler.h"

class IniConfigHandler : public IIniConfigHandler
{
	public:
		IniConfigHandler(void);
		~IniConfigHandler(void);
		
		virtual std::string getValueFromKeyAndSection (std::string strSection,std::string strKey);
		virtual std::vector <std::string> getKeysFromSection (std::string strSection);

		virtual bool setValueFromKeyAndSection (std::string strSection,
												std::string strKey,
												std::string strValue);
										
	private:
		std::string m_strIniPath;


		char * getValueFromKeyAndSectionHelper (const char * pszSection, const char * pszKey);
		bool setValueFromKeyAndSectionHelper (const char * pszSection, const char * pszKey,const char *pszValue);
};
