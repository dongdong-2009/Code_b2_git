/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/maintenance_management/MmsController/src/MainTabControl.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */
// MainTabControl.cpp : implementation file
//

#include "app/maintenance_management/MmsController/src/stdafx.h"
#include "app/maintenance_management/MmsController/src/MainTabControl.h"
#include "bus/application_types/src/apptypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/maintenance_management/MmsController/src/MmsControllerModel.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainTabControl

namespace TA_IRS_App
{	
	CMainTabControl::CMainTabControl() : m_model(NULL)
	{	 
		AddPage(m_scadaMmsInhibit,IDD_SCADAINHIBIT, CString("Scada"));

		// Remove SubSystem Inhibit
		// AddPage(m_systemMmsInhibit, IDD_SUBSYSTEMSINHIBIT, CString("System"));
	}
	
	CMainTabControl::~CMainTabControl()
	{
	}
	
	
	BEGIN_MESSAGE_MAP(CMainTabControl, CTabDialogCtrl)
		//{{AFX_MSG_MAP(CMainTabControl)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()
		
	/////////////////////////////////////////////////////////////////////////////
	// CMainTabControl message handlers
	
	void CMainTabControl::setMmsController(MmsControllerModel* model)
	{
		m_scadaMmsInhibit.setMmsController(model);
		// Remove SubSystem Inhibit
		//m_systemMmsInhibit.setMmsController(model);

		m_model = model;
	}
 


	void CMainTabControl::updateTabControls()
	{
		ETabIndex tabIndex = (ETabIndex) GetCurSel();
		if (tabIndex == EScadaTab)
		{
			m_scadaMmsInhibit.updateSubsystemButtons();		
		}
		// Remove SubSystem Inhibit
// 		else if (tabIndex == ESystemTab)
// 		{
// 			m_systemMmsInhibit.updateSystemInhibitList(false);
// 		}
	}
}