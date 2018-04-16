/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/generic_gui/templates/Dialog/src/TemplateDialogDlg.h $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  *
  */

#if !defined(AFX_TEMPLATEDIALOGDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_)
#define AFX_TEMPLATEDIALOGDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "bus/generic_gui/src/TransActiveDialog.h"

/////////////////////////////////////////////////////////////////////////////
// CTemplateDialogDlg dialog

namespace TA_Base_Bus
{
    class IGUIAccess;
}

class CTemplateDialogDlg : public TA_Base_Bus::TransActiveDialog
{
// Construction
public:
	CTemplateDialogDlg(TA_Base_Bus::IGUIAccess& genericGUICallback,CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTemplateDialogDlg)
	enum { IDD = IDD_TEMPLATEDIALOG_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateDialogDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTemplateDialogDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATEDIALOGDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_)
