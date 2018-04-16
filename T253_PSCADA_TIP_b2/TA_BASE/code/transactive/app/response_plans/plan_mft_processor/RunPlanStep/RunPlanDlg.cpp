//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/RunPlanDlg.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "Stdafx.h"
#include "resource.h"
#include <sstream>
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/applauncher.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MessageBox.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MftPlanAgentAccessFactory.h"
#include "app/response_plans/plan_step_plugin/internal/src/RunPlanResponseParameter.h"
#include "RunPlanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;
const unsigned long RUNPLAN_RESPONSE_TIMEOUT = 60;

/////////////////////////////////////////////////////////////////////////////
// CRunPlanDlg dialog


CRunPlanDlg::CRunPlanDlg(const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
						 const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters) :
CInteractiveStepDlg(CRunPlanDlg::IDD, eType, ulLocation, ulPlanId, ulInstanceNum, ulStepNum, vecParameters, RUNPLAN_RESPONSE_TIMEOUT),
    m_message(""),
	m_params(vecParameters)
{
    FUNCTION_ENTRY( "CRunPlanDlg" );

	std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
	std::auto_ptr<TA_Base_Core::IEntityData> planMgrEntityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName));

	TA_Base_Bus::RightsLibraryFactory rlf;
	m_rightsLibrary = rlf.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);

	TA_Base_Core::IResource* pRes = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( planMgrEntityData->getKey() );
	m_resourceId = pRes->getKey();
	delete pRes;
	pRes = NULL;

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

    FUNCTION_EXIT;
}

CRunPlanDlg::~CRunPlanDlg()
{
	FUNCTION_ENTRY( "~CDecisionDlg" );
	FUNCTION_EXIT;
}

void CRunPlanDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CInteractiveStepDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CRunPlanDlg)
	DDX_Text(pDX, IDC_RUN_PLAN_ACTIVE_INSTANCE_EDIT, m_instance);
	DDX_Text(pDX, IDC_RUN_PLAN_PLAN_PATH_EDIT, m_plan);
	DDX_Text(pDX, IDC_RUN_PLAN_LOCATION_NAME_EDIT, m_location);
	DDX_Text(pDX, IDC_RUN_PLAN_STEP_NUMBER_EDIT, m_step);
    DDX_Text(pDX, IDC_RUN_PLAN_MESSAGE, m_message);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CRunPlanDlg, CInteractiveStepDlg)
    // {{AFX_MSG_MAP(CRunPlanDlg)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRunPlanDlg message handlers

BOOL CRunPlanDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CInteractiveStepDlg::OnInitDialog();

    // TODO: Add extra initialization here
    std::ostringstream childPlan;

    try
    {
        childPlan << "'" << MftPlanAgentAccessFactory::instance().getNodePath(m_ulLocation, m_params.childPlanId) << "'";
    }
    catch (...)
    {
        childPlan << "ID " << m_params.childPlanId;
    }

    TA_Base_Bus::TransActiveMessage userMsg;
    userMsg << childPlan.str() << m_params.childPlanInstance;

    m_message = userMsg.constructMessage(IDS_UI_210057);

    UpdateData(FALSE);

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


CString CRunPlanDlg::getTitlePrefix() const
{
    FUNCTION_ENTRY( "getTitlePrefix" );
    FUNCTION_EXIT;
    return "Run Plan";
}


void CRunPlanDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

	TA_Base_Core::EPlanStepCompletionStatus completionStatus(TA_Base_Core::PSCS_UNDEFINED);
	RECT newRect = { 0, 0, 0, 0};

    if (canControlPlans())
    {
		try
		{
			CPoint pt = GetMessagePos();
			newRect = TA_Base_Bus::AppLauncher::getInstance().getRect( TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC, pt.x );
		}
		catch( const TA_Base_Core::ApplicationException& expWhat )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", expWhat.what());
		}
		catch( const std::exception& expWhat )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, typeid(expWhat).name(), expWhat.what());
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when call getRect.");
		}

		std::ostringstream commandLine;
        commandLine << "--" << RPARAM_VIEWPLANID << "=" << m_params.childPlanId << ":" << m_params.childPlanInstance << ":" << m_params.childPlanLocation;

		try
		{
			// Launch the Plan Manager.
			TA_Base_Bus::AppLauncher::getInstance().launchApplication(PLAN_MANAGER_GUI_APPTYPE, 
				commandLine.str(),
				TA_Base_Bus::TA_GenericGui::POS_BOUNDED, 
				TA_Base_Bus::TA_GenericGui::ALIGN_FIT, 
				NULL, 
				&newRect);
			completionStatus = TA_Base_Core::PSCS_SUCCESS;
		}
		catch( const TA_Base_Core::ApplicationException& expWhat )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", expWhat.what());
			completionStatus = TA_Base_Core::PSCS_FAILURE;
		}
		catch( const std::exception& expWhat )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, typeid(expWhat).name(), expWhat.what());
			completionStatus = TA_Base_Core::PSCS_FAILURE;
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when launch plan manager.");
			completionStatus = TA_Base_Core::PSCS_FAILURE;
		}
	}
	else
	{
			CString actionName;
			actionName.LoadString(IDS_RUN_PLAN_LAUNCH_PLAN_CONTROLLER);
			TA_Base_Bus::TransActiveMessage userMsg;
			std::string userName;
			std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
			TA_Base_Bus::AuthenticationLibrary authLibrary(true);
			TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
			std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
			userName = iOperator->getName();
			userMsg << userName;
			userMsg << actionName;
			UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
	}

    try
    {
		RunPlanResponseParameter responseParam(completionStatus);
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
        MessageBox::error(IDS_RUN_PLAN_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}

bool CRunPlanDlg::canControlPlans() const
{
	FUNCTION_ENTRY( "canControlPlans" );

	if (NULL != m_rightsLibrary)
	{
		try
		{
			std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

			TA_Base_Bus::EDecisionModule decisionModule;
			std::string reasonForDenial;

			FUNCTION_EXIT;
			return m_rightsLibrary->isActionPermittedOnResource(sessionId, m_resourceId,
				TA_Base_Bus::aca_PLAN_CONTROL_PLAN,
				reasonForDenial, decisionModule);
		}
		catch (...)                                                                                                  
		{                                                                                                           
			std::ostringstream errorMessage;                                                                          
			errorMessage << "Unable to determine the operator's control plan rights. Access denied. ";               
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", errorMessage.str().c_str());                          
		}
	}

	FUNCTION_EXIT;
	return false;
}
