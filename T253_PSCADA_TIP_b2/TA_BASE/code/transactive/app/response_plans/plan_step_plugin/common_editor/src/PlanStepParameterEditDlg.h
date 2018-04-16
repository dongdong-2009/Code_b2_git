//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/PlanStepParameterEditDlg.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2014/06/03 09:20:18 $
// Last modified by:  $Author: huangjian $
//
// <description>

#if !defined(AFX_PLANSTEPPARAMETEREDITDLG_H__29F92798_A7CD_4156_A709_06CC773CD9D3__INCLUDED_)
#define AFX_PLANSTEPPARAMETEREDITDLG_H__29F92798_A7CD_4156_A709_06CC773CD9D3__INCLUDED_

#include <afxext.h>
#include "app/response_plans/plan_step_plugin/common_editor/src/DllExport.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanStepParameterForm.h"

class IActivityParameter;

namespace TA_Base_App
{
    class IPlanStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepParameterEditDlg dialog

    class PLAN_COMM_DLL CPlanStepParameterEditDlg : public CDialog, public IPlanStepParameterForm
    {
    // Construction
    public:
        CPlanStepParameterEditDlg( IPlanStepParameter& parameter, bool canEdit );   // standard constructor
        CPlanStepParameterEditDlg( IPlanStepParameter& parameter, UINT templateID, bool canEdit );   // called by derived classes

        IPlanStepParameter* GetStepParameter() const { return &m_parameter; };

        virtual void present();
        virtual void close();

    // Dialog Data
        // {{AFX_DATA(CPlanStepParameterEditDlg)
    CString m_parameterName;
    // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterEditDlg)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();
        virtual afx_msg LRESULT OnAdjustFont( WPARAM wParam, LPARAM lParam );
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        bool m_canEdit;
        IPlanStepParameter& m_parameter;

        // Generated message map functions
        // {{AFX_MSG(CPlanStepParameterEditDlg)
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}  // End namespace

#endif // !defined(AFX_PLANSTEPPARAMETEREDITDLG_H__29F92798_A7CD_4156_A709_06CC773CD9D3__INCLUDED_)
