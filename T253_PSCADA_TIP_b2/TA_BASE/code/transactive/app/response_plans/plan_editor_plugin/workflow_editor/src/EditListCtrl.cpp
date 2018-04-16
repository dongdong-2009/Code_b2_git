// src/EditListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "app/response_plans/plan_step_plugin/common/src/ParameterType.h"
#include "EditListCtrl.h"


// EditListCtrl
const unsigned int ListEditId = 0x1001u;
const unsigned int ListCheckboxId = 0x1002u;
const unsigned int ListButtonId = 0x1003u;
const unsigned int ListDateTimeId = 0x1004u;

EditListCtrl::EditListCtrl() : CListCtrl(),
m_bGroupByColumn( false ),
m_mapNumToEditType(),
m_pCurEditWnd( NULL ),
m_nCurEditRow( -1 ),
m_nCurEditCol( -1 ),
m_pOwner( NULL )
{
}

EditListCtrl::~EditListCtrl()
{
}

BEGIN_MESSAGE_MAP( EditListCtrl, CListCtrl )
	ON_NOTIFY_REFLECT_EX( NM_CLICK, OnClick )
    ON_NOTIFY_REFLECT( LVN_DELETEALLITEMS, OnItemCleared )
    ON_NOTIFY_REFLECT( LVN_ITEMCHANGING, OnLvnItemchanging )
    ON_NOTIFY_EX( TTN_NEEDTEXT, 0, OnToolNeedText )
    ON_NOTIFY( NM_KILLFOCUS, ListDateTimeId, OnDateTimeLostFocus )
    ON_BN_CLICKED( ListButtonId, OnButtonClicked )
    ON_BN_CLICKED( ListCheckboxId, OnCheckBoxClicked )
    ON_EN_KILLFOCUS( ListEditId, OnEditLostFocus )
END_MESSAGE_MAP()

void EditListCtrl::OnLvnItemchanging( NMHDR *pNMHDR, LRESULT *pResult )
{
    // LVN_ITEMCHANGING notification handler
    LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

    // is the user selecting an item?
    if (( 0 != ( pNMLV->uChanged & LVIF_STATE )) && ( 0 != ( pNMLV->uNewState & LVNI_SELECTED )))
    {
        // yes - never allow a selected item
        *pResult = 1;
    }
    else
    {
        // no - allow any other change
        *pResult = 0;
    }
}

void EditListCtrl::OnItemCleared( NMHDR* pNMHDR, LRESULT* pResult )
{
    StopEdit();
}

BOOL EditListCtrl::OnClick( NMHDR* pNMHDR, LRESULT* pResult )
{
    // TODO: Add your control notification handler code here

    *pResult = 1; // suppress the default click action

    StopEdit();
    StartEdit();

	return FALSE;
}

void EditListCtrl::StartEdit()
{
    // Get cell coordinates
    LVHITTESTINFO hitTestInfo;
    ::GetCursorPos( &hitTestInfo.pt );
    ScreenToClient( &hitTestInfo.pt );
    SubItemHitTest( &hitTestInfo );

    int nRow = hitTestInfo.iItem;
    int nCol = hitTestInfo.iSubItem;

    if (( 0 == ( hitTestInfo.flags & LVHT_ONITEM )) || !EnsureVisible( nRow, FALSE ))
    {
        return;
    }

    if ( !m_bGroupByColumn && ( 0 == nCol ))
    {
        return;
    }

	if(NULL != m_pOwner && !m_pOwner->isRowEditable(nRow))
	{
		return;
	}

    std::map<int, int>::iterator itFound = m_mapNumToEditType.find( m_bGroupByColumn ? nCol : nRow );
    if ( m_mapNumToEditType.end() == itFound )
    {
        return;
    }

    CRect cellRect;
    GetSubItemRect( nRow, nCol, LVIR_LABEL, cellRect );

    std::string strCellValue = GetItemText( nRow, nCol ).operator LPCSTR();

    switch ( itFound->second )
    {
    case eBoolean:
    {
        CButton* pEditor = new CButton();
        cellRect.left = cellRect.right - 15;
        pEditor->Create( "", BS_AUTOCHECKBOX | WS_VISIBLE | WS_CHILD, cellRect, this, ListCheckboxId );
        bool bChecked = false;
        if ( NULL != m_pOwner )
        {
            m_pOwner->getBooleanValue( m_nId, nRow, nCol, bChecked );
        }
        pEditor->SetCheck( bChecked ? BST_CHECKED : BST_UNCHECKED );
        m_pCurEditWnd = pEditor;
        break;
    }
    case eUnsignedInteger:
    {
        CEdit* pEditor = new CEdit();
        pEditor->Create( ES_AUTOHSCROLL | WS_VISIBLE | WS_CHILD | ES_LEFT | ES_NUMBER | ES_WANTRETURN, cellRect, this, ListEditId );
        std::string strContent = "";
        size_t szMaxLength = 0u;
        if ( NULL != m_pOwner )
        {
            m_pOwner->getStringValue( m_nId, nRow, nCol, strContent, szMaxLength );
        }
        pEditor->SetWindowText( strCellValue.c_str() );
        pEditor->SetFocus();
        pEditor->SetSel( 0, -1 );
        //pEditor->SetLimitText( szMaxLength );
        m_pCurEditWnd = pEditor;
        break;
    }
    case eInteger:
    {
        CEdit* pEditor = new CEdit();
        pEditor->Create( ES_AUTOHSCROLL | WS_VISIBLE | WS_CHILD | ES_LEFT | ES_NUMBER | ES_WANTRETURN, cellRect, this, ListEditId );
        std::string strContent = "";
        size_t szMaxLength = 0u;
        if ( NULL != m_pOwner )
        {
            m_pOwner->getStringValue( m_nId, nRow, nCol, strContent, szMaxLength );
        }
        pEditor->SetWindowText( strCellValue.c_str() );
        pEditor->SetFocus();
        pEditor->SetSel( 0 , -1 );
        //pEditor->SetLimitText( szMaxLength );
        m_pCurEditWnd = pEditor;
        break;
    }
    case eFloat:
    {
        CEdit* pEditor = new CEdit();
        pEditor->Create( ES_AUTOHSCROLL | WS_VISIBLE | WS_CHILD | ES_LEFT | ES_NUMBER | ES_WANTRETURN, cellRect, this, ListEditId );
        std::string strContent = "";
        size_t szMaxLength = 0u;
        if ( NULL != m_pOwner )
        {
            m_pOwner->getStringValue( m_nId, nRow, nCol, strContent, szMaxLength );
        }
        pEditor->SetWindowText( strCellValue.c_str() );
        pEditor->SetFocus();
        pEditor->SetSel( 0, -1 );
        //pEditor->SetLimitText( szMaxLength );
        m_pCurEditWnd = pEditor;
        break;
    }
    case eString:
    {
        CEdit* pEditor = new CEdit();
        pEditor->Create( ES_AUTOHSCROLL | WS_VISIBLE | WS_CHILD | ES_LEFT | ES_WANTRETURN, cellRect, this, ListEditId );
        std::string strContent = "";
        size_t szMaxLength = 0u;
        if ( NULL != m_pOwner )
        {
            m_pOwner->getStringValue( m_nId, nRow, nCol, strContent, szMaxLength );
        }
        pEditor->SetWindowText( strCellValue.c_str() );
        pEditor->SetFocus();
        pEditor->SetSel( 0, -1 );
        //pEditor->SetLimitText( szMaxLength );
        m_pCurEditWnd = pEditor;
        break;
    }
    case eMmSsTime:
    {
        CDateTimeCtrl* pEditor = new CDateTimeCtrl();
        pEditor->Create( DTS_TIMEFORMAT | WS_VISIBLE | WS_CHILD, cellRect, this, ListDateTimeId );
        pEditor->SetFormat( "mm:ss" );
        time_t tmValue = 0u;
        if ( NULL != m_pOwner )
        {
            m_pOwner->getTimeValue( m_nId, nRow, nCol, tmValue );
        }
        CTime ctmTime( tmValue );
        pEditor->SetTime( &ctmTime );
        m_pCurEditWnd = pEditor;
        break;
    }
    case eHhMmSsTime:
    {
        CDateTimeCtrl* pEditor = new CDateTimeCtrl();
        pEditor->Create( DTS_TIMEFORMAT | WS_VISIBLE | WS_CHILD, cellRect, this, ListDateTimeId );
        pEditor->SetFormat( "hh:mm:ss" );
        time_t tmValue = 0u;
        if ( NULL != m_pOwner )
        {
            m_pOwner->getTimeValue( m_nId, nRow, nCol, tmValue );
        }
        CTime ctmTime( tmValue );
        pEditor->SetTime( &ctmTime );
        m_pCurEditWnd = pEditor;
        break;
    }
    case eHhMmTime:
    {
        CDateTimeCtrl* pEditor = new CDateTimeCtrl();
        pEditor->Create( DTS_TIMEFORMAT | WS_VISIBLE | WS_CHILD, cellRect, this, ListDateTimeId );
        pEditor->SetFormat( "hh:mm" );
        time_t tmValue = 0u;
        if ( NULL != m_pOwner )
        {
            m_pOwner->getTimeValue( m_nId, nRow, nCol, tmValue );
        }
        CTime ctmTime( tmValue );
        pEditor->SetTime( &ctmTime );
        m_pCurEditWnd = pEditor;
        break;
    }
    case eSsTime:
    {
        CDateTimeCtrl* pEditor = new CDateTimeCtrl();
        pEditor->Create( DTS_TIMEFORMAT | WS_VISIBLE | WS_CHILD, cellRect, this, ListDateTimeId );
        pEditor->SetFormat( "ss" );
        time_t tmValue = 0u;
        if ( NULL != m_pOwner )
        {
            m_pOwner->getTimeValue( m_nId, nRow, nCol, tmValue );
        }
        CTime ctmTime( tmValue );
        pEditor->SetTime( &ctmTime );
        m_pCurEditWnd = pEditor;
        break;
    }
    case eYyMmDdTime:
    {
        CDateTimeCtrl* pEditor = new CDateTimeCtrl();
        pEditor->Create( DTS_SHORTDATEFORMAT | WS_VISIBLE | WS_CHILD, cellRect, this, ListDateTimeId );
        pEditor->SetFormat( "DD/MM/YYYY" );
        time_t tmValue = 0u;
        if ( NULL != m_pOwner )
        {
            m_pOwner->getTimeValue( m_nId, nRow, nCol, tmValue );
        }
        CTime ctmTime( tmValue );
        pEditor->SetTime( &ctmTime );
        m_pCurEditWnd = pEditor;
        break;
    }
    case eMmDdTime:
    {
        CDateTimeCtrl* pEditor = new CDateTimeCtrl();
        pEditor->Create( DTS_SHORTDATEFORMAT | WS_VISIBLE | WS_CHILD, cellRect, this, ListDateTimeId );
        pEditor->SetFormat( "DD/MM" );
        time_t tmValue = 0u;
        if ( NULL != m_pOwner )
        {
            m_pOwner->getTimeValue( m_nId, nRow, nCol, tmValue );
        }
        CTime ctmTime( tmValue );
        pEditor->SetTime( &ctmTime );
        m_pCurEditWnd = pEditor;
        break;
    }
    case eFullTime:
    {
        CDateTimeCtrl* pEditor = new CDateTimeCtrl();
        pEditor->Create( DTS_LONGDATEFORMAT | WS_VISIBLE | WS_CHILD, cellRect, this, ListDateTimeId );
        pEditor->SetFormat( "DD/MM/YYYY hh:mm:ss" );
        time_t tmValue = 0u;
        if ( NULL != m_pOwner )
        {
            m_pOwner->getTimeValue( m_nId, nRow, nCol, tmValue );
        }
        CTime ctmTime( tmValue );
        pEditor->SetTime( &ctmTime );
        m_pCurEditWnd = pEditor;
        break;
    }
    case eText:
    case eList:
    case eTree:
    case eGrid:
    {
        CButton* pEditor = new CButton();
        cellRect.left = (cellRect.right - 20);
        pEditor->Create( "...", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, cellRect, this, ListButtonId );
        m_pCurEditWnd = pEditor;
        break;
    }
    case eReadOnly:
    default:
        break;
    }

    if ( NULL != m_pCurEditWnd )
    {
        m_pCurEditWnd->SetFont(GetFont());
    }

    m_nCurEditRow = nRow;
    m_nCurEditCol = nCol;
}

void EditListCtrl::StopEdit()
{
    if ( NULL != m_pCurEditWnd )
    {
        if ( ::IsWindow( m_pCurEditWnd->GetSafeHwnd() ))
        {
            m_pCurEditWnd->ShowWindow( SW_HIDE );
            m_pCurEditWnd->DestroyWindow();
        }

        delete m_pCurEditWnd;
        m_pCurEditWnd = NULL;
    }
}

void EditListCtrl::SetEditType( const int nCol, const int nType )
{
    m_mapNumToEditType[nCol] = nType;
}

void EditListCtrl::SetGroupByColumn( const bool bCol )
{
    m_bGroupByColumn = bCol;
}

void EditListCtrl::OnButtonClicked()
{
    if ( NULL == m_pOwner )
    {
        return;
    }

    std::map<int, int>::iterator itFound = m_mapNumToEditType.find( m_bGroupByColumn ? m_nCurEditCol : m_nCurEditRow );
    if ( m_mapNumToEditType.end() == itFound )
    {
        return;
    }

    switch ( itFound->second )
    {
    case eText:
        m_pOwner->popupText( m_nId, m_nCurEditRow, m_nCurEditCol );
        break;
    case eList:
        m_pOwner->popupList( m_nId, m_nCurEditRow, m_nCurEditCol );
        break;
    case eTree:
        m_pOwner->popupTree( m_nId, m_nCurEditRow, m_nCurEditCol );
        break;
    case eGrid:
        m_pOwner->popupGrid( m_nId, m_nCurEditRow, m_nCurEditCol );
        break;
    default:
        break;
    }
}

void EditListCtrl::OnCheckBoxClicked()
{
    if ( NULL == m_pOwner )
    {
        return;
    }

    std::map<int, int>::iterator itFound = m_mapNumToEditType.find( m_bGroupByColumn ? m_nCurEditCol : m_nCurEditRow );
    if ( m_mapNumToEditType.end() == itFound )
    {
        return;
    }

    CButton* pEditor = reinterpret_cast<CButton*>(m_pCurEditWnd);

    if ( NULL == pEditor )
    {
        return;
    }

    bool bNewValue = ( pEditor->GetCheck() == BST_CHECKED );
    std::string strNewValue = "";
    m_pOwner->booleanValueChanged( m_nId, m_nCurEditRow, m_nCurEditCol, bNewValue, strNewValue );

    SetItemText( m_nCurEditRow, m_nCurEditCol, strNewValue.c_str() );
}

void EditListCtrl::AttachOwner( IEditListOwner* pOwner )
{
    m_pOwner = pOwner;
}

void EditListCtrl::OnEditLostFocus()
{
    CEdit* pEditor = reinterpret_cast<CEdit*>(m_pCurEditWnd);

    if ( NULL == pEditor )
    {
        return;
    }

    CString csValue = "";
    pEditor->GetWindowText( csValue );
    std::string strNewValue = csValue.operator LPCSTR();
    m_pOwner->valueChanged( m_nId, m_nCurEditRow, m_nCurEditCol, strNewValue );

    SetItemText( m_nCurEditRow, m_nCurEditCol, strNewValue.c_str() );
}

void EditListCtrl::OnDateTimeLostFocus( NMHDR* pNMHDR, LRESULT* pResult )
{
    CDateTimeCtrl* pDateTime = reinterpret_cast<CDateTimeCtrl*>(m_pCurEditWnd);

    if ( NULL == pDateTime )
    {
        return;
    }

    CTime ctmTime;
    pDateTime->GetTime( ctmTime );
    std::string strDisplayerValue = "";
    m_pOwner->timeValueChanged( m_nId, m_nCurEditRow, m_nCurEditCol, ctmTime.GetTime(), strDisplayerValue );

    SetItemText( m_nCurEditRow, m_nCurEditCol, strDisplayerValue.c_str() );
}

void EditListCtrl::SetId( const int nId )
{
    m_nId = nId;
}

void EditListCtrl::PreSubclassWindow()
{
    CListCtrl::PreSubclassWindow();

    SetExtendedStyle( LVS_EX_LABELTIP | GetExtendedStyle() );
}

void EditListCtrl::CellHitTest(const CPoint& pt, int& nRow, int& nCol) const
{
    nRow = -1;
    nCol = -1;

    LVHITTESTINFO lvhti = { 0 };
    lvhti.pt = pt;
    nRow = ListView_SubItemHitTest(m_hWnd, &lvhti);	// SubItemHitTest is non-const
    nCol = lvhti.iSubItem;
    if (!(lvhti.flags & LVHT_ONITEMLABEL))
    {
        nRow = -1;
    }
}

BOOL EditListCtrl::OnToolNeedText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
    CPoint pt(GetMessagePos());
    ScreenToClient(&pt);

    int nRow, nCol;
    CellHitTest(pt, nRow, nCol);
    if ( -1 == nRow || -1 == nCol )
    {
        return FALSE;
    }

    CString tooltip = GetItemText(nRow, nCol);
    if (tooltip.IsEmpty())
    {
        return FALSE;
    }

    // Non-unicode applications can receive requests for tooltip-text in unicode
    TOOLTIPTEXT* pTTTA = (TOOLTIPTEXT*)pNMHDR;

    if (pNMHDR->code == TTN_NEEDTEXT)
    {
        lstrcpyn(pTTTA->szText, static_cast<LPCTSTR>(tooltip), sizeof(pTTTA->szText));
    }

    // If wanting to display a tooltip which is longer than 80 characters,
    // one must allocate the needed text-buffer instead of using szText,
    // and point the TOOLTIPTEXT::lpszText to this text-buffer.
    // When doing this, one is required to release this text-buffer again
    return TRUE;
}

// EditListCtrl message handlers


