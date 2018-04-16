#pragma once
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"

// CPlanStepPlanCheckTimerPage dialog
namespace TA_Base_App
{
    class PlanCheckTimerStep;

    class CPlanStepPlanCheckTimerPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepPlanCheckTimerPage)

    public:
        CPlanStepPlanCheckTimerPage(PlanCheckTimerStep* step);
        virtual ~CPlanStepPlanCheckTimerPage();

    private:
        CDateTimeCtrl      m_timeSpan;
        CEdit    m_failStepLabel;
        CEdit    m_successStepLabel;
    protected:
        void ResetSuccessAndFailureStepDisplay();
        virtual void DisplayParameters();
        virtual void UpdateParameters();
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

        // Generated message map functions
        // {{AFX_MSG(CPlanStepPlanCheckTimerPage)
        afx_msg void OnPickFailStepButton();
        afx_msg void OnPickSuccessStepButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}