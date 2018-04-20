/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/MainTab.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/03/05 11:47:34 $
  * Last modified by:  $Author: builder $
  *
  * This creates and manages each of the main tabs at the top of the app.
  *
  */

#include "stdafx.h"
#include "MainTab.h"
#include "REBProgressManager.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{
	TA_Base_Core::NonReEntrantThreadLockable MainTab::s_dipalyThreadLock;
	TA_Base_Core::NonReEntrantThreadLockable MainTab::s_downLoadPageLock;

	UINT diplayPageInitThreadFun ( LPVOID pParam )
	{

		//TA_THREADGUARD( MainTab::s_dipalyThreadLock );
		TA_Base_Core::ThreadGuard guard( MainTab::s_dipalyThreadLock );
		MainTab* pmainTab = reinterpret_cast<MainTab*> (pParam);
		pmainTab->getDisplayPage().InitAll();

		return 0;

	}


	UINT downloadPageInitThreadFun(LPVOID pParam )
	{
		//TA_THREADGUARD( MainTab::s_downLoadPageLock );
		TA_Base_Core::ThreadGuard guard(MainTab::s_downLoadPageLock);
		MainTab* pmainTab = reinterpret_cast<MainTab*>(pParam);
		pmainTab->getdownLoadPage().Init();

		return 0;
	}

    /////////////////////////////////////////////////////////////////////////////
    // MainTab

    MainTab::MainTab() : CTabDialogCtrl()
    {
        FUNCTION_ENTRY( "MainTab" );

        AddPage(m_displayPage,IDD_DISPLAY_PAGE,CString("Station Display"));
        AddPage(m_downloadPage,IDD_VERSION_PAGE,CString("Station Download"));

        FUNCTION_EXIT;
    }


    MainTab::~MainTab()
    {
        FUNCTION_ENTRY( "~MainTab" );
        FUNCTION_EXIT;
		
    }


    BEGIN_MESSAGE_MAP(MainTab, CTabDialogCtrl)
        // {{AFX_MSG_MAP(MainTab)
            // NOTE - the ClassWizard will add and remove mapping macros here.
        // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    // haipeng added for the perfomance issue
    void MainTab::Init()
    {
        FUNCTION_ENTRY( "Init" );
		
        //mgr.SetStaticText(0, "Initializing the Display Page...");

        //Bohong++ change into single thread so as to avoid UI element deadlock
		//AfxBeginThread( diplayPageInitThreadFun, this );
        getDisplayPage().InitAll();

        //m_displayPage.InitAll(mgr);
        //mgr.SetProgress(50);
        //mgr.SetStaticText(0, "Initializing the Library Version Page...");

        //Bohong++ change into single thread so as to avoid UI element deadlock
		//AfxBeginThread( downloadPageInitThreadFun, this );
        getdownLoadPage().Init();

        //m_downloadPage.Init(mgr);
        //mgr.SetProgress(98);

        FUNCTION_EXIT;
    }


//haipeng added for the perfomance issue

    BOOL MainTab::PreTranslateMessage(MSG* pMsg)
    {
        FUNCTION_ENTRY( "PreTranslateMessage" );

        switch ( pMsg->message )
        {
        case WM_KEYDOWN:
            {
                switch ( pMsg->wParam )
                {
                case VK_ESCAPE:
                    {
                        FUNCTION_EXIT;
                        return TRUE;
                    }
                    break;
                case VK_RETURN:
                    {
                        FUNCTION_EXIT;
                        return TRUE;
                    }
                    break;
                default:
                    {
                        // do nothing
                    }
                    break;
                }
            }
            break;
        default:
            {
                // do nothing
            }
        }

        FUNCTION_EXIT;
        return CTabDialogCtrl::PreTranslateMessage(pMsg);
    }


    // Maochun++
    // TD12780
    CDialog* MainTab::getMainTabPage(UINT iPage)
    {
        FUNCTION_ENTRY( "getMainTabPage" );

        switch(iPage)
        {
        case 0: return &m_displayPage;
        case 1: return &m_downloadPage;
        default: return 0;
        }
    }


    // ++Maochun
    // TD12780

    // TD 15349
    // zhou yuan++
    DisplayPage& MainTab::getDisplayPage()
    {
        FUNCTION_ENTRY( "getDisplayPage" );
        FUNCTION_EXIT;
        return m_displayPage;
    }


    // ++zhou yuan


	LibraryVersionPage& MainTab::getdownLoadPage()
	{
		FUNCTION_ENTRY( "getdownLoadPage" );
		FUNCTION_EXIT;
		return m_downloadPage;
	}

} // TA_IRS_App
