/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/CRadioMonitoringSheet.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/radiomanager.h"
#include "app/radio/radio_manager_gui_icdv4/src/CRadioMonitoringSheet.h"

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
     m_CRadioHistoryPage(NULL)

{

}

CRadioMonitoringSheet::CRadioMonitoringSheet(LPCTSTR pszCaption, CWnd* pParentWnd, RECT* sheetSize, UINT iSelectPage)
	:CMultiFontPropertySheet(pszCaption, pParentWnd, iSelectPage),
    m_CRadioStatusPage(NULL),
    m_CCallStackPage(NULL),
    m_CRadioHistoryPage(NULL)
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
	delete m_CRadioStatusPage;
	m_CRadioStatusPage = NULL;

	
	delete m_CCallStackPage;
	m_CCallStackPage = NULL;

	delete m_CRadioHistoryPage;
	m_CRadioHistoryPage = NULL;

	delete m_CRadioCallForwardingPage;
	m_CRadioCallForwardingPage = NULL;

	delete m_CRadioSubsMonitoringPage;
	m_CRadioSubsMonitoringPage = NULL;
}


BEGIN_MESSAGE_MAP(CRadioMonitoringSheet, CPropertySheet)
	//{{AFX_MSG_MAP(CRadioMonitoringSheet)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioMonitoringSheet message handlers



BOOL CRadioMonitoringSheet::OnInitDialog()
{
    
    CMultiFontPropertySheet::OnInitDialog();


    return TRUE;
}
