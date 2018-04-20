/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/incidents/incident_manager/src/PlanDialog.h $
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

#if !defined(AFX_PLANDIALOG_H__980B399D_9CE2_4186_8547_638BDA034E3B__INCLUDED_)
#define AFX_PLANDIALOG_H__980B399D_9CE2_4186_8547_638BDA034E3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/incidents/incident_manager/src/resource.h"
#include "bus/response_plans/plan_tree/src/PlanTreeCtrl.h"
#include "bus/response_plans/plan_tree/src/PlanItem.h"
#include "core/data_access_interface/Incident_dai/src/PlanType.h"
#include "bus/mfc_extensions/src/mfc_extensions.h"

/////////////////////////////////////////////////////////////////////////////
// PlanDialog dialog
using namespace TA_IRS_Core;

namespace TA_IRS_App
{
	
	class PlanDialog : public CDialog
	{
		// Construction
	public:
		/** 
		* PlanDialog
		*
		* Shows the plan dialog.  If readOnly is true, the dialog will not allow a new plan to be selected.  The dialog
		* will highlight the specified plan.
		*
		* @param readOnly If true, operator cannot select new plan
		* @param selectedPlanId The plan Id of the plan to be selected
		* @param pParent The parent of this dialog
		*
		*/
		PlanDialog(bool editable, CWnd* pParent = NULL);   // standard constructor
		
		/** 
		* getSelectedPlan
		*
		* Returns the node Id of the selected plan
		*
		*
		* @return the node Id of the selected plan
		*/
		PlanType getSelectedPlanType() const;
		
		
		// Dialog Data
		//{{AFX_DATA(PlanDialog)
		enum { IDD = IDD_PLAN_DIALOG };
		TA_Base_Bus::CPlanTreeCtrl	m_planCtrl;
		CButton	m_okBtn;
		//}}AFX_DATA
		
		
		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(PlanDialog)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL
		
		// Implementation
	protected:
		
		// Generated message map functions
		//{{AFX_MSG(PlanDialog)
		virtual void OnOK();
		virtual void OnCancel();
		afx_msg void OnClickTreePlan(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg void OnSelchangedTreePlan(NMHDR* pNMHDR, LRESULT* pResult);
		virtual BOOL OnInitDialog();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
			
	private:
		PlanType m_selectedPlanType;
		bool    m_editable;
		bool    m_confirmSelection;
	};
}// end TA_IRS_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANDIALOG_H__980B399D_9CE2_4186_8547_638BDA034E3B__INCLUDED_)
