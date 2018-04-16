// OperatorData.h: interface for the OperatorData class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OPERATORDATA_H__774E9C96_235B_4D20_B645_AB8DF9567E90__INCLUDED_)
#define AFX_OPERATORDATA_H__774E9C96_235B_4D20_B645_AB8DF9567E90__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class OperatorData  
{
public:
	OperatorData(CString pkey, CString name, CString passwd);
	virtual ~OperatorData();

	CString GetPkey();
	CString GetName();
	CString GetPasswd();
	CString GetEncryptedPasswd();

	CString GetUpdateSQL();
	CString GetRevertSQL();

	BOOL isPasswdMayEncrypted();

private:
	CString m_pkey;
	CString m_name;
	CString m_passwd;
	CString m_encryptedPasswd;

	CString m_updateSQL;
	CString m_revertSQL;

	int GetHash(CString& data, CString& results);
	void generateUpdateSQL();
	void generateRevertSQL();
};

#endif // !defined(AFX_OPERATORDATA_H__774E9C96_235B_4D20_B645_AB8DF9567E90__INCLUDED_)
