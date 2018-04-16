/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/telephone/telephone_manager/src/ConfigureDirectoryDialog.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#if !defined(AFX_CONFIGUREDIRECTORYDIALOG_H__33CA3ED5_8829_11D7_B17D_0050BAB1D931__INCLUDED_)
#define AFX_CONFIGUREDIRECTORYDIALOG_H__33CA3ED5_8829_11D7_B17D_0050BAB1D931__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigureDirectoryDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// ConfigureDirectoryDialog dialog

class ConfigureDirectoryDialog : public CDialog
{
// Construction
public:
	ConfigureDirectoryDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(ConfigureDirectoryDialog)
	enum { IDD = IDD_FACILITY_CONFIG_DIRECTORY };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ConfigureDirectoryDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(ConfigureDirectoryDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGUREDIRECTORYDIALOG_H__33CA3ED5_8829_11D7_B17D_0050BAB1D931__INCLUDED_)
