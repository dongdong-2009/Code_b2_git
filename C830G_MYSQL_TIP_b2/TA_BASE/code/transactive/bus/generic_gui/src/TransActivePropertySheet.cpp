/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_gui/src/TransActivePropertySheet.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This inherits from CPropertySheet and implements all common functionality for
  * TransActive property sheets.
  */

#pragma warning(disable:4786)

#include "bus/generic_gui/src/TransActivePropertySheet.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/IGUIAccess.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/resources/resource.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/exceptions/src/ApplicationException.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using namespace TA_Base_Bus::TA_GenericGui;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define APPCOMMAND 0xFFF0

namespace TA_Base_Bus
{

   IMPLEMENT_DYNAMIC(TransActivePropertySheet, CPropertySheet)

   /////////////////////////////////////////////////////////////////////////////
    // CEventsGuiDlg dialog

    TransActivePropertySheet::TransActivePropertySheet(TA_Base_Bus::IGUIAccess& genericGUICallback,
                                         UINT captionId,
                                         CWnd* pParent /*=NULL*/)
        : m_genericGUICallback(genericGUICallback),
          m_arePropertiesSet(false),
          CPropertySheet(captionId, pParent)

    {
        m_resizingProperties.canMaximise = true;
        m_resizingProperties.maxWidth = -1;
        m_resizingProperties.maxHeight = -1;
        m_resizingProperties.minWidth = -1;
        m_resizingProperties.minHeight = -1;
        m_workingArea.SetRectEmpty();
    }


    void TransActivePropertySheet::setResizingProperties(const ResizingProperties& resizingProperties)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "setResizingProperties");
        m_resizingProperties = resizingProperties;
        m_arePropertiesSet = true;
        LOG( SourceInfo, DebugUtil::FunctionExit, "setResizingProperties");
    }


    IGUIAccess& TransActivePropertySheet::getGenericGUICallbackObject()
    {
        return m_genericGUICallback;
    }


BEGIN_MESSAGE_MAP(TransActivePropertySheet, CPropertySheet)
    //{{AFX_MSG_MAP(TransActivePropertySheet)
    ON_WM_SYSCOMMAND()
    ON_COMMAND(IDOK, OnOK)
    ON_COMMAND(IDCANCEL, OnCancel)
    ON_WM_GETMINMAXINFO()
    ON_WM_WINDOWPOSCHANGING()
    ON_REGISTERED_MESSAGE(WM_SET_WINDOW_POSITION, OnSetWindowPosition)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


    /////////////////////////////////////////////////////////////////////////////
    // TransActivePropertySheet message handlers

    BOOL TransActivePropertySheet::OnInitDialog()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "OnInitDialog");

        TA_ASSERT(m_arePropertiesSet, "setResizingProperties must be called in the constructor of the dialog");

        CPropertySheet::OnInitDialog();

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

        // Load any user settings and position the application
        try
        {
            m_genericGUICallback.loadAllUserSettings();
        }
            catch (const TA_Base_Core::UserSettingsException&)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "UserSettingsException", "Exception caught while trying to load the user settings. Defaults will be used instead");
        }

        LOG( SourceInfo, DebugUtil::FunctionExit, "OnInitDialog");
        return TRUE;  // return TRUE  unless you set the focus to a control
    }


    void TransActivePropertySheet::OnSysCommand(UINT nID, LPARAM lParam)
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
	        CPropertySheet::OnSysCommand(nID, lParam);
        }
    }


    void TransActivePropertySheet::OnOK()
    {
        //Do Nothing
    }


    void TransActivePropertySheet::OnCancel()
    {
        //Do Nothing
    }


    BOOL TransActivePropertySheet::DestroyWindow() 
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "DestroyWindow");
         m_genericGUICallback.terminateFromUser();
        
        LOG( SourceInfo, DebugUtil::FunctionExit, "DestroyWindow");
        return CPropertySheet::DestroyWindow();
    }


    void TransActivePropertySheet::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
    {
        CPropertySheet::OnGetMinMaxInfo(lpMMI);

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

    
    void TransActivePropertySheet::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
    {
        CPropertySheet::OnWindowPosChanging(lpwndpos);

        // If the SWP_NOSIZE bit is set then we don't want to check the sizing as it is
        // a waste of time. NB: remove this to enforce the dragging of the window within a bounding box 
        // (not required as yet, so left in).
        if (lpwndpos->flags & SWP_NOSIZE)
        {
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


    LRESULT TransActivePropertySheet::OnSetWindowPosition(WPARAM wParam, LPARAM lParam)
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
}
