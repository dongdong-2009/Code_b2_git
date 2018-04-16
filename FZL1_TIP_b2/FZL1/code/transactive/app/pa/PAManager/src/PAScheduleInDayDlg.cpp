/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File$
  * @author  Huang Qi
  * @version $Revision$
  * Last modification : $DateTime$
  * Last modified by : $Author$
  *
  *
  */
#include "app/pa/PAManager/src/StdAfx.h"

#include "core/data_access_interface/pa/src/PaTableConsts.h"
#include "core/data_access_interface/pa/src/PaScheduleBroadcastAccessFactory.h"

#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/CachedConfig.h"
#include "app/pa/PAManager/src/VariableConvertor.h"
#include "app/pa/PAManager/src/PAScheduleInDayDlg.h"

PAScheduleInDayDlg::PAScheduleInDayDlg( CWnd* pParent ) : 
CDialog(PAScheduleInDayDlg::IDD, pParent),
m_pModel( NULL ),
m_fntList()
{
    m_pModel = new ScheduleBroadcastModel( *this );
    m_fntList.CreatePointFont( LIST_CTRL_FONT_SIZE * 10, PA_MANAGER_FONT_FACE_NAME );
}

PAScheduleInDayDlg::~PAScheduleInDayDlg()
{
    m_fntList.DeleteObject();
    if ( NULL != m_pModel )
    {
        delete m_pModel;
        m_pModel = NULL;
    }
}

void PAScheduleInDayDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST_SCHEDULE_IN_DAY, m_lstScheduleBroadcasts);
    DDX_Control(pDX, IDC_LIST_SCHEDULE_ZONES, m_lbZones);
    DDX_Control(pDX, IDC_BTN_DELETE_SCHEDULE_IN_DAY, m_btnDelSchedule);
}

BEGIN_MESSAGE_MAP(PAScheduleInDayDlg, CDialog)
    ON_WM_MOVING()
    ON_WM_CLOSE()
    ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_SCHEDULE_IN_DAY, OnItemchangedListScheduleInDay)
    ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_SCHEDULE_IN_DAY, OnItemdeletedListScheduleInDay)
    ON_MESSAGE(WM_SCHEDULE_BROADCAST_UPDATE, OnScheduleBroadcastUpdate)
    ON_BN_CLICKED(IDC_BTN_DELETE_SCHEDULE_IN_DAY, OnClickedBtnDeleteScheduleInDay)
    ON_BN_CLICKED(IDC_BTN_HIDE, OnClickedBtnHide)
END_MESSAGE_MAP()

void PAScheduleInDayDlg::OnMoving( UINT unNoUse, LPRECT pRect )
{
    RECT stMyRect = { 0, 0, 0, 0 };
    GetWindowRect( &stMyRect );
    CDialog::OnMoving( unNoUse, pRect );

    pRect->top = stMyRect.top;
    pRect->bottom = stMyRect.bottom;
    pRect->left = stMyRect.left;
    pRect->right = stMyRect.right;
}

BOOL PAScheduleInDayDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    m_btnDelSchedule.EnableWindow( FALSE );

    if ( NULL != m_pModel )
    {
        m_pModel->initialise();
    }

    initialiseScheduleList();

    //adjustWndPos();

    return TRUE;
}

void PAScheduleInDayDlg::initialiseScheduleList()
{
    m_lbZones.SetFont( &m_fntList );
    m_lstScheduleBroadcasts.SetFont( &m_fntList );
    m_lstScheduleBroadcasts.SetExtendedStyle( m_lstScheduleBroadcasts.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP );
    m_lstScheduleBroadcasts.InsertColumn( SCHEDULE_IN_DAY_LIST_COL_NAME_INDEX, SCHEDULE_IN_DAY_LIST_COL_NAME );
    m_lstScheduleBroadcasts.InsertColumn( SCHEDULE_IN_DAY_LIST_COL_START_TIME_INDEX, SCHEDULE_IN_DAY_LIST_COL_START_TIME );
    m_lstScheduleBroadcasts.InsertColumn( SCHEDULE_IN_DAY_LIST_COL_STATUS_INDEX, SCHEDULE_IN_DAY_LIST_COL_STATUS );
    m_lstScheduleBroadcasts.SetColumnWidth( SCHEDULE_IN_DAY_LIST_COL_NAME_INDEX, SCHEDULE_IN_DAY_LIST_COL_NAME_WIDTH );
    m_lstScheduleBroadcasts.SetColumnWidth( SCHEDULE_IN_DAY_LIST_COL_START_TIME_INDEX, SCHEDULE_IN_DAY_LIST_COL_START_TIME_WIDTH );
    m_lstScheduleBroadcasts.SetColumnWidth( SCHEDULE_IN_DAY_LIST_COL_STATUS_INDEX, SCHEDULE_IN_DAY_LIST_COL_STATUS_WIDTH );
    m_lstScheduleBroadcasts.EnableToolTips( TRUE );

    // Disable column header
    CHeaderCtrl* pHeaderCtrl = m_lstScheduleBroadcasts.GetHeaderCtrl();
    if ( NULL != pHeaderCtrl )
    {
        pHeaderCtrl->EnableWindow( FALSE ) ;
    }

    if ( NULL != m_pModel )
    {
        m_pModel->updateScheduleList();
    }
}

int PAScheduleInDayDlg::insertScheduleToList( unsigned long ulKey, 
    const std::string& strName, const std::string& strStartTime, const std::string& strStatus )
{
    int nNewIndex = m_lstScheduleBroadcasts.InsertItem( 0, strName.c_str() );
    m_lstScheduleBroadcasts.SetItemData( nNewIndex, static_cast<DWORD>(ulKey) );
    m_lstScheduleBroadcasts.SetItemText( nNewIndex, SCHEDULE_IN_DAY_LIST_COL_START_TIME_INDEX, strStartTime.c_str() );
    m_lstScheduleBroadcasts.SetItemText( nNewIndex, SCHEDULE_IN_DAY_LIST_COL_STATUS_INDEX, strStatus.c_str() );

    return SUCCESS_NO_ERROR;
}

LRESULT PAScheduleInDayDlg::OnScheduleBroadcastUpdate( WPARAM wParam, LPARAM lParam )
{
    if ( NULL != m_pModel )
    {
        m_pModel->scheduleBroadcastUpdate( reinterpret_cast<ScheduleBroadcastDetail*>(wParam), static_cast<int>(lParam) );
    }

    return 0;
}

int PAScheduleInDayDlg::deleteScheduleFromList( unsigned long ulSchedulekey )
{
    if ( 0 == ulSchedulekey )
    {
        return ERROR_INVALID_INPUT_PARAMETER;
    }

    for ( int nLoop = 0; nLoop < m_lstScheduleBroadcasts.GetItemCount(); ++nLoop )
    {
        if ( ulSchedulekey == m_lstScheduleBroadcasts.GetItemData( nLoop ))
        {
            m_lstScheduleBroadcasts.DeleteItem( nLoop );
            break;
        }
    }

    return SUCCESS_NO_ERROR;
}

int PAScheduleInDayDlg::updateBroadcastParamContent( const std::string& strMsg, 
    const std::string& strCylicTimes, const std::vector<std::string>& vecZones )
{
    SetDlgItemText( IDC_EDIT_SCHEDULE_DVA_MSG, strMsg.c_str() );
    SetDlgItemText( IDC_EDIT_SCHEDULE_CYCLIC_TIMES, strCylicTimes.c_str() );

    m_lbZones.ResetContent();
    for ( std::vector<std::string>::const_iterator itLoop = vecZones.begin();
            vecZones.end() != itLoop; ++itLoop )
    {
        m_lbZones.AddString( itLoop->c_str() );
    }

    return SUCCESS_NO_ERROR;
}

void PAScheduleInDayDlg::onSelchangeListSchedules()
{
    POSITION pPos = m_lstScheduleBroadcasts.GetFirstSelectedItemPosition();

    int nSeledItem = LIST_NO_SEL;
    if ( NULL != pPos )
    {
        nSeledItem = m_lstScheduleBroadcasts.GetNextSelectedItem( pPos );
    }

    BOOL bEnableDel = ( LIST_NO_SEL == nSeledItem ) ? FALSE : TRUE;
    m_btnDelSchedule.EnableWindow( bEnableDel );

    if ( NULL == m_pModel )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER, "m_pModel" );
        return;
    }

    m_pModel->updateScheduleListCurSel( nSeledItem );
}

unsigned long PAScheduleInDayDlg::getScheduleKeyByListItemIndex( int nIndex )
{
    if ( LIST_NO_SEL == nIndex )
    {
        return INVALID_PKEY;
    }

    return static_cast<unsigned long>( m_lstScheduleBroadcasts.GetItemData( nIndex ));
}

void PAScheduleInDayDlg::OnItemchangedListScheduleInDay(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    *pResult = 0;

    if ( NULL == pNMLV )
    {
        return;
    }

    onSelchangeListSchedules();
}

void PAScheduleInDayDlg::OnItemdeletedListScheduleInDay( NMHDR *pNMHDR, LRESULT *pResult )
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    *pResult = 0;

    if ( NULL == pNMLV )
    {
        return;
    }

    if ( NULL == m_pModel )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER, "m_pModel" );
        return;
    }

    m_pModel->onScheduleListItemDeleted( pNMLV->iItem );
}

void PAScheduleInDayDlg::clearScheduleList()
{
    m_lstScheduleBroadcasts.DeleteAllItems();
}

void PAScheduleInDayDlg::OnClickedBtnDeleteScheduleInDay()
{
    if ( !PaErrorHelper::popupConfirmMsgBoxImmidate( this, CONFIRM_DELETE_SCHEDULE_BROADCAST_IN_DAY ))
    {
        return;
    }

    if ( NULL == m_pModel )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER, "m_pModel" );
        return;
    }

    if ( LIST_NO_SEL == m_pModel->getScheduleListCurSel() )
    {
        PaErrorHelper::popupWarningMsgBoxImmidate( this, WARNING_SCHEDULE_NOT_SELECT );
        return;
    }

    int nRet = m_pModel->removeSchedule();
    if ( PaErrorHelper::checkErrorImmidate( this, nRet ))
    {
        return;
    }

    PaErrorHelper::popupSuccessMsgBoxImmidate( this, SUCCESS_REMOVE_SCHEDULE_DVA_BROADCAST );
}

int PAScheduleInDayDlg::updateScheduleStateToList( unsigned long ulSchedulekey, const std::string& strStatus )
{
    if ( 0 == ulSchedulekey )
    {
        return ERROR_INVALID_INPUT_PARAMETER;
    }

    for ( int nLoop = 0; nLoop < m_lstScheduleBroadcasts.GetItemCount(); ++nLoop )
    {
        if ( ulSchedulekey == m_lstScheduleBroadcasts.GetItemData( nLoop ))
        {
            m_lstScheduleBroadcasts.SetItemText( nLoop, SCHEDULE_IN_DAY_LIST_COL_STATUS_INDEX, strStatus.c_str() );
            break;
        }
    }

    return SUCCESS_NO_ERROR;
}

void PAScheduleInDayDlg::adjustWndPos()
{
    CWnd* pParent = GetParent();
    if ( NULL == pParent )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER, "pParent" );
        return;
    }

    RECT stMyRect = { 0, 0, 0, 0 };
    GetWindowRect( &stMyRect );
    RECT stParentRect = { 0, 0, 0, 0 };
    pParent->GetWindowRect( &stParentRect );
    stMyRect.bottom = stMyRect.bottom + stParentRect.top - stMyRect.top;
    stMyRect.right = stMyRect.right + stParentRect.right - stMyRect.left;
    stMyRect.top = stParentRect.top;
    stMyRect.left = stParentRect.right;

    MoveWindow( &stMyRect, FALSE );
}

void PAScheduleInDayDlg::OnClickedBtnHide()
{
    OnClose();
}

void PAScheduleInDayDlg::OnClose()
{
    ShowWindow( SW_HIDE );
    GetParent()->PostMessage( WM_SCHEDULE_IN_DAY_DLG_HIDE );
}

void PAScheduleInDayDlg::OnOK()
{
    // Do nothing
}

void PAScheduleInDayDlg::OnCancel()
{
    // Do nothing
}