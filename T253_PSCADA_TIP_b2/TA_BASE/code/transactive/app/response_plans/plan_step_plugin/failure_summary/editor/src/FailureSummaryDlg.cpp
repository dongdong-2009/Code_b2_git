/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/FailureSummaryDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  */
// FailureSummaryDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FailureSummaryDlg.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MessageBox.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/MftPlanAgentAccessFactory.h"
#include "app/response_plans/plan_step_plugin/failure_summary/common/FailureSummaryResponseParameter.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "core/utilities/src/FunctionEntry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFailureSummaryDlg dialog

using namespace TA_Base_App;

CFailureSummaryDlg::CFailureSummaryDlg(const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
									   const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters) :
CInteractiveStepDlg(CFailureSummaryDlg::IDD, eType, ulLocation, ulPlanId, ulInstanceNum, ulStepNum, vecParameters, 0),
m_params(vecParameters)
{
    FUNCTION_ENTRY( "CFailureSummaryDlg" );

	//Set plan base infos.
    unsigned long ulStepPosition = 0u;
	try
	{
        ulStepPosition = MftPlanAgentAccessFactory::instance().getStepPosition(ulLocation, ulPlanId, ulStepNum);
        m_plan = MftPlanAgentAccessFactory::instance().getNodePath(ulLocation, ulPlanId).c_str();
        m_location = TA_Base_Bus::PlanAgentLocationAccess::getInstance().getLocationName(ulLocation).c_str();
	}
	catch (...)
	{
        m_plan = TA_Base_Bus::StringUtilities::convertToString((unsigned long)ulPlanId);
        m_location = TA_Base_Bus::StringUtilities::convertToString((unsigned long)ulLocation);
	}

    m_step = TA_Base_Bus::StringUtilities::convertToString((unsigned long)ulStepPosition);
    m_instance = TA_Base_Bus::StringUtilities::convertToString((unsigned long)ulInstanceNum);

    FUNCTION_EXIT;
}


CFailureSummaryDlg::~CFailureSummaryDlg()
{
    FUNCTION_ENTRY( "~CFailureSummaryDlg" );
    FUNCTION_EXIT;
}


void CFailureSummaryDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CInteractiveStepDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CFailureSummaryDlg)
	DDX_Text(pDX, IDC_FAILURE_SUMMARY_ACTIVE_INSTANCE_EDIT, m_instance);
	DDX_Text(pDX, IDC_FAILURE_SUMMARY_PLAN_PATH_EDIT, m_plan);
	DDX_Text(pDX, IDC_FAILURE_SUMMARY_LOCATION_NAME_EDIT, m_location);
	DDX_Text(pDX, IDC_FAILURE_SUMMARY_STEP_NUMBER_EDIT, m_step);
    DDX_Control(pDX, IDC_FAILURE_SUMMARY_FAILED_STEPS_LIST, m_failedStepListCtrl);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CFailureSummaryDlg, CInteractiveStepDlg)
    // {{AFX_MSG_MAP(CFailureSummaryDlg)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFailureSummaryDlg message handlers

BOOL CFailureSummaryDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CInteractiveStepDlg::OnInitDialog();

    // TODO: Add extra initialization here
    m_failedStepListCtrl.InsertColumn(0, _T("Step"), LVCFMT_LEFT, 45);
    m_failedStepListCtrl.InsertColumn(1, _T("Name"), LVCFMT_LEFT, 150);
    m_failedStepListCtrl.InsertColumn(2, _T("Error"), LVCFMT_LEFT, 200);

	if(m_params.failedSteps.empty())
    {
        int item = m_failedStepListCtrl.InsertItem(0, _T(""));
        m_failedStepListCtrl.SetItemText(item, 1, _T("No Step failures."));
        m_failedStepListCtrl.EnableWindow(FALSE);
    }
    else
    {
        for (unsigned long i = 0; i < m_params.failedSteps.size(); ++i)
        {
			const FailureStepDetail& failedStep = m_params.failedSteps[i];
            int item = m_failedStepListCtrl.InsertItem(m_failedStepListCtrl.GetItemCount(), _T(""));
            m_failedStepListCtrl.SetItemText(item, 0, TA_Base_Bus::StringUtilities::convertToString(failedStep.pos));
            m_failedStepListCtrl.SetItemText(item, 1, failedStep.name.c_str());
            m_failedStepListCtrl.SetItemText(item, 2, failedStep.error.c_str());
        }
    }

//  move the sending of the step result to reportCompletionStatus(),
//    even if design states otherwise... because the step dialog would be
//  overlapped by the next step dialog
/*    try
    {
        PlanAgentAccess::getInstance().reportFailureSummaryStepResult(getStepDetail().activePlan,
            TA_Base_Core::PSCS_SUCCESS, "");
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_REPORT_COMPLETION_STATUS, ex);
    }
*/

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

CString CFailureSummaryDlg::getTitlePrefix() const
{
    FUNCTION_ENTRY( "getTitlePrefix" );
    FUNCTION_EXIT;
    return "Failed Step Summary";
}


void CFailureSummaryDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

    // do nothing
    // the step is demed to have completed when the dialog is
    // displayed and NOT when the operator has closed it.
    // the completion status is reported from OnInitDialog()
    try
    {
		FailureSummaryResponseParameter responseParam(getCompletionStatus());
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
        MessageBox::error(IDS_FAILURE_SUMMARY_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}
