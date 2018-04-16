// D:\workspace\3001_TIP_NEW\TA_BASE\transactive\app\response_plans\plan_manager\src\PlanStepPlanStartTimeCheckPage.cpp : implementation file
//

#include "StdAfx.h"
#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"
#include "ExecuteScriptStep.h"
#include "PlanStepEditExecuteScriptPage.h"

// CPlanStepExecuteScriptPage dialog

namespace TA_Base_App
{

IMPLEMENT_DYNAMIC(CPlanStepExecuteScriptPage, CPlanStepEditParameterPage)

CPlanStepExecuteScriptPage::CPlanStepExecuteScriptPage(ExecuteScriptStep* step) :
CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_EXECUTE_SCRIPT_DLG, step)
{
    FUNCTION_ENTRY( "CPlanStepExecuteScriptPage" );
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

	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamCommands) == 0 )
		{
			m_commandLineEdit.SetWindowText( itLoop->Item.c_str() );
			break;
		}
	}

    FUNCTION_EXIT;
}

void TA_Base_App::CPlanStepExecuteScriptPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    updateValue( ParamCommands, m_commandLineEdit );

    FUNCTION_EXIT;
}

}
