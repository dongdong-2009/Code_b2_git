//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PromptDlg.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "StdAfx.h"
#include "resource.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MessageBox.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MftPlanAgentAccessFactory.h"
#include "app/response_plans/plan_step_plugin/prompt/common/PromptStepResponseParameter.h"
#include "PromptDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPromptDlg dialog

CPromptDlg::CPromptDlg(const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
					   const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters) :
CInteractiveStepDlg(CPromptDlg::IDD, eType,ulLocation,ulPlanId,ulInstanceNum,ulStepNum,vecParameters, 0),
m_params(ulStepNum, eType)
{
    FUNCTION_ENTRY( "CPromptDlg" );

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

	m_prompt = m_params.message.c_str();
	setResponseTimeout(m_params.dismissTimeout);
	FUNCTION_EXIT;
}


CPromptDlg::~CPromptDlg()
{
    FUNCTION_ENTRY( "~CPromptDlg" );
    FUNCTION_EXIT;
}


void CPromptDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CInteractiveStepDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPromptDlg)
	DDX_Text(pDX, IDC_PROMPT_ACTIVE_INSTANCE_EDIT, m_instance);
	DDX_Text(pDX, IDC_PROMPT_PLAN_PATH_EDIT, m_plan);
	DDX_Text(pDX, IDC_PROMPT_LOCATION_NAME_EDIT, m_location);
	DDX_Text(pDX, IDC_PROMPT_STEP_NUMBER_EDIT, m_step);
    DDX_Text(pDX, IDC_PROMPT_EDIT, m_prompt);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPromptDlg, CInteractiveStepDlg)
    // {{AFX_MSG_MAP(CPromptDlg)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPromptDlg message handlers
BOOL CPromptDlg::OnInitDialog()
{
	FUNCTION_ENTRY( "OnInitDialog" );

	CInteractiveStepDlg::OnInitDialog();

	FUNCTION_EXIT;
	return TRUE;
}

CString CPromptDlg::getTitlePrefix() const
{
    FUNCTION_ENTRY( "getTitlePrefix" );
    FUNCTION_EXIT;
    return "Prompt";
}


void CPromptDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

    try
    {
		PromptStepResponseParameter responseParam(getCompletionStatus());
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
        MessageBox::error(IDS_PROMPT_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}
