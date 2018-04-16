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
#include "app/pa/PAManager/src/PAManagerConsts.h"
#include "app/pa/PAManager/src/PaDataManager.h"
#include "app/pa/PAManager/src/PaErrorHelper.h"
#include "app/pa/PAManager/src/PaRuntimeModel.h"
#include "app/pa/PAManager/src/PaDVAMsgListCtrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

PaDVAMsgListCtrl::PaDVAMsgListCtrl( PaRuntimeModel& refBroadcastModel, int nRecordType ) 
:
TA_Base_Bus::AutoSortListCtrl( 1000 ),
m_refBroadcastModel( refBroadcastModel ),
m_nDVAMsgType( nRecordType ),
m_fntList(),
m_pRootConfigMenu( new CMenu() )
{
    if ( NULL != m_pRootConfigMenu )
    {
        m_pRootConfigMenu->LoadMenu( IDR_MENU_DVA_MSG_SHORTCUT_CONFIG );
    }

    m_fntList.CreatePointFont( LIST_CTRL_FONT_SIZE * 10, PA_MANAGER_FONT_FACE_NAME );
}

PaDVAMsgListCtrl::~PaDVAMsgListCtrl()
{
    m_fntList.DeleteObject();

    if ( NULL != m_pRootConfigMenu )
    {
        m_pRootConfigMenu->DestroyMenu();
        delete m_pRootConfigMenu;
        m_pRootConfigMenu = NULL;
    }
}

BEGIN_MESSAGE_MAP(PaDVAMsgListCtrl, TA_Base_Bus::AutoSortListCtrl)
//{{AFX_MSG_MAP(PaDVAMsgListCtrl)
// NOTE - the ClassWizard will add and remove mapping macros here.
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
    ON_NOTIFY_REFLECT(LVN_KEYDOWN, OnKeydown)
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_CONTEXTMENU()
    ON_MESSAGE(WM_LIST_ITEM_SELECTING_CHANGED, OnUpdateSelectedItem)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void PaDVAMsgListCtrl::PreSubclassWindow() 
{
    SetFont( &m_fntList );
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

void PaDVAMsgListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
    // Item selecting is not changed yet, must post a message to msg queue for handle
    PostMessage( WM_LIST_ITEM_SELECTING_CHANGED );

    AutoSortListCtrl::OnRButtonDown(nFlags, point);
}

void PaDVAMsgListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
    // Item selecting is not changed yet, must post a message to msg queue for handle
    PostMessage( WM_LIST_ITEM_SELECTING_CHANGED );

    AutoSortListCtrl::OnLButtonDown(nFlags, point);
}

void PaDVAMsgListCtrl::OnKeydown(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMLVKEYDOWN pLVKeyDown = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);

    *pResult = 0;

    if ( NULL == pLVKeyDown )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return;
    }

    if (( VK_UP != pLVKeyDown->wVKey) && ( VK_DOWN != pLVKeyDown->wVKey ))
    {
        return;
    }

    // Item selecting is not changed yet, must post a message to msg queue for handle
    PostMessage( WM_LIST_ITEM_SELECTING_CHANGED );
}

LRESULT PaDVAMsgListCtrl::OnUpdateSelectedItem( WPARAM wParam, LPARAM lParam )
{
    POSITION pPos = GetFirstSelectedItemPosition();
    if ( NULL == pPos )
    {
        m_refBroadcastModel.clearCurSelDVAMsg( m_nDVAMsgType, true );
        return 0;
    }

    int nSeledItem = GetNextSelectedItem( pPos );
    m_refBroadcastModel.updateCurSelDVAMsg( m_nDVAMsgType, static_cast<unsigned long>(GetItemData( nSeledItem )), true );

    return 0;
}

void PaDVAMsgListCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{
    CPoint ptHitTest = point;
    ScreenToClient( &ptHitTest );
    int nCol = HitTest( ptHitTest );
    if ( LIST_NO_SEL == nCol )
    {
        // Only popup on DVA message item
        return;
    }

    unsigned long ulKeyOfMsgClicked = static_cast<unsigned long>(GetItemData( nCol ));
    if ( INVALID_DVA_MSG_KEY == ulKeyOfMsgClicked )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_DVA_MSG_NOT_EXIST );
        return;
    }

    if ( NULL == m_pRootConfigMenu )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return;
    }

    CMenu* pConfigMenu = m_pRootConfigMenu->GetSubMenu( 0 );
    if ( NULL == pConfigMenu )
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_UNEXPECTED_NULL_POINTER );
        return;
    }

    int nCmd = static_cast<int>(pConfigMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD, point.x, point.y, pWnd ));
    
	//Process the menu opertion to add playlist.
	if (ID_DVA_INSERT_BROADCAST_LIST == nCmd)
	{
		m_refBroadcastModel.addToPlayDVAMsgList(m_nDVAMsgType,ulKeyOfMsgClicked);
		return;
	}
	
	unsigned long ulShortcutIndex = INVALID_DVA_MSG_SHORTCUT_INDEX;
    switch ( nCmd )
    {
        case ID_DVA_SHORTCUT_A:
        {
            ulShortcutIndex = 1u;
            break;
        }
        case ID_DVA_SHORTCUT_B:
        {
            ulShortcutIndex = 2u;
            break;
        }
        case ID_DVA_SHORTCUT_C:
        {
            ulShortcutIndex = 3u;
            break;
        }
        case ID_DVA_SHORTCUT_D:
        {
            ulShortcutIndex = 4u;
            break;
        }
        case ID_DVA_SHORTCUT_E:
        {
            ulShortcutIndex = 5u;
            break;
        }
        case ID_DVA_SHORTCUT_F:
        {
            ulShortcutIndex = 6u;
            break;
        }
        case ID_DVA_SHORTCUT_G:
        {
            ulShortcutIndex = 7u;
            break;
        }
        case ID_DVA_SHORTCUT_H:
        {
            ulShortcutIndex = 8u;
            break;
        }
        case ID_DVA_SHORTCUT_I:
        {
            ulShortcutIndex = 9u;
            break;
        }
        case ID_DVA_SHORTCUT_J:
        {
            ulShortcutIndex = 10u;
            break;
        }
		case ID_DVA_SHORTCUT_K:
		{
			ulShortcutIndex = 11u;
			break;
		}
		case ID_DVA_SHORTCUT_L:
		{
			ulShortcutIndex = 12u;
			break;
		}
		case ID_DVA_SHORTCUT_M:
		{
			ulShortcutIndex = 13u;
			break;
		}
		case ID_DVA_SHORTCUT_N:
		{
			ulShortcutIndex = 14u;
			break;
		}
		case ID_DVA_SHORTCUT_O:
		{
			ulShortcutIndex = 15u;
			break;
		}
		case ID_DVA_SHORTCUT_P:
		{
			ulShortcutIndex = 16u;
			break;
		}
		case ID_DVA_SHORTCUT_Q:
		{
			ulShortcutIndex = 17u;
			break;
		}
		case ID_DVA_SHORTCUT_R:
		{
			ulShortcutIndex = 18u;
			break;
		}
		case ID_DVA_SHORTCUT_S:
		{
			ulShortcutIndex = 19u;
			break;
		}
		case ID_DVA_SHORTCUT_T:
		{
			ulShortcutIndex = 20u;
			break;
		}
		case ID_DVA_SHORTCUT_U:
		{
			ulShortcutIndex = 21u;
			break;
		}
		case ID_DVA_SHORTCUT_V:
		{
			ulShortcutIndex = 22u;
			break;
		}
		case ID_DVA_SHORTCUT_W:
		{
			ulShortcutIndex = 23u;
			break;
		}
		case ID_DVA_SHORTCUT_X:
		{
			ulShortcutIndex = 24u;
			break;
		}
		case ID_DVA_SHORTCUT_Y:
		{
			ulShortcutIndex = 25u;
			break;
		}
		case ID_DVA_SHORTCUT_Z:
		{
			ulShortcutIndex = 26u;
			break;
		}
		case ID_DVA_INSERT_BROADCAST_LIST:
		{
			ulShortcutIndex = 26u;
			break;
		}		
        default:
        {
            break;
        }
    }

    PaDataManager::getInstance().modifyDVAMsgShortcut( ulShortcutIndex, ulKeyOfMsgClicked );
}

void PaDVAMsgListCtrl::updateList( const std::string& strFilterWord )
{
    DvaMsgKeyToDVAMsgPtrMap mapDVAMsgs;
    DVAMsgInfo* pCulSelInfo = NULL;
    try
    {
        PaDataManager::getInstance().getDVAMsgs( strFilterWord, m_nDVAMsgType, mapDVAMsgs );
        pCulSelInfo = m_refBroadcastModel.getCurSelDVAMsg( m_nDVAMsgType );
    }
    catch (...)
    {
        PaErrorHelper::logErrorMsg( SourceInfo, ERROR_CATCHED_UNKNOWN_EXCEPTION );
    	return;
    }

    DeleteAllItems();

    bool bSelItemExist = false;
    for ( DvaMsgKeyToDVAMsgPtrMap::iterator itLoop = mapDVAMsgs.begin();
          mapDVAMsgs.end() != itLoop; ++itLoop )
    {
        DVAMsgInfo* pDVAMsg = itLoop->second;
        if ( NULL == pDVAMsg )
        {
            continue;
        }
        int nNewPos = InsertItem( 0, pDVAMsg->strID.c_str() );
        SetItemText( nNewPos, DVA_MSG_LIST_NAME_INDEX, pDVAMsg->strName.c_str() );
        SetItemData( nNewPos, static_cast<DWORD>(itLoop->first) );
        if ( pCulSelInfo == pDVAMsg )
        {
            SetItemState( nNewPos, LVIS_SELECTED, LVIS_SELECTED );
            bSelItemExist = true;
        }
    }

    if ( !bSelItemExist )
    {
        m_refBroadcastModel.clearCurSelDVAMsg( m_nDVAMsgType );
    }
}

bool PaDVAMsgListCtrl::selQuickBroadcastItem( WPARAM wParam )
{
    // wParam is the pkey of PA_DVA_MESSAGE
    DWORD dwData = static_cast<DWORD>(wParam);
    if ( NULL == dwData )
    {
        return true;
    }

    // The data is not found in the list, the list is filtered.
    if ( !selectItemByItemData( dwData ))
    {
        return false;
    }

    return true;
}

bool PaDVAMsgListCtrl::selectItemByItemData( DWORD dwData ) 
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
            EnsureVisible( nLoop, FALSE );
            m_refBroadcastModel.updateCurSelDVAMsg( m_nDVAMsgType, static_cast<unsigned long>(dwData), true );
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
