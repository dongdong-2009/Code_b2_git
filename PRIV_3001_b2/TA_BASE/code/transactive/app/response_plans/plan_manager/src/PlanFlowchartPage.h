//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanFlowchartPage.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2018/03/14 13:51:20 $
// Last modified by:  $Author: lim.cy $
//
// <description>

#if !defined(AFX_PLANFLOWCHARTPAGE_H__F29B4AF2_09B5_4738_873A_D193050D1F3C__INCLUDED_)
#define AFX_PLANFLOWCHARTPAGE_H__F29B4AF2_09B5_4738_873A_D193050D1F3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "WebBrowser.h"
#include "PlanDetailPage.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanFlowchartPage dialog

    class CPlanFlowchartPage : public CPlanDetailPage
    {
    // Construction
    public:
        CPlanFlowchartPage();   // standard constructor
        virtual ~CPlanFlowchartPage();

    // Operations
    public:

    protected:
        virtual void SetPlanFilterButtonStates();
        virtual void ShowRelevantControls();
        virtual void RefreshControlContents();
        virtual void RefreshActiveInstances();
        virtual void PurgeActiveInstances();


    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanFlowchartPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL


        // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanFlowchartPage)
        virtual BOOL OnInitDialog();
        virtual BOOL OnSetActive();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()


    // Dialog Data
    protected:
        // {{AFX_DATA(CPlanFlowchartPage)
        enum { IDD = IDD_PLAN_FLOWCHART_DLG };
        CWebBrowser    m_flowchartBrowser;
        // }}AFX_DATA
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANFLOWCHARTPAGE_H__F29B4AF2_09B5_4738_873A_D193050D1F3C__INCLUDED_)
