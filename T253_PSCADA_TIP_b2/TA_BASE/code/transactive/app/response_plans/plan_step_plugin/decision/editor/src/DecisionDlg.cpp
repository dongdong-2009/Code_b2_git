//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/DecisionDlg.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "StdAfx.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "app/response_plans/plan_step_plugin/decision/common/DecisionResponseParameter.h"
#include "app/response_plans/plan_step_plugin/decision/editor/src/DecisionDlg.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MessageBox.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MftPlanAgentAccessFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CDecisionDlg dialog

CDecisionDlg::CDecisionDlg(const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
						   const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters) :
CInteractiveStepDlg(CDecisionDlg::IDD, eType, ulLocation, ulPlanId, ulInstanceNum, ulStepNum, vecParameters, 0),
    //m_query(_T(*(vecParameters.begin()).c_str()/*stepDetail.data.decision().decisionMessage.in()*/))
	m_params(ulStepNum, eType)
{
    FUNCTION_ENTRY( "CDecisionDlg" );

	// Set plan base infos.
	try
	{
		m_plan = MftPlanAgentAccessFactory::instance().getNodePath(m_ulLocation, m_ulPlanId).c_str();
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
	m_query = m_params.decisionMessage.c_str();
	setResponseTimeout(m_params.responseTimeout);

	FUNCTION_EXIT;
}


CDecisionDlg::~CDecisionDlg()
{
    FUNCTION_ENTRY( "~CDecisionDlg" );
    FUNCTION_EXIT;
}


void CDecisionDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CInteractiveStepDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CDecisionDlg)
	DDX_Text(pDX, IDC_DECISION_ACTIVE_INSTANCE_EDIT, m_instance);
	DDX_Text(pDX, IDC_DECISION_PLAN_PATH_EDIT, m_plan);
	DDX_Text(pDX, IDC_DECISION_LOCATION_NAME_EDIT, m_location);
	DDX_Text(pDX, IDC_DECISION_STEP_NUMBER_EDIT, m_step);
    DDX_Text(pDX, IDC_DECISION_QUERY_EDIT, m_query);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CDecisionDlg, CInteractiveStepDlg)
    // {{AFX_MSG_MAP(CDecisionDlg)
    ON_BN_CLICKED(IDYES, OnYes)
    ON_BN_CLICKED(IDNO, OnNo)
    ON_BN_CLICKED(IDPAUSE, OnPause)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDecisionDlg message handlers

//lizettejl++ (DP-changes)
BOOL CDecisionDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CInteractiveStepDlg::OnInitDialog();

    // TODO: Add extra initialization here
    GetDlgItem(IDYES)->SetWindowText(m_params.yesButtonLabel.c_str());
    GetDlgItem(IDNO)->SetWindowText(m_params.noButtonLabel.c_str());
    GetDlgItem(IDPAUSE)->SetWindowText(m_params.pauseButtonLabel.c_str());
	GetDlgItem(IDNO)->EnableWindow((m_params.noButtonEnabled) ? TRUE : FALSE);
	GetDlgItem(IDPAUSE)->EnableWindow((m_params.pauseButtonEnabled) ? TRUE : FALSE);

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CDecisionDlg::OnPause()
{
    FUNCTION_ENTRY( "OnPause" );

    // TODO: Add your control notification handler code here
    setCompletionStatus(IDRETRY);
	EndDialog( IDRETRY );

    FUNCTION_EXIT;
}


//++lizettejl
void CDecisionDlg::OnYes()
{
    FUNCTION_ENTRY( "OnYes" );

    // TODO: Add your control notification handler code here
    setCompletionStatus(IDYES);
	EndDialog( IDYES );

    FUNCTION_EXIT;
}


void CDecisionDlg::OnNo()
{
    FUNCTION_ENTRY( "OnNo" );

    // TODO: Add your control notification handler code here
    setCompletionStatus(IDNO);
	EndDialog( IDNO );

    FUNCTION_EXIT;
}


CString CDecisionDlg::getTitlePrefix() const
{
    FUNCTION_ENTRY( "getTitlePrefix" );
    FUNCTION_EXIT;
    //return LabelMaker::getDecisionStepDialogTitlePrefix();
	return "Decision";
}


void CDecisionDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

    try
    {
		DecisionResponseParameter responseParam(getCompletionStatus());
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
        MessageBox::error(IDS_DECISION_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}


