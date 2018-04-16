// ./src/ScheduleBroadcastDlg.cpp : implementation file
//

#include <afxpriv.h>
#include "app/pa/PAManager/src/stdafx.h"
#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/VariableConvertor.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/PaScheduleManager.h"
#include "app/pa/PAManager/src/ScheduleBroadcastDlg.h"


// ScheduleBroadcastDlg dialog

IMPLEMENT_DYNAMIC(ScheduleBroadcastDlg, CDialog)

ScheduleBroadcastDlg::ScheduleBroadcastDlg( CWnd* pParent ) : 
CDialog(ScheduleBroadcastDlg::IDD, pParent),
m_nOperationMode( VIEW_MODE )
{
}

ScheduleBroadcastDlg::~ScheduleBroadcastDlg()
{
}

void ScheduleBroadcastDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_SCHEDULE_NAME, m_edScheduleName);
    DDX_Control(pDX, IDC_BTN_CONFIRM, m_btnConfirm);
    DDX_Control(pDX, IDC_BTN_CANCEL, m_btnCancel);
    DDX_Control(pDX, IDC_BTN_DELETE_SCHEDULE, m_btnDeleteSchedule);
    DDX_Control(pDX, IDC_EDIT_BROADCAST_MSG, m_edBroadcastMsg);
    DDX_Control(pDX, IDC_EDIT_BROADCAST_ZONES, m_edBroadcastZones);
    DDX_Control(pDX, IDC_LIST_BROADCAST_SCHEDULE, m_lsBroadcastSchedule);
}

BEGIN_MESSAGE_MAP(ScheduleBroadcastDlg, CDialog)
    ON_BN_CLICKED(IDC_BTN_CONFIRM, OnBnClickedBtnConfirm)
    ON_BN_CLICKED(IDC_BTN_CANCEL, OnBnClickedBtnCancel)
END_MESSAGE_MAP()

BOOL ScheduleBroadcastDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    initialiseScheduleList();

    if ( VIEW_MODE != m_nOperationMode )
    {
        updateEditModeCtrls();
    }

    return TRUE;
}

// ScheduleBroadcastDlg message handlers

void ScheduleBroadcastDlg::setOperationMode( int nMode )
{
    m_nOperationMode = nMode;
}

void ScheduleBroadcastDlg::OnBnClickedBtnConfirm()
{
    // Firstly get all edit data and convert to agent type
    std::string strScheduleName = "";
    {
        char strScheduleNameTemp[SCHEDULE_NAME_MAX_CHARS] = { 0 };
        m_edScheduleName.GetWindowText( strScheduleNameTemp, SCHEDULE_NAME_MAX_CHARS );
        strScheduleName = strScheduleNameTemp;
    }
    if ( strScheduleName.empty() )
    {
        PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_SCHEDULE_NAME_NOT_SET );
        return;
    }

    time_t tmStartTime = static_cast<time_t>(m_tmStartTimeEdit.GetTime());
    time_t tmEndTime = static_cast<time_t>(m_tmEndTimeEdit.GetTime());

    // Secondly call agent to add the schedule and save to database
    try
    {
    }
    catch (...)
    {
        PaErrorHelper::popupErrorMsgBoxImmidate( this, ERROR_FAILED_TO_SAVE_NEW_SCHEDULE );
        return;
    }

    // Thirdly insert into list ctrl
    if ( ADD_NEW_MODE == m_nOperationMode )
    {
    }
    else if ( MODIFY_EXIST_MODE == m_nOperationMode )
    {
        modifyExistInList();
    }
    else
    {
        // Log here...
    }

    // Fourthly set operation mode to view and update dialog controls
    updateToViewMode();
}

void ScheduleBroadcastDlg::OnBnClickedBtnCancel()
{
    updateToViewMode();
}

void ScheduleBroadcastDlg::updateToViewMode()
{
    m_nOperationMode = VIEW_MODE;
    m_edScheduleName.EnableWindow( FALSE );
    m_btnCancel.EnableWindow( FALSE );
    m_btnConfirm.EnableWindow( FALSE );
}

void ScheduleBroadcastDlg::setScheduleTime( SYSTEMTIME& stStartTime, SYSTEMTIME& stEndTime )
{
    CTime tmStartTime( stStartTime );
    m_tmStartTimeEdit = tmStartTime;
    CTime tmEndTime( stEndTime );
    m_tmEndTimeEdit = tmEndTime;
}

void ScheduleBroadcastDlg::insertToList()
{
}

void ScheduleBroadcastDlg::modifyExistInList()
{
}

void ScheduleBroadcastDlg::initialiseScheduleList()
{
}

void ScheduleBroadcastDlg::updateEditModeCtrls()
{
    m_edScheduleName.EnableWindow( TRUE );
    m_btnCancel.EnableWindow( TRUE );
    m_btnConfirm.EnableWindow( TRUE );
}