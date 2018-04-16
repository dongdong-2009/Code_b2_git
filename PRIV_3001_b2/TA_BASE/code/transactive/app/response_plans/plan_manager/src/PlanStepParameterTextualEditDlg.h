//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterTextualEditDlg.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2018/03/14 13:51:20 $
// Last modified by:  $Author: lim.cy $
//
// <description>

#if !defined(AFX_PLANSTEPPARAMETERTEXTUALEDITDLG_H)
#define AFX_PLANSTEPPARAMETERTEXTUALEDITDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanStepParameterEditDlg.h"
#include "PlanStepListCtrl.h"


namespace TA_Base_App
{
    class TextualStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // PlanStepParameterTextualEditDlg dialog

    class CPlanStepParameterTextualEditDlg : public CPlanStepParameterEditDlg
    {
    // Construction
    public:
        bool test();
        bool m_test;
        CPlanStepParameterTextualEditDlg(TextualStepParameter& parameter,bool canEdit);   // standard constructor

    // Dialog Data
        // {{AFX_DATA(CPlanStepParameterTextualEditDlg)
    enum { IDD = IDD_PLAN_STEP_PARAMETER_TEXTUAL_EDIT_DLG };
    CEdit   m_textValue;
    // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterTextualEditDlg)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CPlanStepParameterTextualEditDlg)
    virtual BOOL OnInitDialog();
    afx_msg void onOK();
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
} // End namespace
#endif // !defined(AFX_PLANSTEPPARAMETERTEXTUALEDITDLG_H)
