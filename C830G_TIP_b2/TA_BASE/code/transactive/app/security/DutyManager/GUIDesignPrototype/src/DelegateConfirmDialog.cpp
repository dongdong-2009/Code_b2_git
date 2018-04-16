/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/security/DutyManager/GUIDesignPrototype/src/DelegateConfirmDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// DelegateConfirmDialog.cpp : implementation file
//

#include "stdafx.h"
#include "DutyManager.h"
#include "DelegateConfirmDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDelegateConfirmDialog dialog


CDelegateConfirmDialog::CDelegateConfirmDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDelegateConfirmDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDelegateConfirmDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDelegateConfirmDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDelegateConfirmDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDelegateConfirmDialog, CDialog)
	//{{AFX_MSG_MAP(CDelegateConfirmDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDelegateConfirmDialog message handlers
