/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/telephone/telephone_manager/src/CTelephoneFacilitiesPropertySheet.cpp $
  * @author:  Raymond Pau
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Refer to 4669P62040005-Telephone_Manager_GUI_Design_Note.DND section Telephone
  * Directory Facilities for description.
  */

#include "app/telephone/telephone_manager/src/stdafx.h"
#include "app/telephone/telephone_manager/src/TelephoneApp.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/telephone/telephone_manager/src/CTelephoneFacilitiesPropertySheet.h"
#include "app/telephone/telephone_manager/src/CTelephoneManagerDlg.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"


BEGIN_MESSAGE_MAP(CTelephoneFacilitiesPropertySheet, CMultiFontPropertySheet)
    // {{AFX_MSG_MAP(CTelephoneFacilitiesPropertySheet)
    ON_WM_DESTROY()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


CTelephoneFacilitiesPropertySheet::CTelephoneFacilitiesPropertySheet(CWnd* pParentWnd)
    :CMultiFontPropertySheet(_T(""), pParentWnd)
{
    FUNCTION_ENTRY( "CTelephoneFacilitiesPropertySheet" );

    /*
    m_directLinePage.setItaSummary(ptransactiveInterfaces);
    m_directorySearchPage.setItaSummary(ptransactiveInterfaces);
    m_configureDirectLinesPage.setItaSummary(ptransactiveInterfaces);
    m_configureDirectoryPage.setItaSummary(ptransactiveInterfaces);

    AddPage(&m_directLinePage);
    AddPage(&m_directorySearchPage);
    AddPage(&m_configureDirectLinesPage);
    AddPage(&m_configureDirectoryPage);
*/

    FUNCTION_EXIT;
}


void CTelephoneFacilitiesPropertySheet::setTelephoneManagerSummary(ItaTelephoneManagerSummary* ptransactiveInterfaces)
{
    FUNCTION_ENTRY( "setTelephoneManagerSummary" );

    m_directLinePage.setItaSummary(ptransactiveInterfaces);
    m_directorySearchPage.setItaSummary(ptransactiveInterfaces);
    m_configureDirectLinesPage.setItaSummary(ptransactiveInterfaces);
    m_configureDirectoryPage.setItaSummary(ptransactiveInterfaces);

    AddPage(&m_directLinePage);
    AddPage(&m_directorySearchPage);
    AddPage(&m_configureDirectLinesPage);
    AddPage(&m_configureDirectoryPage);

    FUNCTION_EXIT;
}


BOOL CTelephoneFacilitiesPropertySheet::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CMultiFontPropertySheet::OnInitDialog();
	// get HWND of tab control and subclass it
	HWND hWndTab = (HWND)SendMessage(PSM_GETTABCONTROL);
	m_tabCtrl.SubclassDlgItem(::GetDlgCtrlID(hWndTab), this);

    try
    {
        TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);
    }
    catch (...)
    {
    }


    FUNCTION_EXIT;
    return TRUE;
}


CTelephoneFacilitiesPropertySheet::~CTelephoneFacilitiesPropertySheet()
{
    FUNCTION_ENTRY( "~CTelephoneFacilitiesPropertySheet" );
    FUNCTION_EXIT;
}


CTelephoneDirectLinePage* CTelephoneFacilitiesPropertySheet::getDirectLinePage()
{
    FUNCTION_ENTRY( "getDirectLinePage" );
    FUNCTION_EXIT;
    return &m_directLinePage;

}


CTelephoneDirectorySearchPage* CTelephoneFacilitiesPropertySheet::getDirectorySearchPage()
{
    FUNCTION_ENTRY( "getDirectorySearchPage" );
    FUNCTION_EXIT;
    return &m_directorySearchPage;

}


CTelephoneConfigureDirectLinesPage* CTelephoneFacilitiesPropertySheet::getConfigureDirectLinesPage()
{
    FUNCTION_ENTRY( "getConfigureDirectLinesPage" );
    FUNCTION_EXIT;
    return &m_configureDirectLinesPage;

}


CTelephoneConfigureDirectoryPage* CTelephoneFacilitiesPropertySheet::getConfigureDirectoryPage()
{
    FUNCTION_ENTRY( "getConfigureDirectoryPage" );
    FUNCTION_EXIT;
    return &m_configureDirectoryPage;
}


void CTelephoneFacilitiesPropertySheet::enablePage (int nPage, BOOL bEnable)
{
    FUNCTION_ENTRY( "enablePage" );

    // if we want to enable the page
    if (bEnable)
    {
        m_DisabledPages.erase(nPage);
    }
    else
    {
        m_DisabledPages[nPage] = nPage;
    }

    FUNCTION_EXIT;
}


BOOL CTelephoneFacilitiesPropertySheet::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnNotify" );

    NMHDR* pnmh = (NMHDR*)lParam;

    // tab is about to change
    if (TCN_SELCHANGING == pnmh->code)
    {
        m_nLastActive = GetActiveIndex ();  // save the current page index
    }
    else if (TCN_SELCHANGE == pnmh->code)   // tab has been changed
    {
        // get the current page index
        int nCurrentPage = GetActiveIndex ();

        // if current page is in our map of disabled pages
        std::map<unsigned int, unsigned int>::iterator iter = m_DisabledPages.find(nCurrentPage);
        if(iter != m_DisabledPages.end())
        {
            PostMessage (PSM_SETCURSEL, m_nLastActive);// activate the previous page
        }
        else // If page not disable, then notify parent of the change
        {
            // Notify parent of user's selection of a tab
            // TA_ASSERT(0 != GetParent(), "Parent invalid");
            ASSERT_VALID(GetParent());
            // So lParam doesn't get blown away before parent gets message
            // wait until parent processes the message
            GetParent()->SendNotifyMessage(WM_NOTIFY, (WPARAM)(this), lParam);
        }

    }

    FUNCTION_EXIT;
    return CMultiFontPropertySheet::OnNotify(wParam, lParam, pResult);
}


void CTelephoneFacilitiesPropertySheet::onRunParamChange(const std::string& name, const std::string& value)
{
    FUNCTION_ENTRY( "onRunParamChange" );

    if (name == RPARAM_SESSIONID)
    {
        // Possible change in rights
        m_configureDirectLinesPage.DisplayButtons();
        m_directLinePage.DisplayButtons();
    }

    FUNCTION_EXIT;
}


// CL 17641  ++
BOOL CTelephoneFacilitiesPropertySheet::PreTranslateMessage(MSG* pMsg)
{
	return m_tabCtrl.TranslatePropSheetMsg(pMsg) ? TRUE :
		CPropertySheet::PreTranslateMessage(pMsg);
}


BOOL CDisableTabCtrl::IsTabEnabled(int iTab)
{ 
		CTelephoneManagerDlg* pMainWnd = dynamic_cast<CTelephoneManagerDlg*>(AfxGetMainWnd());
		bool canConfigDir = pMainWnd->isFunctionPermitted(TA_Base_Bus::aca_TELEPHONE_CONFIGURE_DIRECTORY);//page 3
		bool canSearchDir = pMainWnd->isFunctionPermitted(TA_Base_Bus::aca_TELEPHONE_SEARCH_DIRECTORY);// page 1

		if (!canSearchDir && canConfigDir) 
		{
           return iTab != 1;
		}
		if (!canSearchDir && !canConfigDir) 
		{
			return iTab != 3 &&  iTab != 1;
		}
		if ( !canConfigDir && canSearchDir) 
		{
			return iTab != 3;
		}	    	
}

// ++ CL 17641 


void CTelephoneFacilitiesPropertySheet::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );
    
    TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);
    CMultiFontPropertySheet::OnDestroy();

    FUNCTION_EXIT;
}


