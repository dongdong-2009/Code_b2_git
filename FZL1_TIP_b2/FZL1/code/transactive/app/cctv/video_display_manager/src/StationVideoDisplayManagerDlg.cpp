/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3002_TIP/3002/transactive/app/cctv/video_display_manager/src/VideoDisplayManagerDlg.cpp $
* @author:  Robert Young
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2009/10/01 16:36:38 $
* Last modified by:  $Author: grace.koh $
*
* TODO: <description>
*/

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif

#include "stdafx.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerPresenter.h"
#include "app/cctv/video_display_manager/src/resource.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManager.h"
#include "app/cctv/video_display_manager/src/StationVideoDisplayManagerDlg.h"
#include "app/cctv/video_display_manager/src/DlgVideoPlay.h"
#include "app/cctv/video_display_manager/src/CCTVCommon.h"
#include "app/cctv/video_display_manager/src/SDKCommunicator.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
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
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SM_CXVIRTUALSCREEN      78
#define MINVALUE    2400
#define NUMSCREENS    3

using TA_Base_Core::RunParams;

namespace TA_IRS_App
{
    StationVideoDisplayManagerDlg::StationVideoDisplayManagerDlg(TA_Base_Bus::IGUIAccess& controlClass,
                                                                 VideoDisplayManagerPresenter& presenter)
        : IGFVideoSwitchDisplayView(controlClass, StationVideoDisplayManagerDlg::IDD, NULL),
          m_mouseTracking(FALSE),
          m_dragImage(NULL),
          m_dragAdjustPoint(0, 0),
          m_pSequenceEditorDlg(0),
          m_isAssignedBySchematic(false),
          m_overriddenLocation(0),
          m_presenter(presenter)
    {
        FUNCTION_ENTRY("VideoDisplayManagerDlg");
        m_sequenceTreeCtrl.setVideoDisplayManagerPresenter(&m_presenter);
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
        // Determine Physical Site of this Console Session
        using TA_Base_Core::DataException;
        using TA_Base_Core::DatabaseException;
        // Location should be depot or station
        TA_Base_Core::ILocation::ELocationType locationType = TA_Base_Core::ILocation::DPT;
        std::string sessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        try
        {
            TA_Base_Core::IConsole* console =
                TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionID);
            TA_Base_Core::ILocation* location =
                TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(console->getLocation());
            locationType = location->getLocationType();
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
        FUNCTION_EXIT;
    }


    StationVideoDisplayManagerDlg::~StationVideoDisplayManagerDlg()
    {
        FUNCTION_ENTRY("~VideoDisplayManagerDlg");
        FUNCTION_EXIT;
    }


    void StationVideoDisplayManagerDlg::setDragImage(CImageList* dragImage)
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


    void StationVideoDisplayManagerDlg::DoDataExchange(CDataExchange* pDX)
    {
        IGFVideoSwitchDisplayView::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(VideoDisplayManagerDlg)
        //}}AFX_DATA_MAP
        DDX_Control(pDX, IDC_STATION_SEQ_STATION_COMB, m_stationListForSEQ);
        DDX_Control(pDX, IDC_STATION_SEQENCE_TREE, m_sequenceTreeCtrl);
        DDX_Control(pDX, IDC_STATION_SEQ_LST, m_sequencesListBox);
        DDX_Control(pDX, IDC_STATION_SEL_STATION_COMB, m_stationListForCamera);
        DDX_Control(pDX, IDC_STATION_EDIT_SEQ_BTN, m_editSequenceBtn);
        DDX_Control(pDX, IDC_QUAD1, m_quad1);
        DDX_Control(pDX, IDC_QUAD2, m_quad2);
        DDX_Control(pDX, IDC_QUAD3, m_quad3);
        DDX_Control(pDX, IDC_QUAD4, m_quad4);
        DDX_Control(pDX, IDC_LOCAL_MONITOR, m_localMonitor);
        //text
        DDX_Control(pDX, IDC_QUAD1_TEXT, m_quad1_Text);
        DDX_Control(pDX, IDC_QUAD2_TEXT, m_quad2_Text);
        DDX_Control(pDX, IDC_QUAD3_TEXT, m_quad3_Text);
        DDX_Control(pDX, IDC_QUAD4_TEXT, m_quad4_Text);
        DDX_Control(pDX, IDC_LOCAL_MONITOR_TEXT, m_localMonitorText);
        //index label
        DDX_Control(pDX, IDC_QUAD1_INDEX, m_quad1_Index);
        DDX_Control(pDX, IDC_QUAD2_INDEX, m_quad2_Index);
        DDX_Control(pDX, IDC_QUAD3_INDEX, m_quad3_Index);
        DDX_Control(pDX, IDC_QUAD4_INDEX, m_quad4_Index);
        //pan button
        DDX_Control(pDX, IDC_PAN_LEFT_BTN, m_panLeftBtn);
        DDX_Control(pDX, IDC_PAN_RIGHT_BTN, m_panRightBtn);
        DDX_Control(pDX, IDC_TILT_UP_BTN, m_tiltUpBtn);
        DDX_Control(pDX, IDC_TILT_DOWN_BTN, m_tiltDownBtn);
        DDX_Control(pDX, IDC_PT_LEFT_UP_BTN, m_ptLeftUpBtn);
        DDX_Control(pDX, IDC_PT_RIGHT_UP_BTN, m_ptRightUpBtn);
        DDX_Control(pDX, IDC_PT_LEFT_DOWN_BTN, m_ptLeftDownBtn);
        DDX_Control(pDX, IDC_PT_RIGHT_DOWN_BTN, m_ptRightDownBtn);
        DDX_Control(pDX, IDC_ZOOM_OUT_BTN, m_zoomOutBtn);
        DDX_Control(pDX, IDC_ZOOM_IN_BTN, m_zoomInBtn);
        DDX_Control(pDX, IDC_STATION_SCHEMATIC_SEL_LIST, m_schematicLevesList);
    }

    BEGIN_MESSAGE_MAP(StationVideoDisplayManagerDlg, IGFVideoSwitchDisplayView)
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
        ON_MESSAGE(TA_INIT_CAMERAS, OnInitCameras)
        ON_MESSAGE(TA_INIT_QUADS, OnInitQuads)
        ON_MESSAGE(TA_INIT_SEQUENCES, OnInitSequences)
        ON_MESSAGE(TA_INIT_MONITORS, OnInitMonitors)
        ON_MESSAGE(TA_INIT_CCTV, OnInitDisplayManager)
        ON_MESSAGE(TA_RUN_PARAM , onHandleRunParam)
        ON_MESSAGE(TA_INIT_STAGES, OnInitStages)
        ON_MESSAGE(TA_INIT_STATION, OnInitStations)
        ON_CBN_SELCHANGE(IDC_STATION_SEQ_STATION_COMB, onSelectionChangeStationList)
        ON_BN_CLICKED(IDC_STATION_EDIT_SEQ_BTN, onEditSequence)
        ON_WM_TIMER()
        ON_LBN_SELCHANGE(IDC_STATION_SCHEMATIC_SEL_LIST, OnLevelSelChange)
        ON_MESSAGE(REFRESH_SCHEMATIC_MESSAGE, onRefreshSchematic)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // VideoDisplayManagerDlg message handlers

    void StationVideoDisplayManagerDlg::initPtzBtnsActionHandler()
    {
        SDKCommunicator* handler = &SDKCommunicator::getInstance();
        m_zoomInBtn.setBtnDownActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_ZOOMIN));
        m_zoomOutBtn.setBtnDownActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_ZOOMOUT));
        m_panLeftBtn.setBtnDownActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_LEFT));
        m_panRightBtn.setBtnDownActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_RIGHT));
        m_tiltUpBtn.setBtnDownActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_UP));
        m_tiltDownBtn.setBtnDownActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_DOWN));
        m_ptLeftDownBtn.setBtnDownActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_LEFTDOWN));
        m_ptLeftUpBtn.setBtnDownActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_LEFTUP));
        m_ptRightDownBtn.setBtnDownActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_RIGHTDOWN));
        m_ptRightUpBtn.setBtnDownActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_RIGHTUP));

		m_zoomInBtn.setBtnUpActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_ALLSTOP));
		m_zoomOutBtn.setBtnUpActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_ALLSTOP));
		m_panLeftBtn.setBtnUpActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_ALLSTOP));
		m_panRightBtn.setBtnUpActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_ALLSTOP));
		m_tiltUpBtn.setBtnUpActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_ALLSTOP));
		m_tiltDownBtn.setBtnUpActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_ALLSTOP));
		m_ptLeftDownBtn.setBtnUpActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_ALLSTOP));
		m_ptLeftUpBtn.setBtnUpActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_ALLSTOP));
		m_ptRightDownBtn.setBtnUpActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_ALLSTOP));
		m_ptRightUpBtn.setBtnUpActionHandler(boost::bind(&SDKCommunicator::ptzRequest, handler, _1, PTZ_ALLSTOP));
    }

    BOOL StationVideoDisplayManagerDlg::OnInitDialog()
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
        IGFVideoSwitchDisplayView::OnInitDialog();
        SetIcon(m_hIcon, TRUE);
        SetIcon(m_hIcon, FALSE);
        //
        // Build the main dialog
        //
        // get the location of the GUI
        TA_Base_Core::ILocation::ELocationType locationType = ((VideoDisplayManagerApp*)(AfxGetApp()))->getLocationType();
        AfxGetApp()->DoWaitCursor(1);
        EnableEditSeqButton(false);
        initPtzBtnsActionHandler();
        m_ptzButtons.push_back(&m_zoomInBtn);
        m_ptzButtons.push_back(&m_zoomOutBtn);
        m_ptzButtons.push_back(&m_panLeftBtn);
        m_ptzButtons.push_back(&m_panRightBtn);
        m_ptzButtons.push_back(&m_tiltUpBtn);
        m_ptzButtons.push_back(&m_tiltDownBtn);
        m_ptzButtons.push_back(&m_ptLeftUpBtn);
        m_ptzButtons.push_back(&m_ptRightUpBtn);
        m_ptzButtons.push_back(&m_ptLeftDownBtn);
        m_ptzButtons.push_back(&m_ptRightDownBtn);
        // Disalbe station select
        m_stationListForSEQ.EnableWindow(FALSE);
        m_stationListForCamera.EnableWindow(FALSE);
        // Disalbe all ptz buttons
        enablePTZButton(FALSE);
        DisplayItemManager::getInstance().initialiseDisplayItems(this);
        SetTimer(HALF_MINUTE_EVENT, 30000, NULL);
		// Monitor index text color
		/*
		m_quad1_Index.SetTextColor(MONITOR_INDEX_TEXT_COLOR);
		m_quad2_Index.SetTextColor(MONITOR_INDEX_TEXT_COLOR);
		m_quad3_Index.SetTextColor(MONITOR_INDEX_TEXT_COLOR);
		m_quad4_Index.SetTextColor(MONITOR_INDEX_TEXT_COLOR);
		*/
        FUNCTION_EXIT;
        return TRUE;
    }

    void StationVideoDisplayManagerDlg::LoadButtonIcons()
    {
        FUNCTION_ENTRY("LoadButtonIcons");
        m_zoomInBtn.setIcons(IDI_ZOOM_IN_ICON);
        m_zoomOutBtn.setIcons(IDI_ZOOM_OUT_ICON);
        m_ptLeftUpBtn.setIcons(IDI_PT_NW_ICON);
        m_ptRightUpBtn.setIcons(IDI_PT_NE_ICON);
        m_ptRightDownBtn.setIcons(IDI_PT_SE_ICON);
        m_ptLeftDownBtn.setIcons(IDI_PT_SW_ICON);
        m_tiltUpBtn.setIcons(IDI_NORTH_ARROW);
        m_panRightBtn.setIcons(IDI_EAST_ARROW);
        m_tiltDownBtn.setIcons(IDI_SOUTH_ARROW);
        m_panLeftBtn.setIcons(IDI_WEST_ARROW);
        FUNCTION_EXIT;
    }

    LRESULT StationVideoDisplayManagerDlg::OnInitStations(WPARAM wParam, LPARAM lParam)
    {
        std::vector < TA_Base_Core::ILocation*> stations = DisplayItemManager::getInstance().getStations();
        if(!stations.empty())
        {
	        int stationID = m_presenter.getLocationId();
			BOOST_FOREACH(TA_Base_Core::ILocation* i, stations)
	        {
	            if(i->getKey() == stationID)
	            {
	                std::string str = i->getDisplayName();
	                int index = m_stationListForCamera.AddString(i->getDisplayName().c_str());
	                m_stationListForCamera.SetItemData(index, i->getKey());
	                m_stationListForCamera.SetCurSel(index);
                    m_schematicLevesList.changeLocation(GetStationIdForGwax());
                    m_presenter.loadStationDiagram(m_schematicLevesList.getFirstItemLevel());
	                index = m_stationListForSEQ.AddString(i->getDisplayName().c_str());
	                m_stationListForSEQ.SetItemData(index, i->getKey());
	                m_stationListForSEQ.SetCurSel(index);
	                break;
	            }
	        }
        }
        return 0;
    }

    LRESULT StationVideoDisplayManagerDlg::OnInitSequences(WPARAM wParam, LPARAM lParam)
    {
        return 0;
    }

    LRESULT StationVideoDisplayManagerDlg::OnInitCameras(WPARAM wParam, LPARAM lParam)
    {
        return 0;
    }
    LRESULT StationVideoDisplayManagerDlg::OnInitQuads(WPARAM wParam, LPARAM lParam)
    {
        return 0;
    }
    LRESULT StationVideoDisplayManagerDlg::OnInitStages(WPARAM wParam, LPARAM lParam)
    {
        return 0;
    }

    LRESULT StationVideoDisplayManagerDlg::OnInitMonitors(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnInitMonitors");
        AfxGetApp()->DoWaitCursor(0);
        TA_Base_Core::ILocation::ELocationType locationType =
            ((VideoDisplayManagerApp*)(AfxGetApp()))->getLocationType();
        setupMonitors(locationType);
        DisplayItemManager::getInstance().registerForMonitorChanges(this);
        FUNCTION_EXIT;
        return 0;
    }


    LRESULT StationVideoDisplayManagerDlg::OnInitDisplayManager(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnInitDisplayManager");
        AfxGetApp()->DoWaitCursor(0);
        LoadButtonIcons();
        m_presenter.onCompleteInitStation();
        m_presenter.setUpdateSequenceTreeCtrl(&m_sequenceTreeCtrl);
        m_presenter.start();
		std::string ptzInterval = RunParams::getInstance().get(PTZREQUESTINTERVALEPRAMETERNAME);
		if(!ptzInterval.empty())
		{
			CPTZButton::setPtzRequestInterval(atoi(ptzInterval.c_str()));
		}
        // DisplayMonitor
        if(TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DISPLAYMONITOR))
        {
            onRunParamChange(RPARAM_DISPLAYMONITOR, TA_Base_Core::RunParams::getInstance().get(RPARAM_DISPLAYMONITOR));
        }
        RunParams::getInstance().registerRunParamUser(this, RPARAM_DISPLAYMONITOR);
        // TriggerEntity
		if(RunParams::getInstance().isSet(RPARAM_TRIGGER_ENTITY))
		{
            onRunParamChange(RPARAM_TRIGGER_ENTITY, RunParams::getInstance().get(RPARAM_TRIGGER_ENTITY));
		}
        RunParams::getInstance().registerRunParamUser(this, RPARAM_TRIGGER_ENTITY);
		// Clear monitor runparam
		if(RunParams::getInstance().isSet(RPARAM_CLEAR_MONITOR))
		{
			onRunParamChange(RPARAM_CLEAR_MONITOR, RunParams::getInstance().get(RPARAM_CLEAR_MONITOR));
		}
        RunParams::getInstance().registerRunParamUser(this, RPARAM_CLEAR_MONITOR);
        // Playback
		if(RunParams::getInstance().isSet(RPARAM_PLAYBACK))
		{
			onRunParamChange(RPARAM_PLAYBACK, RunParams::getInstance().get(RPARAM_PLAYBACK));
		}
        RunParams::getInstance().registerRunParamUser(this, RPARAM_PLAYBACK);
		// set ptz buttons time interval
        AfxGetApp()->DoWaitCursor(-1);
        FUNCTION_EXIT;
        return 0;
    }

    LRESULT StationVideoDisplayManagerDlg::onHandleRunParam(WPARAM wParam, LPARAM lParam)
    {
        RunParamStrut * runparam = (RunParamStrut *) wParam;
        std::string name = runparam->name;
        std::string value = runparam->value;
        delete runparam;
        m_presenter.onRunParamChange(name , value);
        return 0;
    }

    void StationVideoDisplayManagerDlg::OnDestroy()
    {
        FUNCTION_ENTRY("OnDestroy");
        m_presenter.terminateAndWait();
        DisplayItemManager::getInstance().deregisterForMonitorChanges(this);
        CDialog::OnDestroy();
        FUNCTION_EXIT;
    }


    void StationVideoDisplayManagerDlg::configureGUILayout(TA_Base_Core::ILocation::ELocationType locationType)
    {
        //
        // Set-up dialog for OCR, Station, or Depot
        //
        // If at the OCC or depot - remove one of the console monitors
    }

    void StationVideoDisplayManagerDlg::AddSequenceControlToConsole(int consolePreviousElementButton,
                                                                    int consolePlayCheck,
                                                                    int consoleNextElementButton,
                                                                    int consolePauseCheck,
                                                                    int consolePlaybackLabel,
                                                                    int offset)
    {
    }

    void StationVideoDisplayManagerDlg::setupMonitors(TA_Base_Core::ILocation::ELocationType locationType)
    {
        m_presenter.setUpMonitor();
    }


    Monitor* StationVideoDisplayManagerDlg::getMonitor(int controlId)
    {
        FUNCTION_ENTRY("getMonitor");
        // find and return the item in the map
        std::map<int, std::string>::iterator it = m_resourceIdToMonitorNameMap.find(controlId);
        FUNCTION_EXIT;
		return it != m_resourceIdToMonitorNameMap.end() ? DisplayItemManager::getInstance().getMonitor(it->second) : NULL;
    }


    int StationVideoDisplayManagerDlg::getControlId(std::string monitorName)
    {
        FUNCTION_ENTRY("getControlId");
        // find and return the item in the map
        std::map<std::string, int>::iterator it = m_monitorNameToResourceIdMap.find(monitorName);
        FUNCTION_EXIT;
		return it != m_monitorNameToResourceIdMap.end() ? it->second : NULL;
    }





    // If you add a minimize button to your dialog, you will need the code below
    //  to draw the icon.  For MFC applications using the document/view model,
    //  this is automatically done for you by the framework.
    void StationVideoDisplayManagerDlg::OnPaint()
    {
        FUNCTION_ENTRY("OnPaint");
        if(IsIconic())
        {
            CPaintDC dc(this);    // device context for painting
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
        IGFVideoSwitchDisplayView::OnPaint();
        // OnPaint should not directly call com method
        PostMessage(REFRESH_SCHEMATIC_MESSAGE);
        FUNCTION_EXIT;
    }


    // The system calls this to obtain the cursor to display while the user drags
    //  the minimized window.
    HCURSOR StationVideoDisplayManagerDlg::OnQueryDragIcon()
    {
        FUNCTION_ENTRY("OnQueryDragIcon");
        FUNCTION_EXIT;
        return (HCURSOR) m_hIcon;
    }


    BOOL StationVideoDisplayManagerDlg::DestroyWindow()
    {
        FUNCTION_ENTRY("DestroyWindow");
        if(m_dragImage != NULL)
        {
            delete m_dragImage;
        }
        FUNCTION_EXIT;
        return IGFVideoSwitchDisplayView::DestroyWindow();
    }


    // Don't allow an escape key command to be processed.
    BOOL StationVideoDisplayManagerDlg::PreTranslateMessage(MSG* pMsg)
    {
        switch(pMsg->message)
        {
            case WM_KEYDOWN:
                switch(pMsg->wParam)
                {
                    case VK_ESCAPE:
                        return TRUE;
                    case VK_LEFT:
                        return TRUE;
                    case VK_RIGHT:
                        return TRUE;
                        /*
                    case VK_UP:
                        return TRUE;
                        */
                    case VK_TAB:
                        return TRUE;
                    default:
                        break;
                }
                break;
            case WM_SYSCOMMAND:
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
                break;
            default:
                break;
        }
        return IGFVideoSwitchDisplayView::PreTranslateMessage(pMsg);
    }


    void StationVideoDisplayManagerDlg::OnLButtonDown(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY("OnLButtonDown");
        IGFVideoSwitchDisplayView::OnLButtonDown(nFlags, point);
        m_presenter.MainUILButtonDown(point);
        FUNCTION_EXIT;
    }

    void StationVideoDisplayManagerDlg:: beginDragImage()
    {
        SetCapture();
        m_mouseTracking = TRUE;
        CPoint cursorLocation;
        ::GetCursorPos(&cursorLocation);
        m_dragImage->BeginDrag(0, m_dragAdjustPoint);
        m_dragImage->DragEnter(NULL, cursorLocation);
    }


    void StationVideoDisplayManagerDlg::OnLButtonUp(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY("OnLButtonUp");
        IGFVideoSwitchDisplayView::OnLButtonUp(nFlags, point);
        m_presenter.MainUILButtonUp(point);
        FUNCTION_EXIT;
    }


    void StationVideoDisplayManagerDlg::OnMouseMove(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY("OnMouseMove");
        m_presenter.MainUIMouseMove(point);
        IGFVideoSwitchDisplayView::OnMouseMove(nFlags, point);
        FUNCTION_EXIT;
    }


    /**
    * UpdateDialog
    *
    * Sends a WM_PAINT message to the Vidiwall dialog.
    */
    void StationVideoDisplayManagerDlg::updateDialog()
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
    void StationVideoDisplayManagerDlg::invalidateControlLabels()
    {
    }


    void StationVideoDisplayManagerDlg::OnContextMenu(CWnd* pWnd, CPoint point)
    {
        FUNCTION_ENTRY("OnContextMenu");
        ScreenToClient(&point);
        m_presenter.MainUIContextMenu(point);
        CDialog::OnContextMenu(pWnd, point);
        FUNCTION_EXIT;
    }


    void StationVideoDisplayManagerDlg::loadSequenceController(Sequence* sequence)
    {
        /*
        FUNCTION_ENTRY("loadSequenceController");
        TA_ASSERT(sequence != NULL, "Cannot launch sequence editor for null sequence - menu item should not be accessible");
        // launch Sequence Editor
        SequenceEditorDlg sequenceEditorDlg(this, NULL);
        m_pSequenceEditorDlg = &sequenceEditorDlg;
        sequenceEditorDlg.DoModal(sequence, 0);
        m_pSequenceEditorDlg = NULL;
        FUNCTION_EXIT;
        */
    }


    void StationVideoDisplayManagerDlg::OnRButtonDown(UINT nFlags, CPoint point)
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
        IGFVideoSwitchDisplayView::OnRButtonDown(nFlags, point);
        FUNCTION_EXIT;
    }

    VideoInput* StationVideoDisplayManagerDlg::getSelectedVideoSource()
    {
        FUNCTION_ENTRY("getSelectedVideoSource");
        // Hopefully when the back end for train operations is
        // properly implemented - this can all be encapsulated in
        // VideoSourcesPropertySheet
        VideoInput* source = NULL;
        source = m_sequenceTreeCtrl.getSelectedInput();
        return source;
        FUNCTION_EXIT;
        return source;
    }


    void StationVideoDisplayManagerDlg::endDragging()
    {
        FUNCTION_ENTRY("endDragging");
        m_sequenceTreeCtrl.endDragging();
        FUNCTION_EXIT;
    }


    void StationVideoDisplayManagerDlg::OnHelpDisplaymanagerhelp()
    {
        FUNCTION_ENTRY("OnHelpDisplaymanagerhelp");
        TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
        FUNCTION_EXIT;
    }


    void StationVideoDisplayManagerDlg::OnAppAbout()
    {
        FUNCTION_ENTRY("OnAppAbout");
        TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
        FUNCTION_EXIT;
    }

    void StationVideoDisplayManagerDlg::OnClose()
    {
        FUNCTION_ENTRY("OnClose");
        KillTimer(HALF_MINUTE_EVENT);
        try
        {
            m_presenter.closeSchematic();
        }
        catch(...)
        {
        }
        IGFVideoSwitchDisplayView::DestroyWindow();
        FUNCTION_EXIT;
    }





    LRESULT StationVideoDisplayManagerDlg::onMonitorUpdated(WPARAM wParam, LPARAM lParam)
    {
        return 0;
    }



    void StationVideoDisplayManagerDlg::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY("onRunParamChange");
        RunParamStrut * runparam = new RunParamStrut();
        runparam->name = name;
        runparam->value = value;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "get runtime param. name is %s value is %s",
                    name.c_str() , value.c_str());
        PostMessage(TA_RUN_PARAM , (WPARAM) runparam , 0);
    }


    void StationVideoDisplayManagerDlg::onSelectionChangeStationList()
    {
        m_presenter.SelectStation();
    }

    unsigned long StationVideoDisplayManagerDlg::GetStationIdForGwax()
    {
        int curItemIdx = m_stationListForCamera.GetCurSel();
        unsigned long locationId  = 0;
        if(CB_ERR != curItemIdx)
        {
            locationId = m_stationListForCamera.GetItemData(curItemIdx);
        }
        return locationId;
    }

    void StationVideoDisplayManagerDlg::DisplaySequences(std::vector<VideoInput*> & sequences)
    {
        m_sequenceTreeCtrl.DeleteAllItems();
        m_sequenceTreeCtrl.insertVideoSources(sequences);
    }

    void StationVideoDisplayManagerDlg::DisplaySequence(Sequence* sequence)
    {
        m_sequencesListBox.ResetContent();
        // m_sequencesListBox.addSourceString ( sequence->getDescription ().c_str ()  );
        std::ostringstream desc;
        desc << "ÊÓÆµÐòÁÐ" << sequence->getName();
        m_sequencesListBox.addSourceString(desc.str().c_str());
        std::list<Camera*> cameraList = sequence->getCameras();
        std::list<Camera*>::iterator iter = cameraList.begin();
        for(; iter != cameraList.end(); ++iter)
        {
            desc.clear();
            desc.str("");
            desc << "ÉãÏñÍ·" << (*iter)->getName();
            m_sequencesListBox.addSourceString(desc.str().c_str());
        }
    }

    void StationVideoDisplayManagerDlg::UpdateSequenceTreeCtrl(Sequence* sequence)
    {
        m_sequenceTreeCtrl.videoInputUpdated(sequence);
    }


    void StationVideoDisplayManagerDlg::GetSchematicCRET(CRect& markerRect)
    {
        CWnd * marker = GetDlgItem(IDC_STATION_STATION_PIC);
        marker->GetWindowRect(markerRect);
    }

    HWND StationVideoDisplayManagerDlg::GetWndHandle()
    {
        return GetSafeHwnd();
    }

    unsigned long StationVideoDisplayManagerDlg::GetSelectedSequenceId()
    {
        return 0;
    }

    unsigned long StationVideoDisplayManagerDlg:: GetSelectedStation()
    {
        int curItemIdx = m_stationListForSEQ.GetCurSel();
        unsigned long locationId  = 0;
        if(CB_ERR != curItemIdx)
        {
            locationId = m_stationListForSEQ.GetItemData(curItemIdx);
        }
        return locationId;
    }

    void StationVideoDisplayManagerDlg::onEditSequence()
    {
        VideoInput * input = m_sequenceTreeCtrl.getSelectedInput();
        if(NULL != input)
        {
            unsigned long seqId = input->getKey();
            m_presenter.editSequence(seqId);
        }
    }

    void StationVideoDisplayManagerDlg ::EnableEditSeqButton(bool enabled)
    {
        m_editSequenceBtn.EnableWindow(enabled);
    }

    MonitorDisplayItem* StationVideoDisplayManagerDlg::getMonitorDisplayitem(int index , Monitor* monitor)
    {
        MonitorDisplayItem* item = new MonitorDisplayItem();
        item->setParentWnd(this);
        HBITMAP standard = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SINGLEVMON));
        HBITMAP hightlight = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SINGLEVMON_HILITE));

        switch(index)
        {
                //get local monitor
            case 1 :
                item->setIndex(index);
                item->setTextLabel(&m_quad1_Text);
                item->setIndexLabel(&m_quad1_Index);
                m_quad1_Text.SetTransparent(TRUE);
                m_quad1_Index.SetTransparent(TRUE);
                item->setMonitorwnd(new MonitorWnd(&m_quad1, standard, hightlight));
                item->SetMonitor(monitor);
                item->setLocalMonitor(false);
                break;
            case 2:
                item->setIndex(index);
                item->setTextLabel(&m_quad2_Text);
                item->setIndexLabel(&m_quad2_Index);
                m_quad2_Text.SetTransparent(TRUE);
                m_quad2_Index.SetTransparent(TRUE);
                item->setMonitorwnd(new MonitorWnd(&m_quad2, standard, hightlight));
                item->SetMonitor(monitor);
                item->setLocalMonitor(false);
                break;
            case 3 :
                item->setIndex(index);
                item->setTextLabel(&m_quad3_Text);
                item->setIndexLabel(&m_quad3_Index);
                m_quad3_Text.SetTransparent(TRUE);
                m_quad3_Index.SetTransparent(TRUE);
                item->setMonitorwnd(new MonitorWnd(&m_quad3, standard, hightlight));
                item->SetMonitor(monitor);
                item->setLocalMonitor(false);
                break;
            case 4:
                item->setIndex(index);
                item->setTextLabel(&m_quad4_Text);
                item->setIndexLabel(&m_quad4_Index);
                m_quad4_Text.SetTransparent(TRUE);
                m_quad4_Index.SetTransparent(TRUE);
                item->setMonitorwnd(new MonitorWnd(&m_quad4, standard, hightlight));
                item->SetMonitor(monitor);
                item->setLocalMonitor(false);
                break;
            case 0:
                // local monitor
                item->setIndex(index);
                item->setTextLabel(&m_localMonitorText);
                m_localMonitorText.SetTransparent(TRUE);
                item->setMonitorwnd(new MonitorWnd(&m_localMonitor, standard, hightlight));
                item->SetMonitor(monitor);
                item->setLocalMonitor(true);
                break;
            default:
                delete item;
                item = NULL;
                break;
        }
        return item ;
    }

    void StationVideoDisplayManagerDlg::paintStandControl(MonitorWnd * item)
    {
        item->m_wnd->SetBitmap(item->m_standardBitmap);
        item->m_wnd->Invalidate();
        IGFVideoSwitchDisplayView::OnPaint();
    }


    void StationVideoDisplayManagerDlg::paintHightedControl(MonitorWnd * item)
    {
        item->m_wnd->SetBitmap(item->m_highlightBitmap);
        item->m_wnd->Invalidate();
        IGFVideoSwitchDisplayView::OnPaint();
    }

    void StationVideoDisplayManagerDlg::assignMonitor(MonitorDisplayItem * item)
    {
        if(NULL != item)
        {
            CLabel* labelWnd = item->getTextLabel();
            TA_IRS_App::VideoInput* input = item->getVideoInput();
            if(NULL != input)
            {
                labelWnd->SetText(input->getName().c_str());
            }
            else
            {
                labelWnd->SetText("");
            }
            labelWnd->SetTransparent(TRUE);
        }
    }

    void StationVideoDisplayManagerDlg::endDrage()
    {
        ::ReleaseCapture();
        endDragging();
        SetCursor(LoadCursor(NULL, IDC_ARROW));
        m_dragImage->DragLeave(this);
        m_dragImage->EndDrag();
        if(NULL != m_dragImage)
        {
            delete m_dragImage;
            m_dragImage = NULL;
        }
    }

    void StationVideoDisplayManagerDlg:: handleDragImageOnMouseMove(BOOL begin , CPoint& point)
    {
        if(false == begin)
        {
            m_dragImage->DragLeave(NULL);
            m_dragImage->EndDrag();
        }
        else
        {
            ClientToScreen(&point);
            m_dragImage->BeginDrag(0, m_dragAdjustPoint);
            m_dragImage->DragEnter(NULL, point);
            m_contextActive = FALSE;
            m_dragImage->DragMove(point);
            ScreenToClient(&point);
        }
    }

    void StationVideoDisplayManagerDlg::loadSequenceSubMenu(CPoint& point , Monitor * monitor, Sequence * seq, bool bIsLocalMonitor)
    {
        CMenu contextMenu;
        contextMenu.LoadMenu(IDR_MONSEQCONTEXT_MENU);
        /*
        contextMenu.EnableMenuItem(ID_VIDEO_PLAY, (bIsLocalMonitor && !m_videoPlaying) ? MF_ENABLED : MF_GRAYED);
        contextMenu.EnableMenuItem(ID_VIDEO_STOP, (bIsLocalMonitor && m_videoPlaying) ? MF_ENABLED : MF_GRAYED);
        */
        m_contextActive = TRUE;
        CMenu* pContextMenu = contextMenu.GetSubMenu(0);
        ClientToScreen(&point);
        int nCmd = (int) pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
                                                      point.x, point.y, AfxGetMainWnd());
        m_contextActive = FALSE;
        switch(nCmd)
        {
            case ID_CLEAR_MONITOR:
            {
                m_presenter.clearSequenceFromMonitor(monitor);
            }
            break;
            /*
            case ID_NEW_EDIT_SEQUENCE:
                m_presenter.editSequence(seq->getKey());
                break;
            case ID_VIDEO_PLAY:
                m_presenter.playVideo();
                contextMenu.EnableMenuItem(ID_VIDEO_PLAY, MF_GRAYED);
                contextMenu.EnableMenuItem(ID_VIDEO_STOP, MF_ENABLED);
                break;
            case ID_VIDEO_STOP:
                m_presenter.stopVideo();
                contextMenu.EnableMenuItem(ID_VIDEO_PLAY, MF_ENABLED);
                contextMenu.EnableMenuItem(ID_VIDEO_STOP, MF_GRAYED);
                break;
            */
            default:
                break;
        }
    }

    void StationVideoDisplayManagerDlg::loadCameraSubMenu(CPoint& point , Monitor * monitor, bool bIsLcoalMonitor)
    {
        CMenu contextMenu;
        contextMenu.LoadMenu(IDR_MONCAMCONTEXT_MENU);
        CMenu* pContextMenu = contextMenu.GetSubMenu(0);
        /*
        contextMenu.EnableMenuItem(ID_VIDEO_PLAY, (bIsLcoalMonitor && !m_videoPlaying) ? MF_ENABLED : MF_GRAYED);
        contextMenu.EnableMenuItem(ID_VIDEO_STOP, (bIsLcoalMonitor && m_videoPlaying) ? MF_ENABLED : MF_GRAYED);
        */
        m_contextActive = TRUE;
        ClientToScreen(&point);
        int nCmd = (int) pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
                                                      point.x, point.y, AfxGetMainWnd());
        // Process the appropriate context menu command.
        m_contextActive = FALSE;
        switch(nCmd)
        {
            case ID_CLEAR_MONITOR:
            {
                m_presenter.clearCameraFromMonitor(monitor);
            }
            break;
            /*
            case ID_VIDEO_PLAY:
                m_presenter.playVideo();
                contextMenu.EnableMenuItem(ID_VIDEO_PLAY, MF_GRAYED);
                contextMenu.EnableMenuItem(ID_VIDEO_STOP, MF_ENABLED);
                break;
            case ID_VIDEO_STOP:
                m_presenter.stopVideo();
                contextMenu.EnableMenuItem(ID_VIDEO_PLAY, MF_ENABLED);
                contextMenu.EnableMenuItem(ID_VIDEO_STOP, MF_GRAYED);
                break;
            */
            default:
                break;
        }
    }

    void StationVideoDisplayManagerDlg::displayErrorMessage(CString& message)
    {
        AfxMessageBox(message , MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
    }

    void StationVideoDisplayManagerDlg::enablePTZButton(BOOL enabled)
    {
		BOOST_FOREACH(CPTZButton* i, m_ptzButtons)
		{
			i->EnableWindow(enabled);
		}
    }

    void StationVideoDisplayManagerDlg::displayStaionCameraDiagram(std::string& fullFileName)
    {
    }

    /*
    void StationVideoDisplayManagerDlg::refreshIndexLable(int index, bool isQuad)
    {
        switch(index)
        {
            case 1:
                m_quad1_Index.Invalidate();
                break;
            case 2:
                m_quad2_Index.Invalidate();
                break;
            case 3:
                m_quad3_Index.Invalidate();
                break;
            case 4:
                m_quad4_Index.Invalidate();
                break;
            default:
                break;
        }
    }
    */

    unsigned short StationVideoDisplayManagerDlg::getPanSpeed()
    {
        unsigned short speed = 0;
        return speed;
    }

    unsigned short StationVideoDisplayManagerDlg::getTiltSpeed()
    {
        unsigned short speed = 0;
        return speed;
    }

    void StationVideoDisplayManagerDlg::enableStationSelection(BOOL enable)
    {
        m_stationListForSEQ.EnableWindow(enable);
        m_stationListForCamera.EnableWindow(enable);
    }

    void StationVideoDisplayManagerDlg::setSelectedStation(unsigned int index)
    {
        m_stationListForSEQ.SetCurSel(index);
        m_stationListForCamera.SetCurSel(index);
    }

    void StationVideoDisplayManagerDlg::OnTimer(UINT nEventID)
    {
        m_presenter.OnTimer(nEventID);
    }

    LRESULT StationVideoDisplayManagerDlg::onRefreshSchematic(WPARAM wParam, LPARAM lParam)
    {
        try
        {
            m_presenter.refreshSchematic();
        }
        catch(...)
        {
        }
        return 0;
    }

    void StationVideoDisplayManagerDlg::OnLevelSelChange()
    {
        m_presenter.loadStationDiagram(m_schematicLevesList.getCurrentSelLevel());
    }

} // TA_IRS_App
