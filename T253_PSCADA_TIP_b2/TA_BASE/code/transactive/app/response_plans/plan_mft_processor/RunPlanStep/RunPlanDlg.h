//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/RunPlanDlg.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_RUNPLANDLG_H__4FA01EE9_9C54_4E61_BF7B_877E4007909E__INCLUDED_)
#define AFX_RUNPLANDLG_H__4FA01EE9_9C54_4E61_BF7B_877E4007909E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "app/response_plans/plan_step_plugin/common_editor/src/InteractiveStepDlg.h"
#include "app/response_plans/plan_step_plugin/internal/src/RunPlanUserParameter.h"

namespace TA_Base_Bus
{
	class RightsLibrary;
}

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CRunPlanDlg dialog

    class CRunPlanDlg : public CInteractiveStepDlg
    {
    // Construction
    public:
		CRunPlanDlg(const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
			const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters);
        virtual ~CRunPlanDlg();

    // Operations
    protected:
        virtual CString getTitlePrefix() const;
        virtual void reportCompletionStatus() const;


    // Dialog Data
    private:
        // {{AFX_DATA(CRunPlanDlg)
        enum { IDD = IDD_RUN_PLAN_DLG };
		CString    m_instance;
		CString    m_plan;
		CString    m_location;
		CString    m_step;
        CString    m_message;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CRunPlanDlg)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CRunPlanDlg)
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
	private:
		bool canControlPlans() const;
	private:
		RunPlanUserParameter m_params;

		TA_Base_Bus::RightsLibrary* m_rightsLibrary;
		unsigned long m_resourceId;
    };

} // namespace TA_App
#endif // !defined(AFX_RUNPLANDLG_H__4FA01EE9_9C54_4E61_BF7B_877E4007909E__INCLUDED_)
