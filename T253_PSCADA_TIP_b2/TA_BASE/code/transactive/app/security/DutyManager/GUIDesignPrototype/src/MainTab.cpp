/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/security/DutyManager/GUIDesignPrototype/src/MainTab.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/01/23 17:57:05 $
  * Last modified by:  $Author: builder $
  *
  */
// MainTab.cpp : implementation file
//

#include "stdafx.h"
#include "DutyManager.h"
#include "MainTab.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainTab

CMainTab::CMainTab()
{
	// Add the required dialogs
	AddPage(m_siteSummaryDialog,IDD_SITESUMMARY,CString("Locations"));
	AddPage(m_subSystemSummaryDialog,IDD_SUBSYSTEMSUMMARY,CString("Sub Systems"));
}

CMainTab::~CMainTab()
{
}


BEGIN_MESSAGE_MAP(CMainTab, CTabDialogCtrl)
	//{{AFX_MSG_MAP(CMainTab)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainTab message handlers
