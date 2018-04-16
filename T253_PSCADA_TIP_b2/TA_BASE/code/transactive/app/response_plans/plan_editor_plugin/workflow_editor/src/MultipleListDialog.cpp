// src/MultipleListDialog.cpp : implementation file
//

#include "stdafx.h"

#include <map>
#include <set>

#include "resource.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "FontHelper.h"
#include "MultipleListDialog.h"


// MultipleListDialog dialog

IMPLEMENT_DYNAMIC(MultipleListDialog, CDialog)

MultipleListDialog::MultipleListDialog( IListPicker& refPicker, const std::string& strName, const std::vector<ListItem>& vecSourceItems, const std::vector<unsigned int>& vecSelected, const unsigned int unMaxSel, CWnd* pParent, const int nRow ) :
CDialog( IDD_MULTIPLE_LIST_DIALOG, pParent ),
Title( "" ),
DisplayValue( "" ),
ListUpdated( false ),
m_nRow( nRow ),
m_strName( strName.c_str() ),
m_unMaxSelection( unMaxSel ),
m_vecSourceItems( vecSourceItems ),
m_vecSelectedItems( vecSelected ),
m_refListPicker( refPicker )
{
}

MultipleListDialog::~MultipleListDialog()
{
}

void MultipleListDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_LIST_SOURCE_POOL, m_lstSourcePool );
    DDX_Control( pDX, IDC_LIST_SELECTION_POOL, m_lstSelectionPool );
    DDX_Control( pDX, IDC_BTN_SRC_SEL_ALL, m_btnSrcSelectAll );
    DDX_Control( pDX, IDC_BTN_DES_SEL_ALL, m_btnDesSelectAll );
}


BEGIN_MESSAGE_MAP( MultipleListDialog, CDialog )
    ON_BN_CLICKED( IDOK, OnOK )
    ON_BN_CLICKED( IDC_BTN_SELECT, OnSelectItems )
    ON_BN_CLICKED( IDC_BTN_DESELECT, OnDeSelectItems )
    ON_BN_CLICKED( IDC_BTN_SRC_SEL_ALL, OnSrcSelAll )
    ON_BN_CLICKED( IDC_BTN_DES_SEL_ALL, OnDesSelAll )
    ON_LBN_SELCHANGE( IDC_LIST_SELECTION_POOL, OnSelectionPoolSelChanged )
    ON_LBN_SELCHANGE( IDC_LIST_SOURCE_POOL, OnSourcePoolSelChanged )
END_MESSAGE_MAP()


// MultipleListDialog message handlers

BOOL MultipleListDialog::OnInitDialog( )
{
    CDialog::OnInitDialog();

    SetWindowText( Title.c_str() );
    InitListItems();

    FontHelper::adjustChildrenFont( GetSafeHwnd() );

    return FALSE;
}

void MultipleListDialog::OnSelectionPoolSelChanged()
{
    int nChecked = ( m_lstSelectionPool.GetSelCount() == m_lstSelectionPool.GetCount() ) ? BST_CHECKED : BST_UNCHECKED;

    m_btnDesSelectAll.SetCheck( nChecked );
}

void MultipleListDialog::OnSourcePoolSelChanged()
{
    int nChecked = ( m_lstSourcePool.GetSelCount() == m_lstSourcePool.GetCount() ) ? BST_CHECKED : BST_UNCHECKED;

    m_btnSrcSelectAll.SetCheck( nChecked );
}

void MultipleListDialog::OnSelectItems()
{
    m_lstSelectionPool.SelItemRange( FALSE, 0, m_lstSelectionPool.GetCount() );

    const int nSelCount = m_lstSourcePool.GetSelCount();
    LPINT arrSel = new int[nSelCount];
    m_lstSourcePool.GetSelItems( nSelCount, arrSel );

    int nIndex = 0;
    for ( int nLoop = nSelCount - 1; nLoop >= 0; --nLoop )
    {
        nIndex = arrSel[nLoop];
        m_vecSelectedItems.push_back( static_cast<unsigned int>(m_lstSourcePool.GetItemData( nIndex )) );

        CString csSel = "";
        m_lstSourcePool.GetText( nIndex, csSel );
        DWORD_PTR dwData = m_lstSourcePool.GetItemData( nIndex );
        m_lstSourcePool.DeleteString( nIndex );

        nIndex = m_lstSelectionPool.AddString( csSel );
        m_lstSelectionPool.SetItemData( nIndex, dwData );
        m_lstSelectionPool.SetSel( nIndex );
    }

    //InitListItems();

    delete[] arrSel;
}

void MultipleListDialog::OnDeSelectItems()
{
    m_lstSourcePool.SelItemRange( FALSE, 0, m_lstSelectionPool.GetCount() );

    const int nSelCount = m_lstSelectionPool.GetSelCount();
    LPINT arrSel = new int[nSelCount];
    m_lstSelectionPool.GetSelItems( nSelCount, arrSel );

    std::set<unsigned int> setToBeRemoved;

    int nIndex = 0;
    for ( int nLoop = nSelCount - 1; nLoop >= 0; --nLoop )
    {
        nIndex = arrSel[nLoop];
        setToBeRemoved.insert( static_cast<unsigned int>(m_lstSelectionPool.GetItemData( nIndex )));
    }

    for ( std::vector<unsigned int>::iterator itLoop = m_vecSelectedItems.begin(); m_vecSelectedItems.end() != itLoop; )
    {
        if ( setToBeRemoved.end() != setToBeRemoved.find( *itLoop ))
        {
            itLoop = m_vecSelectedItems.erase( itLoop );
        }
        else
        {
            ++itLoop;
        }
    }

    InitListItems();

    delete[] arrSel;
}

void MultipleListDialog::OnSrcSelAll()
{
    BOOL bSelect = (m_btnSrcSelectAll.GetCheck() == BST_CHECKED) ? TRUE : FALSE;

    m_lstSourcePool.SelItemRange( bSelect, 0, m_lstSourcePool.GetCount() );
}

void MultipleListDialog::OnDesSelAll()
{
    BOOL bSelect = (m_btnDesSelectAll.GetCheck() == BST_CHECKED) ? TRUE : FALSE;

    m_lstSelectionPool.SelItemRange( bSelect, 0, m_lstSelectionPool.GetCount() );
}

void MultipleListDialog::InitListItems()
{
    m_lstSourcePool.ResetContent();
    m_lstSelectionPool.ResetContent();

    int nIndex = 0;
    std::map<unsigned int, int> mapSelected;

    int nSelIndex = 0;
    for ( std::vector<unsigned int>::const_iterator itSel = m_vecSelectedItems.begin(); m_vecSelectedItems.end() != itSel; ++itSel )
    {
        mapSelected.insert( std::map<unsigned int, int>::value_type( *itSel, nSelIndex++ ) );
    }

    for ( std::vector<ListItem>::const_iterator itLoop = m_vecSourceItems.begin(); m_vecSourceItems.end() != itLoop; ++itLoop )
    {
        std::map<unsigned int, int>::iterator itSelected = mapSelected.find( itLoop->ID );
        if ( mapSelected.end() == itSelected )
        {
            nIndex = m_lstSourcePool.AddString( itLoop->Item.c_str() );
            m_lstSourcePool.SetItemData( nIndex, itLoop->ID );
        }
        else
        {
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

            nIndex = m_lstSelectionPool.InsertString( nIndex, itLoop->Item.c_str( ) );
            m_lstSelectionPool.SetItemData( nIndex, itLoop->ID );
        }
    }
}

void MultipleListDialog::OnOK()
{
    ListUpdated = m_refListPicker.pickFromList( m_strName, m_vecSelectedItems, DisplayValue, m_nRow );

    CDialog::OnOK();
}


