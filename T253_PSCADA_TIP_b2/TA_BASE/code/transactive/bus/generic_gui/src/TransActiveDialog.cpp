/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/bus/generic_gui/src/TransActiveDialog.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2018/01/26 22:10:04 $
  * Last modified by:  $Author: limin.zhu $
  *
  * This inherits from CDialog and implements all common functionality for
  * TransActive dialogs..
  */

#pragma warning(disable:4786)

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/IGUIAccess.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/resources/resource.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
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
    /////////////////////////////////////////////////////////////////////////////
    // CEventsGuiDlg dialog

    TransActiveDialog::TransActiveDialog(TA_Base_Bus::IGUIAccess& genericGUICallback,
                                         const UINT dialogId,
                                         CWnd* pParent /*=NULL*/)
        : m_genericGUICallback(genericGUICallback),
          m_arePropertiesSet(false),
          CDialog(dialogId, pParent)

    {
        m_resizingProperties.canMaximise = true;
        m_resizingProperties.maxWidth = -1;
        m_resizingProperties.maxHeight = -1;
        m_resizingProperties.minWidth = -1;
        m_resizingProperties.minHeight = -1;
        m_workingArea.SetRectEmpty();
    }

    void TransActiveDialog::setResizingProperties(const ResizingProperties& resizingProperties)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "setResizingProperties");
        m_resizingProperties = resizingProperties;
        m_arePropertiesSet = true;
        LOG( SourceInfo, DebugUtil::FunctionExit, "setResizingProperties");
    }

    IGUIAccess& TransActiveDialog::getGenericGUICallbackObject()
    {
        return m_genericGUICallback;
    }

BEGIN_MESSAGE_MAP(TransActiveDialog, CDialog)
    //{{AFX_MSG_MAP(TransActiveDialog)
    ON_WM_SYSCOMMAND()
    ON_COMMAND(IDOK, OnOK)
    ON_COMMAND(IDCANCEL, OnCancel)
    ON_WM_GETMINMAXINFO()
    ON_WM_WINDOWPOSCHANGING()
    ON_WM_CLOSE()
    ON_REGISTERED_MESSAGE(WM_SET_WINDOW_POSITION, OnSetWindowPosition)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // TransActiveDialog message handlers

    BOOL TransActiveDialog::OnInitDialog()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "OnInitDialog");

        TA_ASSERT(m_arePropertiesSet, "setResizingProperties must be called in the constructor of the dialog");

        CDialog::OnInitDialog();

        // Add "About..." menu item to system menu.

        CMenu* pSysMenu = GetSystemMenu(FALSE);
        if (pSysMenu != NULL)
        {
	        CString strAboutMenu = "¹ØÓÚ";
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

    void TransActiveDialog::OnSysCommand(UINT nID, LPARAM lParam)
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
	        CDialog::OnSysCommand(nID, lParam);
        }
    }

    void TransActiveDialog::OnOK()
    {
        //Do Nothing
    }

    void TransActiveDialog::OnCancel()
    {
        //Do Nothing
    }

    void TransActiveDialog::OnClose()
    {
		//TD18095, jianghp, to fix the performance of showing manager application
        //CDialog::OnCancel();
		DestroyWindow();
    }

    BOOL TransActiveDialog::DestroyWindow()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "DestroyWindow");
        m_genericGUICallback.terminateFromUser();

        LOG( SourceInfo, DebugUtil::FunctionExit, "DestroyWindow");
        return CDialog::DestroyWindow();
    }

    void TransActiveDialog::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
    {
        CDialog::OnGetMinMaxInfo(lpMMI);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "--------OnGetMinMaxInfo");

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
            lpMMI->ptMaxPosition.y = m_workingArea.TopLeft().y; // relative (to current monitor) position
        }

        // Set the maximum size the window can be resized to
        // If both the resizing properties and the working area are valid, then the max size is the smallest of the two
        if (m_resizingProperties.maxWidth >= 0 && !m_workingArea.IsRectEmpty())
        {
            lpMMI->ptMaxTrackSize.x = std::min<size_t>(m_resizingProperties.maxWidth,m_workingArea.Width());
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
            lpMMI->ptMaxTrackSize.y = std::min<size_t>(m_resizingProperties.maxHeight,m_workingArea.Height());
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

    void TransActiveDialog::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos)
    {
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "----------OnWindowPosChanging_Entry");
        CDialog::OnWindowPosChanging(lpwndpos);

        // If the SWP_NOSIZE bit is set then we don't want to check the sizing as it is
        // a waste of time.
        if ( ( lpwndpos->flags & SWP_NOSIZE ) || ( lpwndpos->flags == (SWP_NOZORDER | SWP_NOOWNERZORDER) ) )
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Moved the window SWP_NOSIZE or SWP_NOZORDER|SWP_NOOWNERZORDER");
			CRect windowRect;
			getWorkArea();
			moveToWorkArea(windowRect);

			//we need only the top position, to limited the window in the work area always
			if ( windowRect.top != -1 )
			{
				lpwndpos->y = windowRect.top;
			}

            return;
        }

        if ( (m_resizingProperties.minWidth >= 0) && (lpwndpos->cx < m_resizingProperties.minWidth) )
        {
            lpwndpos->cx = m_resizingProperties.minWidth;
        }
        else if ( (m_resizingProperties.maxWidth >= 0) && (lpwndpos->cx > m_resizingProperties.maxWidth) )
        {
            lpwndpos->cx = m_resizingProperties.maxWidth;
        }

        if ( (m_resizingProperties.minHeight >= 0) && (lpwndpos->cy < m_resizingProperties.minHeight) )
        {
            lpwndpos->cy = m_resizingProperties.minHeight;
        }
        else if ( (m_resizingProperties.maxHeight >= 0) && (lpwndpos->cy > m_resizingProperties.maxHeight) )
        {
            lpwndpos->cy = m_resizingProperties.maxHeight;
        }
    }

    LRESULT TransActiveDialog::OnSetWindowPosition(WPARAM wParam, LPARAM lParam)
    {
		CWnd* childWindow=NULL;
        if (lParam == MINIMISE)
        {
            ShowWindow(SW_MINIMIZE);
            return true;     // just return straightaway
        }

        if(lParam!=REPOSITION_NO_RESTORE)
        {
            if(IsIconic())
            {
                ShowWindow(SW_RESTORE);
            }
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
        case REPOSITION_NO_RESTORE:
            {
                RECT* rect = reinterpret_cast<RECT*>(wParam);

                TA_ASSERT(rect != NULL, "Cannot reposition window with NULL rect");
                TA_ASSERT((rect->right >= rect->left ) && (rect->bottom >= rect->top), "The rect is not valid");
                MoveWindow(rect);

				childWindow = GetLastActivePopup();
				if(childWindow->GetDlgCtrlID()!=this->GetDlgCtrlID())
				{
					childWindow->CenterWindow(this);
				}
            }
            break;
        case CENTRE:
            CenterWindow(CWnd::GetDesktopWindow());
            break;
        default:
            TA_ASSERT(false, "Invalid enumerated type passed for setting window position");
        }
		if(childWindow!=NULL && childWindow->GetDlgCtrlID()!=this->GetDlgCtrlID())
		{
			childWindow->SetForegroundWindow();
			// Set the child window to be the top most so it is given focus and then move it back again.
			childWindow->SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOSENDCHANGING);
			childWindow->SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOSENDCHANGING);
		}
		else
		{
			SetForegroundWindow();
			// Set the window to be the top most so it is given focus and then move it back again.
			SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOSENDCHANGING);
			SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE|SWP_NOSENDCHANGING);
		}
        return true;
    }
    //  [1/12/2009] :zhangziliang
	void TransActiveDialog::getWorkArea()
	{
		FUNCTION_ENTRY("getWorkArea");
		// To work out that space, need to know which screen the current window is in.
        WINDOWPLACEMENT place;
        GetWindowPlacement(&place);

        int screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        int numScreens = GetSystemMetrics(SM_CMONITORS);

        // Determine the screen we are currently on.
        int currentScreen = place.rcNormalPosition.left / (screenWidth / numScreens);

		LOG2(SourceInfo, DebugUtil::DebugInfo, "screenWidth = %d numScreens = %d", screenWidth , numScreens);

        // If we have changed screen, then attempt to retrieve the available working area.

        m_currentScreen = currentScreen;
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "m_currentScreen = %d" , m_currentScreen);
        // Get the available working area from the control station
        try
        {
            m_workingArea = AppLauncher::getInstance().getRect(SCREEN_CURRENT, AREA_SCHEMATIC, place.rcNormalPosition.left);
			LOG4(SourceInfo, DebugUtil::DebugInfo, "----------X=%d,Y=%d,CX=%d,CY=%d",m_workingArea.left,m_workingArea.top,m_workingArea.Width(),m_workingArea.Height());
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

	void TransActiveDialog::moveToWorkArea(CRect& windowRect)
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

		if( 0 == m_workingArea.top && 0 == m_workingArea.left && 0 == m_workingArea.right && 0 == m_workingArea.bottom)
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

	void TransActiveDialog::resizeCommsWindowSize()
	{
        CRect windowSize;
        GetWindowRect(&windowSize);
        TA_Base_Bus::ResizingProperties properties;
        properties.canMaximise = false;
        properties.maxHeight = -1;
        properties.maxWidth = -1;
        properties.minHeight = windowSize.bottom - windowSize.top;
        properties.minWidth = windowSize.right - windowSize.left;
        setResizingProperties(properties);
	}
}
