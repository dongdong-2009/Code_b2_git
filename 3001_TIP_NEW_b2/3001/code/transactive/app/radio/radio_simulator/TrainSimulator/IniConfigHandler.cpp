
#include "IniConfigHandler.h"

#include <atlconv.h>
#include <atlbase.h>
#include <windows.h>

IniConfigHandler::IniConfigHandler(void)
{
	TCHAR szEXEFileName[2048];
	
	USES_CONVERSION;
	DWORD dwRet = ::GetModuleFileName (NULL,szEXEFileName,2048);

	
	PathRemoveFileSpec (szEXEFileName);
	m_strIniPath = W2A (szEXEFileName);

	m_strIniPath+="\\";
	m_strIniPath+="ConfEtc.ini";
}

IniConfigHandler::~IniConfigHandler(void)
{
}


std::string IniConfigHandler::getValueFromKeyAndSection (std::string strSection,std::string strKey)
{
	std::string strReturn;
	
	char * pszData = getValueFromKeyAndSectionHelper (strSection.c_str(),strKey.c_str());
	
	strReturn = pszData;

	delete [] pszData;

	return strReturn;
}

std::vector <std::string> IniConfigHandler::getKeysFromSection (std::string strSection)
{
	std::vector<std::string> vctrRet;


	return vctrRet;
}

bool IniConfigHandler::setValueFromKeyAndSection  (std::string strSection,
										     	   std::string strKey,
												   std::string strValue)
{
	bool bReturn = false;
	
	bReturn = setValueFromKeyAndSectionHelper (strSection.c_str(),strKey.c_str(),strValue.c_str());

	return bReturn;
}


char * IniConfigHandler::getValueFromKeyAndSectionHelper (const char * pszSection, const char * pszKey)
{
	char* pszValue = new char[1024];
	TCHAR * pszValueT = new TCHAR[1024];
	char * pszDefault = new char[1024];

	sprintf (pszDefault,"EMPTY");
	
	USES_CONVERSION;

	DWORD dwItemCount = ::GetPrivateProfileString (A2W (pszSection),A2W (pszKey),A2W(pszDefault),pszValueT,strlen (pszValue),A2W(m_strIniPath.c_str()));
	
	if (0 == dwItemCount)
	{
		strcpy (pszValue,pszDefault);
	}
	
	sprintf (pszValue,"%s",W2A(pszValueT));

	delete [] pszDefault;
	delete [] pszValueT;
	return pszValue;
}

bool IniConfigHandler::setValueFromKeyAndSectionHelper (const char * pszSection, const char * pszKey,const char *pszValue)
{
	bool bReturn = false;
	
	USES_CONVERSION;
	bReturn = WritePrivateProfileString (A2W(pszSection),A2W(pszKey),A2W(pszValue),A2W (m_strIniPath.c_str()));

	return bReturn;
}