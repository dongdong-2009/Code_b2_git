//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/LaunchGuiAppDlg.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include <iostream>
#include <vector>
#include "app/response_plans/plan_manager/src/Stdafx.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "LaunchGuiAppDlg.h"
#include "app/response_plans/plan_step_plugin/launch_gui_app/common/LaunchGuiResponseParameter.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MessageBox.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MftPlanAgentAccessFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
const unsigned long LAUNCHGUIAPP_RESPONSE_TIMEOUT = 30;

/////////////////////////////////////////////////////////////////////////////
// CLaunchGuiAppDlg dialog


CLaunchGuiAppDlg::CLaunchGuiAppDlg(const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
								   const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters) :
CInteractiveStepDlg(CLaunchGuiAppDlg::IDD, eType, ulLocation, ulPlanId, ulInstanceNum, ulStepNum, vecParameters, LAUNCHGUIAPP_RESPONSE_TIMEOUT),
	m_params(ulStepNum, eType)
{
    FUNCTION_ENTRY( "CLaunchGuiAppDlg" );

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

CLaunchGuiAppDlg::~CLaunchGuiAppDlg()
{
	FUNCTION_ENTRY( "~CLaunchGuiAppDlg" );
	FUNCTION_EXIT;
}


void CLaunchGuiAppDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CInteractiveStepDlg::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_GUI_ACTIVE_INSTANCE_EDIT, m_instance);
	DDX_Text(pDX, IDC_GUI_PLAN_PATH_EDIT, m_plan);
	DDX_Text(pDX, IDC_GUI_LOCATION_NAME_EDIT, m_location);
	DDX_Text(pDX, IDC_GUI_STEP_NUMBER_EDIT, m_step);
    // {{AFX_DATA_MAP(CLaunchGuiAppDlg)
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CLaunchGuiAppDlg, CInteractiveStepDlg)
    // {{AFX_MSG_MAP(CLaunchGuiAppDlg)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CLaunchGuiAppDlg message handlers

BOOL CLaunchGuiAppDlg::OnInitDialog()
{
	FUNCTION_ENTRY( "OnInitDialog" );

	CInteractiveStepDlg::OnInitDialog();

	FUNCTION_EXIT;
	return TRUE;
}

CString CLaunchGuiAppDlg::getTitlePrefix() const
{
    FUNCTION_ENTRY( "getTitlePrefix" );
    FUNCTION_EXIT;
    return "Launch GUI Application";
}


void CLaunchGuiAppDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

    TA_Base_Core::EPlanStepCompletionStatus completionStatus(TA_Base_Core::PSCS_UNDEFINED);
    std::string completionError;

    try
    {
        // Determine app position and size. Want the app on the same screen as the main Plan Manager window.
        RECT appBounds(TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT,
                                                                       TA_Base_Bus::TA_GenericGui::AREA_SCREEN, 0));

        unsigned long appPosType(TA_Base_Bus::TA_GenericGui::POS_BOUNDED);
        //TA_Base_Core::LaunchGuiAppParameters parameters(getStepDetail().data.launchGuiApp());
		//appBounds.left += parameters.pos.left;
		//appBounds.top = parameters.pos.top;
		appBounds.left += m_params.pos.left;
        appBounds.top = m_params.pos.top;

        // Width and height are optional parameters of the Launch GUI App step.
        // If not specified, the default values for the app will be used.
        //if (parameters.pos.width != static_cast<unsigned long>(-1) &&
        //    parameters.pos.height != static_cast<unsigned long>(-1))
        //{
        //    appPosType |= TA_Base_Bus::TA_GenericGui::POS_RESIZABLE;

        //    appBounds.right = appBounds.left + parameters.pos.width;
        //    appBounds.bottom = appBounds.top + parameters.pos.height;
        //}
        //else
        //{
        //    appBounds.right = appBounds.bottom = 0;
        //}

        //// Now launch the app
        //TA_Base_Bus::AppLauncher::getInstance().launchApplication(parameters.app,
        //                                                          parameters.args.in(),
        //                                                          appPosType,
        //                                                          0,
        //                                                          NULL,
        //                                                          &appBounds);
		if (m_params.pos.width != static_cast<unsigned long>(-1) &&
			m_params.pos.height != static_cast<unsigned long>(-1))
		{
			appPosType |= TA_Base_Bus::TA_GenericGui::POS_RESIZABLE;

			appBounds.right = appBounds.left + m_params.pos.width;
			appBounds.bottom = appBounds.top + m_params.pos.height;
		}
		else
		{
			appBounds.right = appBounds.bottom = 0;
		}

		// Now launch the app
		TA_Base_Bus::AppLauncher::getInstance().launchApplication(m_params.app,
			m_params.args,
			appPosType,
			0,
			NULL,
			&appBounds);
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
        completionError = "Failed to launch GUI application";
    }

    try
    {
		LaunchGuiResponseParameter responseParam(completionStatus);
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
        MessageBox::error(IDS_GUI_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}


