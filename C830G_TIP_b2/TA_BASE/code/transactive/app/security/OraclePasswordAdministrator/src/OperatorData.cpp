// OperatorData.cpp: implementation of the OperatorData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OraclePasswordAdministrator.h"
#include "OperatorData.h"
#include "cots/openssl/openssl-1.0.0d/build/include/openssl/sha.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

const static int AGENT_PASSWD_LENGTH = 0;
const static int IS_PASSWD_MAY_ENCRYPTED_LENGTH = 40;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

OperatorData::OperatorData(CString pkey, CString name, CString passwd)
	:m_pkey(pkey),m_name(name),m_passwd(passwd),m_updateSQL(""),m_revertSQL("")
{
	CString origin_Passwd = m_name;
	CString encrypted_Passwd;

	origin_Passwd += "/";

	if(m_passwd.GetLength() == AGENT_PASSWD_LENGTH)
	{
		origin_Passwd += "agentOperator";
	}
	else
	{
		origin_Passwd += m_passwd;
	}

	int rtnCode= GetHash(origin_Passwd,encrypted_Passwd);
	if(rtnCode != 0 )		
	{
		AfxMessageBox("Failed to get hash!");
	}
	else
	{
		m_encryptedPasswd = encrypted_Passwd;

		generateUpdateSQL();
		generateRevertSQL();
	}
}

CString OperatorData::GetPkey()
{
	return m_pkey;
}

CString OperatorData::GetName()
{
	return m_name;
}

CString OperatorData::GetPasswd()
{
	return m_passwd;
}

CString OperatorData::GetEncryptedPasswd()
{
	return m_encryptedPasswd;
}

CString OperatorData::GetUpdateSQL()
{
	return m_updateSQL;
}

CString OperatorData::GetRevertSQL()
{
	return m_revertSQL;
}

OperatorData::~OperatorData()
{

}

int OperatorData::GetHash(CString& data, CString& results)
{
	std::string dataStr;

	dataStr = data.GetBuffer(0);
	
	unsigned char obuf[21];
	
	unsigned char* origin = (unsigned char*) dataStr.c_str();
	
	SHA1(origin, strlen(dataStr.data()), obuf);
	
	std::stringstream stmCon;
	stmCon << std::hex;
	for (int i = 0; i < 20; i++) {
		stmCon << std::setw(2) <<std::setfill('0') << (int) (obuf[i]);
	}
	std::string outputString = stmCon.str();
	for(unsigned int j=0;j<outputString.length();j++)
	{
		outputString[j] = toupper(outputString[j]);
	}
	results = outputString.c_str();

	return 0;
}

BOOL OperatorData::isPasswdMayEncrypted()
{
	int passwdLength = m_passwd.GetLength();

	if (passwdLength >= IS_PASSWD_MAY_ENCRYPTED_LENGTH)
	{
		return TRUE;
	}
	
	return FALSE;
}

void OperatorData::generateUpdateSQL()
{
	if (m_encryptedPasswd.GetLength() != IS_PASSWD_MAY_ENCRYPTED_LENGTH)
	{
		AfxMessageBox("Encrypted password length not equal to 40", MB_OK);
		return;
	}

	m_updateSQL = "update operator set passwd = '" + m_encryptedPasswd + "' where pkey = " + m_pkey;
}

void OperatorData::generateRevertSQL()
{
	if (m_passwd.Compare("")==0)
	{
		AfxMessageBox("Password should not be empty.", MB_OK);
		return;
	}

	m_revertSQL = "update operator set passwd = '" + m_passwd + "' where pkey = " + m_pkey;
}