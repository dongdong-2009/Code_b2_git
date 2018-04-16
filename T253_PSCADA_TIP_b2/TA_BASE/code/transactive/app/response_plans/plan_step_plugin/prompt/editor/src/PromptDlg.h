//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PromptDlg.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_PROMPTDLG_H__C36A3A03_FE79_4CCF_8AD8_A40239D5EB24__INCLUDED_)
#define AFX_PROMPTDLG_H__C36A3A03_FE79_4CCF_8AD8_A40239D5EB24__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/InteractiveStepDlg.h"
#include "app/response_plans/plan_step_plugin/prompt/common/PromptStepParameter.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPromptDlg dialog

    class CPromptDlg : public CInteractiveStepDlg
    {
    // Construction/destruction
    public:
		CPromptDlg(const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
			const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters);
        virtual ~CPromptDlg();

    // Operations
    protected:
        virtual CString getTitlePrefix() const;
        virtual void reportCompletionStatus() const;


    // Dialog Data
    private:
        // {{AFX_DATA(CPromptDlg)
        enum { IDD = IDD_PROMPT_DLG };
		CString    m_instance;
		CString    m_plan;
		CString    m_location;
		CString    m_step;
        CString    m_prompt;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPromptDlg)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual BOOL OnInitDialog();
        // }}AFX_VIRTUAL

        // Generated message map functions
        // {{AFX_MSG(CPromptDlg)
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
	private:
		PromptStepParameter m_params;
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PROMPTDLG_H__C36A3A03_FE79_4CCF_8AD8_A40239D5EB24__INCLUDED_)
