#pragma once
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"
#include "PlanStepEditParameterPage.h"
#include "StepNumberStepParameter.h"
#include "TimeSpanStepParameter.h"
#include "resource.h"
#if defined (STEPTYPE_PLAN_CHECK_TIMER)

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

        // Dialog Data
        enum { IDD = IDD_PLAN_STEP_EDIT_PLAN_CHECK_TIMER_DLG };
    private:
        TimeSpanStepParameter m_timeSpanParameter;
        StepNumberStepParameter m_successParameter;
        StepNumberStepParameter m_failureParameter;

        CDateTimeCtrl      m_timeSpan;
        CString    m_failStepLabel;
        CString    m_successStepLabel;
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


#endif // STEPTYPE_PLAN_CHECK_TIMER