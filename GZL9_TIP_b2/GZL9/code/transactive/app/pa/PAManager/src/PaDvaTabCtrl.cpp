// PaDvaTabCtrl.cpp : implementation file
//

#include "app/pa/PAManager/src/StdAfx.h"
#include "app/pa/PAManager/src/PaDvaTabCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PaDvaTabCtrl

PaDvaTabCtrl::PaDvaTabCtrl() : CTabCtrl(),
m_mapPages(),
m_stPageRect()
{
}

PaDvaTabCtrl::~PaDvaTabCtrl()
{
}


BEGIN_MESSAGE_MAP(PaDvaTabCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(PaDvaTabCtrl)
	ON_NOTIFY_REFLECT(TCN_SELCHANGE, OnSelchange)
	ON_NOTIFY_REFLECT(TCN_SELCHANGING, OnSelchanging)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void PaDvaTabCtrl::addPage( CPropertyPage* pPage, UINT unTemplateId, const std::string& strCaption )
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
// PaDvaTabCtrl message handlers

void PaDvaTabCtrl::PreSubclassWindow() 
{
    CTabCtrl::PreSubclassWindow();

    int nNewIndex = InsertItem( 0, "" );
    RECT stItemRect = { 0, 0, 0, 0 };
    GetItemRect( 0, &stItemRect );
    DeleteItem( 0 );

    GetClientRect( &m_stPageRect );
    m_stPageRect.top += stItemRect.bottom;
    m_stPageRect.top += stItemRect.top;
    m_stPageRect.top += stItemRect.left;
    m_stPageRect.left += stItemRect.top;
    m_stPageRect.right -= stItemRect.top;
    m_stPageRect.bottom -= stItemRect.top;
}

void PaDvaTabCtrl::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
    *pResult = 0;

    if ( NULL != m_mapPages[GetCurSel()] )
    {
        m_mapPages[GetCurSel()]->ShowWindow( SW_SHOW );
    }
}

void PaDvaTabCtrl::OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult) 
{
    *pResult = 0;

    if ( NULL != m_mapPages[GetCurSel()] )
    {
        m_mapPages[GetCurSel()]->ShowWindow( SW_HIDE );
    }
}

void PaDvaTabCtrl::showPage( int nIndex )
{
    if ( NULL != m_mapPages[GetCurSel()] )
    {
        m_mapPages[GetCurSel()]->ShowWindow( SW_HIDE );
    }

    if ( NULL != m_mapPages[nIndex] )
    {
        m_mapPages[nIndex]->ShowWindow( SW_SHOW );
    }
}

void PaDvaTabCtrl::showPage( CPropertyPage* pPage )
{
    if ( NULL == pPage )
    {
        return;
    }

    for ( std::map<int, CPropertyPage*>::iterator itLoop = m_mapPages.begin(); 
            m_mapPages.end() != itLoop; ++itLoop )
    {
        if ( pPage == itLoop->second )
        {
            CPropertyPage* pCurPage = m_mapPages[GetCurSel()];
            if (( NULL != pCurPage ) && ( pPage != pCurPage ))
            {
                SetCurSel( itLoop->first );
                // SetCurSel won't create SelChanged or SelChanging event, need to manually Show / Hide page
                m_mapPages[GetCurSel()]->ShowWindow( SW_HIDE );
                pPage->ShowWindow( SW_SHOW );
            }

            break;
        }
    }
}