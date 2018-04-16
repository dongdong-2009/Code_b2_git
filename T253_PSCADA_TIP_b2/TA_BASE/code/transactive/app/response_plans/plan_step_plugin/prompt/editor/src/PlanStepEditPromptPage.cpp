//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditPromptPage.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "StdAfx.h"
#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanStepEditPromptPage.h"
#include "PromptStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"

namespace TA_Base_App
{

static const unsigned int PROMPT_EDIT_TXT = 950;//DB limit is 1000 but this should be more then enoug
const unsigned int MIN_INTERACTIVE_STEP_TIMEOUT = 10;   // seconds
const unsigned int MAX_INTERACTIVE_STEP_TIMEOUT = 300;  // seconds
/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLogPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditPromptPage, CPlanStepEditParameterPage)

CPlanStepEditPromptPage::CPlanStepEditPromptPage(PromptStep* step)
  : CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_PROMPT_DLG,step)
{
    FUNCTION_ENTRY( "CPlanStepEditPromptPage" );
    FUNCTION_EXIT;
}


CPlanStepEditPromptPage::~CPlanStepEditPromptPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditPromptPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditPromptPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditPromptPage)
    DDX_Control (pDX, IDC_PROMPT_PROMPT_MESSAGE, m_promptMsg);
    DDX_Control(pDX, IDC_PROMPT_RESPONSE_TIMEOUT, m_responseTimeout);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditPromptPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditPromptPage)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLogPage message handlers

BOOL CPlanStepEditPromptPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

	m_responseTimeout.SetFormat( "mm:ss" );
    m_promptMsg.SetLimitText (PROMPT_EDIT_TXT);//as per TD 14921
    FUNCTION_EXIT;
    return TRUE;
}


void CPlanStepEditPromptPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    m_promptMsg.SetReadOnly(!enable);
    m_responseTimeout.EnableWindow(enable);

    FUNCTION_EXIT;
}


void CPlanStepEditPromptPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamMessage) == 0 )
		{
			m_promptMsg.SetWindowText( itLoop->Item.c_str() );
			break;
		}
	}

	fillTimePicker( ParamDismissTimeout, m_responseTimeout );

    FUNCTION_EXIT;
}


void CPlanStepEditPromptPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

	updateValue( ParamMessage, m_promptMsg );
	updateTimeValue( ParamDismissTimeout, m_responseTimeout );

    FUNCTION_EXIT;
}

}
