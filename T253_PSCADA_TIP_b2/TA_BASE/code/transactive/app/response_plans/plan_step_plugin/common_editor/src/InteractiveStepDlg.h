//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/InteractiveStepDlg.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2014/06/03 09:20:18 $
// Last modified by:  $Author: huangjian $
//
// <description>

#if !defined(AFX_INTERACTIVESTEPDLG_H__53DD899C_1F2F_47F2_B2F9_953290E43456__INCLUDED_)
#define AFX_INTERACTIVESTEPDLG_H__53DD899C_1F2F_47F2_B2F9_953290E43456__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include "app/response_plans/plan_step_plugin/common_editor/src/DllExport.h"

namespace TA_Base_App
{
    //class CPlanControllerDoc;


    /////////////////////////////////////////////////////////////////////////////
    // CInteractiveStepDlg dialog

    class PLAN_COMM_DLL CInteractiveStepDlg : public CDialog
    {
    // Construction
    protected:
		CInteractiveStepDlg(const unsigned int unResTemplateId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
			const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters, time_t timeout);
        virtual ~CInteractiveStepDlg();

    // Operations
    protected:

        virtual CString getTitlePrefix() const = 0;
        virtual void reportCompletionStatus() const = 0;
        virtual void startTimeoutCountdown();
        virtual void updateTimeoutCountdownDisplay();

        virtual void setCompletionStatus(int exitCode);
        virtual int getCompletionStatus() const;

        void setResponseTimeout(unsigned long timeout);

    // Attributes
    private:
        //TA_Base_Core::InteractiveStepData m_stepDetail;
        int m_completionStatus;
        time_t m_timeout;
        time_t m_secondsToTimeout;

        bool m_stepExecutionInterrupted;

    // Dialog Data
    private:
        // {{AFX_DATA(CDecisionDlg)
        //CString    m_instance;
        //CString    m_plan;
        //CString    m_location;
        //CString    m_step;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CInteractiveStepDlg)
        afx_msg virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
        afx_msg virtual BOOL OnInitDialog();
        afx_msg virtual void OnCreate( LPCREATESTRUCT lpCreateStruct );
        afx_msg virtual void OnOK();
        afx_msg virtual void OnCancel();
        afx_msg virtual void PostNcDestroy();
        afx_msg void OnTimer( UINT nIDEvent );
        afx_msg void OnDestroy();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
	protected:
		int m_eType;
		unsigned int m_ulLocation;
		unsigned int m_ulPlanId; 
		unsigned int m_ulInstanceNum;
		unsigned int m_ulStepNum;
		std::vector<std::string> m_parameters;
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_INTERACTIVESTEPDLG_H__53DD899C_1F2F_47F2_B2F9_953290E43456__INCLUDED_)
