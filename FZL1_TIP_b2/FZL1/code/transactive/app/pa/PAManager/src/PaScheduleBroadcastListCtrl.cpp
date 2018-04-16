#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/VariableConvertor.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/PaScheduleManager.h"
#include "app/pa/PAManager/src/PaScheduleBroadcastListCtrl.h"

PaScheduleBroadcastListCtrl::PaScheduleBroadcastListCtrl() : 
TA_Base_Bus::AutoSortListCtrl( SCHEDULE_BROADCAST_LIST_ITEM_MAX_COUNT )
{
    m_fntList.CreatePointFont( LIST_CTRL_FONT_SIZE * 10, PA_MANAGER_FONT_FACE_NAME );
}

PaScheduleBroadcastListCtrl::~PaScheduleBroadcastListCtrl(void)
{
    m_fntList.DeleteObject();
}

BEGIN_MESSAGE_MAP(PaScheduleBroadcastListCtrl, TA_Base_Bus::AutoSortListCtrl)
//{{AFX_MSG_MAP(PaDVAMsgListCtrl)
// NOTE - the ClassWizard will add and remove mapping macros here.
    ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void PaScheduleBroadcastListCtrl::PreSubclassWindow() 
{
    SetFont( &m_fntList );
    SetExtendedStyle( GetExtendedStyle() | LVS_EX_FULLROWSELECT );

    InsertColumn( SCHEDULE_BROADCAST_LIST_COL_NAME_INDEX, SCHEDULE_BROADCAST_LIST_COL_NAME );
    InsertColumn( SCHEDULE_BROADCAST_LIST_COL_START_TIME_INDEX, SCHEDULE_BROADCAST_LIST_COL_START_TIME );
    InsertColumn( SCHEDULE_BROADCAST_LIST_COL_END_TIME_INDEX, SCHEDULE_BROADCAST_LIST_COL_END_TIME );
    InsertColumn( SCHEDULE_BROADCAST_LIST_COL_FREQUENCY_INDEX, SCHEDULE_BROADCAST_LIST_COL_FREQUENCY );
    InsertColumn( SCHEDULE_BROADCAST_LIST_COL_AUTO_DEL_INDEX, SCHEDULE_BROADCAST_LIST_COL_AUTO_DEL );
    SetColumnWidth( SCHEDULE_BROADCAST_LIST_COL_NAME_INDEX, SCHEDULE_BROADCAST_LIST_COL_NAME_WIDTH );
    SetColumnWidth( SCHEDULE_BROADCAST_LIST_COL_START_TIME_INDEX, SCHEDULE_BROADCAST_LIST_COL_START_TIME_WIDTH );
    SetColumnWidth( SCHEDULE_BROADCAST_LIST_COL_END_TIME_INDEX, SCHEDULE_BROADCAST_LIST_COL_END_TIME_WIDTH );
    SetColumnWidth( SCHEDULE_BROADCAST_LIST_COL_FREQUENCY_INDEX, SCHEDULE_BROADCAST_LIST_COL_FREQUENCY_WIDTH );
    SetColumnWidth( SCHEDULE_BROADCAST_LIST_COL_AUTO_DEL_INDEX, SCHEDULE_BROADCAST_LIST_COL_AUTO_DEL_WIDTH );

    AutoSortListCtrl::PreSubclassWindow();
}

void PaScheduleBroadcastListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
    *pResult = 0;

    POSITION pPos = GetFirstSelectedItemPosition();
    if ( NULL == pPos )
    {
        return;
    }

    int nSeledItem = GetNextSelectedItem( pPos );
}

void PaScheduleBroadcastListCtrl::insertNewSchedule( const std::string& strName, time_t tmStartTime, time_t tmEndTime, int nFrequencyType, int nFrequencyDetail, bool bAutoDel )
{
    int nInsertPos = InsertItem( 0, strName.c_str() );
    CTime tmStart( tmStartTime );
    CTime tmEnd( tmEndTime );
    CString csTimeString = tmStart.Format( "%Y-%m-%d %H:%M:%S" );
    SetItemText( nInsertPos, SCHEDULE_BROADCAST_LIST_COL_START_TIME_INDEX, csTimeString.operator LPCTSTR() );
    csTimeString = tmEnd.Format( "%Y-%m-%d %H:%M:%S" );
    SetItemText( nInsertPos, SCHEDULE_BROADCAST_LIST_COL_END_TIME_INDEX, csTimeString.operator LPCTSTR() );
    std::string strFrequency = ""; // Need to get string from PaScheduleManager
    SetItemText( nInsertPos, SCHEDULE_BROADCAST_LIST_COL_FREQUENCY_INDEX, strFrequency.c_str() );
    std::string strAutoDel = ""; // Need to get string from PaScheduleManager
    SetItemText( nInsertPos, SCHEDULE_BROADCAST_LIST_COL_AUTO_DEL_INDEX, strAutoDel.c_str() );
}