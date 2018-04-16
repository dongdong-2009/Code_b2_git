#pragma once
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"
#include "PlanStepEditParameterPage.h"
#include "StepNumberStepParameter.h"
#include "TimeSpanStepParameter.h"
#include "resource.h"
#if defined (STEPTYPE_PLAN_CHECK_TIMER)

// CPlanStepPlanSetTimerPage dialog
namespace TA_Base_App
{
    class PlanSetTimerStep;

    class CPlanStepPlanSetTimerPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepPlanSetTimerPage)

    public:
        CPlanStepPlanSetTimerPage(PlanSetTimerStep* step);
        virtual ~CPlanStepPlanSetTimerPage();

        // Dialog Data
        enum { IDD = IDD_PLAN_STEP_EDIT_PLAN_SET_TIMER_DLG };
    private:
        TimeSpanStepParameter m_timeSpanParameter;

        CDateTimeCtrl      m_timeSpan;

    protected:
        void ResetSuccessAndFailureStepDisplay();
        virtual void DisplayParameters();
        virtual void UpdateParameters();
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

        // Generated message map functions
        // {{AFX_MSG(CPlanStepPlanSetTimerPage)

        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}


#endif // STEPTYPE_PLAN_CHECK_TIMER