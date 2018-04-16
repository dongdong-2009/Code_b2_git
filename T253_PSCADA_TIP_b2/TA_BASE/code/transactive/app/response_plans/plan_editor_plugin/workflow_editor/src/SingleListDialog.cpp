// src/SingleListDialog.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "FontHelper.h"
#include "SingleListDialog.h"


// SingleListDialog dialog

IMPLEMENT_DYNAMIC(SingleListDialog, CDialog)

SingleListDialog::SingleListDialog( IListPicker& refPicker, const std::string& strName, const std::vector<ListItem>& vecSourceItems, const std::vector<unsigned int>& vecSelected, CWnd* pParent, const int nRow ) :
CDialog( IDD_SINGLE_LIST_DIALOG, pParent ),
Title( "" ),
DisplayValue( "" ),
ListUpdated( false ),
m_nRow( nRow ),
m_strName( strName.c_str() ),
m_vecSourceItems( vecSourceItems ),
m_vecSelectedItems( vecSelected ),
m_refListPicker( refPicker )
{
}

SingleListDialog::~SingleListDialog()
{
}

void SingleListDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_LIST_SOURCE_POOL, m_lstSourcePool );
}


BEGIN_MESSAGE_MAP( SingleListDialog, CDialog )
    ON_BN_CLICKED( IDOK, OnOK )
    ON_LBN_SELCHANGE( IDC_LIST_SOURCE_POOL, OnSelectionChanged )
END_MESSAGE_MAP()


// SingleListDialog message handlers

BOOL SingleListDialog::OnInitDialog( )
{
    CDialog::OnInitDialog();

    SetWindowText( Title.c_str() );
    InitListItems();

    FontHelper::adjustChildrenFont( GetSafeHwnd() );

    return FALSE;
}

void SingleListDialog::InitListItems()
{
    int nIndex = 0;

    for ( std::vector<ListItem>::const_iterator itLoop = m_vecSourceItems.begin(); m_vecSourceItems.end() != itLoop; ++itLoop )
    {
        nIndex = m_lstSourcePool.AddString( itLoop->Item.c_str() );
        m_lstSourcePool.SetItemData( nIndex, itLoop->ID );

        if ( !m_vecSelectedItems.empty() && ( m_vecSelectedItems.front() == itLoop->ID ))
        {
            m_lstSourcePool.SetCurSel( nIndex );
        }
    }
}

void SingleListDialog::OnSelectionChanged()
{
    int nSel = m_lstSourcePool.GetCurSel();

    if ( nSel < 0 )
    {
        m_vecSelectedItems.clear();
        return;
    }

    m_vecSelectedItems.clear();
    m_vecSelectedItems.push_back( static_cast<unsigned int>(m_lstSourcePool.GetItemData( nSel )));
}

void SingleListDialog::OnOK()
{
    ListUpdated = m_refListPicker.pickFromList( m_strName, m_vecSelectedItems, DisplayValue, m_nRow );

    CDialog::OnOK();
}
