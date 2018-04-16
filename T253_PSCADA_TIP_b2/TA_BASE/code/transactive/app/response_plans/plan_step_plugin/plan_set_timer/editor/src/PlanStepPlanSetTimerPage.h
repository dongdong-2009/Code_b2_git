#pragma once
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"

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

    private:
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