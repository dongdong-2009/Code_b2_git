/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/incident_manager/src/PlanDialog.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * The dialog that will be shown to the operator when the showJobDetailsDialog() method is called from
  * the IJobDetailsComponent interface.  The dialog will allow the operator to select a plan.
  * 
  */

#include "app/incidents/incident_manager/src/stdafx.h"
#include "app/incidents/incident_manager/src/PlanDialog.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PlanDialog dialog

namespace TA_IRS_App
{
	PlanDialog::PlanDialog(bool editable, CWnd* pParent /*=NULL*/)
		: CDialog(PlanDialog::IDD,pParent), 
		m_planCtrl(TA_Base_Bus::PlanFilter::PFT_APPROVED_PLANS, true), 
		m_editable(editable),
		m_confirmSelection(false)
	{
		FUNCTION_ENTRY( "Constructor" );
		//{{AFX_DATA_INIT(PlanDialog)
		//}}AFX_DATA_INIT
		FUNCTION_EXIT;
		
	}
	
	
	void PlanDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(PlanDialog)
		DDX_Control(pDX, IDOK, m_okBtn);
		DDX_Control(pDX, IDC_TREE_PLAN, m_planCtrl);
		//}}AFX_DATA_MAP
	}
	
	
	BEGIN_MESSAGE_MAP(PlanDialog, CDialog)
		//{{AFX_MSG_MAP(PlanDialog)
		ON_NOTIFY(NM_CLICK, IDC_TREE_PLAN, OnClickTreePlan)
		ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PLAN, OnSelchangedTreePlan)
		//}}AFX_MSG_MAP
		END_MESSAGE_MAP()
		
		/////////////////////////////////////////////////////////////////////////////
		// PlanDialog message handlers
		
		
		void PlanDialog::OnOK() 
	{
		FUNCTION_ENTRY( "OnOK" );
		
		TA_Base_Bus::CPlanItem* planItem = m_planCtrl.getSelectedPlan();
		if (planItem == NULL)
		{
			m_confirmSelection = false;
		}
		else
		{
			m_confirmSelection = true;
			m_selectedPlanType = planItem->getPlanPath();
		}
		
		CDialog::OnOK();
		FUNCTION_EXIT;
	}
	
	void PlanDialog::OnCancel() 
	{
		FUNCTION_ENTRY( "OnCancel" );
		m_confirmSelection = false;
		CDialog::OnCancel();
		FUNCTION_EXIT;
	}
	
	PlanType PlanDialog::getSelectedPlanType() const
	{
		FUNCTION_ENTRY( "getSelectedPlanType" );
		TA_ASSERT(m_confirmSelection, "Operator has not confirmed the selection!");
		FUNCTION_EXIT;
		return m_selectedPlanType;
	}
	
	void PlanDialog::OnClickTreePlan(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		FUNCTION_ENTRY( "OnClickTreePlan" );
		
		GetDlgItem(IDOK)->EnableWindow( m_planCtrl.getSelectedPlan() != NULL );
		*pResult = 0;
		FUNCTION_EXIT;
	}
	
	void PlanDialog::OnSelchangedTreePlan(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		FUNCTION_ENTRY( "OnSelchangedTreePlan" );
		GetDlgItem(IDOK)->EnableWindow( m_planCtrl.getSelectedPlan() != NULL );
		
		*pResult = 0;
		FUNCTION_EXIT;
	}
	
	BOOL PlanDialog::OnInitDialog() 
	{
		FUNCTION_ENTRY( "OnInitDialog" );
		CDialog::OnInitDialog();
		GetDlgItem(IDOK)->EnableWindow(FALSE);
		
		FUNCTION_EXIT;
		return TRUE;  // return TRUE unless you set the focus to a control
		// EXCEPTION: OCX Property Pages should return FALSE
	}
}// end TA_IRS_App
