/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/MainTabCtrl.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/01/23 17:49:34 $
  * Last modified by:  $Author: builder $
  *
  */
// MainTabCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "equipmentstatusviewer.h"
#include "MainTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainTabCtrl

CMainTabCtrl::CMainTabCtrl()
{
	AddPage(m_equipmentStatusDialog,IDD_EQUIPMENT_STATUS,CString("Equipment Status"));
	AddPage(m_configurationDialog,IDD_CONFIGURATION,CString("Configuration"));
}

CMainTabCtrl::~CMainTabCtrl()
{
}


BEGIN_MESSAGE_MAP(CMainTabCtrl, CTabDialogCtrl)
	//{{AFX_MSG_MAP(CMainTabCtrl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainTabCtrl message handlers
