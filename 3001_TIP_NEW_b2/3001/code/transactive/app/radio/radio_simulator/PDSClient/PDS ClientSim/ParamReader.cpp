// ParamReader.cpp: implementation of the CParamReader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PDS ClientSim.h"
#include "ParamReader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_IRS_App
{
	CParamReader::CParamReader()
	{
		
	}

	CParamReader::~CParamReader()
	{

	}


	std::string CParamReader::getValueFromSection(std::string strSection)
	{
		
		char * pszDefData = new char [1024];
		char * pszRetData = new char [1024];

		strcpy (pszDefData,"Empty");
		
		char * pszDataPath = new char [1024];
		::GetCurrentDirectory(1024,pszDataPath);

		sprintf (pszDataPath,"\%s\\%s",pszDataPath,PARAM_FILE_NAME);

		DWORD dwRet = ::GetPrivateProfileString (strSection.c_str(),COMMON_KEY_NAME,pszDefData,pszRetData,MAX_PATH,pszDataPath);
		
		std::string strRetData = pszRetData;

		delete [] pszDefData;
		delete [] pszRetData;	
		delete [] pszDataPath;

		return strRetData;
	}

	void CParamReader::setValueToSection(std::string strSection, std::string strValue)
	{
		
		char * pszDataPath = new char [1024];
		::GetCurrentDirectory(1024,pszDataPath);
		
		sprintf (pszDataPath,"\%s\\%s",pszDataPath,PARAM_FILE_NAME);
		
		DWORD dwRet = ::WritePrivateProfileString(strSection.c_str(),COMMON_KEY_NAME,strValue.c_str(),pszDataPath);
		
		delete [] pszDataPath;
	}
}