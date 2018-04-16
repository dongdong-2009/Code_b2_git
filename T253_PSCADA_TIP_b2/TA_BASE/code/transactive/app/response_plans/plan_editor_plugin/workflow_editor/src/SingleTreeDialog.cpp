// src/TreeSelectionDialog.cpp : implementation file
//

#include "stdafx.h"

#include <map>

#include "resource.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "FontHelper.h"
#include "SingleTreeDialog.h"


// TreeSelectionDialog dialog

IMPLEMENT_DYNAMIC( SingleTreeDialog, CDialog )

SingleTreeDialog::SingleTreeDialog( ITreePicker& refPicker, const std::string& strName, const std::vector<TreeItem>& vecSourceItems, std::vector<unsigned int> vecSelected, CWnd* pParent, const int nRow ) :
CDialog( IDD_SINGLE_TREE_DIALOG, pParent ),
Title( "" ),
DisplayValue( "" ),
ListUpdated( false ),
m_nRow( nRow ),
m_strName( strName.c_str() ),
m_vecSourceItems( vecSourceItems ),
m_vecSelectedItems( vecSelected ),
m_refTreePicker( refPicker )
{
}

SingleTreeDialog::~SingleTreeDialog()
{
}

void SingleTreeDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_TREE_SOURCE_POOL, m_treSourcePool );
}

BEGIN_MESSAGE_MAP( SingleTreeDialog, CDialog )
    ON_BN_CLICKED( IDOK, OnOK )
    ON_NOTIFY( TVN_ITEMEXPANDING, IDC_TREE_SOURCE_POOL, OnTreeSourceExpanding )
	ON_NOTIFY( TVN_SELCHANGED, IDC_TREE_SOURCE_POOL, OnTreeSelChanged)
END_MESSAGE_MAP()

// TreeSelectionDialog message handlers

BOOL SingleTreeDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

    SetWindowText( Title.c_str() );
    InitTreeItems();

    FontHelper::adjustChildrenFont( GetSafeHwnd() );

    return FALSE;
}

void SingleTreeDialog::InitTreeItems()
{
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

        if ( !m_vecSelectedItems.empty() && ( m_vecSelectedItems.front() == itLoop->ID ))
        {
            m_treSourcePool.SelectItem( hIndex );
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

            hIndex = InsertItem( m_treSourcePool, itLoop->Item.c_str(), itLoop->HasChild, itParent->second );
            m_treSourcePool.SetItemData( hIndex, itLoop->ID );

            if ( !m_vecSelectedItems.empty() && ( m_vecSelectedItems.front() == itLoop->ID ))
            {
                m_treSourcePool.SelectItem( hIndex );
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

void SingleTreeDialog::OnTreeSourceExpanding( NMHDR *pNMHDR, LRESULT *pResult )
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

void SingleTreeDialog::OnOK()
{
    ListUpdated = m_refTreePicker.pickFromTree( m_strName, m_vecSelectedItems, DisplayValue, m_nRow );

    CDialog::OnOK();
}

HTREEITEM SingleTreeDialog::InsertItem( CTreeCtrl& refTree, const std::string& strName, const bool bHasChild, HTREEITEM hParent )
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

void SingleTreeDialog::OnTreeSelChanged( NMHDR *pNMHDR, LRESULT *pResult )
{
	*pResult = 0;
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);

	m_vecSelectedItems.clear();
	m_vecSelectedItems.push_back(m_treSourcePool.GetItemData(pNMTreeView->itemNew.hItem));
}
