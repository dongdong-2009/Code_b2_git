// D:\workspace\3001_TIP_NEW\TA_BASE\transactive\app\response_plans\plan_manager\src\PlanStepPlanStartTimeCheckPage.cpp : implementation file
//

#include "StdAfx.h"

#if defined (STEPTYPE_EXECUTE_SCRIPT)

#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanManager.h"
#include "ExecuteScriptStep.h"
#include "PlanStepEditExecuteScriptPage.h"
#include "PlanStepEditParameterPage.h"
#include "Utilities.h"
#include "LabelMaker.h"
#include "MessageBox.h"


// CPlanStepExecuteScriptPage dialog

using namespace TA_Base_App;

IMPLEMENT_DYNAMIC(CPlanStepExecuteScriptPage, CPlanStepEditParameterPage)

CPlanStepExecuteScriptPage::CPlanStepExecuteScriptPage(ExecuteScriptStep* step) :
CPlanStepEditParameterPage(CPlanStepExecuteScriptPage::IDD, step),
m_commandlineParameter(*step->getCommandLineParameter()),
m_commandLine(_T(""))
{
    FUNCTION_ENTRY( "CPlanStepEditDataPointCheckPage" );
    FUNCTION_EXIT;
}

CPlanStepExecuteScriptPage::~CPlanStepExecuteScriptPage()
{
    FUNCTION_ENTRY( "~CPlanStepExecuteScriptPage" );
    FUNCTION_EXIT;
}

void CPlanStepExecuteScriptPage::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepExecuteScriptPage)
    DDX_Control(pDX, IDC_EXECUTE_SCRIPT_COMMAND_LINE, m_commandLineEdit);
    // }}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlanStepExecuteScriptPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepExecuteScriptPage)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


void TA_Base_App::CPlanStepExecuteScriptPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    ExecuteScriptStep *step = dynamic_cast<ExecuteScriptStep *>(GetPlanStep());
    TA_ASSERT(step != NULL, "Incorrect step type for the execute script step page");

    TextualStepParameter *commandLineParameter = step->getCommandLineParameter();

    m_commandLine = commandLineParameter->getMessageText().c_str();
    m_commandLineEdit.SetWindowText(m_commandLine);

    UpdateData(FALSE);

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepExecuteScriptPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    UpdateData(TRUE);

    ExecuteScriptStep* step = dynamic_cast<ExecuteScriptStep *>(GetPlanStep());
    TA_ASSERT(step != NULL, "Incorrect step type for the execute script step page");

    TextualStepParameter *commandLineParameter = step->getCommandLineParameter();

    CString editText;
    m_commandLineEdit.GetWindowText(editText);
    commandLineParameter->setText((LPCTSTR) editText);

    FUNCTION_EXIT;
}

#endif
