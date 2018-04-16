// PaTabCtrl.cpp : implementation file
//

#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/PaTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PaTabCtrl

PaTabCtrl::PaTabCtrl() : CTabCtrl(),
m_mapPages(),
m_stPageRect()
{
}

PaTabCtrl::~PaTabCtrl()
{
}

BEGIN_MESSAGE_MAP(PaTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(PaTabCtrl)
	ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnSelChanged)
	ON_NOTIFY_REFLECT_EX(TCN_SELCHANGING, OnSelChanging)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void PaTabCtrl::addPage( CPropertyPage* pPage, UINT unTemplateId, const std::string& strCaption )
{
    if ( NULL == pPage )
    {
        return;
    }

    pPage->Create( unTemplateId, this );
    pPage->SetParent( this );
    pPage->ShowWindow( SW_HIDE );
    pPage->MoveWindow( &m_stPageRect );

    int nNewIndex = InsertItem( static_cast<int>(m_mapPages.size()), strCaption.c_str() );

    if ( -1 != nNewIndex )
    {
        m_mapPages[nNewIndex] = pPage;
    }
}
/////////////////////////////////////////////////////////////////////////////
// PaTabCtrl message handlers

void PaTabCtrl::PreSubclassWindow() 
{
    CTabCtrl::PreSubclassWindow();

    int nNewIndex = InsertItem( 0, "" );
    RECT stItemRect = { 0, 0, 0, 0 };
    GetItemRect( 0, &stItemRect );
    DeleteItem( 0 );

    GetClientRect( &m_stPageRect );
    m_stPageRect.top += stItemRect.bottom;
    m_stPageRect.top += stItemRect.top;
    m_stPageRect.left += stItemRect.left;
    m_stPageRect.right -= stItemRect.left;
    m_stPageRect.bottom -= stItemRect.top;
}

BOOL PaTabCtrl::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
    *pResult = 0;

    if ( NULL != m_mapPages[GetCurSel()] )
    {
        m_mapPages[GetCurSel()]->ShowWindow( SW_SHOW );
        m_mapPages[GetCurSel()]->OnSetActive();
    }

    return FALSE;
}

BOOL PaTabCtrl::OnSelChanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
    *pResult = 0;

    if ( NULL != m_mapPages[GetCurSel()] )
    {
        m_mapPages[GetCurSel()]->ShowWindow( SW_HIDE );
        m_mapPages[GetCurSel()]->OnKillActive();
    }

    return FALSE;
}

void PaTabCtrl::showPage( int nIndex )
{
    if ( NULL != m_mapPages[GetCurSel()] )
    {
        m_mapPages[GetCurSel()]->ShowWindow( SW_HIDE );
        m_mapPages[GetCurSel()]->OnKillActive();
    }

    if ( NULL != m_mapPages[nIndex] )
    {
        m_mapPages[nIndex]->ShowWindow( SW_SHOW );
        m_mapPages[nIndex]->OnSetActive();
    }
}