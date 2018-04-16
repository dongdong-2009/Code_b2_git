/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/stis_manager/src/MainTab.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/04/12 10:01:53 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * This creates and manages each of the main tabs at the top of the app.
  *
  */

#include "stdafx.h"
#include "MainTab.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{
    /////////////////////////////////////////////////////////////////////////////
    // MainTab

    MainTab::MainTab() : CTisDialogCtrl(),m_isOccLocation(false)
    {
        FUNCTION_ENTRY( "MainTab" );

        AddPage(m_displayPage,IDD_DISPLAY_PAGE,CString("Station Display"));
		// Check location, if OCC, add time schedule page
		try
		{
			unsigned long occLocationKey = TA_Base_Core::LocationAccessFactory::getInstance().getOccLocationKey();
			std::stringstream ss;
			ss << TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY);
			unsigned long locationKey = 0;
			ss >> locationKey;
			TA_ASSERT(locationKey != 0, "Location key could not be converted to a number, or it is 0");
			m_isOccLocation = (occLocationKey == locationKey);
		}
		catch(const TA_Base_Core::DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, ex.what(), "");
		}
		catch(const TA_Base_Core::DataException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, ex.what(), "");
		}
		if(m_isOccLocation)
		{
			AddPage(m_timeSchedulePage, IDD_TIME_SCHEDULE_PAGE, CString("Time Schedule"));
		}

        AddPage(m_upgradePage,IDD_VERSION_PAGE,CString("Library Upgrade"));

        FUNCTION_EXIT;
    }


    MainTab::~MainTab()
    {
        FUNCTION_ENTRY( "~MainTab" );
        FUNCTION_EXIT;
		
    }


    BEGIN_MESSAGE_MAP(MainTab, CTisDialogCtrl)
        // {{AFX_MSG_MAP(MainTab)
            // NOTE - the ClassWizard will add and remove mapping macros here.
        // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void MainTab::Init()
    {
        FUNCTION_ENTRY( "Init" );
		
        getDisplayPage().InitAll();
        getupgradePage().Init();
		if(m_isOccLocation)
		{
			getTimeSchedulePage().Init();
		}


        FUNCTION_EXIT;
    }


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
        return CTisDialogCtrl::PreTranslateMessage(pMsg);
    }

    CDialog* MainTab::getMainTabPage(UINT iPage)
    {
        FUNCTION_ENTRY( "getMainTabPage" );
		FUNCTION_EXIT;
		if(false == m_isOccLocation)
		{
	        switch(iPage)
	        {
		        case 0: return &m_displayPage;
		        case 1: return &m_upgradePage;
		        default: return 0;
	        }
		}
		else
		{
			switch(iPage)
			{
				case 0: return &m_displayPage;
				case 1: return &m_timeSchedulePage;
				case 2: return &m_upgradePage;
				default: return 0;
			}
		}
    }


    DisplayPage& MainTab::getDisplayPage()
    {
        FUNCTION_ENTRY( "getDisplayPage" );
        FUNCTION_EXIT;
        return m_displayPage;
    }

	TimeSchedulePage& MainTab::getTimeSchedulePage()
	{
		FUNCTION_ENTRY("getTimeSchedulePage");
		FUNCTION_EXIT;
		return m_timeSchedulePage;
	}


	LibraryVersionPage& MainTab::getupgradePage()
	{
		FUNCTION_ENTRY( "getupgradePage" );
		FUNCTION_EXIT;
		return m_upgradePage;
	}

} // TA_IRS_App
