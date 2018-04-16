#pragma once
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"

// CPlanStepExecuteStepPage dialog
namespace TA_Base_App
{
    class ExecuteScriptStep;

    class CPlanStepExecuteScriptPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepExecuteScriptPage)

    public:
        CPlanStepExecuteScriptPage(ExecuteScriptStep* step);
        virtual ~CPlanStepExecuteScriptPage();

    private:
        CEdit    m_commandLineEdit;

    protected:

        virtual void DisplayParameters();
        virtual void UpdateParameters();
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

        // Generated message map functions
        // {{AFX_MSG(CPlanStepExecuteScriptPage)

        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}