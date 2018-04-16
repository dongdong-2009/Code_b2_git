// PaDVAMsgListCtrl.cpp: implementation of the PaDVAMsgListCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/PaDVAMsgListCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PaDVAMsgListCtrl::PaDVAMsgListCtrl( int nRecordType ) :
TA_Base_Bus::AutoSortListCtrl( DVA_MSG_LIST_ITEM_MAX_COUNT ),
m_nDVAMsgType( nRecordType )
{
}

PaDVAMsgListCtrl::~PaDVAMsgListCtrl()
{
}

BEGIN_MESSAGE_MAP(PaDVAMsgListCtrl, TA_Base_Bus::AutoSortListCtrl)
//{{AFX_MSG_MAP(PaDVAMsgListCtrl)
// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_NOTIFY_REFLECT(NM_CLICK, OnClick)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void PaDVAMsgListCtrl::PreSubclassWindow() 
{
    SetExtendedStyle( GetExtendedStyle() | LVS_EX_FULLROWSELECT );
    InsertColumn( DVA_MSG_LIST_ID_INDEX, DVA_MSG_LIST_COL_ID );
    InsertColumn( DVA_MSG_LIST_NAME_INDEX, DVA_MSG_LIST_COL_NAME );
    SetColumnWidth( DVA_MSG_LIST_ID_INDEX, DVA_MSG_LIST_COL_ID_WIDTH );
    SetColumnWidth( DVA_MSG_LIST_NAME_INDEX, DVA_MSG_LIST_COL_NAME_WIDTH );

    AutoSortListCtrl::PreSubclassWindow();
}

int PaDVAMsgListCtrl::InsertItem( int nItem, LPCTSTR lpszItem )
{
    return AutoSortListCtrl::InsertItem( nItem, lpszItem );
}

void PaDVAMsgListCtrl::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
    *pResult = 0;

    if (( NULL == pNMLV ) || ( LVIF_STATE != pNMLV->uChanged ) || ( 0 == pNMLV->uNewState ))
    {
        // When notify for cancel selected item, do nothing
        return;
    }

    // Do nothing here, auto sort list will reinsert item and make item changed when paint
}

void PaDVAMsgListCtrl::OnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
    *pResult = 0;

    POSITION pPos = GetFirstSelectedItemPosition();
    if ( NULL == pPos )
    {
        PaDataManager::getInstance().updateCurSelDVAMsg( m_nDVAMsgType, NULL, true );
        return;
    }

    int nSeledItem = GetNextSelectedItem( pPos );
    DVAMsgInfo* pInfo = reinterpret_cast<DVAMsgInfo*>(GetItemData( nSeledItem ));
    PaDataManager::getInstance().updateCurSelDVAMsg( m_nDVAMsgType, pInfo, true );
}

void PaDVAMsgListCtrl::updateList( const std::string& strFilterWord )
{
    std::list<DVAMsgInfo*> lstInfos;
    PaDataManager::getInstance().getDVAMsgs( strFilterWord, m_nDVAMsgType, lstInfos );
    const DVAMsgInfo* pCulSelInfo = PaDataManager::getInstance().getCurSelDVAMsg( m_nDVAMsgType );

    DeleteAllItems();

    bool bSelItemExist = false;
    for ( std::list<DVAMsgInfo*>::iterator itLoop = lstInfos.begin();
          lstInfos.end() != itLoop; ++itLoop )
    {
        DVAMsgInfo* pInfo = *itLoop;
        if ( NULL == pInfo )
        {
            continue;
        }
        int nNewPos = InsertItem( 0, pInfo->strID.c_str() );
        SetItemText( nNewPos, DVA_MSG_LIST_NAME_INDEX, pInfo->strName.c_str() );
        SetItemData( nNewPos, reinterpret_cast<DWORD>(pInfo) );
        if ( pCulSelInfo == pInfo )
        {
            SetItemState( nNewPos, LVIS_SELECTED, LVIS_SELECTED );
            bSelItemExist = true;
        }
    }

    if ( !bSelItemExist )
    {
        PaDataManager::getInstance().updateCurSelDVAMsg( m_nDVAMsgType, NULL );
    }
}

void PaDVAMsgListCtrl::selQuickBroadcastItem( WPARAM wParam )
{
    // wParam is a pointer of BroadcastRecordInfo
    DWORD dwData = static_cast<DWORD>(wParam);
    if ( NULL == dwData )
    {
        return;
    }

    // The data is not found in the list, the list is filtered.
    if ( !SelectItemByItemData( dwData ))
    {
        updateList( std::string("") );
        SelectItemByItemData( dwData );
    }
}

bool PaDVAMsgListCtrl::SelectItemByItemData( DWORD dwData ) 
{
    const int nItemCount = GetItemCount();
    if ( 0 == nItemCount )
    {
        return false;
    }

    int nLoop = 0;
    for ( nLoop = 0; nLoop < nItemCount; ++nLoop )
    {
        if ( dwData == GetItemData( nLoop ))
        {
            POSITION pPos = GetFirstSelectedItemPosition();
            if ( NULL != pPos )
            {
                SetItemState( GetNextSelectedItem( pPos ), 0, LVIS_SELECTED );
            }
            SetItemState( nLoop, LVIS_SELECTED, LVIS_SELECTED );
            PaDataManager::getInstance().updateCurSelDVAMsg( m_nDVAMsgType, reinterpret_cast<DVAMsgInfo*>(dwData), true );
            break;
        }
    }

    // The ItemData is not in the list
    if ( nItemCount == nLoop )
    {
        return false;
    }

    return true;
}