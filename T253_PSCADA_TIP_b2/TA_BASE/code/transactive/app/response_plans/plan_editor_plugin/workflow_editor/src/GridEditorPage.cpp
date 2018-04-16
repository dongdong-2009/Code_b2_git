// src/GridEditorPage.cpp : implementation file
//

#include "stdafx.h"

#include "resource.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "TextEditDialog.h"
#include "GridEditorPage.h"
#include "ListPickerDialog.h"
#include "TreePickerDialog.h"
#include "GridEditorDialog.h"
#include "FontHelper.h"
#include "GridEditorPage.h"


// GridEditorPage dialog

IMPLEMENT_DYNAMIC(GridEditorPage, CDialog)

GridEditorPage::GridEditorPage( IGridEditor& refEditor, const std::string& strName, CWnd* pParent ) :
CDialog( IDD_GRID_EDITOR_FORM, pParent ),
DisplayValue( "" ),
m_strName( strName.c_str() ),
m_vecSourceItems(),
m_vecGridColumns(),
m_mapColNames(),
m_refEditor( refEditor ),
m_bRowFixed( false ),
m_bForcedRowEditable( false )
{
}

GridEditorPage::~GridEditorPage()
{
}

void GridEditorPage::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange( pDX );
    DDX_Control( pDX, IDC_GRID_DETAILS, m_lcAttributes );
}

BEGIN_MESSAGE_MAP(GridEditorPage, CDialog)
    ON_WM_DESTROY()
	ON_NOTIFY(NM_CLICK, IDC_GRID_DETAILS, OnListCtrlClick)
END_MESSAGE_MAP()

// GridEditorPage message handlers

BOOL GridEditorPage::OnInitDialog()
{
    CDialog::OnInitDialog();

	CBitmap* maskImage = NULL;
	m_imageList.Create(16, 16, ILC_COLOR4, 0, 1);
	m_bitMapAdd.LoadBitmap(IDB_BITMAP_DELETE);
	m_imageList.Add(&m_bitMapAdd, maskImage);
	m_bitMapDelete.LoadBitmap(IDB_BITMAP_ADD);
	m_imageList.Add(&m_bitMapDelete, maskImage);

    InitGridItems();

    FontHelper::adjustChildrenFont(GetSafeHwnd());


    return FALSE;
}

void GridEditorPage::OnDestroy()
{
    if ( NULL != m_imageList.GetSafeHandle() )
    {
        m_imageList.DeleteImageList();
    }

    CDialog::OnDestroy();
}

void GridEditorPage::createWnd( CWnd& refParent, const bool bForcedRowEditable )
{
    m_bForcedRowEditable = bForcedRowEditable;

    Create( IDD_GRID_EDITOR_FORM, &refParent );
}

void GridEditorPage::InitGridItems()
{
	m_vecGridColumns.clear();
	m_vecSourceItems.clear();
    m_refEditor.getGridItems( m_strName, m_vecGridColumns, m_vecSourceItems, m_bRowFixed );

    if ( m_bForcedRowEditable && m_bRowFixed )
    {
        m_bRowFixed = false;
    }

	DWORD listExtendStyle = m_lcAttributes.GetExtendedStyle();
	if(m_bRowFixed)
	{
		if((listExtendStyle & LVS_EX_SUBITEMIMAGES) != 0)
		{
			m_lcAttributes.SetExtendedStyle(listExtendStyle & (~LVS_EX_SUBITEMIMAGES));
		}
		if(m_lcAttributes.GetImageList(LVSIL_SMALL) != NULL)
		{
			m_lcAttributes.SetImageList(NULL, LVSIL_SMALL);;
		}
	}
	else
	{
		if((listExtendStyle & LVS_EX_SUBITEMIMAGES) == 0)
		{
			m_lcAttributes.SetExtendedStyle(listExtendStyle | LVS_EX_SUBITEMIMAGES);
		}
		if(m_lcAttributes.GetImageList(LVSIL_SMALL) != &m_imageList)
		{
			m_lcAttributes.SetImageList(&m_imageList, LVSIL_SMALL);;
		}
	}

    m_lcAttributes.AttachOwner( this );
    m_lcAttributes.SetGroupByColumn( true );
    m_lcAttributes.SetId( 0 );

    m_mapColNames.clear();
    m_lcAttributes.DeleteAllItems();
    while ( m_lcAttributes.DeleteColumn( 0 ))
    {
    }

	// Buttons column
	int nCol = 0;
	if(!m_vecGridColumns.empty() && !m_bRowFixed)
	{
		m_lcAttributes.InsertColumn(0, "", LVCFMT_CENTER, 40);
		nCol += 1;
	}

    for ( std::vector<GridColumn>::const_iterator itLoop = m_vecGridColumns.begin( ); m_vecGridColumns.end() != itLoop; ++itLoop )
    {
        m_lcAttributes.InsertColumn( nCol, itLoop->DisplayName.c_str(), LVCFMT_CENTER, itLoop->DisplayName.length() * 20 );
        m_lcAttributes.SetEditType( nCol, itLoop->Type );
        m_mapColNames.insert( std::map<int, std::string>::value_type(nCol, itLoop->Name) );
        ++nCol;
    }

    const int nColCount = m_lcAttributes.GetHeaderCtrl()->GetItemCount();
	int colAdjust = (m_bRowFixed ? 0 : 1);

    for ( std::vector<GridItem>::const_iterator itLoop = m_vecSourceItems.begin(); m_vecSourceItems.end() != itLoop; ++itLoop )
    {
        if (( m_lcAttributes.GetItemCount() < itLoop->Row ) || ( nColCount <= itLoop->Column + colAdjust))
        {
            continue;
        }
        
        if ( itLoop->Row == m_lcAttributes.GetItemCount() )
        {
			if(m_bRowFixed)
			{
                m_lcAttributes.InsertItem( itLoop->Row, "");
			}
			else
			{
                m_lcAttributes.InsertItem( itLoop->Row, "", 0 );
			}
        }

        m_lcAttributes.SetItemText( itLoop->Row, itLoop->Column + colAdjust, itLoop->Item.c_str() );
    }

	if(!m_vecGridColumns.empty() && !m_bRowFixed)
	{
		insertAddIcon();
	}
}

void GridEditorPage::valueChanged( const int nId, const int nRow, const int nCol, const std::string& strDisplayValue )
{
	int colAdjust = (m_bRowFixed ? 0 : 1);
    if ( static_cast<size_t>(nCol) >= m_vecGridColumns.size() + colAdjust)
    {
        return;
    }

    std::string strName = m_vecGridColumns[nCol - colAdjust].Name;
    bool bValid = false;
    IStringEditor* pStringEditor = m_refEditor.getStringEditor( strName );
    bool bListUpdated = false;

    if ( NULL != pStringEditor )
    {
        bListUpdated = pStringEditor->updateValue( strName, strDisplayValue, bValid, nRow );
    }

    if ( bListUpdated )
    {
        InitGridItems();
    }
}

void GridEditorPage::booleanValueChanged( const int nId, const int nRow, const int nCol, const bool bNewValue, std::string& strDisplayValue )
{
	int colAdjust = (m_bRowFixed ? 0 : 1);
    if ( static_cast<size_t>(nCol) >= m_vecGridColumns.size() + colAdjust)
    {
        return;
    }

    std::string strName = m_vecGridColumns[nCol - colAdjust].Name;
    bool bValid = false;
    IBoolEditor* pBoolEditor = m_refEditor.getBoolEditor( strName );
    bool bListUpdated = false;

    if ( NULL != pBoolEditor )
    {
        bListUpdated = pBoolEditor->updateBooleanValue( strName, bNewValue, strDisplayValue, bValid, nRow );
    }

    if ( bListUpdated )
    {
        InitGridItems();
    }
}

void GridEditorPage::timeValueChanged( const int nId, const int nRow, const int nCol, const time_t tmNewValue, std::string& strDisplayValue )
{
	int colAdjust = (m_bRowFixed ? 0 : 1);
    if ( static_cast<size_t>(nCol) >= m_vecGridColumns.size() + colAdjust )
    {
        return;
    }

    std::string strName = m_vecGridColumns[nCol - colAdjust].Name;
    bool bValid = false;
    ITimeEditor* pTimeEditor = m_refEditor.getTimeEditor( strName );
    bool bListUpdated = false;

    if ( NULL != pTimeEditor )
    {
        bListUpdated = pTimeEditor->updateTimeValue( strName, tmNewValue, strDisplayValue, bValid, nRow );
    }

    if ( bListUpdated )
    {
        InitGridItems();
    }
}

void GridEditorPage::getBooleanValue( const int nId, const int nRow, const int nCol, bool& bCurValue )
{
	int colAdjust = (m_bRowFixed ? 0 : 1);
    if ( static_cast<size_t>(nCol) >= m_vecGridColumns.size() + colAdjust )
    {
        return;
    }

    std::string strName = m_vecGridColumns[nCol - colAdjust].Name;
    IBoolEditor* pBoolEditor = m_refEditor.getBoolEditor( strName );

    if ( NULL != pBoolEditor )
    {
        pBoolEditor->getBooleanValue( strName, bCurValue, nRow );
    }
}

void GridEditorPage::getTimeValue( const int nId, const int nRow, const int nCol, time_t& tmCurValue )
{
	int colAdjust = (m_bRowFixed ? 0 : 1);
    if ( static_cast<size_t>(nCol) >= m_vecGridColumns.size() + colAdjust )
    {
        return;
    }

    std::string strName = m_vecGridColumns[nCol - colAdjust].Name;
    ITimeEditor* pTimeEditor = m_refEditor.getTimeEditor( strName );

    if ( NULL != pTimeEditor )
    {
        pTimeEditor->getTimeValue( strName, tmCurValue, nRow );
    }
}

void GridEditorPage::getStringValue( const int nId, const int nRow, const int nCol, std::string& strCurValue, size_t& szMaxLength )
{
	int colAdjust = (m_bRowFixed ? 0 : 1);
    if ( static_cast<size_t>(nCol) >= m_vecGridColumns.size() + colAdjust )
    {
        return;
    }

    std::string strName = m_vecGridColumns[nCol - colAdjust].Name;
    IStringEditor* pStringEditor = m_refEditor.getStringEditor( strName );

    if ( NULL != pStringEditor )
    {
        pStringEditor->getValue( strName, strCurValue, szMaxLength, nRow );
    }
}

void GridEditorPage::popupList( const int nId, const int nRow, const int nCol )
{
	int colAdjust = (m_bRowFixed ? 0 : 1);
    if ( static_cast<size_t>(nCol) >= m_vecGridColumns.size() + colAdjust)
    {
        return;
    }

    std::string strName = m_vecGridColumns[nCol - colAdjust].Name;

    IListPicker* pListPicker = m_refEditor.getListPicker( strName );

    if ( NULL == pListPicker )
    {
        return;
    }

    ListPickerDialog dlgPopup( *pListPicker, strName, this, nRow );

    dlgPopup.Title = m_vecGridColumns[nCol - colAdjust].DisplayName;

    int nAction = dlgPopup.DoModal();

    if ( IDOK == nAction )
    {
        m_lcAttributes.SetItemText( nRow, nCol, dlgPopup.DisplayValue.c_str() );
        m_lcAttributes.StopEdit();

        if ( dlgPopup.ListUpdated )
        {
            InitGridItems();
        }
    }
}

void GridEditorPage::popupTree( const int nId, const int nRow, const int nCol )
{
	int colAdjust = (m_bRowFixed ? 0 : 1);
    if ( static_cast<size_t>(nCol) >= m_vecGridColumns.size() + colAdjust )
    {
        return;
    }

    std::string strName = m_vecGridColumns[nCol - colAdjust].Name;

    ITreePicker* pTreePicker = m_refEditor.getTreePicker( strName );

    if ( NULL == pTreePicker )
    {
        return;
    }

    TreePickerDialog dlgPopup( *pTreePicker, strName, this, nRow );

    dlgPopup.Title = m_vecGridColumns[nCol - colAdjust].DisplayName;

    int nAction = dlgPopup.DoModal();

    if ( IDOK == nAction )
    {
        m_lcAttributes.SetItemText( nRow, nCol, dlgPopup.DisplayValue.c_str() );
        m_lcAttributes.StopEdit();

        if ( dlgPopup.ListUpdated )
        {
            InitGridItems();
        }
    }
}

void GridEditorPage::popupGrid( const int nId, const int nRow, const int nCol )
{
	int colAdjust = (m_bRowFixed ? 0 : 1);
    if ( static_cast<size_t>(nCol) >= m_vecGridColumns.size() + colAdjust )
    {
        return;
    }

    std::string strName = m_vecGridColumns[nCol - colAdjust].Name;

    IGridEditor* pGridEditor = m_refEditor.getGridEditor( strName );

    if ( NULL == pGridEditor )
    {
        return;
    }

    GridEditorDialog dlgPopup( *pGridEditor, strName, this );

    dlgPopup.Title = m_vecGridColumns[nCol - colAdjust].DisplayName;

    int nAction = dlgPopup.DoModal();

    if ( IDOK == nAction )
    {
        m_lcAttributes.StopEdit();
    }
}

void GridEditorPage::popupText( const int nId, const int nRow, const int nCol )
{
	int colAdjust = (m_bRowFixed ? 0 : 1);
    TextEditDialog dlgPopup( this );

    dlgPopup.Title = m_vecGridColumns[nCol - colAdjust].DisplayName;
    dlgPopup.Content = m_lcAttributes.GetItemText( nRow, nCol ).operator LPCSTR();

    int nAction = dlgPopup.DoModal();

    if ( IDOK == nAction )
    {
        bool bValid = false;
        if ( static_cast<size_t>(nCol) >= m_vecGridColumns.size() + colAdjust )
        {
            return;
        }

        std::string strName = m_vecGridColumns[nCol - colAdjust].Name;

        IStringEditor* pStringEditor = m_refEditor.getStringEditor( strName );
        bool bListUpdated = false;

        if ( NULL != pStringEditor )
        {
            bListUpdated = pStringEditor->updateValue( strName, dlgPopup.Content, bValid, nRow );
        }

        m_lcAttributes.SetItemText( nRow, nCol, dlgPopup.Content.c_str() );
        m_lcAttributes.StopEdit();

        if ( bListUpdated )
        {
            InitGridItems();
        }
    }
}

void GridEditorPage::OnOK()
{
}

void GridEditorPage::OnCancel()
{
}

void GridEditorPage::insertAddIcon()
{
	LVITEM lvItem = {0};
	lvItem.iItem = m_lcAttributes.GetItemCount();
	lvItem.iSubItem = 0;
	lvItem.iImage = 1;
	lvItem.pszText = "";
	lvItem.mask = LVIF_TEXT | LVIF_IMAGE;
	m_lcAttributes.InsertItem(&lvItem);
}

void GridEditorPage::OnListCtrlClick( NMHDR* pNMHDR, LRESULT* pResult )
{
	if(m_bRowFixed)
	{
		return;
	}
    // Get cell coordinates
    LVHITTESTINFO hitTestInfo;
    ::GetCursorPos( &hitTestInfo.pt );
    m_lcAttributes.ScreenToClient( &hitTestInfo.pt );
    m_lcAttributes.SubItemHitTest( &hitTestInfo );

    int nRow = hitTestInfo.iItem;
    int nCol = hitTestInfo.iSubItem;
	int nItemCount = m_lcAttributes.GetItemCount();

	if(nCol != 0 || nRow + 1 > nItemCount)
	{
		return;
	}

	if(!(hitTestInfo.flags & LVHT_ONITEMICON))
	{
		return;
	}

	if(nRow + 1 < nItemCount)
	{
		onDeleteIconClicked(nRow);
	}
	else
	{
		onAddIconClicked();
	}
}

void GridEditorPage::onDeleteIconClicked( const int row )
{
	int lastRow = m_lcAttributes.GetItemCount() - 1;

	m_lcAttributes.DeleteItem(row);

	bool needRefresh = m_refEditor.deleteRow(row);

	if(needRefresh || row + 1 < lastRow)
	{
		InitGridItems();
	}
	else
	{
		for(std::vector<GridItem>::iterator it = m_vecSourceItems.begin(); it != m_vecSourceItems.end();)
		{
			if((*it).Row == row)
			{
				it = m_vecSourceItems.erase(it);
			}
			else
			{
				++it;
			}
		}
	}
}

void GridEditorPage::onAddIconClicked()
{
	int lastRow = m_lcAttributes.GetItemCount() - 1;

	m_lcAttributes.DeleteItem(lastRow);

	std::vector<GridItem> newGridItems;
	int col = 0;
	for(std::vector<GridColumn>::iterator it = m_vecGridColumns.begin(); it != m_vecGridColumns.end(); ++it)
	{
		GridItem tmp = {lastRow, col++, ""};
		newGridItems.push_back(tmp);
	}

	if(!newGridItems.empty())
	{
        m_lcAttributes.InsertItem(newGridItems[0].Row, "", 0);
		for(std::vector<GridItem>::iterator it = newGridItems.begin(); it != newGridItems.end(); ++it)
		{
			m_lcAttributes.SetItemText((*it).Row, (*it).Column + 1, (*it).Item.c_str());
		}
		m_vecSourceItems.insert(m_vecSourceItems.end(), newGridItems.begin(), newGridItems.end());
		bool needRefresh = m_refEditor.newRow();
		if(needRefresh)
		{
			InitGridItems();
		}
		else
		{
			insertAddIcon();
		}
	}
}

bool GridEditorPage::isRowEditable( const int nRow )
{
	if(m_bRowFixed)
	{
		return true;
	}
	return nRow + 1 < m_lcAttributes.GetItemCount();
}
