/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/security/DutyManager/GUIDesignPrototype/src/MainTab.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
