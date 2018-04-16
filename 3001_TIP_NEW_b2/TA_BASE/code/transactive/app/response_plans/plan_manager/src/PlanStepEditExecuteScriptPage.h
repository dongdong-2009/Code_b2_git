#pragma once
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStepEditParameterPage.h"
#include "TextualStepParameter.h"
#include "resource.h"
#if defined (STEPTYPE_EXECUTE_SCRIPT)

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

        // Dialog Data
        enum { IDD = IDD_PLAN_STEP_EDIT_EXECUTE_SCRIPT_DLG };
    private:
        TextualStepParameter m_commandlineParameter;

        CString    m_commandLine;

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


#endif // STEPTYPE_EXECUTE_SCRIPT