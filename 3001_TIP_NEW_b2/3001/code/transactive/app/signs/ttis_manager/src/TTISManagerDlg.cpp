/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/ttis_manager/src/TTISManagerDlg.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * The top level dialog for the application
  *
  */


#include "stdafx.h"
#include "TTISManager.h"
#include "TTISManagerDlg.h"
#include "TTISMessageManager.h"
#include "REBProgressManager.h"

#include "bus/generic_gui/src/HelpLauncher.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TA_INIT_TTIS (WM_USER + 101)

namespace TA_IRS_App
{
    static const std::string SELECTED_TTIS_MSG_TAG = "SelectedTTISMessage";

    using TA_Base_Bus::TransActiveDialog;

    /////////////////////////////////////////////////////////////////////////////
    // TTISManagerDlg dialog

    TTISManagerDlg::TTISManagerDlg(TA_Base_Bus::IGUIAccess& controlClass)
        : TransActiveDialog(controlClass, TTISManagerDlg::IDD, NULL), m_initReady(false)
    {
        FUNCTION_ENTRY( "TTISManagerDlg" );

        // {{AFX_DATA_INIT(TTISManagerDlg)
            // NOTE: the ClassWizard will add member initialization here
        // }}AFX_DATA_INIT
        // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

        FUNCTION_EXIT;
    }


    void TTISManagerDlg::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY( "DoDataExchange" );

        TransActiveDialog::DoDataExchange(pDX);
        // {{AFX_DATA_MAP(TTISManagerDlg)
        DDX_Control(pDX, IDC_MAIN_TAB, m_mainTab);
        // }}AFX_DATA_MAP

        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(TTISManagerDlg, TransActiveDialog)
        // {{AFX_MSG_MAP(TTISManagerDlg)
        ON_WM_PAINT()
        ON_WM_QUERYDRAGICON()
        ON_COMMAND(ID_FILE_EXIT, OnFileExit)
        ON_COMMAND(ID_HELP_TTISMANAGERHELP, OnHelp)
        ON_WM_KEYDOWN()
        ON_WM_CLOSE()
        ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
        ON_BN_CLICKED(IDC_CLOSE, OnCloseButton)
        ON_BN_CLICKED(IDC_HELP_BUTTON, OnHelp)
        ON_WM_SHOWWINDOW()
        ON_MESSAGE(TA_INIT_TTIS, OnInitTTIS)
        // }}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // TTISManagerDlg message handlers

    BOOL TTISManagerDlg::OnInitDialog()
    {
        FUNCTION_ENTRY( "OnInitDialog" );

        CRect windowSize;
        GetWindowRect(&windowSize);
        TA_Base_Bus::ResizingProperties properties;
        properties.canMaximise = false;
        properties.maxHeight = -1;
        properties.maxWidth = -1;
        properties.minHeight = windowSize.bottom - windowSize.top;
        properties.minWidth = windowSize.right - windowSize.left;
        setResizingProperties(properties);
        // If toolbars or menus are loaded then the resizing properties will need to be set again

        TransActiveDialog::OnInitDialog();

        // Set the icon for this dialog.  The framework does this automatically
        // when the application's main window is not a dialog
        SetIcon(m_hIcon, TRUE);            // Set big icon
        SetIcon(m_hIcon, FALSE);        // Set small icon

        // turn off message suppression
        FUNCTION_EXIT;
        return TRUE;  // return TRUE  unless you set the focus to a control
    }


    void TTISManagerDlg::OnShowWindow( BOOL bShow, UINT nStatus )
    {
        FUNCTION_ENTRY( "OnShowWindow" );

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "zhaohua: show the window");
        TransActiveDialog::OnShowWindow(bShow, nStatus);
        if (!m_initReady)
        {
            PostMessage(TA_INIT_TTIS, 0, 0);
        }

        FUNCTION_EXIT;
    }


    LRESULT TTISManagerDlg::OnInitTTIS( WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "OnInitTTIS" );

        CWaitCursor cos;
        InitAll();
        cos.Restore();

        FUNCTION_EXIT;
        return 0;
    }


    void TTISManagerDlg::InitAll()
    {
        FUNCTION_ENTRY( "InitAll" );

        // show the progress dialog
        // TODO: Add extra validation here
        REBProgressManager mgr;

        mgr.Init(GetDesktopWindow()->m_hWnd);    //
        mgr.EnableProgress(true);
        mgr.SetVisible(TRUE);
        mgr.EnableCancel(FALSE);
        mgr.BeginProgressDialog();
        mgr.SetCaption("TTIS manager initializing...");

        mgr.SetProgress(1);
        mgr.SetStaticText(0, "Initializing the subsystem pages...");

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "zhaohua: start maintab init");
        m_mainTab.Init(mgr);
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "zhaohua: end maintab init");

        TTISMessageManager::getInstance().setMessageSuppression(false);

        // TD 15349
        // zhou yuan++
        TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, SELECTED_TTIS_MSG_TAG.c_str());

        std::string preselectMsg = TA_Base_Core::RunParams::getInstance().get(SELECTED_TTIS_MSG_TAG.c_str());
        if( preselectMsg != "")
        {
            onRunParamChange( SELECTED_TTIS_MSG_TAG, preselectMsg);
        }

        // ++zhou yuan

        FUNCTION_EXIT;
    }


    // If you add a minimize button to your dialog, you will need the code below
    // to draw the icon.  For MFC applications using the document/view model,
    // this is automatically done for you by the framework.

    void TTISManagerDlg::OnPaint()
    {
        FUNCTION_ENTRY( "OnPaint" );

        if (IsIconic())
        {
            CPaintDC dc(this); // device context for painting

            SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

            // Center icon in client rectangle
            int cxIcon = GetSystemMetrics(SM_CXICON);
            int cyIcon = GetSystemMetrics(SM_CYICON);
            CRect rect;
            GetClientRect(&rect);
            int x = (rect.Width() - cxIcon + 1) / 2;
            int y = (rect.Height() - cyIcon + 1) / 2;

            // Draw the icon
            dc.DrawIcon(x, y, m_hIcon);
        }
        else
        {
            TransActiveDialog::OnPaint();
        }

        FUNCTION_EXIT;
    }


    // The system calls this to obtain the cursor to display while the user drags
    // the minimized window.
    HCURSOR TTISManagerDlg::OnQueryDragIcon()
    {
        FUNCTION_ENTRY( "OnQueryDragIcon" );
        FUNCTION_EXIT;
        return (HCURSOR) m_hIcon;
    }


    void TTISManagerDlg::OnFileExit()
    {
        FUNCTION_ENTRY( "OnFileExit" );

        // TD18095, jianghp, to fix the performance of showing manager application
        TransActiveDialog::DestroyWindow();

        FUNCTION_EXIT;
    }


    void TTISManagerDlg::OnClose()
    {
        FUNCTION_ENTRY( "OnClose" );

        // TD18095, jianghp, to fix the performance of showing manager application
        TransActiveDialog::DestroyWindow();

        FUNCTION_EXIT;
    }


    void TTISManagerDlg::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        FUNCTION_ENTRY( "OnKeyDown" );

        // TODO: Add your message handler code here and/or call default

        TransActiveDialog::OnKeyDown(nChar, nRepCnt, nFlags);

        FUNCTION_EXIT;
    }


    BOOL TTISManagerDlg::PreTranslateMessage(MSG* pMsg)
    {
        FUNCTION_ENTRY( "PreTranslateMessage" );

        // TODO: Add your specialized code here and/or call the base class
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
        return TransActiveDialog::PreTranslateMessage(pMsg);
    }


    void TTISManagerDlg::initToolbar()
    {
        FUNCTION_ENTRY( "initToolbar" );

        m_wndToolBar.GetToolBarCtrl().SetButtonSize(CSize(69,55));

        // m_wndToolBar.SetSizes(CSize(60,50),CSize(22,27));
        // m_wndToolBar.SetSizes(CSize(69,37),CSize(22,27));

        m_wndToolBar.SetButtonText(0,"Login");
        m_wndToolBar.SetButtonText(1,"Logout");
        m_wndToolBar.SetButtonText(2,"Start");


       /* m_wndToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_START,false);
        m_wndToolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_STOP,false);      */

        FUNCTION_EXIT;
    }


    void TTISManagerDlg::OnHelpAbout()
    {
        FUNCTION_ENTRY( "OnHelpAbout" );

        TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();

        FUNCTION_EXIT;
    }


    void TTISManagerDlg::OnHelp()
    {
        FUNCTION_ENTRY( "OnHelp" );

        TA_Base_Bus::HelpLauncher::getInstance().displayHelp();

        FUNCTION_EXIT;
    }


    void TTISManagerDlg::OnCloseButton()
    {
        FUNCTION_ENTRY( "OnCloseButton" );

        TransActiveDialog::OnClose();

        FUNCTION_EXIT;
    }


    // TD 15349
    // zhou yuan++
    void TTISManagerDlg::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY( "onRunParamChange" );

        if( (name.compare(SELECTED_TTIS_MSG_TAG) == 0) && !(value.empty()) )
        {
            CTrainDisplayPage* displayPage = m_mainTab.getTrainDisplayPage();
            displayPage->findAndSelectStationMessage(convertRunParam(value));
        }

        FUNCTION_EXIT;
    }


    std::string TTISManagerDlg::convertRunParam(const std::string& org)
    {
        FUNCTION_ENTRY( "convertRunParam" );

        std::string ret = org;
        std::string::size_type index = ret.find('_');
        while(index != std::string::npos)
        {
            ret.replace(index, 1, " ");
            index = ret.find('_', index);
        }

        FUNCTION_EXIT;
        return ret;
    }


    // ++zhou yuan

} // TA_IRS_App
