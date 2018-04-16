/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditLaunchSchematicParameterPage.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#include "StdAfx.h"
#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/data_access_interface/src/ScadaDisplayAccessFactory.h"
#include "core/data_access_interface/src/IScadaDisplay.h"
#include "PlanStepEditLaunchSchematicPage.h"
#include "LaunchSchematicStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/TextualStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/NumericStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MessageBox.h"

static const UINT MIN_SCREEN_NUMBER = 1;
static const UINT MAX_SCREEN_NUMBER = 10;
static const unsigned long COMMAND_LINE_TXT_LIMIT = 250;//DB limit is 1000 but this should be more then enough

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLaunchSchematicParameterPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditLaunchSchematicParameterPage, CPlanStepEditParameterPage)

CPlanStepEditLaunchSchematicParameterPage::CPlanStepEditLaunchSchematicParameterPage(LaunchSchematicStep* step) :
    CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_LAUNCH_SCHEMATIC_DLG, step)
{
    FUNCTION_ENTRY( "CPlanStepEditLaunchSchematicParameterPage" );
    FUNCTION_EXIT;
}


CPlanStepEditLaunchSchematicParameterPage::~CPlanStepEditLaunchSchematicParameterPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditLaunchSchematicParameterPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditLaunchSchematicParameterPage::EnableParameterControls(bool enable)
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    m_schematicNameCombo.EnableWindow(enable);
    m_screenNumberEdit.SetReadOnly(!enable);
    m_commandLineTxt.SetReadOnly(!enable);

	FUNCTION_EXIT;
}


void CPlanStepEditLaunchSchematicParameterPage::DisplayParameters()
{
	FUNCTION_ENTRY( "DisplayParameters" );
	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamSchematic ) == 0 )
		{
			int schematicIdx = m_schematicNameCombo.FindStringExact(-1, itLoop->Item.c_str());
			if (schematicIdx != CB_ERR)
			{
				m_schematicNameCombo.SetCurSel(schematicIdx);
			}
		}
		else if ( itLoop->Name.compare( ParamCommands ) == 0 )
		{
			m_commandLineTxt.SetWindowText( itLoop->Item.c_str() );
		}
		else if ( itLoop->Name.compare( ParamScreen ) == 0 )
		{
			m_screenNumberEdit.SetWindowText( itLoop->Item.c_str() );
		}
	}

    FUNCTION_EXIT;
}


void CPlanStepEditLaunchSchematicParameterPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

	int curSel = m_schematicNameCombo.GetCurSel ();
	unsigned int key = m_schematicNameCombo.GetItemData (curSel);
	std::vector<unsigned int> vecSelectedKeys;
	vecSelectedKeys.push_back(key);
	std::string strDisplayValue = "";
	GetPlanStep()->getActivityParameter().getListPicker(ParamSchematic)->pickFromList(ParamSchematic,vecSelectedKeys,strDisplayValue);

	updateValue( ParamCommands, m_commandLineTxt );
	updateValue( ParamScreen, m_screenNumberEdit );

    FUNCTION_EXIT;
}


void CPlanStepEditLaunchSchematicParameterPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditLaunchSchematicParameterPage)
    DDX_Control(pDX, IDC_LAUNCH_SCHEMATIC_COMMAND_LINE_EDIT, m_commandLineTxt);
    DDX_Text(pDX, IDC_LAUNCH_SCHEMATIC_SCREEN_NUMBER_EDIT, m_screenNumber);
	DDX_Control(pDX, IDC_LAUNCH_SCHEMATIC_SCREEN_NUMBER_EDIT, m_screenNumberEdit);
    DDX_Control(pDX, IDC_LAUNCH_SCHEMATIC_SCREEN_NUMBER_SPIN, m_screenNumberSpin);
    DDX_Control(pDX, IDC_LAUNCH_SCHEMATIC_SCHEMATIC_NAME_COMBO, m_schematicNameCombo);
    DDV_MinMaxUInt(pDX, m_screenNumber, MIN_SCREEN_NUMBER, MAX_SCREEN_NUMBER);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditLaunchSchematicParameterPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditLaunchSchematicParameterPage)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLaunchSchematicParameterPage message handlers

BOOL CPlanStepEditLaunchSchematicParameterPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return TRUE;
    }

    m_screenNumberSpin.SetBuddy(GetDlgItem(IDC_LAUNCH_SCHEMATIC_SCREEN_NUMBER_EDIT));
    m_screenNumberSpin.SetRange(MIN_SCREEN_NUMBER, MAX_SCREEN_NUMBER);

    m_commandLineTxt.SetLimitText (COMMAND_LINE_TXT_LIMIT);

	m_schematicNameCombo.ResetContent();
	std::vector<ListItem> vecKeyItems;
	std::vector<unsigned int> vecSelected;
	unsigned int nMaxSelection = 0u;
	GetPlanStep()->getActivityParameter().getListPicker( ParamSchematic)->getInputListItems( ParamSchematic, vecKeyItems, vecSelected, nMaxSelection );
	for ( std::vector<ListItem>::iterator itLoop = vecKeyItems.begin(); vecKeyItems.end() != itLoop; ++itLoop )
	{
		int nIdx = m_schematicNameCombo.AddString( itLoop->Item.c_str() );
		m_schematicNameCombo.SetItemData(nIdx, itLoop->ID );
	}

    FUNCTION_EXIT;
    return TRUE;
}

}