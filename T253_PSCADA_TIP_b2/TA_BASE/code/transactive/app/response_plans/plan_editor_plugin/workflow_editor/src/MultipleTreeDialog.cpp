// src/MultipleListDialog.cpp : implementation file
//

#include "stdafx.h"

#include <map>
#include <set>

#include "resource.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "FontHelper.h"
#include "MultipleTreeDialog.h"


// MultipleListDialog dialog

IMPLEMENT_DYNAMIC(MultipleTreeDialog, CDialog)

MultipleTreeDialog::MultipleTreeDialog( ITreePicker& refPicker, const std::string& strName, const std::vector<TreeItem>& vecSourceItems, std::vector<unsigned int> vecSelected, const unsigned int unMaxSelection, CWnd* pParent, const int nRow ) :
CDialog( IDD_MULTIPLE_TREE_DIALOG, pParent ),
Title( "" ),
DisplayValue( "" ),
ListUpdated( false ),
m_nRow( nRow ),
m_strName( strName.c_str() ),
m_unMaxSelection( unMaxSelection ),
m_vecSourceItems( vecSourceItems ),
m_vecSelectedItems( vecSelected ),
m_refTreePicker( refPicker )
{
}

MultipleTreeDialog::~MultipleTreeDialog()
{
}

void MultipleTreeDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_TREE_SOURCE_POOL, m_treSourcePool );
    DDX_Control( pDX, IDC_LIST_SELECTION_POOL, m_lstSelectionPool );
    DDX_Control( pDX, IDC_BTN_SRC_SEL_ALL, m_btnSrcSelectAll );
}

BEGIN_MESSAGE_MAP( MultipleTreeDialog, CDialog )
    ON_BN_CLICKED( IDOK, OnOK )
    ON_BN_CLICKED( IDC_BTN_SELECT, OnSelectItems )
    ON_BN_CLICKED( IDC_BTN_SRC_SEL_ALL, OnSrcSelAll )
    ON_LBN_SELCHANGE( IDC_LIST_SELECTION_POOL, OnSelectionPoolSelChanged )
    ON_LBN_SELCHANGE( IDC_LIST_SOURCE_POOL, OnSourcePoolSelChanged )
    ON_NOTIFY( TVN_ITEMEXPANDING, IDC_TREE_SOURCE_POOL, OnTreeSourceExpanding )
END_MESSAGE_MAP()

// MultipleListDialog message handlers

BOOL MultipleTreeDialog::OnInitDialog( )
{
    CDialog::OnInitDialog();

    SetWindowText( Title.c_str() );
    InitTreeItems();

    FontHelper::adjustChildrenFont( GetSafeHwnd() );

    return FALSE;
}

void MultipleTreeDialog::OnSelectionPoolSelChanged()
{
    int nChecked = ( m_lstSelectionPool.GetSelCount() == m_lstSelectionPool.GetCount() ) ? BST_CHECKED : BST_UNCHECKED;
}

void MultipleTreeDialog::OnSourcePoolSelChanged()
{
    int nChecked = ( false ) ? BST_CHECKED : BST_UNCHECKED;

    m_btnSrcSelectAll.SetCheck( nChecked );
}

void MultipleTreeDialog::OnSelectItems()
{
    m_lstSelectionPool.SelItemRange( FALSE, 0, m_lstSelectionPool.GetCount() );

    //const int nSelCount = m_treSourcePool.GetSelCount();
    //LPINT arrSel = new int[nSelCount];
    //m_treSourcePool.GetSelItems( nSelCount, arrSel );

    //int nIndex = 0;
    //for ( int nLoop = nSelCount - 1; nLoop >= 0; --nLoop )
    //{
    //    nIndex = arrSel[nLoop];
    //    SelectedItems.push_back( static_cast<unsigned int>(m_treSourcePool.GetItemData( nIndex )) );

    //    CString csSel = "";
    //    m_treSourcePool.GetText( nIndex, csSel );
    //    DWORD_PTR dwData = m_treSourcePool.GetItemData( nIndex );
    //    m_treSourcePool.DeleteString( nIndex );

    //    nIndex = m_lstSelectionPool.AddString( csSel );
    //    m_lstSelectionPool.SetItemData( nIndex, dwData );
    //    m_lstSelectionPool.SetSel( nIndex );
    //}

    //InitListItems();

    //delete[] arrSel;
}


void MultipleTreeDialog::OnSrcSelAll()
{
    BOOL bSelect = (m_btnSrcSelectAll.GetCheck() == BST_CHECKED) ? TRUE : FALSE;

    //m_treSourcePool.SelItemRange( bSelect, 0, m_treSourcePool.GetCount() );
}

void MultipleTreeDialog::InitTreeItems()
{
    m_treSourcePool.DeleteAllItems();
    m_lstSelectionPool.ResetContent();

    int nIndex = 0;
    std::map<unsigned int, int> mapSelected;

    int nSelIndex = 0;
    for ( std::vector<unsigned int>::const_iterator itSel = m_vecSelectedItems.begin(); m_vecSelectedItems.end() != itSel; ++itSel )
    {
        mapSelected.insert( std::map<unsigned int, int>::value_type( *itSel, nSelIndex++ ) );
    }

    HTREEITEM hIndex = NULL;

    std::map<unsigned int, HTREEITEM> mapCurLevelItems;

    for ( std::vector<TreeItem>::const_iterator itLoop = m_vecSourceItems.begin(); m_vecSourceItems.end() != itLoop; ++itLoop )
    {
        // Find and insert root item whose parent id is itself first
        if ( itLoop->ID != itLoop->ParentID )
        {
            continue;
        }

        hIndex = InsertItem( m_treSourcePool, itLoop->Item.c_str(), itLoop->HasChild, NULL );
        m_treSourcePool.SetItemData( hIndex, itLoop->ID );

        std::map<unsigned int, int>::iterator itSelected = mapSelected.find( itLoop->ID );
        if ( mapSelected.end() != itSelected )
        {
            m_treSourcePool.SetItemState( hIndex, TVIS_SELECTED, TVIS_SELECTED );

            nIndex = 0;
            for ( int nLoop = 0; nLoop < m_lstSelectionPool.GetCount(); ++nLoop )
            {
                unsigned int unId = static_cast<unsigned int>(m_lstSelectionPool.GetItemData( nLoop ));
                std::map<unsigned int, int>::iterator itSelLoop = mapSelected.find( unId );
                if ( itSelLoop == mapSelected.end() )
                {
                    continue;
                }

                if ( itSelLoop->second < itSelected->second )
                {
                    nIndex = nLoop;
                    break;
                }
            }

            nIndex = m_lstSelectionPool.InsertString( nIndex, itLoop->Item.c_str() );
            m_lstSelectionPool.SetItemData( nIndex, itLoop->ID );
        }

        if ( itLoop->HasChild )
        {
            mapCurLevelItems.insert( std::map<unsigned int, HTREEITEM>::value_type(itLoop->ID, hIndex) );
        }
    }

    while ( !mapCurLevelItems.empty() )
    {
        std::map<unsigned int, HTREEITEM> mapNextLevelItems;
        for ( std::vector<TreeItem>::const_iterator itLoop = m_vecSourceItems.begin(); m_vecSourceItems.end() != itLoop; ++itLoop )
        {
            // Skip root item
            if ( itLoop->ID == itLoop->ParentID )
            {
                continue;
            }

            // Insert items whose parent is in current level
            std::map<unsigned int, HTREEITEM>::iterator itParent = mapCurLevelItems.find( itLoop->ParentID );
            if ( mapCurLevelItems.end() == itParent )
            {
                continue;
            }

            hIndex = InsertItem( m_treSourcePool, itLoop->Item.c_str( ), itLoop->HasChild, itParent->second );
            m_treSourcePool.SetItemData( hIndex, itLoop->ID );

            std::map<unsigned int, int>::iterator itSelected = mapSelected.find( itLoop->ID );
            if ( mapSelected.end() != itSelected )
            {
                m_treSourcePool.SetItemState( hIndex, TVIS_SELECTED, TVIS_SELECTED );

                nIndex = 0;
                for ( int nLoop = 0; nLoop < m_lstSelectionPool.GetCount(); ++nLoop )
                {
                    unsigned int unId = static_cast<unsigned int>(m_lstSelectionPool.GetItemData( nLoop ));
                    std::map<unsigned int, int>::iterator itSelLoop = mapSelected.find( unId );
                    if ( itSelLoop == mapSelected.end() )
                    {
                        continue;
                    }

                    if ( itSelLoop->second < itSelected->second )
                    {
                        nIndex = nLoop;
                        break;
                    }
                }

                nIndex = m_lstSelectionPool.InsertString( nIndex, itLoop->Item.c_str() );
                m_lstSelectionPool.SetItemData( nIndex, itLoop->ID );
            }

            if ( itLoop->HasChild )
            {
                mapNextLevelItems.insert( std::map<unsigned int, HTREEITEM>::value_type(itLoop->ID, hIndex) );
            }
        }

        mapCurLevelItems.clear();

        mapCurLevelItems = mapNextLevelItems;
    }
}

void MultipleTreeDialog::OnOK()
{
    ListUpdated = m_refTreePicker.pickFromTree( m_strName, m_vecSelectedItems, DisplayValue, m_nRow );

    CDialog::OnOK();
}

void MultipleTreeDialog::OnTreeSourceExpanding( NMHDR *pNMHDR, LRESULT *pResult )
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

    *pResult = 0;

    if ( NULL != m_treSourcePool.GetChildItem( pNMTreeView->itemNew.hItem ) )
    {
        // Children inserted, no need to populate children again
        return;
    }

    const unsigned int unNodeId = static_cast<unsigned int>(m_treSourcePool.GetItemData( pNMTreeView->itemNew.hItem ));

    std::vector<TreeItem> vecChildItems;
    m_refTreePicker.getChildItems( m_strName, vecChildItems, unNodeId );

    HTREEITEM hIndex = NULL;

    for ( std::vector<TreeItem>::const_iterator itLoop = vecChildItems.begin(); vecChildItems.end() != itLoop; ++itLoop )
    {
        // Skip root item
        if ( itLoop->ID == itLoop->ParentID )
        {
            continue;
        }

        // Skip item whose parent is not this node
        if ( itLoop->ParentID != unNodeId )
        {
            continue;
        }

        hIndex = InsertItem( m_treSourcePool, itLoop->Item.c_str(), itLoop->HasChild, pNMTreeView->itemNew.hItem );
        m_treSourcePool.SetItemData( hIndex, itLoop->ID );

        if ( !m_vecSelectedItems.empty() && (m_vecSelectedItems.front() == itLoop->ID) )
        {
            m_treSourcePool.SelectItem( hIndex );
        }
    }
}

HTREEITEM MultipleTreeDialog::InsertItem( CTreeCtrl& refTree, const std::string& strName, const bool bHasChild, HTREEITEM hParent /*= NULL */ )
{
    TVINSERTSTRUCT tvis;

    tvis.hParent = hParent;
    tvis.hInsertAfter = (NULL == hParent) ? TVI_LAST : TVI_ROOT;
    tvis.item.mask = TVIF_TEXT | TVIF_CHILDREN;
    tvis.item.pszText = new char[strName.length() + 1];
    tvis.item.pszText[strName.length()] = '\0';
    memcpy( tvis.item.pszText, strName.c_str(), strName.length() );
    tvis.item.iImage = 0;
    tvis.item.iSelectedImage = 0;
    tvis.item.cChildren = bHasChild ? 1 : 0;
    tvis.item.lParam = NULL;

    return refTree.InsertItem( &tvis );
}
