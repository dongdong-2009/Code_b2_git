//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/InteractiveStepDlg.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2014/06/03 09:20:18 $
// Last modified by:  $Author: huangjian $
//
// <description>

#include "stdafx.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "resource.h"
#include "InteractiveStepDlg.h"
#include "ApplicationFont.h"


static const UINT INTERACTIVE_STEP_TIMEOUT         = 100;
static const UINT INTERACTIVE_STEP_TIMER_COUNTDOWN = 101;

namespace TA_Base_App
{


/////////////////////////////////////////////////////////////////////////////
// CInteractiveStepDlg dialog


CInteractiveStepDlg::CInteractiveStepDlg( const unsigned int unResTemplateId, const int eType, const unsigned int ulLocation, const unsigned int ulPlanId,
    const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters, time_t timeout) : CDialog( unResTemplateId ),
    //m_stepDetail(stepDetail),
m_eType(eType),
m_ulLocation(ulLocation),
m_ulPlanId(ulPlanId),
m_ulInstanceNum(ulInstanceNum),
m_ulStepNum(ulStepNum),
m_parameters(vecParameters),
m_completionStatus(0u),
m_timeout(timeout),
m_secondsToTimeout(0),
m_stepExecutionInterrupted(false)
{
    FUNCTION_ENTRY( "CInteractiveStepDlg" );

    FUNCTION_EXIT;
}


CInteractiveStepDlg::~CInteractiveStepDlg()
{
    FUNCTION_ENTRY( "~CInteractiveStepDlg" );

    FUNCTION_EXIT;
}


void CInteractiveStepDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CInteractiveStepDlg)
    //DDX_Text(pDX, IDC_ACTIVE_INSTANCE_EDIT, m_instance);
    //DDX_Text(pDX, IDC_PLAN_PATH_EDIT, m_plan);
    //DDX_Text(pDX, IDC_LOCATION_NAME_EDIT, m_location);
    //DDX_Text(pDX, IDC_STEP_NUMBER_EDIT, m_step);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CInteractiveStepDlg, CDialog)
    // {{AFX_MSG_MAP(CInteractiveStepDlg)
    ON_WM_TIMER()
    ON_WM_DESTROY()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInteractiveStepDlg message handlers

void CInteractiveStepDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    lpCreateStruct->style |= DS_SYSMODAL;

    CDialog::OnCreate(lpCreateStruct);
}

BOOL CInteractiveStepDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
  //  try
  //  {
  //      //m_plan = PlanAgentAccess::getInstance().getNodePath(m_stepDetail.activePlan.plan).c_str();//lixiaoxia??
		//m_plan = TA_Base_Bus::PlanDataReader::instance()->getNodePath( m_stepDetail.activePlan.plan).c_str();
  //      m_location = TA_Base_Bus::PlanAgentLocationAccess::getInstance().getLocationName(m_stepDetail.activePlan.location).c_str();
  //  }
  //  catch (...)
  //  {
  //      m_plan = TA_Base_Bus::StringUtilities::convertToString(m_stepDetail.activePlan.plan);
  //      m_location = TA_Base_Bus::StringUtilities::convertToString(m_stepDetail.activePlan.location);
  //  }

  //  m_step = TA_Base_Bus::StringUtilities::convertToString(m_stepDetail.step);
  //  m_instance = TA_Base_Bus::StringUtilities::convertToString(m_stepDetail.activePlan.instance);

    // Position the dialog relative to the parent window and other interactive
    // step dialogs that the parent window may have active. Start by placing the
    // dialog in the centre of the parent window and then continue moving it
    // as necessary until an unoccupied position is found.
    CenterWindow();
    SetForegroundWindow();

    // Ensure the window stays on top of other windows and is not obscured so that the
    // operator can respond to the step asap.
    SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
    UpdateData(FALSE);

    // Hide icon in task bar
    ModifyStyleEx(WS_EX_APPWINDOW, WS_EX_TOOLWINDOW);

    // Start user response timer
    if ( 0u == m_timeout )
    {
        // time out does not apply for the failure summary step
        // therefore the INTERACTIVE_STEP_TIMEOUT timer should not be
        // started
        SetWindowText(_T(getTitlePrefix()));
    }
    else
    {
        startTimeoutCountdown();
    }

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CInteractiveStepDlg::OnTimer(UINT nIDEvent)
{
    FUNCTION_ENTRY( "OnTimer" );

    // TODO: Add your message handler code here and/or call default
    if (nIDEvent == INTERACTIVE_STEP_TIMER_COUNTDOWN)
    {
        updateTimeoutCountdownDisplay();
    }
    else if (nIDEvent == INTERACTIVE_STEP_TIMEOUT)
    {
        setCompletionStatus(IDABORT);
        EndDialog( IDABORT );
    }
    else
    {
        CDialog::OnTimer(nIDEvent);
    }

    FUNCTION_EXIT;
}


void CInteractiveStepDlg::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );

    // TODO: Add your message handler code here
    KillTimer(INTERACTIVE_STEP_TIMEOUT);
    KillTimer(INTERACTIVE_STEP_TIMER_COUNTDOWN);

    // If the window is being destroyed due to the plan instance having been stopped,
    // then we should not attempt to report the step's completion status.
    if (!m_stepExecutionInterrupted)
    {
        reportCompletionStatus();
    }

    CDialog::OnDestroy();

    FUNCTION_EXIT;
}


void CInteractiveStepDlg::PostNcDestroy()
{
    FUNCTION_ENTRY( "PostNcDestroy" );

    CDialog::PostNcDestroy();

    FUNCTION_EXIT;
}


void CInteractiveStepDlg::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    setCompletionStatus(IDOK);

    CDialog::OnOK();

    FUNCTION_EXIT;
}


void CInteractiveStepDlg::OnCancel()
{
    FUNCTION_ENTRY( "OnCancel" );

    // Trap cancellations such as those resulting from the Esc key being pressed

    FUNCTION_EXIT;
}


void CInteractiveStepDlg::startTimeoutCountdown()
{
    FUNCTION_ENTRY( "startTimeoutCountdown" );

    SetTimer(INTERACTIVE_STEP_TIMEOUT, m_timeout * 1000, NULL);
    SetTimer(INTERACTIVE_STEP_TIMER_COUNTDOWN, 1000, NULL);

    m_secondsToTimeout = m_timeout;

    updateTimeoutCountdownDisplay();

    FUNCTION_EXIT;
}


void CInteractiveStepDlg::updateTimeoutCountdownDisplay()
{
    FUNCTION_ENTRY( "updateTimeoutCountdownDisplay" );

    CString title;
    title.Format("%s [timeout in %lu sec]", getTitlePrefix(), m_secondsToTimeout);

    SetWindowText(_T(title));

    m_secondsToTimeout--;

    FUNCTION_EXIT;
}


void CInteractiveStepDlg::setCompletionStatus(int exitCode)
{
    FUNCTION_ENTRY( "setCompletionStatus" );

    switch (exitCode)
    {
        case IDOK:
            m_completionStatus = TA_Base_Core::PSCS_ACKNOWLEDGEMENT;
            break;

        case IDYES:
            m_completionStatus = TA_Base_Core::PSCS_YES_RESPONSE;
            break;

        case IDNO:
            m_completionStatus = TA_Base_Core::PSCS_NO_RESPONSE;
            break;

        case IDABORT:
            m_completionStatus = TA_Base_Core::PSCS_TIMEOUT;
            break;
        // lizettejl++ (DP-changes)
        case IDRETRY:
            m_completionStatus = TA_Base_Core::PSCS_PAUSE_RESPONSE;
            break;
        // ++lizettejl
        default:
            m_completionStatus = TA_Base_Core::PSCS_UNDEFINED;
    }

    FUNCTION_EXIT;
}


int CInteractiveStepDlg::getCompletionStatus() const
{
    FUNCTION_ENTRY( "getCompletionStatus" );
    FUNCTION_EXIT;
    return m_completionStatus;
}

void CInteractiveStepDlg::setResponseTimeout(unsigned long timeout)
{
    m_timeout = timeout;
}

//TA_Base_Core::InteractiveStepData CInteractiveStepDlg::getStepDetail() const
//{
//    FUNCTION_ENTRY( "getStepDetail" );
//    FUNCTION_EXIT;
//    return m_stepDetail;
//}


}
