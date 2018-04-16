#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/system_control/banner/pages/ats_page/src/AtsPage.cpp $
  * @author:  andrewdc
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * AtsPage.cpp : implementation file
  */

#include "app/system_control/banner/pages/ats_page/src/stdafx.h"
#include "app/system_control/banner/pages/ats_page/src/AtsPage.h"

#include "bus/resources/resource.h"
#include "bus/banner_framework/src/BannerFrameworkDialog.h"

#include "bus/sound/sound_client/src/SoundManagerProxy.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/ats/src/AtsMssZoneAccessFactory.h"
#include "core/data_access_interface/ats/src/IAtsMssZone.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/SoundPlayerException.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include <afxole.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

/////////////////////////////////////////////////////////////////////////////
// AtsPage dialog

const std::string ATS_ALARM_CATEGORY = "AtsAlarm";
const std::string ISCS_ALARM_CATEGORY = "Alarm";

AtsPage::AtsPage(const std::string& buttonName, int xPos, int yPos, int width, int height, TA_Base_Bus::BannerFrameworkDialog* parentDialog)
	: BannerPage(buttonName, AtsPage::IDD, xPos, yPos, width, height, parentDialog),
    m_internalRepaint(false), m_atsAlarmListFilename(""), m_atsAlarmTotalsFilename(""),
    m_silenceButton(ATS_ALARM_CATEGORY)
{
	//{{AFX_DATA_INIT(AtsPage)
	//}}AFX_DATA_INIT
}

AtsPage::~AtsPage()
{
    TA_Base_Bus::SoundManagerProxy* soundInterface = NULL;

	try
	{
		soundInterface = new TA_Base_Bus::SoundManagerProxy();
	}
	catch(TA_Base_Core::SoundPlayerException& ex)
	{
		std::string error = "Unable to initialise SoundManager: ";
		error += ex.what();
		LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());
	}
	
	if (soundInterface != NULL)
	{
		try
		{
			// Turn off sound
			soundInterface->setPriority(ATS_ALARM_CATEGORY, 0);
		}
		catch(TA_Base_Core::SoundPlayerException& ex)
		{
			std::string error = "Error setting priority on SoundManager: ";
			error += ex.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());
		}

		delete soundInterface;
	}
}


void AtsPage::DoDataExchange(CDataExchange* pDX)
{
	TStackedPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(AtsPage)
	DDX_Control(pDX, IDC_TOTALS_EXPLORER, m_totalsExplorerCtrl);
	DDX_Control(pDX, IDC_LIST_EXPLORER, m_listExplorerCtrl);
   	DDX_Control(pDX, IDC_SILENCE, m_silenceButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(AtsPage, TStackedPage)
	//{{AFX_MSG_MAP(AtsPage)
    ON_WM_PAINT()
	ON_WM_SHOWWINDOW()
  	ON_BN_CLICKED(IDC_SILENCE, OnSilence)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AtsPage message handlers

BOOL AtsPage::OnInitDialog() 
{
	TStackedPage::OnInitDialog();

	//TD20131,hongzhi,An "Server Busy" dialog is launched each time client Builder is launch from ISCS navigator
	COleMessageFilter* pFilter = AfxOleGetMessageFilter();
	pFilter->SetMessagePendingDelay(1000000); //permit to delay the apparition of the message box , the default seem to be 5000 milliseconds 
	//pFilter->EnableBusyDialog(FALSE); //Specifies whether the "busy" dialog box is enabled or disabled. 
    //pFilter->EnableNotRespondingDialog(FALSE); //Specifies whether the "not responding" dialog box is enabled or disabled. 
    //pFilter->SetBusyReply(SERVERCALL_RETRYLATER); //default, this seem to be put more in server so ClientBuilder, so no interest 

	TA_Base_Bus::SoundManagerProxy* soundInterface = NULL;

	try
	{
		soundInterface = new TA_Base_Bus::SoundManagerProxy();
	}
	catch(TA_Base_Core::SoundPlayerException& ex)
	{
		std::string error = "Unable to initialise SoundManager: ";
		error += ex.what();
		LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());
	}

	if (soundInterface != NULL)
	{
		try
		{
			soundInterface->joinCategories(ISCS_ALARM_CATEGORY, ATS_ALARM_CATEGORY);
		}
		catch(TA_Base_Core::SoundPlayerException& ex)
		{
			std::string error = "Unable to initialise SoundManager: ";
			error += ex.what();
			LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());
		}

		delete soundInterface;
	}

    m_silenceButton.initialise(false);

	{
		TA_Base_Core::IConsole* console = NULL;
		TA_Base_Core::ILocation* location = NULL;
		TA_IRS_Core::IAtsMssZones mssZones;

		try
		{
			std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

			console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);

			if (console != NULL)
			{
				// Get physical location
				unsigned long locationKey = console->getLocation();

				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
							"Located local console location - session %s, console %s, location %d", 
							sessionId.c_str(), console->getName().c_str(), locationKey);

				location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationKey);

				std::string path = "";

				if (location != NULL)
				{
					if (location->getName() == "OCC")
					{
						path = "C:/ATS_GUI/OCC_MFT/Mimic Files/";
					}
					else if (location->getName() == "KCD")
					{
						path = "C:/ATS_GUI/DEPOT_MFT/Mimic Files/";
					}
					else
					{
						// Need to find the MSS
						
						mssZones = TA_IRS_Core::AtsMssZoneAccessFactory::getInstance().getAtsMssZonesWhere(false, locationKey);

						if (mssZones.size() == 1)
						{
							path = "C:/ATS_GUI/Station" + mssZones[0]->getMssZone() + "_MFT/Mimic Files/";
						}
					}
				}

				if (path != "")
				{
					m_atsAlarmListFilename = path + "AlarmsBanner.fvm";
					m_atsAlarmTotalsFilename = path + "AlarmsNr.fvm";

					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
								"Located ATS Banner mimic filenames - list %s, totals %s", 
								m_atsAlarmListFilename.c_str(), m_atsAlarmTotalsFilename.c_str());
			}
		}
	}
	catch(TA_Base_Core::DataException& ex)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
	}
	catch(TA_Base_Core::DatabaseException& ex)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
	}
	catch(std::exception& ex)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what());
	}
	catch(...)
	{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown exception caught while constructing ATS Banner Page");
		}

		delete console;
		delete location;

		for (TA_IRS_Core::IAtsMssZonesIt it = mssZones.begin();
			 it != mssZones.end();
			 it++)
		{
			if (*it != NULL)
			{
				delete (*it);
				(*it) = NULL;
			}
		}
	}

	try
	{
		// Size the dialog and the two components
		m_totalsExplorerCtrl.MoveWindow( 0, 0, 126, 64);
		m_listExplorerCtrl.MoveWindow( 126, 0, getWidth()-106, getHeight()-2);
	}
	catch(std::exception& ex)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what());
	}
	catch(...)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown exception caught while initialising CWebBrowser controls");
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
}


void AtsPage::onButtonDoubleClick()
{
    // do nothing
}


void AtsPage::addBannerPage()
{
    FUNCTION_ENTRY("addBannerPage");

    TA_Base_Bus::BannerIcon bannerIcon;
    bannerIcon.onIcon = IDB_ATS;
    bannerIcon.offIcon = IDB_ATSG;
	bannerIcon.onDownIcon = IDB_ATSD;
	bannerIcon.offDownIcon = IDB_ATSDG;

    getParentDialog()->AddBannerPage(IDD_ATS_ALARMS, getButtonName(), bannerIcon, this);

    FUNCTION_EXIT;
}


void AtsPage::OnPaint()
{
    BannerPage::OnPaint();
    
    // Must flag internally to indicate the OnPaint notification
    // received within this block should not cause recursive OnPaint notifications
    if (!m_internalRepaint)
    {
        // Prompt an internal redraw (flag as such, so this redraw doesn't cause another, which
        // would cause an infinite loop)
        m_internalRepaint = true;

        CRect rc;    
        m_totalsExplorerCtrl.GetWindowRect(rc);        
        ScreenToClient(rc);
        // Lock redraw so don't see the pixel height increase
        m_totalsExplorerCtrl.SetRedraw(FALSE);
        // Perform a dummy resize to force a redraw condition - note that
        // Invalidate(TRUE), InvalidateRect(0, TRUE) do not cause a proper redraw
        // for these web browser controls (for whatever reason)
        m_totalsExplorerCtrl.MoveWindow(rc.left, rc.top, rc.Width(), rc.Height() + 1);
        // Unlock prior to restoring original size (hence will be drawn with proper dimensions -
        // so user doesn't see any flicker between the dummy resize and proper dimensions)
        m_totalsExplorerCtrl.SetRedraw(TRUE);
        // Restore original size
        m_totalsExplorerCtrl.MoveWindow(rc);    
            
        m_listExplorerCtrl.GetWindowRect(rc);    
        ScreenToClient(rc);
        m_listExplorerCtrl.SetRedraw(FALSE);
        m_listExplorerCtrl.MoveWindow(rc.left, rc.top, rc.Width(), rc.Height() + 1);
        m_listExplorerCtrl.SetRedraw(TRUE);
        m_listExplorerCtrl.MoveWindow(rc);
    }
    else
    {
        // Need to flip internal paint states to avoid infinite loop in paint
        m_internalRepaint = false;
    }
}

void AtsPage::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	TStackedPage::OnShowWindow(bShow, nStatus);
	
	if (bShow == TRUE)
	{
		// TD 11907: When Naviagate() is called, if the file does not exist, a modal dialog is launched
        // indicating that it can't find the file. Sometimes this dialog appears behind the schematics
        // and is not seen, so isn't dismissed. Hence this OnShowWindow method is not completed. 
        // To solve this, here we check first whether the file exists first before calling Navigate().
        
        if(fileExists(m_atsAlarmTotalsFilename))
        {
            try
		    {
			    VARIANT flags, targetFrameName, postData, headers;
			    m_totalsExplorerCtrl.Navigate(m_atsAlarmTotalsFilename.c_str(),
									    &flags, &targetFrameName, &postData, &headers );
		    }
		    catch(std::exception& ex)
		    {
			    LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what());
		    }
		    catch(...)
		    {
			    LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown exception caught while initialising CWebBrowser controls");
		    }
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "File doesn't exist: %s", m_atsAlarmTotalsFilename.c_str());
        }

        if(fileExists(m_atsAlarmListFilename))
        {
		    try
		    {
			    VARIANT flags, targetFrameName, postData, headers;
			    m_listExplorerCtrl.Navigate(m_atsAlarmListFilename.c_str(),
									     &flags, &targetFrameName, &postData, &headers );
		    }
		    catch(std::exception& ex)
		    {
			    LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what());
		    }
		    catch(...)
		    {
			    LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown exception caught while initialising CWebBrowser controls");
		    }
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "File doesn't exist: %s", m_atsAlarmListFilename.c_str());
        }
	}
}


void AtsPage::OnSilence() 
{
    m_silenceButton.toggleState();
}


bool AtsPage::fileExists(const std::string& fileName)
{
    CString filePath(fileName.c_str());
    
    HANDLE fileHandle = CreateFile(
        filePath, 
        GENERIC_READ, 
        FILE_SHARE_READ, 
        NULL, 
        OPEN_EXISTING,  
        FILE_ATTRIBUTE_NORMAL, 
        0); 

    bool fileExists = false;
    if(fileHandle != INVALID_HANDLE_VALUE )
    {
        fileExists = true;
    }

    CloseHandle(fileHandle);

    return fileExists;
}

