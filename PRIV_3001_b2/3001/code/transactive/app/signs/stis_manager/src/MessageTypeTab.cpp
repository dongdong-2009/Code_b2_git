/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/stis_manager/src/MessageTypeTab.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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

    CMessageTypeTab::CMessageTypeTab() : CTisDialogCtrl(),
										 m_isOcc(false),
										 m_previousSelect(0)
    {
	    AddPage(m_predefinedPage,IDD_PREDEFINED,CString("Predefined"));
	    AddPage(m_freeTextPage,IDD_FREE_TEXT,CString("Ad Hoc"));

		std::stringstream locationKeyStream;
		locationKeyStream << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
		unsigned long locationKey = 0;
		locationKeyStream >> locationKey;

		m_isOcc = (locationKey == 1) ;
    }

    CMessageTypeTab::~CMessageTypeTab()
    {
    }


    BEGIN_MESSAGE_MAP(CMessageTypeTab, CTisDialogCtrl)
	    //{{AFX_MSG_MAP(CMessageTypeTab)
        ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnSelchange)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

	void CMessageTypeTab::InitAll()
	{
		// Set message page parent
		m_predefinedPage.setHandleTab(this);
		m_freeTextPage.setHandleTab(this);

		m_predefinedPage.Init();
		m_freeTextPage.init();
	}


    void CMessageTypeTab::setMessageSelectionListener( IMessageSelectionListener* messageSelectionListener)
    {
        // pass it to child pages
        m_predefinedPage.setMessageSelectionListener(messageSelectionListener);
	    m_freeTextPage.setMessageSelectionListener(messageSelectionListener);

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
        }
    }


    BOOL CMessageTypeTab::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        // change tab
	    BOOL res = CTisDialogCtrl::OnSelchange(pNMHDR, pResult);

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
		else
		{
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


 
    ITimeTypeListener& CMessageTypeTab::getTimeTypeLisener()
    {
        return *(m_freeTextPage.getDisplayAttributesManager());
    }

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

} // TA_IRS_App
