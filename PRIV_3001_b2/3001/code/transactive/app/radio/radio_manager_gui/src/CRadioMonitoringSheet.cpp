/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/CRadioMonitoringSheet.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/radiomanager.h"
#include "app/radio/radio_manager_gui/src/CRadioMonitoringSheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRadioMonitoringSheet

IMPLEMENT_DYNAMIC(CRadioMonitoringSheet, CPropertySheet)

CRadioMonitoringSheet::CRadioMonitoringSheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CMultiFontPropertySheet(nIDCaption, pParentWnd, iSelectPage),
     m_CRadioStatusPage(NULL),
     m_CCallStackPage(NULL),
	 m_CRadioCallForwardingPage(NULL),
	 m_CRadioSubsMonitoringPage(NULL)

{

}

CRadioMonitoringSheet::CRadioMonitoringSheet(LPCTSTR pszCaption, CWnd* pParentWnd, RECT* sheetSize, UINT iSelectPage)
	:CMultiFontPropertySheet(pszCaption, pParentWnd, iSelectPage),
    m_CRadioStatusPage(NULL),
    m_CCallStackPage(NULL),
	m_CRadioCallForwardingPage(NULL),
	m_CRadioSubsMonitoringPage(NULL)
{
    if (sheetSize != NULL)
    {
        m_parentRect = *sheetSize;
    }
	m_CRadioStatusPage  = new CRadioStatusPage(sheetSize);
	m_CCallStackPage    = new CCallStackPage(sheetSize);
	//m_CRadioHistoryPage = new CRadioHistoryPage(sheetSize);
	m_CRadioCallForwardingPage = new CRadioCallForwardingPage(sheetSize);
	m_CRadioSubsMonitoringPage = new CRadioSubsMonitoringPage(sheetSize, m_CRadioStatusPage);

	AddPage(m_CRadioStatusPage);

	//AddPage(m_CRadioHistoryPage);

	AddPage(m_CCallStackPage);

	AddPage(m_CRadioCallForwardingPage);

	AddPage(m_CRadioSubsMonitoringPage);

}

CRadioMonitoringSheet::~CRadioMonitoringSheet()
{
	if (NULL != m_CRadioStatusPage) {
		delete m_CRadioStatusPage;
		m_CRadioStatusPage = NULL;
	}

	if (NULL != m_CCallStackPage) {
		delete m_CCallStackPage;
		m_CCallStackPage = NULL;
	}

	if (NULL != m_CRadioCallForwardingPage) {
		delete m_CRadioCallForwardingPage;
		m_CRadioCallForwardingPage = NULL;
	}

	if (NULL != m_CRadioSubsMonitoringPage) {
		delete m_CRadioSubsMonitoringPage;
		m_CRadioSubsMonitoringPage = NULL;
	}
}


BEGIN_MESSAGE_MAP(CRadioMonitoringSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CRadioMonitoringSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
	ON_WM_DRAWITEM()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioMonitoringSheet message handlers



BOOL CRadioMonitoringSheet::OnInitDialog()
{
    
    CMultiFontPropertySheet::OnInitDialog();

	GetTabControl()->ModifyStyle(NULL, GetTabControl()->GetExtendedStyle() | TCS_OWNERDRAWFIXED, 0);
	m_tabNames[0] = "Radio Status";
	m_tabNames[1] = "Call Stack"; 
	m_tabNames[2] = "Call Forwarding";
	m_tabNames[3] = "Sub. Monitoring";
    return TRUE;
}

void CRadioMonitoringSheet::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
	if (ODT_TAB == lpDrawItemStruct->CtlType)
	{
		CDC *oldDC = GetDC();
		HDC hDC = lpDrawItemStruct->hDC;
		CDC* pDC = CDC::FromHandle(hDC);
		lpDrawItemStruct->itemAction = ODA_DRAWENTIRE;
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetBkColor(GetSysColor(COLOR_3DFACE));
		CRect rect(lpDrawItemStruct->rcItem);
		CPoint tl(rect.TopLeft().x+1,rect.TopLeft().y+3);
		CPoint br(rect.BottomRight().x-1, rect.BottomRight().y);
		CRect rect1(tl,br);
		FillRect(lpDrawItemStruct->hDC,rect1,(HBRUSH)(CreateSolidBrush(GetSysColor(COLOR_3DFACE))));
		pDC = oldDC;

		CDC aDC;
		aDC.Attach(lpDrawItemStruct->hDC);
		int OldBKmode = aDC.SetBkMode(TRANSPARENT);
		int OldTextColor = aDC.SetTextColor(RGB(0,0,0));
		aDC.DrawText(m_tabNames[lpDrawItemStruct->itemID],&lpDrawItemStruct->rcItem, DT_CENTER|DT_VCENTER|DT_SINGLELINE);
		aDC.SetTextColor(OldTextColor);
		aDC.SetBkMode(OldBKmode);
		aDC.Detach();
	}
	else
		CPropertySheet::OnDrawItem(nIDCtl, lpDrawItemStruct);
}