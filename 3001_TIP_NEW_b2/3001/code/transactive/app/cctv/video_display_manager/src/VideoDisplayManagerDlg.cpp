/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_display_manager/src/VideoDisplayManagerDlg.cpp $
  * @author:  Robert Young
  * @version: $Revision: #18 $
  *
  * Last modification: $DateTime: 2016/02/19 14:03:00 $
  * Last modified by:  $Author: hongzhi.zhang $
  *
  * TODO: <description>
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif

#include "app/cctv/video_display_manager/src/stdafx.h"
#include "app/cctv/video_display_manager/src/resource.h"
#include "app/cctv/video_display_manager/src/BvsStage.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/Quad.h"
#include "app/cctv/video_display_manager/src/QuadControllerDlg.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManager.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerDlg.h"
#include "app/cctv/video_display_manager/src/LocationSelectionDialog.h"
#include "app/cctv/video_display_manager/src/DlgAlarmEventTest.h"

#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/trains/TrainCommonLibrary/src/ProtocolCctvTypes.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"

#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/AppLauncher.h"


#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SM_CXVIRTUALSCREEN      78
#define MINVALUE                2400
#define NUMSCREENS              3

using TA_Base_Bus::TransActiveDialog;
using TA_Base_Core::RunParams;

namespace TA_IRS_App
{
    const std::string CONSSOLEMONITOR3STR = std::string("Consolemonitor3Inservice");
    const char* NODEVICE = "No item is currently selected";
    const unsigned int TRAIN_COMMAND_OFFSET = 1;
    /////////////////////////////////////////////////////////////////////////////
    // VideoDisplayManagerDlg dialog

    VideoDisplayManagerDlg::VideoDisplayManagerDlg(TA_Base_Bus::IGUIAccess& controlClass, unsigned long dialogID)
        : TransActiveDialog(controlClass, dialogID, NULL),
          m_camerasPage(NULL),
          m_sequencesPage(NULL),
          m_trainsPage(NULL),
          m_videoSourcesPropertySheet(NULL),
          m_mouseTracking(FALSE),
          m_hilitedControlID(0),
          m_prvHilitedControlID(0),
          m_restoring(FALSE),
          m_contextActive(FALSE),
          m_hilitingPerformed(TRUE),
          m_normalisationPerformed(TRUE),
          m_dragImage(NULL),
          m_dragAdjustPoint(0, 0),
          m_pQuadControllerDlg(0),
          m_paramCameraAddress(""),
          m_paramMonitorName(""),
          m_pSequenceEditorDlg(0),
          m_isLaunched(false),
          m_isAssignedBySchematic(false),
          m_overriddenLocation(0),
          m_consoleKey(0l),
          m_dlgAlarmEventTest(NULL),
		  m_dlgTemplateID(dialogID)
    {
        FUNCTION_ENTRY("VideoDisplayManagerDlg");
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
        // Determine Physical Site of this Console Session
        using TA_Base_Core::DataException;
        using TA_Base_Core::DatabaseException;
        TA_Base_Core::ILocation::ELocationType locationType = TA_Base_Core::ILocation::OCC;
        m_locationName = "OCC";
        std::string sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        try
        {
            TA_Base_Core::IConsole* console =
                TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionID);
            TA_Base_Core::ILocation* location =
                TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(console->getLocation());
            m_consoleKey = console->getKey();
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Console key is:%d", m_consoleKey);
            locationType = location->getLocationType();
            m_locationName = location->getName();
			if(m_dlgTemplateID == IDD_VIDEODISPLAYMANAGER_DIALOG_OCC)
            {
                m_ctrlTab = IDC_VD_OCC_TAB_VIDEOINPUT;
                m_ctrlDetail = IDC_VD_OCC_DETAILS_EDIT;
            }
			else if(m_dlgTemplateID == IDD_VIDEODISPLAYMANAGER_DIALOG_KOCC)
            {
                m_ctrlTab = IDC_VD_KOCC_TAB_VIDEOINPUT;
                m_ctrlDetail = IDC_VD_KOCC_DETAILS_EDIT;
            }
			else if(m_dlgTemplateID == IDD_VIDEODISPLAYMANAGER_DIALOG_GBD)
            {
                m_ctrlTab = IDC_VD_GBD_TAB_VIDEOINPUT;
                m_ctrlDetail = IDC_VD_GBD_DETAILS_EDIT;
            }
			else if(m_dlgTemplateID == IDD_VIDEODISPLAYMANAGER_DIALOG_TMF)
            {
                m_ctrlTab = IDC_VD_TMF_TAB_VIDEOINPUT;
                m_ctrlDetail = IDC_VD_TMF_DETAILS_EDIT;
            }
			else if(m_dlgTemplateID == IDD_VIDEODISPLAYMANAGER_DIALOG_TDS)
            {
                m_ctrlTab = IDC_VD_TDS_TAB_VIDEOINPUT;
                m_ctrlDetail = IDC_VD_TDS_DETAILS_EDIT;
            }
            else     //PSC
            {
                m_ctrlTab = IDC_VD_PSC_TAB_VIDEOINPUT;
                m_ctrlDetail = IDC_VD_PSC_DETAILS_EDIT;
            }
        }
        catch(DataException& ex)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << ex.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010029);
        }
        catch(DatabaseException& ex)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << ex.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010029);
        }
        ((VideoDisplayManagerApp*)(AfxGetApp()))->setLocationType(locationType);
        if(TA_Base_Core::RunParams::getInstance().isSet("TestAlarmEvent"))
        {
            m_dlgAlarmEventTest = new DlgAlarmEventTest(m_locationName, this);
            m_dlgAlarmEventTest->Create(IDD_DLG_ALARM_TEST);
            m_dlgAlarmEventTest->ShowWindow(SW_SHOW);
        }
        FUNCTION_EXIT;
    }


    VideoDisplayManagerDlg::~VideoDisplayManagerDlg()
    {
        FUNCTION_ENTRY("~VideoDisplayManagerDlg");
        if(m_dlgAlarmEventTest != NULL)
        {
            delete m_dlgAlarmEventTest;
            m_dlgAlarmEventTest = NULL;
        }
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerDlg::setDragImage(CImageList* dragImage)
    {
        FUNCTION_ENTRY("setDragImage");
        if(m_dragImage != NULL)
        {
            delete m_dragImage;
            m_dragImage = NULL;
        }
        m_dragImage = dragImage;
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerDlg::DoDataExchange(CDataExchange* pDX)
    {
        FUNCTION_ENTRY("DoDataExchange");
        TransActiveDialog::DoDataExchange(pDX);
        DDX_Control(pDX, m_ctrlDetail, m_detailsBox);
        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(VideoDisplayManagerDlg, TransActiveDialog)
        //{{AFX_MSG_MAP(VideoDisplayManagerDlg)
        ON_WM_PAINT()
        ON_WM_QUERYDRAGICON()
        ON_WM_LBUTTONDOWN()
        ON_WM_LBUTTONUP()
        ON_WM_MOUSEMOVE()
        ON_WM_CONTEXTMENU()
        ON_WM_RBUTTONDOWN()
        ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
        ON_COMMAND(ID_APP_HELP, OnHelpDisplaymanagerhelp)
        ON_WM_LBUTTONDBLCLK()
        ON_WM_CLOSE()
        ON_WM_DESTROY()
        ON_MESSAGE(WM_MONITOR_UPDATED, onMonitorUpdated)
        ON_REGISTERED_MESSAGE(TA_Base_Bus::TA_GenericGui::WM_SET_WINDOW_POSITION, OnSetWindowPosition)
        ON_MESSAGE(TA_INIT_CAMERAS, OnInitCameras)
        ON_MESSAGE(TA_INIT_SEQUENCES, OnInitSequences)
        ON_MESSAGE(TA_INIT_MONITORS, OnInitMonitors)
        ON_MESSAGE(TA_INIT_CCTV, OnInitDisplayManager)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // VideoDisplayManagerDlg message handlers

    BOOL VideoDisplayManagerDlg::OnInitDialog()
    {
        FUNCTION_ENTRY("OnInitDialog");
        CRect windowSize;
        GetWindowRect(&windowSize);
        TA_Base_Bus::ResizingProperties properties;
        properties.canMaximise = false;
        properties.minHeight = windowSize.bottom - windowSize.top;
        properties.minWidth = windowSize.right - windowSize.left;
        properties.maxHeight = properties.minHeight;
        properties.maxWidth = properties.minWidth;
        setResizingProperties(properties);
        TA_Base_Core::RunParams::getInstance().set("AppName", AfxGetApp()->m_pszAppName);
        TransActiveDialog::OnInitDialog();
        SetIcon(m_hIcon, TRUE);        // Set big icon
        ModifyWindowPos();
        TA_Base_Core::ILocation::ELocationType locationType =
            ((VideoDisplayManagerApp*)(AfxGetApp()))->getLocationType();
        configureGUILayout(locationType);
        m_camerasPage->m_cameraTreeCtrl.EnableWindow(FALSE);
        m_videoSourcesPropertySheet->EnableItem(1, FALSE);
        m_sequencesPage->m_sequenceTreeCtrl.EnableWindow(FALSE);
        m_videoSourcesPropertySheet->EnableItem(2, FALSE);
        AfxGetApp()->DoWaitCursor(1);
        // load and initialise display items
        DisplayItemManager::getInstance().initialiseDisplayItems(this);
        FUNCTION_EXIT;
        return TRUE;  // return TRUE  unless you set the focus to a control
    }


    LRESULT VideoDisplayManagerDlg::OnInitCameras(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnInitCameras");
        AfxGetApp()->DoWaitCursor(0);
        m_camerasPage->Init();
        CRect rect;
        m_camerasPage->GetWindowRect(&rect);
        bool bVisible = m_camerasPage->IsWindowVisible();
        m_camerasPage->m_cameraTreeCtrl.setDetailsPointer(&m_detailsBox);
        m_camerasPage->m_cameraTreeCtrl.EnableWindow(TRUE);
        m_videoSourcesPropertySheet->EnableItem(1, TRUE);
        FUNCTION_EXIT;
        return 0;
    }

    LRESULT VideoDisplayManagerDlg::OnInitSequences(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnInitSequences");
        AfxGetApp()->DoWaitCursor(0);
        m_sequencesPage->Init();
        m_sequencesPage->m_sequenceTreeCtrl.setDetailsPointer(&m_detailsBox);
        m_sequencesPage->m_sequenceTreeCtrl.EnableWindow(TRUE);
        m_videoSourcesPropertySheet->EnableItem(2, TRUE);
        FUNCTION_EXIT;
        return 0;
    }

    LRESULT VideoDisplayManagerDlg::OnInitMonitors(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnInitMonitors");
        AfxGetApp()->DoWaitCursor(0);
        setupMonitors();
        DisplayItemManager::getInstance().registerForMonitorChanges(this);
        FUNCTION_EXIT;
        return 0;
    }


    LRESULT VideoDisplayManagerDlg::OnInitDisplayManager(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnInitDisplayManager");
        AfxGetApp()->DoWaitCursor(0);
        m_detailsBox.SetWindowText(NODEVICE);
        // for each monitor on this page
        for(std::map<std::string, int>::iterator iter = m_monitorNameToResourceIdMap.begin();
                iter != m_monitorNameToResourceIdMap.end(); iter++)
        {
            Monitor* monitor = getMonitor(iter->second);
            if(monitor != NULL)
            {
                onMonitorUpdated(NULL, reinterpret_cast<LPARAM>(monitor));
            }
        }
        //  Check the commandline parameters for a camera-to-monitor switch request (TD 13455).
        if(TA_Base_Core::RunParams::getInstance().isSet(RPARAM_ENTITY) &&
                TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DISPLAYMONITOR))
        {
            m_paramCameraAddress = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITY);
            onRunParamChange(RPARAM_DISPLAYMONITOR, TA_Base_Core::RunParams::getInstance().get(RPARAM_DISPLAYMONITOR));
            m_isAssignedBySchematic = false;
        }
        RunParams::getInstance().registerRunParamUser(this, RPARAM_ENTITY);
        RunParams::getInstance().registerRunParamUser(this, RPARAM_DISPLAYMONITOR);
        m_isLaunched = true;
        AfxGetApp()->DoWaitCursor(-1);
        FUNCTION_EXIT;
        return 0;
    }

    void VideoDisplayManagerDlg::OnTcnSelchangeTabVideoSource(NMHDR *pNMHDR, LRESULT *pResult)
    {
    }

    void VideoDisplayManagerDlg::OnDestroy()
    {
        FUNCTION_ENTRY("OnDestroy");
        DisplayItemManager::getInstance().deregisterForMonitorChanges(this);
        DisplayItemManager::removeInstance();
        CDialog::OnDestroy();
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerDlg::configureGUILayout(TA_Base_Core::ILocation::ELocationType locationType)
    {
        FUNCTION_ENTRY("configureGUILayout");
        CWnd* marker = GetDlgItem(m_ctrlTab);
        CRect markerRect;
        marker->GetWindowRect(&markerRect);
        ScreenToClient(&markerRect);
        m_videoSourcesPropertySheet = new VideoSourcesPropertySheet(IDS_PROPSHT_CAPTION, RECT(markerRect), this, 0);
        m_camerasPage    = new CamerasPage();
        ASSERT(m_camerasPage);
        m_sequencesPage    = new SequencesPage();
        ASSERT(m_sequencesPage);
        m_videoSourcesPropertySheet->AddPage(m_camerasPage);
        m_videoSourcesPropertySheet->AddPage(m_sequencesPage);
        m_videoSourcesPropertySheet->Create(this, WS_VISIBLE | WS_CHILD, WS_EX_CONTROLPARENT);
        m_videoSourcesPropertySheet->SetWindowPos(NULL, markerRect.left, markerRect.top, markerRect.Width(), markerRect.Height(), SWP_NOZORDER | SWP_NOSIZE);
        FUNCTION_EXIT;
    }

    void VideoDisplayManagerDlg::setupMonitors()
    {
        if(false == m_screen.isEmpty())
        {
            return;
        }
        UINT screenID, screenTextID, index = 1;
        CString strScreenName;
        m_screen.setWnd(this);
		if(m_dlgTemplateID == IDD_VIDEODISPLAYMANAGER_DIALOG_OCC)
        {
            for(screenID = IDC_VD_OCC_WALL_MON_1, screenTextID = IDC_VD_OCC_WALL_MON_TEXT_1; screenID <= IDC_VD_OCC_WALL_MON_18; screenID++, screenTextID++, index++)
            {
                strScreenName.Format("WallMonitor%d", screenID);
                std::string name(strScreenName);
                m_screen.insertScreen(name, screenID, screenTextID, index, ScreenWall);
            }
            m_screen.insertScreen("ConsoleMonitor1", IDC_VD_OCC_CONSOLE_MON_1, IDC_VD_OCC_CONSOLE_MON_TEXT_1, 1, ScreenConsole);
            m_screen.insertScreen("OverAllMonitor1", IDC_VD_OCC_OVERALL_MON_1, IDC_VD_OCC_OVERALL_MON_TEXT_1, 1, ScreenOverAll);
        }
		else if(m_dlgTemplateID == IDD_VIDEODISPLAYMANAGER_DIALOG_KOCC)
        {
            for(screenID = IDC_VD_KOCC_WALL_MON_1, screenTextID = IDC_VD_KOCC_WALL_MON_TEXT_1; screenID <= IDC_VD_KOCC_WALL_MON_4; screenID++, screenTextID++, index++)
            {
                strScreenName.Format("WallMonitor%d", screenID);
                std::string name(strScreenName);
                m_screen.insertScreen(name, screenID, screenTextID, index, ScreenWall);
            }
            m_screen.insertScreen("ConsoleMonitor1", IDC_VD_KOCC_CONSOLE_MON_1, IDC_VD_KOCC_CONSOLE_MON_TEXT_1, 1, ScreenConsole);
            m_screen.insertScreen("OverAllMonitor1", IDC_VD_KOCC_OVERALL_MON_1, IDC_VD_KOCC_OVERALL_MON_TEXT_1, 1, ScreenOverAll);
        }
		else if(m_dlgTemplateID == IDD_VIDEODISPLAYMANAGER_DIALOG_GBD)
        {
            m_screen.insertScreen("ConsoleMonitor1", IDC_VD_GBD_CONSOLE_MON_1, IDC_VD_GBD_CONSOLE_MON_TEXT_1, 1, ScreenConsole);
            m_screen.insertScreen("ConsoleMonitor2", IDC_VD_GBD_CONSOLE_MON_2, IDC_VD_GBD_CONSOLE_MON_TEXT_2, 2, ScreenConsole);
            m_screen.insertScreen("OverAllMonitor1", IDC_VD_GBD_OVERALL_MON_1, IDC_VD_GBD_OVERALL_MON_TEXT_1, 1, ScreenOverAll);
        }
		else if(m_dlgTemplateID == IDD_VIDEODISPLAYMANAGER_DIALOG_TMF)
        {
            m_screen.insertScreen("ConsoleMonitor1", IDC_VD_TMF_CONSOLE_MON_1, IDC_VD_TMF_CONSOLE_MON_TEXT_1, 1, ScreenConsole);
            m_screen.insertScreen("ConsoleMonitor2", IDC_VD_TMF_CONSOLE_MON_2, IDC_VD_TMF_CONSOLE_MON_TEXT_2, 2, ScreenConsole);
        }
		else if(m_dlgTemplateID == IDD_VIDEODISPLAYMANAGER_DIALOG_TDS)
        {
            m_screen.insertScreen("ConsoleMonitor1", IDC_VD_TDS_CONSOLE_MON_1, IDC_VD_TDS_CONSOLE_MON_TEXT_1, 1, ScreenConsole);
        }
        else     //PSC
        {
            m_screen.insertScreen("ConsoleMonitor1", IDC_VD_PSC_CONSOLE_MON_1, IDC_VD_PSC_CONSOLE_MON_TEXT_1, 1, ScreenConsole);
            m_screen.insertScreen("ConsoleMonitor2", IDC_VD_PSC_CONSOLE_MON_2, IDC_VD_PSC_CONSOLE_MON_TEXT_2, 2, ScreenConsole);
            m_screen.insertScreen("ConsoleMonitor3", IDC_VD_PSC_CONSOLE_MON_3, IDC_VD_PSC_CONSOLE_MON_TEXT_3, 3, ScreenConsole);
        }
        m_screen.setMaps(::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_VD_WALL_SMALL)),
                         ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_VD_WALL_SMALL_H)),
                         ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_VD_CONSOLE)),
                         ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_VD_CONSOLE_H)),
                         ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_VD_OVERALL)),
                         ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_VD_OVERALL_H)));
        std::map<std::string, Monitor*> monitors = DisplayItemManager::getInstance().getAllMonitors();
        std::map<std::string, Monitor*>::iterator it = monitors.begin();
        ScreenType screenType;
        for(int i = 0; it != monitors.end(); it++, i++)
        {
            if(it->second->isConsoleMonitor())
            {
                if(m_locationName == LOCATION_NAME_OCC)
                {
                    if(it->second->getConsoleKey() != m_consoleKey)
                        continue;
                }
                screenType = ScreenConsole;
            }
            else if(it->second->isOverAllMonitor())
            {
                screenType = ScreenOverAll;
            }
            else
            {
                screenType = ScreenWall;
            }
            m_screen.setMonitor(it->second->getDisplayOrder(), screenType, it->second);
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "m_screen.setMonitor(%ld,%d,%d )", it->second->getKey(), it->second->getDisplayOrder(), screenType);
        }
        m_screen.drawAll();
    }

    Monitor* VideoDisplayManagerDlg::getMonitor(int controlId)
    {
        FUNCTION_ENTRY("getMonitor");
        // find and return the item in the map
        std::map<int, std::string>::iterator findIter = m_resourceIdToMonitorNameMap.find(controlId);
        if(findIter != m_resourceIdToMonitorNameMap.end())
        {
            FUNCTION_EXIT;
            return DisplayItemManager::getInstance().getMonitor(findIter->second);
        }
        // not found
        FUNCTION_EXIT;
        return NULL;
    }


    int VideoDisplayManagerDlg::getControlId(std::string monitorName)
    {
        FUNCTION_ENTRY("getControlId");
        // find and return the item in the map
        std::map<std::string, int>::iterator findIter = m_monitorNameToResourceIdMap.find(monitorName);
        if(findIter != m_monitorNameToResourceIdMap.end())
        {
            FUNCTION_EXIT;
            return findIter->second;
        }
        // not found
        FUNCTION_EXIT;
        return NULL;
    }


    void VideoDisplayManagerDlg::LoadButtonIcons()
    {
        FUNCTION_ENTRY("LoadButtonIcons");
        // Load check box icons.
        m_previousElementIcon = AfxGetApp()->LoadIcon(IDI_PREVIOUSELEMENT_RW_ICON);
        m_nextElementIcon = AfxGetApp()->LoadIcon(IDI_NEXTELEMENT_FF_ICON);
        m_playIcon = AfxGetApp()->LoadIcon(IDI_PLAY_ICON);
        m_pauseIcon = AfxGetApp()->LoadIcon(IDI_PAUSE_ICON);
        // Set the check box icons.
        m_commonPreviousElement.SetIcon(m_previousElementIcon);
        m_commonNextElement.SetIcon(m_nextElementIcon);
        m_commonPause.SetIcon(m_pauseIcon);
        m_commonPlay.SetIcon(m_playIcon);
        m_consolePreviousElement.SetIcon(m_previousElementIcon);
        m_consoleNextElement.SetIcon(m_nextElementIcon);
        m_consolePause.SetIcon(m_pauseIcon);
        m_consolePlay.SetIcon(m_playIcon);
        m_commonPreviousElement.EnableWindow(FALSE);
        m_commonNextElement.EnableWindow(FALSE);
        m_commonPause.EnableWindow(FALSE);
        m_commonPlay.EnableWindow(FALSE);
        m_consolePreviousElement.EnableWindow(FALSE);
        m_consoleNextElement.EnableWindow(FALSE);
        m_consolePause.EnableWindow(FALSE);
        m_consolePlay.EnableWindow(FALSE);
        FUNCTION_EXIT;
    }


    // If you add a minimize button to your dialog, you will need the code below
    //  to draw the icon.  For MFC applications using the document/view model,
    //  this is automatically done for you by the framework.
    void VideoDisplayManagerDlg::OnPaint()
    {
        FUNCTION_ENTRY("OnPaint");
        if(IsIconic())
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
            // Invalidate both appropriate control labels.
            invalidateControlLabels();
            TransActiveDialog::OnPaint();
            // Paint non-hilited control.
            if((m_prvHilitedControlID != 0) && (m_normalisationPerformed == FALSE))
            {
                paintStandardControl();
            }
            // Paint hilited control.
            if((m_hilitedControlID != 0) && (m_hilitingPerformed == FALSE))
            {
                paintHilitedControl();
            }
        }
        FUNCTION_EXIT;
    }


    // The system calls this to obtain the cursor to display while the user drags
    //  the minimized window.
    HCURSOR VideoDisplayManagerDlg::OnQueryDragIcon()
    {
        FUNCTION_ENTRY("OnQueryDragIcon");
        FUNCTION_EXIT;
        return (HCURSOR) m_hIcon;
    }


    BOOL VideoDisplayManagerDlg::DestroyWindow()
    {
        FUNCTION_ENTRY("DestroyWindow");
        if(m_dragImage != NULL)
        {
            delete m_dragImage;
        }
        if(m_videoSourcesPropertySheet != NULL)
        {
            delete m_videoSourcesPropertySheet;
            m_videoSourcesPropertySheet = NULL;
        }
        // delete pages
        if(m_camerasPage != NULL)
        {
            delete m_camerasPage;
            m_camerasPage = NULL;
        }
        if(m_sequencesPage != NULL)
        {
            delete m_sequencesPage;
            m_sequencesPage = NULL;
        }
        if(m_trainsPage != NULL)
        {
            delete m_trainsPage;
            m_trainsPage = NULL;
        }
        FUNCTION_EXIT;
        return TransActiveDialog::DestroyWindow();
    }


    // Don't allow an escape key command to be processed.
    BOOL VideoDisplayManagerDlg::PreTranslateMessage(MSG* pMsg)
    {
        FUNCTION_ENTRY("PreTranslateMessage");
        switch(pMsg->message)
        {
            case WM_KEYDOWN:
            {
                switch(pMsg->wParam)
                {
                    case VK_ESCAPE:
                    {
                        FUNCTION_EXIT;
                        return TRUE;
                    }
                    break;
                    case VK_LEFT:
                    {
                        FUNCTION_EXIT;
                        return TRUE;
                    }
                    break;
                    case VK_RIGHT:
                    {
                        FUNCTION_EXIT;
                        return TRUE;
                    }
                    break;
                    case VK_UP:
                    {
                        FUNCTION_EXIT;
                        return TRUE;
                    }
                    break;
                    case VK_TAB:
                    {
                        FUNCTION_EXIT;
                        return TRUE;
                    }
                    break;
                    default:
                    {
                        //do nothing
                    }
                    break;
                }
            }
            case WM_SYSCOMMAND:
            {
                // otherwise wont respond to Alt/F4 during capture
                if(pMsg->wParam == SC_CLOSE)
                {
                    ::ReleaseCapture();
                    if(m_mouseTracking && (m_dragImage != NULL))
                    {
                        m_dragImage->DragLeave(this);
                        m_dragImage->EndDrag();
                        delete m_dragImage;
                        m_dragImage = NULL;
                        SetCursor(LoadCursor(NULL, IDC_ARROW));
                    }
                }
            }
            break;
            default:
                break;
        }
        FUNCTION_EXIT;
        return TransActiveDialog::PreTranslateMessage(pMsg);
    }

    void VideoDisplayManagerDlg::OnLButtonDown(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY("OnLButtonDown");
        TransActiveDialog::OnLButtonDown(nFlags, point);
        CPoint nullPt(0, 0);
        // point == nullPt if the user clicked in the Video Source region.
        if(point == nullPt)
        {
            // Make sure that the user selected a list box item.
            if(TRUE)
            {
                SetCapture();
                m_mouseTracking = TRUE;
                CPoint cursorLocation;
                ::GetCursorPos(&cursorLocation);
                m_dragImage->BeginDrag(0, m_dragAdjustPoint);
                m_dragImage->DragEnter(NULL, cursorLocation);
            }
        }
        // The user clicked outside the Video Sources region.
        else
        {
            ClientToScreen(&point);
            stuMonitorScreen* screen = m_screen.getScreenFromPointer(point);
            m_screen.setSelScreen(screen);
            updateDetailsBox();
        }
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerDlg::OnLButtonUp(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY("OnLButtonUp");
        TransActiveDialog::OnLButtonUp(nFlags, point);
        // Release mouse and load standard arrow cursor.
        if(TRUE == m_mouseTracking)
        {
            ::ReleaseCapture();
            endDragging();
            m_mouseTracking = FALSE;
            m_contextActive = FALSE;
            SetCursor(LoadCursor(NULL, IDC_ARROW));
            m_dragImage->DragLeave(this);
            m_dragImage->EndDrag();
            if(NULL != m_dragImage)
            {
                delete m_dragImage;
                m_dragImage = NULL;
            }
            ClientToScreen(&point);
            stuMonitorScreen* screen = m_screen.getScreenFromPointer(point);
            m_screen.setSelScreen(screen);
            if(screen != NULL)
            {
                Monitor* monitor = m_screen.getSelScreenMonitor();
                VideoInput* input = getSelectedVideoSource(true);
                // if there are both monitor and video input objects
                if((monitor != NULL) && (input != NULL))
                {
                    {
                        try
                        {
                            TA_IRS_App::SwitchAgentCommunicator::getInstance().assignMonitor(monitor, input);
                            //  m_screen.draw(*screen);
                        }
                        catch(TA_Base_Core::TransactiveException& te)
                        {
                            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
                            AfxMessageBox(te.what());
                            FUNCTION_EXIT;
                            return;
                        }
                    }
                }
            }
        }
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerDlg::OnMouseMove(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY("OnMouseMove");
        if(m_mouseTracking)
        {
            stuMonitorScreen* orgSelScreen = m_screen.getSelScreen();
            ClientToScreen(&point);
            stuMonitorScreen* screen = m_screen.getScreenFromPointer(point);
            m_screen.setSelScreen(screen);
            SetCursor(AfxGetApp()->LoadCursor(IDC_DRAG_CURSOR));
            if(orgSelScreen != screen)
            {
                m_dragImage->DragLeave(NULL);
                m_dragImage->EndDrag();
                m_contextActive = TRUE;
                updateDialog();
            }
            else
            {
                //   ClientToScreen(&point);
                if(TRUE == m_contextActive)
                {
                    m_dragImage->BeginDrag(0, m_dragAdjustPoint);
                    m_dragImage->DragEnter(NULL, point);
                    m_contextActive = FALSE;
                }
                m_dragImage->DragMove(point);
                ScreenToClient(&point);
            }
        }
        TransActiveDialog::OnMouseMove(nFlags, point);
        FUNCTION_EXIT;
    }


    /**
      * UpdateDialog
      *
      * Sends a WM_PAINT message to the Vidiwall dialog.
      */
    void VideoDisplayManagerDlg::updateDialog()
    {
        FUNCTION_ENTRY("updateDialog");
        SendMessage(WM_PAINT, NULL, NULL);
        FUNCTION_EXIT;
    }


    /**
      * InvalidateControlLabels
      *
      * Invalidates the control labels associated with
      * the previous and currently highlighted monitors.
      */
    void VideoDisplayManagerDlg::invalidateControlLabels()
    {
        FUNCTION_ENTRY("invalidateControlLabels");
        FUNCTION_EXIT;
    }


    /**
      * IsOverMonitor
      *
      * TRUE if the mouse is over a monitor.
      */
    BOOL VideoDisplayManagerDlg::isOverMonitor(CPoint& pt, bool doCheckForSequence)
    {
        return FALSE;
    }


    /**
      * PaintStandardControl
      *
      * Paints the non-hilited bitmap in the control that lost focus.
      */
    void VideoDisplayManagerDlg::paintStandardControl()
    {
        FUNCTION_ENTRY("paintStandardControl");
        FUNCTION_EXIT;
    }


    /**
      * PaintHilitedStandardControl
      *
      * Paints the hilited bitmap in the control that lost focus.
      */
    void VideoDisplayManagerDlg::paintHilitedControl()
    {
        FUNCTION_ENTRY("paintHilitedControl");
        m_hilitingPerformed = TRUE;
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerDlg::OnContextMenu(CWnd* pWnd, CPoint point)
    {
        FUNCTION_ENTRY("OnContextMenu");
        // if context menu currently displayed
        if((TRUE == m_contextActive) || (TRUE == m_mouseTracking))
        {
            FUNCTION_EXIT;
            return;
        }
        CPoint pt(point);
        ScreenToClient(&point);
        // Load the context menu.
        CMenu contextMenu;
        stuMonitorScreen* screen = m_screen.getScreenFromPointer(pt);
        m_screen.setSelScreen(screen);
        Monitor* monitor = m_screen.getSelScreenMonitor();
        if(monitor != NULL)
        {
            updateDialog();
            VideoInput* input = NULL;
            if(monitor != NULL)
            {
                input = monitor->getInput();
            }
            else
            {
                FUNCTION_EXIT;
                return;
            }
            // load the context menu based on the type of assignment
            Sequence* seq = dynamic_cast<Sequence*>(input);
            Camera* camera = dynamic_cast<Camera*>(input);
            std::vector<VideoOutput::TrainCamera> stages;
            std::vector<VideoOutput::CCTVTriggeredAlarm> triggeredAlarms;
            if(input == NULL)
            {
                if(monitor->isInAlarmMode())
                {
                    triggeredAlarms = monitor->getAlarmStackItems();
                    if(monitor->getShowingAckAlarm())
                    {
                        contextMenu.LoadMenu(IDR_MONCAMCONTEXT_MENU);
                    }
                    else
                    {
                        FUNCTION_EXIT;
                        return;
                    }
                }
                else
                {
                    contextMenu.LoadMenu(IDR_MONQUADCONTEXT_MENU);
                }
            }
            else
            {
                if(seq != NULL)
                {
                    contextMenu.LoadMenu(IDR_MONSEQCONTEXT_MENU);
                }
                else if(camera != NULL)
                {
                    contextMenu.LoadMenu(IDR_MONCAMCONTEXT_MENU);
                }
                else
                {
                    TA_ASSERT(false, "Unknown video input type assigned to monitor");
                }
            }
            CMenu* pContextMenu = contextMenu.GetSubMenu(0);
            // Display the context menu.
            m_contextActive = TRUE;
            ClientToScreen(&point);
            int nCmd = (int) pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
                                                          point.x, point.y, AfxGetMainWnd());
            // Process the appropriate context menu command.
            m_contextActive = FALSE;
            // check the result
            if(nCmd > static_cast<long>(stages.size()) || nCmd == 0)
            {
                switch(nCmd)
                {
                    case ID_CLEAR_MONITOR:
                    {
                        if(false == monitor->isInAlarmMode()  || monitor->getShowingAckAlarm())
                        {
                            clearMonitor(monitor);
                        }
                    }
                    break;
                    case ID_LOAD_CONTROLLER:
                        loadQuadController(monitor);
                        break;
                    case ID_NEW_EDIT_SEQUENCE:
                        loadSequenceController(seq);
                        break;
                    default:
                        break;
                }
                FUNCTION_EXIT;
                return;
            }
            else
            {
                try
                {
                    m_trainCctvCorbaProxy->removeAlarmImage(static_cast<unsigned char>(stages[nCmd - TRAIN_COMMAND_OFFSET].trainId),
                                                            static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>(stages[nCmd - TRAIN_COMMAND_OFFSET].cameraId),
                                                            TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));
                }
                catch(TA_Base_Core::TransactiveException& e)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", e.what());
                    std::ostringstream trainStream;
                    trainStream << "PV" << std::setw(2) << std::setfill('0') << stages[nCmd - TRAIN_COMMAND_OFFSET].trainId;
                    trainStream << ". " << e.what();
                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << trainStream.str();
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);
                }
                FUNCTION_EXIT;
                return;
            }
        }
        VideoInput* input = getSelectedVideoSource();
        ClientToScreen(&point);
        HWND hwnd = WindowFromPoint(point)->m_hWnd;
        // if one of the list controls is active
        if((hwnd == m_camerasPage->m_cameraTreeCtrl.GetSafeHwnd()) ||
                (hwnd == m_sequencesPage->m_sequenceTreeCtrl.GetSafeHwnd())
          )
        {
            Sequence* seq = dynamic_cast<Sequence*>(input);
            Camera* camera = dynamic_cast<Camera*>(input);
            if(seq != NULL && (!seq->getReadOnly()))
            {
                contextMenu.LoadMenu(IDR_PAGESEQCONTEXT_MENU);
            }
            else if(camera != NULL)
            {
                contextMenu.LoadMenu(IDR_MONCAMCONTEXT_MENU);
            }
            else
            {
                FUNCTION_EXIT;
                return;
            }
            CMenu* pContextMenu = contextMenu.GetSubMenu(0);
            // Display the context menu.
            m_contextActive = TRUE;
            int nCmd = (int) pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
                                                          point.x, point.y, AfxGetMainWnd());
            m_contextActive = FALSE;
            // Process the appropriate context menu command.
            switch(nCmd)
            {
                case ID_NEW_EDIT_SEQUENCE:
                    loadSequenceController(seq)    ;
                    break;
                case ID_LOAD_CONTROLLER:
                {
                    loadQuadController(monitor);
                }
                break;
                default:
                    break;
            }
            FUNCTION_EXIT;
            return;
        }
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerDlg::clearMonitor(Monitor* monitor)
    {
        FUNCTION_ENTRY("clearMonitor");
        TA_ASSERT(monitor != NULL, "Cannot clear a NULL monitor - menu item should be disabled");
        // get the current input
        VideoInput* input = monitor->getInput();
        if(!monitor->isQuadInput() && !monitor->getShowingAckAlarm() && input == NULL)
        {
            // monitor is clear
            FUNCTION_EXIT;
            return;
        }
        try
        {
            TA_IRS_App::SwitchAgentCommunicator::getInstance().clearMonitor(monitor);
        }
        catch(TA_Base_Core::TransactiveException& te)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
            AfxMessageBox(te.what());
        }
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerDlg::loadQuadController(Monitor* monitor)//Quad* quad)
    {
        FUNCTION_ENTRY("loadQuadController");
        TA_ASSERT(monitor != NULL, "Cannot load a controller for a NULL quad");
        QuadControllerDlg quadContDlg;
        m_pQuadControllerDlg = &quadContDlg;
        quadContDlg.DoModal(monitor);
        m_pQuadControllerDlg = NULL;
        FUNCTION_EXIT;
    }

    void VideoDisplayManagerDlg::loadSequenceController(Sequence* sequence)
    {
        FUNCTION_ENTRY("loadSequenceController");
        TA_ASSERT(sequence != NULL, "Cannot launch sequence editor for null sequence - menu item should not be accessible");
        // launch Sequence Editor
        SequenceEditorDlg sequenceEditorDlg(this, &m_sequencesPage->m_sequenceTreeCtrl);
        m_pSequenceEditorDlg = &sequenceEditorDlg;
        sequenceEditorDlg.DoModal(sequence);
        m_pSequenceEditorDlg = NULL;
        FUNCTION_EXIT;
    }

    void VideoDisplayManagerDlg::sendCarrierOff(unsigned short trainId)
    {
        try
        {
            m_trainCctvCorbaProxy->setCarrierOff((trainId % 0x0100), TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));
        }
        catch(TA_Base_Core::TransactiveException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", e.what());
            std::ostringstream trainStream;
            trainStream << "PV" << std::setw(2) << std::setfill('0') << trainId;
            trainStream << ". " << e.what();
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << trainStream.str();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_090076);
        }
    }


    void VideoDisplayManagerDlg::OnRButtonDown(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY("OnRButtonDown");
        // Invoke the left button down functionality to ensure
        // that the hilited and non hilited monitors are updated.
        if(FALSE == m_contextActive)
        {
            LPARAM lParam = 0;
            lParam = ((point.x) + (point.y << 16));
            SendMessage(WM_LBUTTONDOWN, MK_RBUTTON, lParam);
        }
        TransActiveDialog::OnRButtonDown(nFlags, point);
        FUNCTION_EXIT;
    }


    VideoInput* VideoDisplayManagerDlg::getSelectedVideoSource(bool allowDelocalisedTrains)
    {
        FUNCTION_ENTRY("getSelectedVideoSource");
        // Hopefully when the back end for train operations is
        // properly implemented - this can all be encapsulated in
        // VideoSourcesPropertySheet
        VideoInput* source = NULL;
        // get the source based on the active sheet
        int focus = m_videoSourcesPropertySheet->GetActiveIndex();
        switch(focus)
        {
            case 0:
                source = m_camerasPage->m_cameraTreeCtrl.getSelectedInput();
                break;
            case 1:
                source = m_sequencesPage->m_sequenceTreeCtrl.getSelectedInput();
                break;
            default:
                break;
        }
        FUNCTION_EXIT;
        return source;
    }


    unsigned short VideoDisplayManagerDlg::getSelectedTrainId()
    {
        FUNCTION_ENTRY("getSelectedTrainId");
        FUNCTION_EXIT;
        return 0;
    }


    void VideoDisplayManagerDlg::endDragging()
    {
        FUNCTION_ENTRY("endDragging");
        int focus = m_videoSourcesPropertySheet->GetActiveIndex();
        switch(focus)
        {
            case 0:
                m_camerasPage->m_cameraTreeCtrl.endDragging();
                break;
            case 1:
                m_sequencesPage->m_sequenceTreeCtrl.endDragging();
                break;
            default:
                break;
        }
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerDlg::OnHelpDisplaymanagerhelp()
    {
        FUNCTION_ENTRY("OnHelpDisplaymanagerhelp");
        TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerDlg::OnAppAbout()
    {
        FUNCTION_ENTRY("OnAppAbout");
        TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY("OnLButtonDblClk");
        if(m_screen.getSelScreenMonitor() != NULL)
        {
            updateDialog();
            ClientToScreen(&point);
            stuMonitorScreen* screen = m_screen.getScreenFromPointer(point);
            m_screen.setSelScreen(screen);
            Monitor* monitor = m_screen.getSelScreenMonitor();
            // get the associated input
            VideoInput* input = NULL;
            if(monitor != NULL)
            {
                input = monitor->getInput();
            }
            if(input == NULL || monitor->isQuadInput())
            {
                loadQuadController(monitor);
            }
            else if(input != NULL)
            {
                // load the context menu based on the type of assignment
                Sequence* seq = dynamic_cast<Sequence*>(input);
                if(seq != NULL)
                {
                    loadSequenceController(seq);
                }
            }
        }
        TransActiveDialog::OnLButtonDblClk(nFlags, point);
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerDlg::OnClose()
    {
        FUNCTION_ENTRY("OnClose");
        TransActiveDialog::DestroyWindow();
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerDlg::updateDetailsBox()
    {
        FUNCTION_ENTRY("updateDetailsBox");
        Monitor* monitor = m_screen.getSelScreenMonitor();
        if(monitor != NULL)
        {
            m_detailsBox.SetWindowText(monitor->getDetails().c_str());
        }
        else
        {
            m_detailsBox.SetWindowText(NODEVICE);
        }
        FUNCTION_EXIT;
    }

    LRESULT VideoDisplayManagerDlg::onMonitorUpdated(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("onMonitorUpdated");
        DisplayItem* item = reinterpret_cast<DisplayItem*>(lParam);
        TA_ASSERT(item != NULL, "Monitor update received for NULL display item");
        Monitor* monitor = dynamic_cast<Monitor*>(item);
        TA_ASSERT(monitor != NULL, "Monitor update received for not a monitor");
        UINT monitorAddr = atoi(monitor->getAddress().c_str());
        stuMonitorScreen* screen = m_screen.getScreenByMonitorAddress(monitorAddr);
        if(screen != NULL)
        {
            m_screen.draw(*screen);
        }
        if(monitor == m_screen.getSelScreenMonitor())
        {
            updateDetailsBox();
        }
        FUNCTION_EXIT;
        return 0;
    }


    void VideoDisplayManagerDlg::buildTrainContextMenu(CMenu* menu, std::vector<VideoOutput::TrainCamera> stages)
    {
        FUNCTION_ENTRY("buildTrainContextMenu");
        CMenu* subMenu = menu->GetSubMenu(0);
        subMenu->RemoveMenu(0, MF_BYPOSITION);
        unsigned long previousTrainId = 0;
        std::vector<VideoOutput::TrainCamera>::iterator itr;
        int i = TRAIN_COMMAND_OFFSET;
        for(itr = stages.begin(); itr != stages.end(); itr++, i++)
        {
            if(previousTrainId != 0 && previousTrainId != (*itr).trainId)
            {
                subMenu->AppendMenu(MF_SEPARATOR, i, "");
            }
            previousTrainId = (*itr).trainId;
            std::stringstream train;
            train << "Train ";
            if((*itr).serviceNum != "")
            {
                train << (*itr).serviceNum;
            }
            else
            {
                train << (*itr).trainId;
            }
            train << " Camera ";
            train << (*itr).cameraId;
            subMenu->AppendMenu(MF_STRING, i, (LPCTSTR)(train.str().c_str()));
        }
        FUNCTION_EXIT;
    }

    void VideoDisplayManagerDlg::TestRunParamChange(const std::string& name, const std::string& value)
    {
        onRunParamChange(name, value);
    }

    void VideoDisplayManagerDlg::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY("onRunParamChange");
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "onRunParamChange(%s,%s)", name.c_str(), value.c_str());
        //  return;
        CWaitCursor waitcursor;
        if(name == RPARAM_ENTITY)
        {
            m_paramCameraAddress = value;
        }
        if(name == RPARAM_DISPLAYMONITOR)
        {
            std::vector<std::string> consoleMonitors = DisplayItemManager::getInstance().getConsoleMonitors();
            std::vector<std::string>::iterator  it;
            for(it = consoleMonitors.begin(); it != consoleMonitors.end(); it++)
            {
                std::string monitorName = *it;
                if(monitorName.find(value) != std::string::npos)
                {
                    m_paramMonitorName = monitorName;
                }
            }
        }
        //  When both runParam strings are filled then the camera-to-monitor switch
        //  can be performed.
        if(!(m_paramMonitorName.empty() || m_paramCameraAddress.empty()))
        {
            Monitor* monitor = DisplayItemManager::getInstance().getMonitor(m_paramMonitorName);
            Camera* camera = DisplayItemManager::getInstance().getCameraByAddress(m_paramCameraAddress);
            CString sMsg("");
            if(monitor == NULL)
            {
                sMsg.Format("Specified monitor (%s) is invalid", m_paramMonitorName.c_str());
            }
            else if(camera == NULL)
            {
                sMsg.Format("Specified camera (%s) is invalid", m_paramCameraAddress.c_str());
            }
            //  This runParam switch command has been triggered, so reset the trigger strings.
            m_paramCameraAddress = "";
            m_paramMonitorName = "";
            if((monitor == NULL) || (camera == NULL))
            {
                AfxMessageBox(sMsg, MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
                FUNCTION_EXIT;
                return;
            }
            try
            {
                TA_IRS_App::SwitchAgentCommunicator::getInstance().assignMonitor(monitor, camera);
                m_isAssignedBySchematic = true;
            }
            catch(TA_Base_Core::TransactiveException& te)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
                AfxMessageBox(te.what());
            }
        }
        FUNCTION_EXIT;
    }


    LRESULT VideoDisplayManagerDlg::OnSetWindowPosition(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnSetWindowPosition");
        if(m_isLaunched && m_isAssignedBySchematic && (lParam == TA_Base_Bus::TA_GenericGui::REPOSITION))
        {
            m_isAssignedBySchematic = false;
            FUNCTION_EXIT;
            return TransActiveDialog::OnSetWindowPosition(wParam, TA_Base_Bus::TA_GenericGui::REPOSITION_NO_RESTORE);
        }
        else
        {
            FUNCTION_EXIT;
            return TransActiveDialog::OnSetWindowPosition(wParam, lParam);
        }
        FUNCTION_EXIT;
    }

    void VideoDisplayManagerDlg::createCctvProxy()
    {
        FUNCTION_ENTRY("createCctvProxy");
        m_trainCctvCorbaProxy = TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainCctvCorbaProxy();
        FUNCTION_EXIT;
    }

    void VideoDisplayManagerDlg::ModifyWindowPos() //This function to modify the dialog position in three monitor
    {
        int left, top, width, height;
        int screenWidth, screenHeight, currentScreen, perScreenWidth;
        int numScreens = 1;
        CPoint   pt;
        GetCursorPos(&pt);
        // Get dialog size
        RECT windowRect;
        GetWindowRect(&windowRect);
        LONG windowWidth = windowRect.right - windowRect.left + 1;
        LONG windowHeight = windowRect.bottom - windowRect.top + 1;
        // Get schematic rect
        RECT boundary;
        try
        {
            boundary = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT,
                                                                       TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,
                                                                       pt.x);
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Exception", "");
            return;
        }
        // Get screen information
        screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
        screenHeight = GetSystemMetrics(SM_CYVIRTUALSCREEN);
        if(screenWidth >= MINVALUE)
        {
            numScreens = NUMSCREENS;
        }
        perScreenWidth = screenWidth / numScreens;
        currentScreen = pt.x / perScreenWidth;
        // Calculate the window position
        left    = currentScreen * perScreenWidth + (perScreenWidth - windowWidth) / 2;
        width   = windowWidth;
        if(windowHeight > (boundary.bottom - boundary.top + 1))
        {
            top = boundary.top;
        }
        else
        {
            top = boundary.top + (boundary.bottom - boundary.top + 1 - windowHeight) / 2;
        }
        height  = windowHeight;
        this->MoveWindow(left, top, width, height, NULL);
    }
} // TA_IRS_App
