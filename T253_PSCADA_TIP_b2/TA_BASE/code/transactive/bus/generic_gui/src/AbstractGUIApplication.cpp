/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/generic_gui/src/AbstractGUIApplication.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * This inherits from IGUIApplication and IGUIAccess and implements all methods that are common
  * between dialogs, property sheets and documents. It also introduces some protected methods to 
  * be implemented by child classes to aid in consistency.
  */

#pragma warning(disable:4786)

#include "bus\generic_gui\src\StdAfx.h"
#include "bus\generic_gui\src\AbstractGUIApplication.h"
#include "bus\generic_gui\src\AppLauncher.h"
#include "bus\generic_gui\src\TransActiveDialog.h"
#include "bus\generic_gui\src\TransActiveFrame.h"
#include "bus\generic_gui\src\TransActiveMDIFrame.h"
#include "bus\generic_gui\src\TransActivePropertySheet.h"
#include "bus\generic_gui\src\TransActiveMessage.h"

#include "bus\user_settings\src\SettingsMgr.h"
#include "core\data_access_interface\entity_access\src\IEntityData.h"
#include "core\utilities\src\RunParams.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\TAAssert.h"
#include "core\exceptions\src\GenericGUIException.h"
#include "core\exceptions\src\UserSettingsException.h"

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::TransactiveException;
using TA_Base_Core::GenericGUIException;
using namespace TA_Base_Bus::TA_GenericGui;

// the following bit saves you having to install 500 MB worth of MS SDK
// for just a few defines...
#ifndef SPI_SETFOREGROUNDLOCKTIMEOUT
#define SPI_SETFOREGROUNDLOCKTIMEOUT 0x2001
#endif // SPI_SETFOREGROUNDLOCKTIMEOUT

namespace TA_Base_Bus
{
    using TA_GenericGui::WM_SET_WINDOW_POSITION;

    AbstractGUIApplication::AbstractGUIApplication() : 	        
         m_guiEntity(NULL),        // set in checkEntity()
         m_application(NULL),      // set in child constructor
         m_userSettings(NULL),      // set in first settings method
		 m_genericGUIStartCompleted(false)
    {
        if (!SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, static_cast<LPVOID>(0), SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE))
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Could not set the system parametesr so that windows are launched to the top");
        }
        m_positionInfo.posFlag = 0;
        m_positionInfo.alignFlag = 0;
        m_positionInfo.objectDim = DEFAULT_RECT;
        m_positionInfo.boundaryDim = DEFAULT_RECT;
        m_positionInfo.appDim = DEFAULT_RECT;
        m_isFirst = true;
    }


    AbstractGUIApplication::~AbstractGUIApplication()
    {
        try
        {
            if (m_userSettings != NULL)  // PW2500
            {
                delete m_userSettings;
                m_userSettings = NULL;
            }
        }
        catch( ... )
        {
			LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    std::string AbstractGUIApplication::getSessionId() 
    { 
        return RunParams::getInstance().get(RPARAM_SESSIONID);
    }


    void AbstractGUIApplication::changeFocus(const TA_Base_Core::EFocusType focus)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "changeFocus" );
        
        // If the application is null then we are trying to change the focus before it has
        // set up and initialised correctly. We will just return.
        if ((m_application == NULL) || (m_application->m_hWnd == NULL) || !::IsWindow(m_application->m_hWnd))
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,"changeFocus() was called on the application before it was initialised correctly. It will not change focus");
            LOG( SourceInfo, DebugUtil::FunctionExit, "changeFocus" );
            return;
        }

        // if it is transactive window then we'll post a message to the window thread, otherwise, 
        // we'll just do it from this thread
        bool isTransActiveWnd = dynamic_cast<TransActiveDialog*>(m_application) ||
                                dynamic_cast<TransActiveFrame*>(m_application) ||
                                dynamic_cast<TransActiveMDIFrame*>(m_application) ||
                                dynamic_cast<TransActivePropertySheet*>(m_application);

        if (isTransActiveWnd)
        {
            EWindowPosition foc;
            switch(focus)
            {
            case(TA_Base_Core::Minimise):
                foc = MINIMISE;
                break;

            case(TA_Base_Core::Maximise):
                foc = MAXIMISE;
                break;

            case(TA_Base_Core::GetFocus):
                foc = FOCUS;
                break;

            default:
                TA_ASSERT(false, "Invalid enumerated type passed for window focus change");
            }
            m_application->PostMessage(WM_SET_WINDOW_POSITION, NULL, foc);

            LOG( SourceInfo, DebugUtil::FunctionExit, "changeFocus" );
            return;
        }

        switch(focus)
        {
        case(TA_Base_Core::Minimise):
            // If window is not already minimised then minimise it
            if(!m_application->IsIconic())
            {
            	m_application->ShowWindow(SW_MINIMIZE);
            }
            break;

        case(TA_Base_Core::Maximise):
           	m_application->ShowWindow(SW_MAXIMIZE);
            m_application->SetForegroundWindow();
            // Set the window to be the top most so it is given focus and then move it back again.
            m_application->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOSENDCHANGING);
            m_application->SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOSENDCHANGING);
            break;

        case(TA_Base_Core::GetFocus):
            if(m_application->IsIconic())
            {
            	m_application->ShowWindow(SW_SHOWNORMAL);
            }
            m_application->SetForegroundWindow();
            // Set the window to be the top most so it is given focus and then move it back again.
            m_application->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOSENDCHANGING);
            m_application->SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOSENDCHANGING);
            break;

        default:
            TA_ASSERT(false, "Invalid enumerated type passed for window focus change");
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "changeFocus" );
    }

    void AbstractGUIApplication::dutyChanged()
    {
        // This method is a stub to prevent applications from having to support a method they don't need. 
        // This stub will be removed as of IT21.
    }


    void AbstractGUIApplication::terminateFromControlStation()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "terminateFromControlStation" );

        this->terminateGUI();

        LOG( SourceInfo, DebugUtil::FunctionExit, "terminateFromControlStation" );
    }


    void AbstractGUIApplication::terminateFromUser()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "terminateFromUser" );

        prepareForClose();

        LOG( SourceInfo, DebugUtil::FunctionExit, "terminateFromUser" );
    }


    void AbstractGUIApplication::terminateGUI()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "terminateGUI" );

        prepareForClose();

        // Close all transactive popup messages
        TransActiveMessage::closeAllMessages();

        // Closing the main window may not be enough if there are some other
        // modal (or modeless) dialogs floating around...
        // So we enumerate all top level windows. The enumeration handler
        // will determine if they were created by our process, and if so
        // it will destroy them.
        ::EnumWindows(WndEnumHandler, ::GetCurrentProcessId());

        LOG( SourceInfo, DebugUtil::FunctionExit, "terminateGUI" );
    }


    BOOL CALLBACK AbstractGUIApplication::WndEnumHandler(HWND hWnd, LPARAM lParam)
    {
        // lparam contains the current process' ID
        // this saves us having to query it for every call...
        DWORD dwOurProcessID = static_cast<DWORD>(lParam);

        // find out which process & thread created this window
        DWORD dwThisWindowProcessID;
        DWORD dwThisWindowThreadID;
        dwThisWindowThreadID = ::GetWindowThreadProcessId(hWnd, &dwThisWindowProcessID);

        // if it's our process
        if (dwOurProcessID == dwThisWindowProcessID)
        {
	        // Close the window...
	        // The method to use depends on the type of window:
	        // modal dialogs should be ended with ::EndDialog(),
	        // other windows with ::DestroyWindow().
	        // I haven't found a fireproof way of determining
	        // if a window is a modal dialog!
	        // Calling ::EndDialog() on a modeless window is OK, as
	        // long as we also call ::DestroyWindow() after.
	        // ::EndDialog(hWnd,0); // cause hang sometimes
	        ::DestroyWindow(hWnd);

	        // Our GUIs windows should PostQuitMessage() when they
	        // get a WM_DESTROY but I don't trust them to do so,
	        // so let's do it for them...
	        ::PostThreadMessage(dwThisWindowThreadID,WM_QUIT,0,0);
        }
        
        return TRUE;
    }


    void AbstractGUIApplication::setPosition(  const unsigned long posFlag,
                                               const unsigned long alignFlag,
                                               const RECT& objectDim,
                                               const RECT& boundaryDim)
    {
		FUNCTION_ENTRY("setPosition");

		// The GUI should just be given focus.
		if (posFlag == POS_NONE)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Will give focus to GUI, rather than moving it");
			changeFocus(TA_Base_Core::GetFocus);

			FUNCTION_EXIT;
			return;
		}

        // If this is the first time we're displaying the rect or if any of the flags/rects
        // are different to the current ones, then we'll have to reset everything and 
        // recalculate the position.
        if (((posFlag != POS_NONE) && (m_positionInfo.posFlag != posFlag)) ||
           (m_positionInfo.alignFlag != alignFlag) ||
           (!areRectsEqual(m_positionInfo.objectDim, objectDim)) ||
           (!areRectsEqual(m_positionInfo.boundaryDim, boundaryDim)))
        {
            m_positionInfo.posFlag = posFlag;
            m_positionInfo.alignFlag = alignFlag;
            m_positionInfo.objectDim = objectDim;
            m_positionInfo.boundaryDim = boundaryDim;
            // reset the application dimension
            m_positionInfo.appDim = DEFAULT_RECT;
            setWindowPosition();

			FUNCTION_EXIT;
            return;
        }

        // if the new position is exactly the same as the old one, then we don't have
        // to recalculate the position again.  We'll only move the window if it's equal
        // to the default rect (which is what we set the appDim to when the application is
        // not meant to be repositioned)
        moveWindow();

		FUNCTION_EXIT;
    }


    void AbstractGUIApplication::setWindowPosition()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "setWindowPosition" );

        // If the application is null then we are trying to move it before it has
        // set up and initialised correctly. We will just return.
        if ((m_application == NULL) || (m_application->m_hWnd == NULL) || !::IsWindow(m_application->m_hWnd))
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,"setWindowPosition() was called on the application before it was initialised correctly. It will not change focus");
            LOG( SourceInfo, DebugUtil::FunctionExit, "setWindowPosition" );
            return;
        }

        RECT windowRect;
        m_application->GetWindowRect(&windowRect);

        bool isRepositioned = TA_Base_Bus::AppLauncher::getInstance().calculateNewCoordinates(windowRect, 
                                                            m_positionInfo.objectDim, 
                                                            m_positionInfo.boundaryDim, 
                                                            m_positionInfo.posFlag, 
                                                            m_positionInfo.alignFlag);

        // stores the new window dimension
        m_positionInfo.appDim = isRepositioned ? windowRect : DEFAULT_RECT;
        moveWindow();
        m_isFirst = false;

        LOG( SourceInfo, DebugUtil::FunctionExit, "setWindowPosition" );
    }


    void AbstractGUIApplication::moveWindow()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "moveWindow" );

		//Maochun Sun++
		//TD12779
		if ((m_application == NULL) || (m_application->m_hWnd == NULL) || !::IsWindow(m_application->m_hWnd))
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
				"moveWindow() was called on the application before it was initialised correctly.");
            LOG( SourceInfo, DebugUtil::FunctionExit, "moveWindow" );
            return;
        }
		//++Maochun Sun
		//TD12779		

        // see if the application window inherits from one of our TransActive classes.  If it is, then
        // we can safely send the WM_SET_WINDOW_POSITION message to the window which will have the 
        // appropriate handler for handing it.  Otherwise, if it is not a TransActive window, 
        // then we'll have to reposition the window from this thread.
        bool isTransActiveWnd = dynamic_cast<TransActiveDialog*>(m_application) ||
                                dynamic_cast<TransActiveFrame*>(m_application) ||
                                dynamic_cast<TransActiveMDIFrame*>(m_application) ||
                                dynamic_cast<TransActivePropertySheet*>(m_application);

        // if its coordinates are changed, then move to the new coordinates, otherwise, centre the
        // application without changing the size of the application.
        if (!areRectsEqual(m_positionInfo.appDim, DEFAULT_RECT))
        {
            RECT& windowRect = m_positionInfo.appDim;
            if (isTransActiveWnd)
            {
                // Have to use post message rather than send message 'cos we want it to fully repositioned itself
                // before giving it focus.
				m_application->SendMessage(WM_SET_WINDOW_POSITION, reinterpret_cast<WPARAM>(&windowRect), REPOSITION);
				
            }
            else
            {
				m_application->MoveWindow(&windowRect);
                changeFocus(TA_Base_Core::GetFocus);

            }
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Moved to: (%li,%li,%li,%li)", windowRect.left, windowRect.top, windowRect.right, windowRect.bottom);
            LOG( SourceInfo, DebugUtil::FunctionExit, "moveWindow" );
            return;
        }

        // not going to reposition it.  If it's the first time, then we'll centre it.  If it's the subsequent time
        // then we won't bother
        if (isTransActiveWnd)
        {
            // send the set window message to the main window thread.  Have to wait for the set 
            // window position method to finish 'cos we want the new position
            m_application->SendMessage(WM_SET_WINDOW_POSITION, NULL, m_isFirst?CENTRE:NO_REPOSITION);
        }
        else
        {
            if (m_isFirst)
            {
                m_application->CenterWindow(CWnd::GetDesktopWindow());
            }
            changeFocus(TA_Base_Core::GetFocus);
        }
        LOG( SourceInfo, DebugUtil::FunctionExit, "moveWindow" );
    }

	
    RECT AbstractGUIApplication::getWindowCoordinates()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "getWindowCoordinates" );
        RECT rect;
        m_application->GetWindowRect(&rect);
        LOG( SourceInfo, DebugUtil::FunctionExit, "getWindowCoordinates" );
        return rect;
    }

    
    void AbstractGUIApplication::loadPositionSettings()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "loadPositionSettings" );

        if ( m_userSettings == NULL )
        {
            m_userSettings = new TA_Base_Bus::SettingsMgr(
                    getSessionId(), getApplicationType());
            if (m_userSettings == NULL)
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new on SettingsMgr returned NULL");
                return;
            }
        }

        m_userSettings->restoreWindowPlacement(m_application);

        LOG( SourceInfo, DebugUtil::FunctionExit, "loadPositionSettings" );
    }


    void AbstractGUIApplication::savePositionSettings()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "savePositionSettings" );

        if ( m_userSettings == NULL )
        {
            m_userSettings = new TA_Base_Bus::SettingsMgr(getSessionId(),getApplicationType());
            if (m_userSettings == NULL)
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "new on SettingsMgr returned NULL");
                return;
            }
        }

        m_userSettings->saveWindowPlacement(m_application);

        LOG( SourceInfo, DebugUtil::FunctionExit, "savePositionSettings" );
    }


    void AbstractGUIApplication::setUserSetting(TA_Base_Bus::SettingsMgr& userSettings)
    {
        // prevent memory leak - if the new object is not the same as the
        // old one, we need to delete the old one 'cos we're responsible for
        // deleting m_userSettings.
        if ((m_userSettings != NULL) && (m_userSettings != &userSettings))
        {
            delete m_userSettings;
            m_userSettings = NULL;
        }
        m_userSettings = &userSettings;
    }
}

