#if !defined(AFX_UPDATEDATABASEDLG_H__3FCD0AD0_D2B4_407A_BFDE_A82C0C41B4FC__INCLUDED_)
#define AFX_UPDATEDATABASEDLG_H__3FCD0AD0_D2B4_407A_BFDE_A82C0C41B4FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UpdateDatabaseDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUpdateDatabaseDlg dialog
#include "OperatorData.h"
#include "DataBase.h"
#include "ExecuteSql.h"
#include <vector>

class CUpdateDatabaseDlg : public CDialog
{
// Construction
public:
	CUpdateDatabaseDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUpdateDatabaseDlg)
	enum { IDD = IDD_UPDATE_DIALOG };
	CButton	m_exportBtn;
	CButton	m_copyBtn;
	CEdit	m_updateStatementEdit;
	CStatic	m_connectInfoStatic;
	CString	m_updateStatementValue;
	CString	m_connectInfoValue;
	//}}AFX_DATA
public:
	CString m_servername;
	CString	m_username;
	CString	m_password;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdateDatabaseDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUpdateDatabaseDlg)
	afx_msg void OnBtnGenerate();
	afx_msg void OnBtnPerform();
	afx_msg void OnClose();
	afx_msg void OnBtnCopy();
	afx_msg void OnChangeEditUpdatesqlStatement();
	afx_msg void OnBtnExport();
	afx_msg void OnBtnRevert();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	HICON m_hIcon;

private:
	DataBase *m_dbConn;
	ExecuteSql *m_exeSql;

	enum SQLTYPE
	{
		Update = 0,
		Revert
	};

	std::vector<OperatorData*> m_operatorDatas;
	std::vector<CString> m_updateSQLs;
	std::vector<CString> m_revertSQLs;
	
	BOOL m_isAlreadyGeneratedSQL;
	BOOL m_isAlreadyPerformedUpdate;

	BOOL m_noticeSaveRevertSQLWhenExit;

	void initConnectDatabase();
	void clearOperatorDataRelated();

	void generateSQL();
	void updateSQLStatement();
	void revertSQLStatement();

	void saveSQLFile(SQLTYPE sqlType, CTime currentTime);

	// Tools function
	CString toUpper(CString str);
	CString toLower(CString str);

	CString getDefaultSaveFileName(CString sqlTypeStr, CTime currentTime);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATEDATABASEDLG_H__3FCD0AD0_D2B4_407A_BFDE_A82C0C41B4FC__INCLUDED_)
