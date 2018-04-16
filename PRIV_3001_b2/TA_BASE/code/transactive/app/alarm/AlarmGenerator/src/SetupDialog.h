/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source: /cvs/1TS-9999-T/code/tools/integration/AlarmGenerator/src/SetupDialog.h,v $
  * @author:  Andrew Del Carlo
  * @version: $Revision: 1.1.2.3.16.1 $
  *
  * Last modification: $Date: 2005/05/25 03:02:53 $
  * Last modified by:  $Author: derrickl $
  * 
  * implementation of the setup dialog
  */

#if !defined(AFX_SETUPDIALOG_H__48C8B3D3_2FEB_486A_AC78_65121B11016D__INCLUDED_)
#define AFX_SETUPDIALOG_H__48C8B3D3_2FEB_486A_AC78_65121B11016D__INCLUDED_

#include <string>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetupDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSetupDialog dialog

class CAlarmGeneratorDlg;

class CSetupDialog : public CDialog
{
// Construction
public:
	CSetupDialog(CAlarmGeneratorDlg* pParent, std::string* notifyHosts, std::string* dbConnection, std::string* locationName, std::string* profileId, std::string* userId, std::string* mgrPort);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetupDialog)
	enum { IDD = IDD_SETUP_DIALOG };
	CEdit	m_locationNameEdit;
	CEdit	m_notifyHostsEdit;
	CEdit	m_dbConnectionEdit;
	CEdit	m_profileIDEdit;
	CEdit	m_userIDEdit;
	CEdit	m_mgrPortEdit;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	std::string* m_dbConnection;
	std::string* m_notifyHosts;
	std::string* m_locationName;
	std::string* m_profileID;
	std::string* m_userID;
	std::string* m_mgrPort; 
	CAlarmGeneratorDlg* m_parent;
	// Generated message map functions
	//{{AFX_MSG(CSetupDialog)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETUPDIALOG_H__48C8B3D3_2FEB_486A_AC78_65121B11016D__INCLUDED_)
