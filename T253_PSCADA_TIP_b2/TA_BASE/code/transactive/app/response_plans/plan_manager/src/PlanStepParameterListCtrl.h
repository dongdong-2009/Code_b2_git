//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterListCtrl.h $
// @author:  Bart Golab
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2015/01/14 19:04:52 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#if !defined(AFX_PLANSTEPPARAMETERLISTCTRL_H__ABCD3DA6_B2F8_4DAC_AB0E_3FC548AD4068__INCLUDED_)
#define AFX_PLANSTEPPARAMETERLISTCTRL_H__ABCD3DA6_B2F8_4DAC_AB0E_3FC548AD4068__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepParameterListCtrl window

    class CPlanStepParameterListCtrl : public CListCtrl
    {
    // Construction
    public:
        CPlanStepParameterListCtrl();
        virtual ~CPlanStepParameterListCtrl();

    // Operations
    public:
        virtual BOOL SetReadOnly(BOOL readOnly = TRUE);
        virtual void AddParameter(CString parameterName, CString parameterValue);

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterListCtrl)
    protected:
        virtual void PreSubclassWindow();
        // }}AFX_VIRTUAL

    // Implementation
        // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanStepParameterListCtrl)
        afx_msg void OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
    // }}AFX_MSG

        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANSTEPPARAMETERLISTCTRL_H__ABCD3DA6_B2F8_4DAC_AB0E_3FC548AD4068__INCLUDED_)
