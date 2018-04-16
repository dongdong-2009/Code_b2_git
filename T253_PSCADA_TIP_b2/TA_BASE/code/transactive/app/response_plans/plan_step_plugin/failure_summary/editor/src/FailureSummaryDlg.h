
//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/FailureSummaryDlg.h $
// @author:  Lizette Lingo
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>


#if !defined(AFX_FAILURESUMMARYDLG_H__B1C6D463_D910_417F_B687_E49413F9F4DB__INCLUDED_)
#define AFX_FAILURESUMMARYDLG_H__B1C6D463_D910_417F_B687_E49413F9F4DB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FailureSummaryDlg.h : header file
//

#include "resource.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/InteractiveStepDlg.h"
#include "app/response_plans/plan_step_plugin/failure_summary/common/FailureSummaryUserParameter.h"

/////////////////////////////////////////////////////////////////////////////
// CFailureSummaryDlg dialog
namespace TA_Base_App
{

    class CFailureSummaryDlg : public CInteractiveStepDlg
    {
        // Construction
    public:
		CFailureSummaryDlg(const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
			const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters);
        virtual ~CFailureSummaryDlg();

        // Operations
    protected:
        virtual CString getTitlePrefix() const;
        virtual void reportCompletionStatus() const;

        // Dialog Data
        // {{AFX_DATA(CFailureSummaryDlg)
    enum { IDD = IDD_FAILURE_SUMMARY_DLG };
	CString    m_instance;
	CString    m_plan;
	CString    m_location;
	CString    m_step;
    CListCtrl    m_failedStepListCtrl;
    // }}AFX_DATA


        // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CFailureSummaryDlg)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

        // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CFailureSummaryDlg)
    virtual BOOL OnInitDialog();
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
	private:
		FailureSummaryUserParameter m_params;
    };
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif // !defined(AFX_FAILURESUMMARYDLG_H__B1C6D463_D910_417F_B687_E49413F9F4DB__INCLUDED_)
