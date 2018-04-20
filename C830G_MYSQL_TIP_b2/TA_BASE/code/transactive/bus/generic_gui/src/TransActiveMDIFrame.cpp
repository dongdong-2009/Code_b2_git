/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Karen Graham
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This inherits from CMDIFrameWnd and implements all common functionality for
  * TransActive document MDIFrames.
  */

#include "bus/generic_gui/src/stdafx.h"
#include "bus/generic_gui/src/TransActiveMDIFrame.h"
#include "bus/generic_gui/src/IGUIAccess.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/resources/resource.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/exceptions/src/ApplicationException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using namespace TA_Base_Bus::TA_GenericGui;

#define APPCOMMAND 0xFFF0

namespace TA_Base_Bus
{
    /////////////////////////////////////////////////////////////////////////////
    // TransActiveMDIFrame

    IMPLEMENT_DYNCREATE(TransActiveMDIFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(TransActiveMDIFrame, CMDIFrameWnd)
    //{{AFX_MSG_MAP(TransActiveMDIFrame)
    ON_WM_CLOSE()
    ON_WM_SYSCOMMAND()
    ON_WM_GETMINMAXINFO()
    ON_WM_WINDOWPOSCHANGING()
    ON_REGISTERED_MESSAGE(WM_SET_WINDOW_POSITION, OnSetWindowPosition)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // TransActiveMDIFrame construction/destruction

    TransActiveMDIFrame::TransActiveMDIFrame() : m_genericGUICallback(NULL), m_arePropertiesSet(false)
    {
        m_resizingProperties.canMaximise = true;
        m_resizingProperties.maxWidth = -1;
        m_resizingProperties.maxHeight = -1;
        m_resizingProperties.minWidth = -1;
        m_resizingProperties.minHeight = -1;	        
        m_workingArea.SetRectEmpty();
    }


    TransActiveMDIFrame::~TransActiveMDIFrame()
    {
    }


    void TransActiveMDIFrame::setResizingProperties(const ResizingProperties& resizingProperties)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "setResizingProperties");
        m_resizingProperties = resizingProperties;
        m_arePropertiesSet = true;
        LOG( SourceInfo, DebugUtil::FunctionExit, "setResizingProperties");
    }

        
    void TransActiveMDIFrame::setCallbackObject( IGUIAccess& gui)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "setCallbackObject");

        TA_ASSERT(m_arePropertiesSet, "setResizingProperties must be called in the constructor of the MDIFrame");

        m_genericGUICallback = &gui;

        // Load any user settings and position the application
        try
        {
            m_genericGUICallback->loadAllUserSettings();
        }
        catch (const TA_Base_Core::UserSettingsException&)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "UserSettingsException", "Exception caught while trying to load the user settings. Defaults will be used instead");
        }


        // Add "About..." menu item to system menu.
        CMenu* pSysMenu = GetSystemMenu(FALSE);
        if (pSysMenu != NULL)
        {
	        CString strAboutMenu = "About ";
			strAboutMenu += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            strAboutMenu += "...";
	        pSysMenu->AppendMenu(MF_SEPARATOR);
	        pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "setCallbackObject");
    }


    IGUIAccess& TransActiveMDIFrame::getGenericGUICallbackObject()
    {
        TA_ASSERT(m_genericGUICallback != NULL, "GUI Callback object is NULL");
        return *m_genericGUICallback;
    }

    /////////////////////////////////////////////////////////////////////////////
    // TransActiveMDIFrame message handlers


    void TransActiveMDIFrame::OnClose() 
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "OnClose");

        if (m_genericGUICallback != NULL)
        {
             m_genericGUICallback->terminateFromUser();
        }
        CMDIFrameWnd::OnClose();

        LOG( SourceInfo, DebugUtil::FunctionExit, "OnClose");
    }


    void TransActiveMDIFrame::OnSysCommand(UINT nID, LPARAM lParam) 
    {
        // In WM_SYSCOMMAND messages, the four low-order bits of the nID parameter 
        // are used internally by Windows CE. When an application tests the value of 
        // nID, it must combine the value 0xFFF0 with the nID value by using the 
        // bitwise-AND operator to obtain the correct result.
        if ((nID & APPCOMMAND) == IDM_ABOUTBOX)
        {
            HelpLauncher::getInstance().displayAboutBox();
        }
        else
        {
	        CMDIFrameWnd::OnSysCommand(nID, lParam);
        }
        
    }


    void TransActiveMDIFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
    {
        CMDIFrameWnd::OnGetMinMaxInfo(lpMMI);

        // Set the minimum size the window can be resized to
        if (m_resizingProperties.minWidth >= 0)
        {
            lpMMI->ptMinTrackSize.x = m_resizingProperties.minWidth;
        }

        if (m_resizingProperties.minHeight >= 0)
        {
            lpMMI->ptMinTrackSize.y = m_resizingProperties.minHeight;
        }

        // Maximum size is restricted by the size of the available workstation screen (i.e. the parts not
        // taken up by banner and/or control station bar)

        // To work out that space, need to know which screen the current window is in.
        WINDOWPLACEMENT place;
        GetWindowPlacement(&place);        

        int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int numScreens = GetSystemMetrics(SM_CMONITORS);

        // Determine the screen we are currently on.
        int currentScreen = place.rcNormalPosition.left / (screenWidth / numScreens);

        // If we have changed screen, then attempt to retrieve the available working area.

        if (currentScreen != m_currentScreen)
        {
            m_currentScreen = currentScreen;
            // Get the available working area from the control station
            try
            {
                m_workingArea = AppLauncher::getInstance().getRect(SCREEN_CURRENT, AREA_SCHEMATIC, place.rcNormalPosition.left);
            }
            catch(TA_Base_Core::ApplicationException& ex)
            {
                // Log the catch, and set the workingArea rect to 0.
                std::stringstream errorMessage;
                errorMessage << "An exception was caught while attempting to retrieve the schematic area of the current"
                    << "screen. X coord = " << place.rcNormalPosition.left << ". Error Message:";
                LOG_EXCEPTION_CATCH(SourceInfo,"ApplicationException",errorMessage.str().c_str());
                LOGMORE(SourceInfo,ex.what());

                // Set the workingArea rect to 0
                m_workingArea.SetRectEmpty();
            }
        }

        // Set the max position to be the top left of the working area
        if (!m_workingArea.IsRectEmpty())
        {
            lpMMI->ptMaxPosition = m_workingArea.TopLeft();
        }

        // Set the maximum size the window can be resized to
        // If both the resizing properties and the working area are valid, then the max size is the smallest of the two
        if (m_resizingProperties.maxWidth >= 0 && !m_workingArea.IsRectEmpty())
        {
            lpMMI->ptMaxTrackSize.x = min(m_resizingProperties.maxWidth,m_workingArea.Width());
        }
        // But if only one is valid, then the max size is the one that is valid
        else if (m_resizingProperties.maxWidth >= 0 || !m_workingArea.IsRectEmpty())
        {
            lpMMI->ptMaxTrackSize.x = 
                m_workingArea.IsRectEmpty() ? m_resizingProperties.maxWidth : m_workingArea.Width();
        }
        // If neither is valid, then we don't set the maxt track size

        // Do the same for the height
        if (m_resizingProperties.maxHeight >= 0 && !m_workingArea.IsRectEmpty())
        {
            lpMMI->ptMaxTrackSize.y = min(m_resizingProperties.maxHeight,m_workingArea.Height());
        }
        else if (m_resizingProperties.maxHeight >= 0 || !m_workingArea.IsRectEmpty())
        {
            lpMMI->ptMaxTrackSize.y = 
                m_workingArea.IsRectEmpty() ? m_resizingProperties.maxHeight : m_workingArea.Height();
        }

        // If we cannot maximise then leave the size the same as the current size
        if(!m_resizingProperties.canMaximise)
        {
            lpMMI->ptMaxPosition.x = place.rcNormalPosition.left;
            lpMMI->ptMaxPosition.y = place.rcNormalPosition.top;
            lpMMI->ptMaxSize.x = place.rcNormalPosition.right - place.rcNormalPosition.left;
            lpMMI->ptMaxSize.y = place.rcNormalPosition.bottom - place.rcNormalPosition.top;
        }
        else
        {
            lpMMI->ptMaxSize.x = lpMMI->ptMaxTrackSize.x;
            lpMMI->ptMaxSize.y = lpMMI->ptMaxTrackSize.y;
        }
    }

    
    void TransActiveMDIFrame::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
    {
        CMDIFrameWnd::OnWindowPosChanging(lpwndpos);

        // If the SWP_NOSIZE bit is set then we don't want to check the sizing as it is
        // a waste of time. NB: remove this to enforce the dragging of the window within a bounding box 
        // (not required as yet, so left in).
        if ( ( lpwndpos->flags & SWP_NOSIZE ) || ( lpwndpos->flags == (SWP_NOZORDER | SWP_NOOWNERZORDER) ) )
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Moved the window SWP_NOSIZE");		
			CRect windowRect;
			getWorkArea();			
			moveToWorkArea(windowRect);	
			
			//we need only the top position, to limited the window in the work area always
			if(windowRect.top != -1)
			{
				lpwndpos->y = windowRect.top;
			}
            return;
        }

        if( (m_resizingProperties.minWidth >=0) && (lpwndpos->cx < m_resizingProperties.minWidth) )
        {
            lpwndpos->cx = m_resizingProperties.minWidth;
        }
        else if( (m_resizingProperties.maxWidth >=0) && (lpwndpos->cx > m_resizingProperties.maxWidth) )
        {
            lpwndpos->cx = m_resizingProperties.maxWidth;
        }

        if( (m_resizingProperties.minHeight >=0) && (lpwndpos->cy < m_resizingProperties.minHeight) )
        {
            lpwndpos->cy = m_resizingProperties.minHeight;
        }
        else if( (m_resizingProperties.maxHeight >=0) && (lpwndpos->cy > m_resizingProperties.maxHeight) )
        {
            lpwndpos->cy = m_resizingProperties.maxHeight;
        }
    }


    LRESULT TransActiveMDIFrame::OnSetWindowPosition(WPARAM wParam, LPARAM lParam)
    {
        if (lParam == MINIMISE)
        {
            ShowWindow(SW_MINIMIZE);
            return true;     // just return straightaway
        }

        if(IsIconic())
        {
            ShowWindow(SW_RESTORE);
        }

        switch (lParam)
        {
        case MAXIMISE:
            ShowWindow(SW_MAXIMIZE);
            break;
        case FOCUS:
        case NO_REPOSITION:
            break;
        case REPOSITION:
            {
                RECT* rect = reinterpret_cast<RECT*>(wParam);
                TA_ASSERT(rect != NULL, "Cannot reposition window with NULL rect");
                TA_ASSERT((rect->right >= rect->left ) && (rect->bottom >= rect->top), "The rect is not valid");
                MoveWindow(rect);
            }
            break;
        case CENTRE:
            CenterWindow(CWnd::GetDesktopWindow());
            break;
        default:
            TA_ASSERT(false, "Invalid enumerated type passed for setting window position");
        }

        SetForegroundWindow();
        // Set the window to be the top most so it is given focus and then move it back again.
        SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOSENDCHANGING);
        SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOSENDCHANGING);
        return true;
    }

	void TransActiveMDIFrame::getWorkArea()
	{
		FUNCTION_ENTRY("getWorkArea");
		// To work out that space, need to know which screen the current window is in.
        WINDOWPLACEMENT place;
        GetWindowPlacement(&place);
		
        int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int numScreens = GetSystemMetrics(SM_CMONITORS);
		
        // Determine the screen we are currently on.
        int currentScreen = place.rcNormalPosition.left / (screenWidth / numScreens);
		
		LOG2(SourceInfo, DebugUtil::DebugDebug, "screenWidth = %d numScreens = %d", screenWidth , numScreens);
		
        // If we have changed screen, then attempt to retrieve the available working area.		
		
        m_currentScreen = currentScreen;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "m_currentScreen = %d" , m_currentScreen);
        // Get the available working area from the control station
        try
        {
            m_workingArea = AppLauncher::getInstance().getRect(SCREEN_CURRENT, AREA_SCHEMATIC, place.rcNormalPosition.left);
			LOG4(SourceInfo, DebugUtil::DebugDebug, "----------X=%d,Y=%d,CX=%d,CY=%d",m_workingArea.left,m_workingArea.top,m_workingArea.Width(),m_workingArea.Height());
        }
        catch(TA_Base_Core::ApplicationException& ex)
        {
            // Log the catch, and set the workingArea rect to 0.
            std::stringstream errorMessage;
            errorMessage << "An exception was caught while attempting to retrieve the schematic area of the current"
                << "screen. X coord = " << place.rcNormalPosition.left << ". Error Message:";
            LOG_EXCEPTION_CATCH(SourceInfo,"ApplicationException",errorMessage.str().c_str());
            LOGMORE(SourceInfo,ex.what());
			
            // Set the workingArea rect to 0
            m_workingArea.SetRectEmpty();
        }
		
		FUNCTION_EXIT;
	}
	
	void TransActiveMDIFrame::moveToWorkArea(CRect& windowRect)
	{
		FUNCTION_ENTRY("moveToWorkArea");
		
		RECT windowOldRect, newWindowRect;
		GetWindowRect(&windowOldRect);
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		
		int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
		int captionHeight = GetSystemMetrics(SM_CYCAPTION);
		int SingleScreenWidth = 1280;		
		int currentScreen = (cursorPos.x + 1)/SingleScreenWidth;
		int windowHeight = windowOldRect.bottom - windowOldRect.top;
		
		//the follow line is center window to the screen which cursor in
		//newWindowRect.left = (currentScreen * SingleScreenWidth ) + ((SingleScreenWidth - (windowOldRect.right - windowOldRect.left)) / 2);		
		if(0 == m_workingArea.top && 0 == m_workingArea.left && 0 == m_workingArea.right && 0 == m_workingArea.bottom)
		{
			newWindowRect.top = -1;
		}
		else if(cursorPos.y < m_workingArea.top ||  cursorPos.y - captionHeight < m_workingArea.top)
		{
			newWindowRect.top = m_workingArea.top;
		}
		else if(cursorPos.y + windowHeight > m_workingArea.bottom || cursorPos.y + windowHeight + captionHeight > m_workingArea.bottom )
		{
			newWindowRect.top = m_workingArea.bottom - windowHeight;
		}
		else
		{
			newWindowRect.top = -1;
		}
		newWindowRect.left = windowOldRect.left;
		newWindowRect.right = newWindowRect.left + (windowOldRect.right - windowOldRect.left);
		newWindowRect.bottom = newWindowRect.top + (windowOldRect.bottom - windowOldRect.top);		
		
		windowRect.left = newWindowRect.left;
		windowRect.top = newWindowRect.top;
		windowRect.right = newWindowRect.right;
		windowRect.bottom = newWindowRect.bottom;
		FUNCTION_EXIT;
	}
}
