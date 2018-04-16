//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/InteractiveStepDlg.h $
// @author:  Bart Golab
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2013/01/18 17:55:27 $
// Last modified by:  $Author: huangjian $
//
// <description>

#if !defined(AFX_INTERACTIVESTEPDLG_H__53DD899C_1F2F_47F2_B2F9_953290E43456__INCLUDED_)
#define AFX_INTERACTIVESTEPDLG_H__53DD899C_1F2F_47F2_B2F9_953290E43456__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/response_plans/plan_manager/IDL/src/IPlanManagerCorbaDef.h"
#include "IActivePlanObserver.h"


namespace TA_Base_App
{
    class CPlanControllerDoc;


    /////////////////////////////////////////////////////////////////////////////
    // CInteractiveStepDlg dialog

    class CInteractiveStepDlg : public CDialog,
                                public IActivePlanObserver
    {
    // Construction
    protected:
        CInteractiveStepDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail, TA_Base_Core::Seconds timeout); // standard constructor
        virtual ~CInteractiveStepDlg();

    // Operations
    protected:
        // IActivePlanObserver interface
        void activeStateChanged(const TA_Base_Core::ActivePlanDetail &activePlanDetail);

        virtual CString getTitlePrefix() const = 0;
        virtual void reportCompletionStatus() const = 0;
        virtual void startTimeoutCountdown();
        virtual void updateTimeoutCountdownDisplay();

        virtual void setCompletionStatus(int exitCode);
        virtual TA_Base_Core::EPlanStepCompletionStatus getCompletionStatus() const;
        virtual TA_Base_Core::CurrentPlanStepDetail getStepDetail() const;


    // Attributes
    private:
        TA_Base_Core::CurrentPlanStepDetail m_stepDetail;
        TA_Base_Core::EPlanStepCompletionStatus m_completionStatus;
        unsigned long m_timeout;
        unsigned long m_secondsToTimeout;

        bool m_stepExecutionInterrupted;


    // Dialog Data
    private:
        // {{AFX_DATA(CDecisionDlg)
        CString    m_instance;
        CString    m_plan;
        CString    m_location;
        CString    m_step;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CInteractiveStepDlg)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();
        virtual void OnOK();
        virtual void OnCancel();
        virtual void PostNcDestroy();
        // }}AFX_VIRTUAL

        // Generated message map functions
        // {{AFX_MSG(CInteractiveStepDlg)
        afx_msg void OnTimer(UINT nIDEvent);
        afx_msg void OnDestroy();
        // }}AFX_MSG
        afx_msg LRESULT OnStepExecutionInterrupted(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_INTERACTIVESTEPDLG_H__53DD899C_1F2F_47F2_B2F9_953290E43456__INCLUDED_)
