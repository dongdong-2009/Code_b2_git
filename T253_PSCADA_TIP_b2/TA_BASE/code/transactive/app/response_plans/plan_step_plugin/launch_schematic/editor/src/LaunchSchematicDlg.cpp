/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/LaunchSchematicDlg.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Facilitates the launching of a display when executing a Launch Schematic step.
  *
  */

#include "Stdafx.h"
#include "resource.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MessageBox.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MftPlanAgentAccessFactory.h"
#include "app/response_plans/plan_step_plugin/launch_schematic/common/LaunchSchematicResponseParameter.h"
#include "LaunchSchematicDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::StringUtilities;
using namespace TA_Base_App;
const unsigned long LAUNCHSCHEMATIC_RESPONSE_TIMEOUT = 60;

/////////////////////////////////////////////////////////////////////////////
// CLaunchSchematicDlg dialog


CLaunchSchematicDlg::CLaunchSchematicDlg(const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
										 const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters) :
CInteractiveStepDlg(CLaunchSchematicDlg::IDD, eType, ulLocation, ulPlanId, ulInstanceNum, ulStepNum, vecParameters, LAUNCHSCHEMATIC_RESPONSE_TIMEOUT),
m_params(ulStepNum, eType)
{
    FUNCTION_ENTRY( "CLaunchSchematicDlg" );

	//Set plan base infos.
	try
	{
		m_plan = MftPlanAgentAccessFactory::instance().getNodePath( m_ulLocation, m_ulPlanId ).c_str();
		m_location = TA_Base_Bus::PlanAgentLocationAccess::getInstance().getLocationName(m_ulLocation).c_str();
	}
	catch (...)
	{
		m_plan = TA_Base_Bus::StringUtilities::convertToString((unsigned long)m_ulPlanId);
		m_location = TA_Base_Bus::StringUtilities::convertToString((unsigned long)m_ulLocation);
	}

	m_step = TA_Base_Bus::StringUtilities::convertToString((unsigned long)m_ulStepNum);
	m_instance = TA_Base_Bus::StringUtilities::convertToString((unsigned long)m_ulInstanceNum);

	m_params.deserialize(vecParameters);
    FUNCTION_EXIT;
}

CLaunchSchematicDlg::~CLaunchSchematicDlg()
{
	FUNCTION_ENTRY( "~CLaunchSchematicDlg" );

	FUNCTION_EXIT;
}

void CLaunchSchematicDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CInteractiveStepDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CLaunchSchematicDlg)
	DDX_Text(pDX, IDC_LAUNCH_SCHEMATIC_ACTIVE_INSTANCE_EDIT, m_instance);
	DDX_Text(pDX, IDC_LAUNCH_SCHEMATIC_PLAN_PATH_EDIT, m_plan);
	DDX_Text(pDX, IDC_LAUNCH_SCHEMATIC_LOCATION_NAME_EDIT, m_location);
	DDX_Text(pDX, IDC_LAUNCH_SCHEMATIC_STEP_NUMBER_EDIT, m_step);
    DDX_Control(pDX, IDC_LAUNCH_SCHEMATIC_SCREEN_NUMBER_COMBO, m_screenNumberCombo);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CLaunchSchematicDlg, CInteractiveStepDlg)
    // {{AFX_MSG_MAP(CLaunchSchematicDlg)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLaunchSchematicDlg message handlers

BOOL CLaunchSchematicDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CInteractiveStepDlg::OnInitDialog();

    // Enable the operator to change the pre-configured target screen to display the schematic on.
    RECT lastScreenBounds = { 0, 0, 100, 100 };
    try
    {
        lastScreenBounds = TA_Base_Bus::AppLauncher::getInstance().getRect(
            TA_Base_Bus::TA_GenericGui::SCREEN_LAST, TA_Base_Bus::TA_GenericGui::AREA_SCREEN);
    }
    catch (...)
    {
    }

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
        "Last screen:  left = %d / right = %d", lastScreenBounds.left, lastScreenBounds.right);

    int lastScreen(lastScreenBounds.right / (lastScreenBounds.right - lastScreenBounds.left));
    int targetScreen(m_params.screen);

    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
        "Configured target screen:  %d", lastScreen);
    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
        "Last screen:  %d", lastScreen);

    // Reset the target screen if it's greater than the last available screen.
    if (targetScreen > lastScreen)
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "Configured target screen > last screen, resetting to %d", lastScreen);

        targetScreen = lastScreen;
    }

    for (int screen = 1; screen <= lastScreen; screen++)
    {
        m_screenNumberCombo.AddString(StringUtilities::convertToString(screen));
    }

    m_screenNumberCombo.SelectString(CB_ERR, StringUtilities::convertToString(targetScreen));

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


CString CLaunchSchematicDlg::getTitlePrefix() const
{
    FUNCTION_ENTRY( "getTitlePrefix" );
    FUNCTION_EXIT;
    return "Launch Schematic";
}


void CLaunchSchematicDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

    TA_Base_Core::EPlanStepCompletionStatus completionStatus(TA_Base_Core::PSCS_UNDEFINED);
    std::string completionError;

    try
    {
        // Get the target screen number as selected in the screen list.
        CString selectedScreen;
        m_screenNumberCombo.GetWindowText(selectedScreen);
        int targetScreen(::atoi(selectedScreen));

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
            "Target screen for schematic: %d", targetScreen);

        // Determine the coordinate of the left side of the target screen.
        RECT screenBounds(TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_SPECIFIC,
                                                                          TA_Base_Bus::TA_GenericGui::AREA_SCREEN,
                                                                          targetScreen));

        // If additional schematic commands have been configured for this step, append them
        // to the schematic's path. The protocol tag to be used here is PLANS.
        std::string schematic(m_params.schematic);
        std::string commands(m_params.commands);

        if (!commands.empty())
        {
            if (commands.find("#PLANS,") != 0)
            {
                commands = "#PLANS," + commands;
            }

            schematic += commands;
        }

        // Now launch the schematic.
        TA_Base_Bus::AppLauncher::getInstance().launchGraphWorxDisplay(schematic, "", screenBounds.left);

        completionStatus = TA_Base_Core::PSCS_SUCCESS;
        completionError = "";
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo,    "TransactiveException", ex.what());

        completionStatus = TA_Base_Core::PSCS_FAILURE;
        completionError = ex.what();
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo,    "UnknownException", "");

        completionStatus = TA_Base_Core::PSCS_FAILURE;
        completionError = "Failed to launch schematic display";
    }

    try
    {
		LaunchSchematicResponseParameter responseParam(completionStatus);
		std::vector<std::string> stringParams = responseParam.serialize();
		TA_Base_Core::InteractiveStepData stepData;
		stepData.id = m_ulStepNum;
		stepData.type = m_eType;

		TA_Base_Core::StepParameters parameters;
		parameters.length(stringParams.size());
		unsigned long index = 0;
		for(std::vector<std::string>::const_iterator itParameter = stringParams.begin(); itParameter != stringParams.end(); ++itParameter)
		{
			parameters[index] = (*itParameter).c_str();
			index++;
		}

		stepData.parameters = parameters;

		TA_Base_Core::ActivePlanId activePlanId;
		activePlanId.instance = m_ulInstanceNum;
		activePlanId.location = m_ulLocation;
		activePlanId.plan = m_ulPlanId;

		MftPlanAgentAccessFactory::instance().processStepResponse(activePlanId, stepData);
	}
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_LAUNCH_SCHEMATIC_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}
