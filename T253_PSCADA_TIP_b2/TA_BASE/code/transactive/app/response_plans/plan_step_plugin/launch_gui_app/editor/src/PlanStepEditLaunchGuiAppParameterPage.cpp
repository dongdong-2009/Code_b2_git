//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditLaunchGuiAppParameterPage.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "StdAfx.h"
#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/IGui.h"
#include "core/data_access_interface/src/GuiAccessFactory.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "PlanStepEditLaunchGuiAppParameterPage.h"
#include "LaunchGuiAppStep.h"
#include "GuiAppTypeStepParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MessageBox.h"

static const unsigned long COMMAND_LINE_TXT_LIMIT = 250;//DB limit is 1000 but this should be more then enough

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLaunchGuiAppParameterPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditLaunchGuiAppParameterPage, CPlanStepEditParameterPage)

CPlanStepEditLaunchGuiAppParameterPage::CPlanStepEditLaunchGuiAppParameterPage(LaunchGuiAppStep* step) :
    CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_LAUNCH_GUI_APP_DLG, step),
    m_xPositionEdit(true, false),
    m_yPositionEdit(true, false),
    m_widthEdit(true, false),
    m_heightEdit(true, false)
{
    FUNCTION_ENTRY( "CPlanStepEditLaunchGuiAppParameterPage" );

    // {{AFX_DATA_INIT(CPlanStepEditLaunchGuiAppParameterPage)
    // }}AFX_DATA_INIT
    
    FUNCTION_EXIT;
}


CPlanStepEditLaunchGuiAppParameterPage::~CPlanStepEditLaunchGuiAppParameterPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditLaunchGuiAppParameterPage" );
    FUNCTION_EXIT;
}


void CPlanStepEditLaunchGuiAppParameterPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    m_appTypeCombo.EnableWindow(enable);
    m_commandLineEdit.SetReadOnly(!enable);
    m_xPositionEdit.SetReadOnly(!enable);
    m_yPositionEdit.SetReadOnly(!enable);
    m_widthEdit.SetReadOnly(!enable);
    m_heightEdit.SetReadOnly(!enable);

    FUNCTION_EXIT;
}


void CPlanStepEditLaunchGuiAppParameterPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );

	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamApp ) == 0 )
		{
			int appTypeIdx = m_appTypeCombo.FindStringExact(-1, itLoop->Item.c_str());
			if (appTypeIdx != CB_ERR)
			{
				m_appTypeCombo.SetCurSel(appTypeIdx);
			}
		}
		else if ( itLoop->Name.compare( ParamArgs ) == 0 )
		{
			m_commandLineEdit.SetWindowText( itLoop->Item.c_str() );
		}
		else if ( itLoop->Name.compare( ParamPositionTop ) == 0 )
		{
			m_xPositionEdit.SetWindowText( itLoop->Item.c_str() );
		}
		else if ( itLoop->Name.compare( ParamPositionLeft ) == 0 )
		{
			m_yPositionEdit.SetWindowText( itLoop->Item.c_str() );
		}
		else if ( itLoop->Name.compare( ParamPositionWidth ) == 0 )
		{
			m_widthEdit.SetWindowText( itLoop->Item.c_str() );
		}
		else if ( itLoop->Name.compare( ParamPositionHeight ) == 0 )
		{
			m_heightEdit.SetWindowText( itLoop->Item.c_str() );
		}
	}


    FUNCTION_EXIT;
}


void CPlanStepEditLaunchGuiAppParameterPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

	int curSel = m_appTypeCombo.GetCurSel ();
	unsigned int key = m_appTypeCombo.GetItemData (curSel);
	std::vector<unsigned int> vecSelectedKeys;
	vecSelectedKeys.push_back(key);
	std::string strDisplayValue = "";
	GetPlanStep()->getActivityParameter().getListPicker(ParamApp)->pickFromList(ParamApp,vecSelectedKeys,strDisplayValue);

    updateValue( ParamArgs, m_commandLineEdit);
	updateValue( ParamPositionTop, m_xPositionEdit );
	updateValue( ParamPositionLeft, m_yPositionEdit );
	updateValue( ParamPositionWidth, m_widthEdit );
	updateValue( ParamPositionHeight, m_heightEdit );

    FUNCTION_EXIT;
}


void CPlanStepEditLaunchGuiAppParameterPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditLaunchGuiAppParameterPage)
    DDX_Control(pDX, IDC_Y_POS_EDIT, m_yPositionEdit);
    DDX_Control(pDX, IDC_X_POS_EDIT, m_xPositionEdit);
    DDX_Control(pDX, IDC_WIDTH_EDIT, m_widthEdit);
    DDX_Control(pDX, IDC_COMMAND_LINE_EDIT, m_commandLineEdit);
    DDX_Control(pDX, IDC_HEIGHT_EDIT, m_heightEdit);
    DDX_Control(pDX, IDC_APP_TYPE_COMBO, m_appTypeCombo);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditLaunchGuiAppParameterPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditLaunchGuiAppParameterPage)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLaunchGuiAppParameterPage message handlers

BOOL CPlanStepEditLaunchGuiAppParameterPage::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    if (!CPlanStepEditParameterPage::OnInitDialog())
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    long screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
    long screenHeight = ::GetSystemMetrics(SM_CYSCREEN);

    m_xPositionEdit.SetRange(0, screenWidth);
    m_yPositionEdit.SetRange(0, screenHeight);
    m_widthEdit.SetRange(0, screenWidth);
    m_heightEdit.SetRange(0, screenHeight);

    // as per TD 12384
    m_commandLineEdit.SetLimitText(COMMAND_LINE_TXT_LIMIT);

	m_appTypeCombo.ResetContent();
	std::vector<ListItem> vecKeyItems;
	std::vector<unsigned int> vecSelected;
	unsigned int nMaxSelection = 0u;
	GetPlanStep()->getActivityParameter().getListPicker( ParamApp)->getInputListItems( ParamApp, vecKeyItems, vecSelected, nMaxSelection );
	for ( std::vector<ListItem>::iterator itLoop = vecKeyItems.begin(); vecKeyItems.end() != itLoop; ++itLoop )
	{
		int nIdx = m_appTypeCombo.AddString( itLoop->Item.c_str() );
		m_appTypeCombo.SetItemData(nIdx, itLoop->ID );
	}

    FUNCTION_EXIT;
    return TRUE;
}

}
