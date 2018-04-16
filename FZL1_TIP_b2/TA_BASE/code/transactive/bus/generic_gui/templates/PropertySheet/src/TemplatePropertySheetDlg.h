/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_gui/templates/PropertySheet/src/TemplatePropertySheetDlg.h $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  *
  */

#if !defined(AFX_TEMPLATEPROPERTYSHEETDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_)
#define AFX_TEMPLATEPROPERTYSHEETDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "bus/generic_gui/src/TransActivePropertySheet.h"

/////////////////////////////////////////////////////////////////////////////
// CTemplatePropertySheetDlg dialog

namespace TA_Base_Bus
{
    class IGUIAccess;
}

class CTemplatePropertySheetDlg : public TA_Base_Bus::TransActivePropertySheet
{
    DECLARE_DYNAMIC(CTemplatePropertySheetDlg)
// Construction
public:
	CTemplatePropertySheetDlg(TA_Base_Bus::IGUIAccess& genericGUICallback,CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CECSManagerDlg)
	enum { IDD = IDD_TEMPLATEPROPERTYSHEET_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplatePropertySheetDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CTemplatePropertySheetDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATEPROPERTYSHEETDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_)
