/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/CRadioMonitoringSheet.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */


#if !defined(AFX_CRADIOMONITORINGSHEET_H__601C4E94_7768_4FAF_A71E_DCC60306A63F__INCLUDED_)
#define AFX_CRADIOMONITORINGSHEET_H__601C4E94_7768_4FAF_A71E_DCC60306A63F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/mfc_extensions/src/multi_font_property_sheet/MultiFontPropertySheet.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/CRadioStatusPage.h"
#include "app/radio/radio_manager_gui/src/RadioCallstackPage/CCallStackPage.h"
#include "app/radio/radio_manager_gui/src/RadioCallForwardingPage/CRadioCallForwardingPage.h"
#include "app/radio/radio_manager_gui/src/RadioSubsMonitoringPage/CRadioSubsMonitoringPage.h"

/////////////////////////////////////////////////////////////////////////////
// CRadioMonitoringSheet

class CRadioMonitoringSheet : public CMultiFontPropertySheet
{
	DECLARE_DYNAMIC(CRadioMonitoringSheet)

	// Construction
	public:

		CRadioMonitoringSheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
		CRadioMonitoringSheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, RECT* sheetSize = NULL, UINT iSelectPage = 0);

		virtual ~CRadioMonitoringSheet();

    	virtual BOOL OnInitDialog();


	protected:
		//{{AFX_MSG(CRadioMonitoringSheet)
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
		afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);


	// Attributes
	public:

		CRadioStatusPage *m_CRadioStatusPage;

		CCallStackPage *m_CCallStackPage;

		CRadioCallForwardingPage *m_CRadioCallForwardingPage;

		CRadioSubsMonitoringPage *m_CRadioSubsMonitoringPage;
        
        RECT m_parentRect;

		CString m_tabNames[4];

};


#endif // !defined(AFX_CRADIOMONITORINGSHEET_H__601C4E94_7768_4FAF_A71E_DCC60306A63F__INCLUDED_)
