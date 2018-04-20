/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/MessageTypeTab.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * The tab control that holds the
  * predefined, adhoc and ratis pages.
  *
  */

#include "stdafx.h"
#include "stismanager.h"
#include "MessageTypeTab.h"

#include "app/signs/stis_manager/src/STISManagerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{

    CMessageTypeTab::CMessageTypeTab() : CTabDialogCtrl(),
										 m_isOcc(false),
										 m_previousSelect(0)
    {
	    AddPage(m_predefinedPage,IDD_PREDEFINED,CString("Predefined"));
	    AddPage(m_freeTextPage,IDD_FREE_TEXT,CString("Ad Hoc"));
	    AddPage(m_RATISPage,IDD_RATIS,CString("RATIS"));

		std::stringstream locationKeyStream;
		locationKeyStream << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
		unsigned long locationKey = 0;
		locationKeyStream >> locationKey;

		m_isOcc = (locationKey == 1) ;
    }

    CMessageTypeTab::~CMessageTypeTab()
    {
    }


    BEGIN_MESSAGE_MAP(CMessageTypeTab, CTabDialogCtrl)
	    //{{AFX_MSG_MAP(CMessageTypeTab)
        ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnSelchange)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

	void CMessageTypeTab::InitAll()
	{
		m_predefinedPage.Init();
		m_freeTextPage.init();
		m_RATISPage.Init();
	}


    void CMessageTypeTab::setMessageSelectionListener( IMessageSelectionListener* messageSelectionListener)
    {
        // pass it to child pages
        m_predefinedPage.setMessageSelectionListener(messageSelectionListener);
	    m_freeTextPage.setMessageSelectionListener(messageSelectionListener);
	    m_RATISPage.setMessageSelectionListener(messageSelectionListener);

        // call windowShown on the active one to update the status on the main dialog

        if (messageSelectionListener != NULL)
        {
            // get the active dialog and update the parent window
            CDialog* newPage = getActiveDialog();

            if (newPage == &m_predefinedPage)
            {
                m_predefinedPage.windowShown();
            }
            else if (newPage == &m_freeTextPage)
            {
                m_freeTextPage.windowShown();
            }
            else if (newPage == &m_RATISPage)
            {
                m_RATISPage.windowShown();
            }
        }
    }


    BOOL CMessageTypeTab::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        // change tab
	    BOOL res = CTabDialogCtrl::OnSelchange(pNMHDR, pResult);

        // get the active dialog and update the parent window
        CDialog* newPage = getActiveDialog();

        if (newPage == &m_predefinedPage)
        {
            m_predefinedPage.windowShown();
			m_previousSelect = 0;
        }
        else if (newPage == &m_freeTextPage)
        {
            m_freeTextPage.windowShown();
			m_previousSelect = 1;
        }
        else if (newPage == &m_RATISPage && m_isOcc)
        {
            m_RATISPage.windowShown();
			m_previousSelect = 2;
        }
		else
		{
			//in station, the ratis part should be disable
			this->SetCurSel(m_previousSelect);
		}

	    return res;
    }


    TA_Base_Core::PredefinedMessage CMessageTypeTab::getPredefinedMessage() const
    {
        return m_predefinedPage.getMessage();
    }


	FreeTextMessage CMessageTypeTab::getFreeTextMessage() const
    {
        return m_freeTextPage.getMessage();
    }


	RatisMessage CMessageTypeTab::getRatisMessage() const
    {
        return m_RATISPage.getMessage();
    }

 
    ITimeTypeListener& CMessageTypeTab::getTimeTypeLisener()
    {
        return m_freeTextPage;
    }

	//TD 15349
	//zhou yuan++
	bool CMessageTypeTab::findAndSelectStationMessage(const std::string& messageName)
	{
		bool ret = false;
		if(m_predefinedPage.findAndSelectStationMessage(messageName))
		{
			this->SetCurSel(0);
			m_predefinedPage.windowShown();
			ret	= true;
		}
		else if(m_freeTextPage.findAndSelectStationMessage(messageName))
		{
			this->SetCurSel(1);
			m_freeTextPage.windowShown();
			ret = true;
		}
		return ret;
	}
	//++zhou yuan

	//TD 16275
	//zhou yuan++
	//////////////////
    // Draw tab text. You can override to use different color/font.
    //
    void CMessageTypeTab::OnDrawText(CDC& dc, CRect rc,	CString sText, BOOL bDisabled)
    {
        if (bDisabled)
        {
            // Create a shadow effect by first drawing the text in light highlight colour
            // at a slight south-east offset.
            CRect shadowRc(rc);
            shadowRc += CPoint(1,1);
            dc.SetTextColor(::GetSysColor(COLOR_3DHILIGHT));
            dc.DrawText(sText, &shadowRc, DT_CENTER | DT_VCENTER);
        }
		
        // Draw the text in normal or disabled text colour
        dc.SetTextColor(::GetSysColor(bDisabled? COLOR_GRAYTEXT : COLOR_BTNTEXT));
        dc.DrawText(sText, &rc, DT_CENTER | DT_VCENTER);
    }

	//////////////////
    // Draw the tab: mimic SysTabControl32, except use gray if tab is disabled
    //
    void CMessageTypeTab::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
    {
        DRAWITEMSTRUCT& ds = *lpDrawItemStruct;
		
        int iItem = ds.itemID;
		
        // Get tab item info
        char text[128];
        TCITEM tci;
        tci.mask = TCIF_TEXT;
        tci.pszText = text;
        tci.cchTextMax = sizeof(text);
        GetItem(iItem, &tci);
		
        // use draw item DC
        CDC dc;
        dc.Attach(ds.hDC);
		
        // draw text background
        CRect bkRc = ds.rcItem;
        bkRc.top += ::GetSystemMetrics(SM_CYEDGE);
        dc.SetBkMode(TRANSPARENT);
        dc.FillSolidRect(bkRc, ::GetSysColor(COLOR_BTNFACE));
		
        // calculate text rectangle
        CRect rc = ds.rcItem;
        rc += CPoint(0,3);
		
        // draw the text
		if (iItem == 2 && !m_isOcc)
        {
            OnDrawText(dc, rc, text, true);
        }
        else
        {
            OnDrawText(dc, rc, text, false);
        }
		
        dc.Detach();
    }

	void CMessageTypeTab::PreSubclassWindow() 
    {
		// Perform normal stuff
		CTabDialogCtrl::PreSubclassWindow();
		
        ModifyStyle(0, TCS_OWNERDRAWFIXED);
    }
	//++zhou yuan

} // TA_IRS_App
