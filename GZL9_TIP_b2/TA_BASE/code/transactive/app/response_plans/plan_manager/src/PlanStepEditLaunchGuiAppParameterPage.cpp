//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditLaunchGuiAppParameterPage.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#include "StdAfx.h"

#if defined (STEPTYPE_LAUNCH_GUI_APP)

#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/src/IGui.h"
#include "core/data_access_interface/src/GuiAccessFactory.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "PlanManager.h"
#include "PlanStepEditLaunchGuiAppParameterPage.h"
#include "LaunchGuiAppStep.h"
#include "GuiAppTypeStepParameter.h"
#include "TextualStepParameter.h"
#include "NumericStepParameter.h"
#include "PlanStepEditParameterPage.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const unsigned long COMMAND_LINE_TXT_LIMIT = 250;//DB limit is 1000 but this should be more then enough

using namespace TA_Base_App;

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditLaunchGuiAppParameterPage property page

IMPLEMENT_DYNAMIC(CPlanStepEditLaunchGuiAppParameterPage, CPlanStepEditParameterPage)

CPlanStepEditLaunchGuiAppParameterPage::CPlanStepEditLaunchGuiAppParameterPage(LaunchGuiAppStep* step) :
    CPlanStepEditParameterPage(CPlanStepEditLaunchGuiAppParameterPage::IDD, step)
{
    FUNCTION_ENTRY( "CPlanStepEditLaunchGuiAppParameterPage" );

    // {{AFX_DATA_INIT(CPlanStepEditLaunchGuiAppParameterPage)
    // }}AFX_DATA_INIT

	// Get the schematic area size.
	RECT appBounds(TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_ALL,
		TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC, 0));

	m_yRange.min = appBounds.top;
	m_yRange.max = appBounds.bottom;
	m_xRange.min = 0;
	m_xRange.max = appBounds.right;
	m_wRange.min = 0;
	m_wRange.max = appBounds.right;
	m_hRange.min = 0;
	m_hRange.max = appBounds.bottom - appBounds.top;

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
	static_cast<CEdit*>(GetDlgItem(IDC_X_POS_EDIT))->SetReadOnly(!enable);
	static_cast<CEdit*>(GetDlgItem(IDC_Y_POS_EDIT))->SetReadOnly(!enable);
	static_cast<CEdit*>(GetDlgItem(IDC_WIDTH_EDIT))->SetReadOnly(!enable);
	static_cast<CEdit*>(GetDlgItem(IDC_HEIGHT_EDIT))->SetReadOnly(!enable);

	m_yPositionSpin.EnableWindow(enable);
	m_xPositionSpin.EnableWindow(enable);
	m_widthSpin.EnableWindow(enable);
	m_heightSpin.EnableWindow(enable);

    FUNCTION_EXIT;
}

void CPlanStepEditLaunchGuiAppParameterPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

    LaunchGuiAppStep *step = dynamic_cast<LaunchGuiAppStep *>(GetPlanStep());
    TA_ASSERT(step != NULL, "Incorrect step type for the launch GUI app step page");

    GuiAppTypeStepParameter *appTypeParameter = step->getAppTypeParameter();
    TextualStepParameter *commandLineParameter = step->getCommandLineParameter();
    NumericStepParameter *xPositionParameter = step->getXPositionParameter();
    NumericStepParameter *yPositionParameter = step->getYPositionParameter();
    NumericStepParameter *widthParameter = step->getWidthParameter();
    NumericStepParameter *heightParameter = step->getHeightParameter();

    m_commandLineEdit.SetWindowText(commandLineParameter->getMessageText().c_str());

    if (!appTypeParameter->isHiddenValue())
    {
        int appTypeIdx = m_appTypeCombo.FindStringExact(-1, appTypeParameter->getApplicationName().c_str());
        if (appTypeIdx != CB_ERR)
        {
            m_appTypeCombo.SetCurSel(appTypeIdx);
        }
    }

    if (!xPositionParameter->isHiddenValue())
    {
        m_xPositionEdit = xPositionParameter->getNumericValue();
    }

    if (!yPositionParameter->isHiddenValue())
    {
        m_yPositionEdit = yPositionParameter->getNumericValue();
    }

    if (!widthParameter->isHiddenValue())
    {
        m_widthEdit = TA_Base_Bus::StringUtilities::convertToString(widthParameter->getNumericValue());
    }

    if (!heightParameter->isHiddenValue())
    {
        m_heightEdit = TA_Base_Bus::StringUtilities::convertToString(heightParameter->getNumericValue());
    }

	UpdateData(FALSE);

    FUNCTION_EXIT;
}

void CPlanStepEditLaunchGuiAppParameterPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );

    LaunchGuiAppStep* step = dynamic_cast<LaunchGuiAppStep *>(GetPlanStep());
    TA_ASSERT(step != NULL, "Incorrect step type for the launch GUI app step page");

    GuiAppTypeStepParameter *appTypeParameter = step->getAppTypeParameter();
    TextualStepParameter *commandLineParameter = step->getCommandLineParameter();
    NumericStepParameter *xPositionParameter = step->getXPositionParameter();
    NumericStepParameter *yPositionParameter = step->getYPositionParameter();
    NumericStepParameter *widthParameter = step->getWidthParameter();
    NumericStepParameter *heightParameter = step->getHeightParameter();

    int appTypeIdx = m_appTypeCombo.GetCurSel();
    if (appTypeIdx != CB_ERR)
    {
        appTypeParameter->setValue(m_appTypeCombo.GetItemData(appTypeIdx));
        appTypeParameter->hideValue(false);
    }

    CString editText;
    m_commandLineEdit.GetWindowText(editText);
    commandLineParameter->setText((LPCTSTR) editText);

    static_cast<CEdit*>(GetDlgItem(IDC_X_POS_EDIT))->GetWindowText(editText);
    unsigned long xPosition = (editText.IsEmpty()? LaunchGuiAppStep::DEFAULT_X_POSITION : atol(editText));
    xPositionParameter->setValue(xPosition);
//    xPositionParameter->hideValue(editText.IsEmpty());

    static_cast<CEdit*>(GetDlgItem(IDC_Y_POS_EDIT))->GetWindowText(editText);
    unsigned long yPosition = (editText.IsEmpty()? LaunchGuiAppStep::DEFAULT_Y_POSITION : atol(editText));
    yPositionParameter->setValue(yPosition);
//    yPositionParameter->hideValue(editText.IsEmpty());

    unsigned long width = (m_widthEdit.IsEmpty()? LaunchGuiAppStep::DEFAULT_WIDTH : atol(m_widthEdit));
    widthParameter->setValue(width);
    widthParameter->hideValue(m_widthEdit.IsEmpty() != 0);

    unsigned long height = (m_heightEdit.IsEmpty()? LaunchGuiAppStep::DEFAULT_HEIGHT : atol(m_heightEdit));
    heightParameter->setValue(height);
    heightParameter->hideValue(m_heightEdit.IsEmpty() != 0);

    FUNCTION_EXIT;
}

void CPlanStepEditLaunchGuiAppParameterPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditLaunchGuiAppParameterPage)
	DDX_Text(pDX, IDC_Y_POS_EDIT, m_yPositionEdit);
	DDV_MinMaxUInt(pDX, m_yPositionEdit, m_yRange.min, m_yRange.max);
	DDX_Text(pDX, IDC_X_POS_EDIT, m_xPositionEdit);
	DDV_MinMaxUInt(pDX, m_xPositionEdit, m_xRange.min, m_xRange.max);
	DDX_Text(pDX, IDC_WIDTH_EDIT, m_widthEdit);
	DDV_MinMaxUInt(pDX, atoi(m_widthEdit), m_wRange.min, m_wRange.max);
	DDX_Text(pDX, IDC_HEIGHT_EDIT, m_heightEdit);
	DDV_MinMaxUInt(pDX, atoi(m_heightEdit), m_hRange.min, m_hRange.max);
	DDX_Control(pDX, IDC_Y_POS_SPIN, m_yPositionSpin);
	DDX_Control(pDX, IDC_X_POS_SPIN, m_xPositionSpin);
	DDX_Control(pDX, IDC_WIDTH_SPIN, m_widthSpin);
	DDX_Control(pDX, IDC_HEIGHT_SPIN, m_heightSpin);
    DDX_Control(pDX, IDC_COMMAND_LINE_EDIT, m_commandLineEdit);
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

     // as per TD 12384
     m_commandLineEdit.SetLimitText (COMMAND_LINE_TXT_LIMIT);

	 m_yPositionSpin.SetBuddy(GetDlgItem(IDC_Y_POS_EDIT));
	 m_xPositionSpin.SetBuddy(GetDlgItem(IDC_X_POS_EDIT));
	 m_widthSpin.SetBuddy(GetDlgItem(IDC_WIDTH_EDIT));
	 m_heightSpin.SetBuddy(GetDlgItem(IDC_HEIGHT_EDIT));

	 m_yPositionSpin.SetRange(m_yRange.min, m_yRange.max);
	 m_xPositionSpin.SetRange(m_xRange.min, m_xRange.max);
	 m_widthSpin.SetRange(m_wRange.min, m_wRange.max);
	 m_heightSpin.SetRange(m_hRange.min, m_hRange.max);

    try
    {
        typedef  std::vector<TA_Base_Core::IGui *> GuiList;
        GuiList guiList = TA_Base_Core::GuiAccessFactory::getInstance().getAllGuis();

        for (GuiList::const_iterator iter = guiList.begin(); iter != guiList.end(); iter++)
        {
            std::auto_ptr<TA_Base_Core::IGui> gui(*iter);

            if (gui.get() != NULL && gui->getKey() > 0)
            {
                int appTypeIdx = m_appTypeCombo.AddString(gui->getFullName().c_str());
                m_appTypeCombo.SetItemData(appTypeIdx, gui->getKey());
            }
        }

        guiList.clear();
    }
    catch (const TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_LOAD_APPLICATIONS, ex);

        FUNCTION_EXIT;
        return FALSE;
    }

    FUNCTION_EXIT;
    return TRUE;
}

#endif // defined (STEPTYPE_LAUNCH_GUI_APP)