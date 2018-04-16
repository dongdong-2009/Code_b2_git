/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/scada/inspector_panel/src/ConfirmCommandDialog.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class manages the dialog which confirms that the data point is to be updated.
  * It provides details of the data point and the new value.
  * The return from DoModal() is used to indicate if the update should proceed (IDOK) or
  * be cancelled (IDCANCEL).
  */

#include "app/scada/Inspector_Panel/src/stdafx.h"
#include "app/scada/Inspector_Panel/src/InspectorPanel.h"
#include "app/scada/Inspector_Panel/src/ConfirmCommandDialog.h"
#include "bus/generic_gui/src/HelpLauncher.h" //TD15124 azenitha

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CConfirmCommandDialog dialog


	CConfirmCommandDialog::CConfirmCommandDialog(CWnd* pParent /*=NULL*/)
		: CDialog(CConfirmCommandDialog::IDD, pParent)
	{
		//{{AFX_DATA_INIT(CConfirmCommandDialog)
		m_equipment = _T("");
		m_newValue = _T("");
		m_dataPoint = _T("");
		//}}AFX_DATA_INIT
	}


	void CConfirmCommandDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CConfirmCommandDialog)
		//DDX_Text(pDX, IDC_EQUIPMENT, m_equipment);
		DDX_Text(pDX, IDC_NEW_VALUE, m_newValue);
		DDX_Text(pDX, IDC_DATA_POINT, m_dataPoint);
		//}}AFX_DATA_MAP
		GetDlgItem(IDC_EDIT_EQUIPMENT)->SetWindowText(m_equipment);
	}


	BEGIN_MESSAGE_MAP(CConfirmCommandDialog, CDialog)
		//{{AFX_MSG_MAP(CConfirmCommandDialog)
			// NOTE: the ClassWizard will add message map macros here
		ON_BN_CLICKED(ID_HELP_CC, OnHelpButton) //TD15124 azenitha
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CConfirmCommandDialog message handlers

	//TD15124 azenitha++
	void CConfirmCommandDialog::OnHelpButton() 
	{
		TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
	}
	//TD15124 ++azenitha
}
