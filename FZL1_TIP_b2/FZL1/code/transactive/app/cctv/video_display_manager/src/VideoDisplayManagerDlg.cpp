/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File$
* @author:  Robert Young
* @version: $Revision$
*
* Last modification: $DateTime$
* Last modified by:  $Author$
*
* TODO: <description>
*/

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif

#include "stdafx.h"
#include "app/cctv/video_display_manager/src/resource.h"
#include "app/cctv/video_display_manager/src/Camera.h"
#include "app/cctv/video_display_manager/src/DisplayItemManager.h"
#include "app/cctv/video_display_manager/src/Monitor.h"
#include "app/cctv/video_display_manager/src/Sequence.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManager.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerDlg.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerPresenter.h"
#include "app/cctv/video_display_manager/src/SequenceEditorDlg.h"
#include "app/cctv/video_display_manager/src/CCTVCommon.h"
#include "app/cctv/video_display_manager/src/DlgVideoPlay.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/generic_gui/src/GenericGuiConstants.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include <iomanip>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SM_CXVIRTUALSCREEN      78      //zhongjie++
#define MINVALUE                2400    //zhongjie++
#define NUMSCREENS              3       //zhongjie++

using TA_Base_Core::RunParams;

namespace TA_IRS_App
{
    const char* NODEVICE = "No item is currently selected";
    static const UINT WALLMONITOR_START_ID = 10000;
    static const UINT WALLMONITOR_TEXT_START_ID = WALLMONITOR_START_ID + NUMBER_WALL_MONITORS;
    static const UINT WALLMONITOR_INDEX_START_ID = WALLMONITOR_TEXT_START_ID + NUMBER_WALL_MONITORS;

    static const UINT SINGLE_WALLMONITOR_START_ID = WALLMONITOR_INDEX_START_ID + NUMBER_WALL_MONITORS;
    static const UINT SINGLE_WALLMONITOR_TEXT_START_ID = SINGLE_WALLMONITOR_START_ID + NUMBER_SINGLE_WALL_MONITORS;
    static const UINT SINGLE_WALLMONITOR_INDEX_START_ID = SINGLE_WALLMONITOR_TEXT_START_ID + NUMBER_SINGLE_WALL_MONITORS;
    /////////////////////////////////////////////////////////////////////////////
    // VideoDisplayManagerDlg dialog

    VideoDisplayManagerDlg::VideoDisplayManagerDlg(TA_Base_Bus::IGUIAccess& controlClass, VideoDisplayManagerPresenter& presenter)
        : IGFVideoSwitchDisplayView(controlClass, VideoDisplayManagerDlg::IDD, NULL),
          m_mouseTracking(FALSE),
          m_hilitedControlID(0),
          m_prvHilitedControlID(0),
          m_restoring(FALSE),
          m_contextActive(FALSE),
          m_hilitingPerformed(TRUE),
          m_normalisationPerformed(TRUE),
          m_dragImage(NULL),
          m_dragAdjustPoint(0, 0),
          m_pSequenceEditorDlg(0),
          m_isAssignedBySchematic(false),
          m_presenter(presenter)
    {
        FUNCTION_ENTRY("VideoDisplayManagerDlg");
        //{{AFX_DATA_INIT(CGenericGUIDialogTestDlg)
        // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
        // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
        m_sequenceTreeCtrl.setVideoDisplayManagerPresenter(&m_presenter);
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
        // Determine Physical Site of this Console Session
        using TA_Base_Core::DataException;
        using TA_Base_Core::DatabaseException;
        TA_Base_Core::ILocation::ELocationType locationType = TA_Base_Core::ILocation::OCC;
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
            //CString str("Caught DataException trying to determine site from console session: ");
            //str += ex.what();
            //AfxMessageBox(str);
        }
        catch(DatabaseException& ex)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << ex.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010029);
            //CString str("Caught DatabaseException trying to determine site from console session: ");
            //str += ex.what();
            //AfxMessageBox(str);
        }
        ((VideoDisplayManagerApp*)(AfxGetApp()))->setLocationType(locationType);
        // construct sheet
        FUNCTION_EXIT;
    }


    VideoDisplayManagerDlg::~VideoDisplayManagerDlg()
    {
        FUNCTION_ENTRY("~VideoDisplayManagerDlg");
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
        IGFVideoSwitchDisplayView::DoDataExchange(pDX);
        DDX_Control(pDX, IDC_GF_OCC_SEQ_STATION_COMB, m_stationListForSEQ);
        DDX_Control(pDX, IDC_GF_SEQENCE_TREE, m_sequenceTreeCtrl);
        DDX_Control(pDX, IDC_GF_SEQ_LST, m_sequencesListBox);
        DDX_Control(pDX, IDC_GF_OCC_SEL_STATION_COMB, m_stationListForCamera);
        DDX_Control(pDX, IDC_GF_EDIT_SEQ_BTN, m_editSequenceBtn);
		DDX_Control(pDX, IDC_QUAD1, m_quad1);
		DDX_Control(pDX, IDC_QUAD2, m_quad2);
		DDX_Control(pDX, IDC_QUAD3, m_quad3);
		DDX_Control(pDX, IDC_QUAD4, m_quad4);
        DDX_Control(pDX, IDC_CONSOLE_MONITOR_1, m_localMonitor);
		//text
		DDX_Control(pDX, IDC_QUAD1_TEXT, m_quad1_Text);
		DDX_Control(pDX, IDC_QUAD2_TEXT, m_quad2_Text);
		DDX_Control(pDX, IDC_QUAD3_TEXT, m_quad3_Text);
		DDX_Control(pDX, IDC_QUAD4_TEXT, m_quad4_Text);
        DDX_Control(pDX, IDC_TEXT_CONSOLE_MONITOR_2, m_localMonitorText);
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
        DDX_Control(pDX, IDC_OCC_SCHEMATIC_SEL_LIST, m_schematicLevesList);
    }


    BEGIN_MESSAGE_MAP(VideoDisplayManagerDlg, IGFVideoSwitchDisplayView)
        //{{AFX_MSG_MAP(VideoDisplayManagerDlg)
        ON_WM_PAINT()
        ON_WM_QUERYDRAGICON()
        ON_WM_LBUTTONDOWN()
        ON_WM_LBUTTONUP()
        ON_WM_MOUSEMOVE()
        ON_WM_CONTEXTMENU()
        ON_WM_RBUTTONDOWN()
        //      ON_WM_RBUTTONUP()
        ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
        ON_COMMAND(ID_APP_HELP, OnHelpDisplaymanagerhelp)
        ON_WM_LBUTTONDBLCLK()
        ON_WM_CLOSE()
        ON_WM_DESTROY()
        ON_MESSAGE(WM_MONITOR_UPDATED, onMonitorUpdated)
        ON_MESSAGE(TA_INIT_CAMERAS, OnInitCameras)
        ON_MESSAGE(TA_INIT_SEQUENCES, OnInitSequences)
        ON_MESSAGE(TA_INIT_STAGES, OnInitStages)
        ON_MESSAGE(TA_INIT_MONITORS, OnInitMonitors)
        ON_MESSAGE(TA_INIT_CCTV, OnInitDisplayManager)
        ON_MESSAGE(TA_RUN_PARAM , onHandleRunParam)
        ON_MESSAGE(TA_INIT_STATION, OnInitStations)
        ON_MESSAGE(REFRESH_SCHEMATIC_MESSAGE, onRefreshSchematic)
        ON_CBN_SELCHANGE(IDC_GF_OCC_SEQ_STATION_COMB, onSelectionChangeStationList)
        ON_CBN_SELCHANGE(IDC_GF_OCC_SEL_STATION_COMB, onSelectionChangeStationDiagramList)
        ON_BN_CLICKED(IDC_GF_EDIT_SEQ_BTN, onEditSequence)
        ON_WM_TIMER()
        ON_LBN_SELCHANGE(IDC_OCC_SCHEMATIC_SEL_LIST, OnLevelSelChange)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // VideoDisplayManagerDlg message handlers

	void VideoDisplayManagerDlg::initPtzBtnsActionHandler()
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
        IGFVideoSwitchDisplayView::OnInitDialog();
        SetIcon(m_hIcon, TRUE);        // Set big icon
        // Setup wall monitors gui component
        RECT rect;
        GetDlgItem(IDC_WALLMONITOR_PLACEHOLDER)->GetWindowRect(&rect);
        ScreenToClient(&rect);
		const int perQuadWith = (rect.right-rect.left+1)/6;

		BITMAP bm;
		m_wallMonitorBitmap.LoadBitmap(IDB_SINGLEODS_DIV_4);
		m_wallMonitorHighLightBitmap.LoadBitmap(IDB_SINGLEODS_HILITE_DIV_4);
		m_wallMonitorBitmap.GetBitmap(&bm);
		const int perMonitorWidth = bm.bmWidth;
		const int perMonitorHeight = bm.bmHeight;

		int quadLeftPos = rect.left;
		int quadTopPos = rect.top;
		UINT monitorId = WALLMONITOR_START_ID;
		UINT labelsId = WALLMONITOR_TEXT_START_ID;
		UINT indexId = WALLMONITOR_INDEX_START_ID;
		UINT count = 0;

        BITMAP bm2;
        m_singleWallMonitorBitmap.LoadBitmap(IDB_SINGLEODS);
        m_singleWallMonitorHighLightBitmap.LoadBitmap(IDB_SINGLEODS_HILITE);
		m_singleWallMonitorBitmap.GetBitmap(&bm2);
		const int perSingleWallMonitorWidth = bm2.bmWidth;
		const int perSingleWallMonitorHeight = bm2.bmHeight;
		// 6 quad
		for(int i = 0; i < 6; ++i, quadLeftPos += perQuadWith)
		{
			// 4 monitors per quad
			for(int j = 0; j < 4; ++j)
			{
				// Calculate monitor position
				RECT monitorRect;
				monitorRect.left = quadLeftPos + perMonitorWidth*(j%2);
                monitorRect.right = monitorRect.left + perMonitorWidth - 1;
                monitorRect.top = quadTopPos + perMonitorHeight*(j > 1 ? 1 : 0);
                monitorRect.bottom = monitorRect.top + perMonitorHeight - 1;

                m_wallMonitors[count].Create("",WS_CHILD|WS_VISIBLE|SS_BITMAP, monitorRect, this, monitorId++);
                m_wallMonitors[count].SetBitmap(m_wallMonitorBitmap);
                // Create index label
                CClientDC dc(this);
                CSize fontSize = dc.GetTextExtent("24");
                RECT indexLabelRect;
				indexLabelRect.left = monitorRect.right - fontSize.cx + 1;
				indexLabelRect.right = indexLabelRect.left + fontSize.cx - 1;
				indexLabelRect.bottom = monitorRect.top + fontSize.cy - 1;
				indexLabelRect.top = monitorRect.top + 1;
                m_wallMonitorsIndex[count].Create("", WS_CHILD|WS_VISIBLE|SS_CENTER, indexLabelRect, this, indexId++);
                // Create text label
                fontSize = dc.GetTextExtent("²âÊÔ");
                RECT textLabelRect;
                textLabelRect.left = monitorRect.left + 3;
                textLabelRect.right = monitorRect.right - 3;
				textLabelRect.top = indexLabelRect.bottom;
				textLabelRect.bottom = textLabelRect.top + fontSize.cy - 1;
                m_wallMonitorsText[count].Create("", WS_CHILD|WS_VISIBLE|SS_CENTER, textLabelRect, this, labelsId++);
                count++;
            }
            // Create single wall monitors
            RECT singleWallMonitorRect;
            singleWallMonitorRect.left = quadLeftPos;
            singleWallMonitorRect.right = quadLeftPos + perSingleWallMonitorWidth - 1;
            singleWallMonitorRect.top = quadTopPos;
            singleWallMonitorRect.bottom = quadTopPos + perSingleWallMonitorHeight - 1;
            m_singleWallMonitors[i].Create("", WS_CHILD | SS_BITMAP, singleWallMonitorRect, this, SINGLE_WALLMONITOR_START_ID + i);
            m_singleWallMonitors[i].SetBitmap(m_singleWallMonitorBitmap);

            CClientDC dc(this);
            CSize fontSize = dc.GetTextExtent("²âÊÔ");
            RECT singleWallMonitorTextRect;
            singleWallMonitorTextRect.left = singleWallMonitorRect.left + 3;
            singleWallMonitorTextRect.right = singleWallMonitorRect.right - 3;
			singleWallMonitorTextRect.top = singleWallMonitorRect.top + (singleWallMonitorRect.bottom - singleWallMonitorRect.top + 1 - fontSize.cy)/2;
			singleWallMonitorTextRect.bottom = singleWallMonitorTextRect.top + fontSize.cy - 1;
            m_singleWallMonitorsText[i].Create("", WS_CHILD|SS_CENTER, singleWallMonitorTextRect, this, SINGLE_WALLMONITOR_TEXT_START_ID + i);

            fontSize = dc.GetTextExtent("24");
            RECT indexLabelRect;
			indexLabelRect.left = singleWallMonitorRect.right - fontSize.cx + 1;
			indexLabelRect.right = indexLabelRect.left + fontSize.cx - 1;
			indexLabelRect.bottom = singleWallMonitorRect.top + fontSize.cy - 1;
			indexLabelRect.top = singleWallMonitorRect.top + 1;
            m_singleWallMonitorsIndex[i].Create("", WS_CHILD|SS_CENTER, indexLabelRect, this, SINGLE_WALLMONITOR_INDEX_START_ID + i);
        }

        //
        // Build the main dialog
        //
        // get the location of the GUI
        TA_Base_Core::ILocation::ELocationType locationType =
            ((VideoDisplayManagerApp*)(AfxGetApp()))->getLocationType();
        AfxGetApp()->DoWaitCursor(1);
        // load and initialise display items
        DisplayItemManager::getInstance().initialiseDisplayItems(this);
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
		// Disalbe all ptz buttons
		enablePTZButton(FALSE);
        SetTimer(HALF_MINUTE_EVENT, 30000, NULL);

		// Text of monitor index is red
		/*
		m_monitorIndex_01.SetTextColor(MONITOR_INDEX_TEXT_COLOR);
		m_monitorIndex_02.SetTextColor(MONITOR_INDEX_TEXT_COLOR);
		m_monitorIndex_03.SetTextColor(MONITOR_INDEX_TEXT_COLOR);
		m_monitorIndex_04.SetTextColor(MONITOR_INDEX_TEXT_COLOR);
		m_monitorIndex_05.SetTextColor(MONITOR_INDEX_TEXT_COLOR);
		m_monitorIndex_06.SetTextColor(MONITOR_INDEX_TEXT_COLOR);
		*/
        FUNCTION_EXIT;
        return TRUE;  // return TRUE  unless you set the focus to a control
    }


    LRESULT VideoDisplayManagerDlg::OnInitCameras(WPARAM wParam, LPARAM lParam)
    {
        return 0;
    }

	void VideoDisplayManagerDlg::LoadButtonIcons()
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

    LRESULT VideoDisplayManagerDlg::OnInitStations(WPARAM wParam, LPARAM lParam)
    {
        std::vector < TA_Base_Core::ILocation*> stations = DisplayItemManager::getInstance().getStations();
        if(stations.empty())
		{
			return 0;
		}
        m_stationListForCamera.AddString("Ñ¡Ôñ³µÕ¾");
        //m_stationListForSEQ.AddString("Ñ¡Ôñ³µÕ¾");
		BOOST_FOREACH(TA_Base_Core::ILocation* station, stations)
        {
            int index = 0;
            if(TA_Base_Core::ILocation::OCC == station->getLocationType())
            {
                m_stationListForCamera.SetItemData(0, station->getKey());
                //m_stationListForSEQ.SetItemData(0, station->getKey());
                // Ensure the occ is first
                index = m_stationListForCamera.InsertString(1, station->getDisplayName().c_str());
                m_stationListForCamera.SetItemData(index, station->getKey());
                index = m_stationListForSEQ.InsertString(0, station->getDisplayName().c_str());
                m_stationListForSEQ.SetItemData(index, station->getKey());
            }
			else if(station->getKey() == 0)
			{
				continue;
			}
            else
            {
                index = m_stationListForCamera.AddString(station->getDisplayName().c_str());
                m_stationListForCamera.SetItemData(index, station->getKey());
                index = m_stationListForSEQ.AddString(station->getDisplayName().c_str());
                m_stationListForSEQ.SetItemData(index, station->getKey());
            }
        }
        m_stationListForSEQ.SetCurSel(0);
        m_stationListForCamera.SetCurSel(0);
        onSelectionChangeStationDiagramList();
        return 0;
    }

    LRESULT VideoDisplayManagerDlg::OnInitSequences(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnInitSequences");
        GetDlgItem(IDC_GF_OCC_SEQ_STATION_COMB)->EnableWindow(TRUE);
        GetDlgItem(IDC_GF_OCC_SEL_STATION_COMB)->EnableWindow(TRUE);
        FUNCTION_EXIT;
        return 0;
    }


    LRESULT VideoDisplayManagerDlg::OnInitStages(WPARAM wParam, LPARAM lParam)
    {
        return 0;
    }


    LRESULT VideoDisplayManagerDlg::OnInitMonitors(WPARAM wParam, LPARAM lParam)
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


    LRESULT VideoDisplayManagerDlg::OnInitDisplayManager(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnInitDisplayManager");
        AfxGetApp()->DoWaitCursor(0);
		LoadButtonIcons();
        m_presenter.onCompleteInitStation();
        m_presenter.setUpdateSequenceTreeCtrl(&m_sequenceTreeCtrl);
        m_presenter.start();
        // DisplayMonitor
        if(RunParams::getInstance().isSet(RPARAM_DISPLAYMONITOR))
        {
            onRunParamChange(RPARAM_DISPLAYMONITOR, RunParams::getInstance().get(RPARAM_DISPLAYMONITOR));
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
        AfxGetApp()->DoWaitCursor(-1);
        FUNCTION_EXIT;
        return 0;
    }

    LRESULT VideoDisplayManagerDlg::onHandleRunParam(WPARAM wParam, LPARAM lParam)
    {
		std::auto_ptr<RunParamStrut> runparam((RunParamStrut *) wParam);
		if(runparam.get() != NULL)
        {
            std::string name = runparam->name;
            std::string value = runparam->value;
            m_presenter.onRunParamChange(name , value);
        }
        return 0;
    }

    LRESULT VideoDisplayManagerDlg::onRefreshSchematic(WPARAM wParam, LPARAM lParam)
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

    void VideoDisplayManagerDlg::OnDestroy()
    {
        FUNCTION_ENTRY("OnDestroy");
        m_presenter.terminateAndWait();
        DisplayItemManager::getInstance().deregisterForMonitorChanges(this);
        CDialog::OnDestroy();
        FUNCTION_EXIT;
    }

    void VideoDisplayManagerDlg::AddMonitorToConsole(int consoleMonitorId, int labelConsoleMonitorId, int textConsoleMonitorId, int offset)
    {
    }

    void VideoDisplayManagerDlg::AddSequenceControlToConsole(int consolePreviousElementButton, int consolePlayCheck, int consoleNextElementButton, int consolePauseCheck, int consolePlaybackLabel, int offset)
    {
    }

    void VideoDisplayManagerDlg::setupMonitors(TA_Base_Core::ILocation::ELocationType locationType)
    {
        m_presenter.setUpMonitor();
    }


    Monitor* VideoDisplayManagerDlg::getMonitor(int controlId)
    {
        FUNCTION_ENTRY("getMonitor");
        // find and return the item in the map
        std::map<int, std::string>::iterator it = m_resourceIdToMonitorNameMap.find(controlId);
        FUNCTION_EXIT;
		return it != m_resourceIdToMonitorNameMap.end() ? DisplayItemManager::getInstance().getMonitor(it->second) : NULL;
    }


    int VideoDisplayManagerDlg::getControlId(std::string monitorName)
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
    void VideoDisplayManagerDlg::OnPaint()
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
        FUNCTION_EXIT;
        return IGFVideoSwitchDisplayView::DestroyWindow();
    }


    // Don't allow an escape key command to be processed.
    BOOL VideoDisplayManagerDlg::PreTranslateMessage(MSG* pMsg)
    {
        FUNCTION_ENTRY("PreTranslateMessage");
        switch(pMsg->message)
        {
            case WM_KEYDOWN:
                switch(pMsg->wParam)
                {
                    case VK_ESCAPE:
                    case VK_LEFT:
                    case VK_RIGHT:
                        /*
                    case VK_UP:
                    */
                    case VK_TAB:
                        FUNCTION_EXIT;
                        return TRUE;
                    default:
                        break;
                }
            case WM_SYSCOMMAND:
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
        FUNCTION_EXIT;
        return IGFVideoSwitchDisplayView::PreTranslateMessage(pMsg);
    }


    void VideoDisplayManagerDlg::OnLButtonDown(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY("OnLButtonDown");
        IGFVideoSwitchDisplayView::OnLButtonDown(nFlags, point);
        m_presenter.MainUILButtonDown(point);
        FUNCTION_EXIT;
    }

    void VideoDisplayManagerDlg:: beginDragImage()
    {
        SetCapture();
        m_mouseTracking = TRUE;
        CPoint cursorLocation;
        ::GetCursorPos(&cursorLocation);
        m_dragImage->BeginDrag(0, m_dragAdjustPoint);
        m_dragImage->DragEnter(NULL, cursorLocation);
    }


    void VideoDisplayManagerDlg::OnLButtonUp(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY("OnLButtonUp");
        IGFVideoSwitchDisplayView::OnLButtonUp(nFlags, point);
        m_presenter.MainUILButtonUp(point);
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerDlg::OnMouseMove(UINT nFlags, CPoint point)
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
    }


    void VideoDisplayManagerDlg::OnContextMenu(CWnd* pWnd, CPoint point)
    {
        FUNCTION_ENTRY("OnContextMenu");
        ScreenToClient(&point);
        m_presenter.MainUIContextMenu(point);
        CDialog::OnContextMenu(pWnd, point);
        // if context menu currently displayed
        FUNCTION_EXIT;
    }

    void VideoDisplayManagerDlg::loadSequenceController(Sequence* sequence)
    {
        /*
        FUNCTION_ENTRY ( "loadSequenceController" );
        TA_ASSERT ( sequence != NULL, "Cannot launch sequence editor for null sequence - menu item should not be accessible" );
        SequenceEditorDlg sequenceEditorDlg ( this, &m_sequencesPage->m_sequenceTreeCtrl );
        m_pSequenceEditorDlg = &sequenceEditorDlg;
        sequenceEditorDlg.DoModal ( sequence, GetSelectedStation() );
        m_pSequenceEditorDlg = NULL;
        FUNCTION_EXIT;
        */
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
        IGFVideoSwitchDisplayView::OnRButtonDown(nFlags, point);
        FUNCTION_EXIT;
    }

    VideoInput* VideoDisplayManagerDlg::getSelectedVideoSource()
    {
        FUNCTION_ENTRY("getSelectedVideoSource");
        // Hopefully when the back end for train operations is
        // properly implemented - this can all be encapsulated in
        // VideoSourcesPropertySheet
        VideoInput* source = NULL;
        source = m_sequenceTreeCtrl.getSelectedInput();
        FUNCTION_EXIT;
        return source;
    }


    void VideoDisplayManagerDlg::endDragging()
    {
        FUNCTION_ENTRY("endDragging");
        m_sequenceTreeCtrl.endDragging();
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


    void VideoDisplayManagerDlg::OnClose()
    {
        FUNCTION_ENTRY("OnClose");
        KillTimer(HALF_MINUTE_EVENT);
        //  m_presenter.saveMonitorRelationship();
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



    LRESULT VideoDisplayManagerDlg::onMonitorUpdated(WPARAM wParam, LPARAM lParam)
    {
        /*
        DisplayItem* item = reinterpret_cast<DisplayItem*>(lParam);
        Monitor* monitor = dynamic_cast<Monitor*>(item);
        if(NULL == Monitor)
        {
            return 0;
        }
        */
        return 0;
    }

    void VideoDisplayManagerDlg::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY("onRunParamChange");
        RunParamStrut * runparam = new RunParamStrut();
        runparam->name = name;
        runparam->value = value;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "get runtime param. name is %s value is %s",
                    name.c_str() , value.c_str());
        PostMessage(TA_RUN_PARAM , (WPARAM) runparam , 0);
        FUNCTION_EXIT;
    }


    void VideoDisplayManagerDlg::onSelectionChangeStationList()
    {
        int sel = m_stationListForSEQ.GetCurSel();
        if(sel >= 0)
        {
            m_presenter.SelectStation();
	        m_sequencesListBox.ResetContent();
        }
    }

    void VideoDisplayManagerDlg::onSelectionChangeStationDiagramList()
    {
		static bool firstRun = true;
        int sel = m_stationListForCamera.GetCurSel();
        if(firstRun && sel > 0)
        {
			// Delete the "Ñ¡Ôñ³µÕ¾" item
			m_stationListForCamera.DeleteString(0);
            firstRun = false;
        }
        m_schematicLevesList.changeLocation(GetStationIdForGwax());
        // First item as the default schematic
        m_presenter.loadStationDiagram(m_schematicLevesList.getFirstItemLevel());
    }

    unsigned long VideoDisplayManagerDlg::GetStationIdForGwax()
    {
        int curItemIdx = m_stationListForCamera.GetCurSel();
        unsigned long locationId  = 0;
        if(CB_ERR != curItemIdx)
        {
            locationId = m_stationListForCamera.GetItemData(curItemIdx);
        }
        else
        {
            locationId = m_stationListForCamera.GetItemData(0);
        }
        return locationId;
    }

    void VideoDisplayManagerDlg::DisplaySequences(std::vector<VideoInput*> & sequences)
    {
        m_sequenceTreeCtrl.DeleteAllItems();
        m_sequenceTreeCtrl.insertVideoSources(sequences);
    }

    void VideoDisplayManagerDlg::DisplaySequence(Sequence* sequence)
    {
        m_sequencesListBox.ResetContent();
        // m_sequencesListBox.addSourceString ( sequence->getDescription ().c_str ()  );
        std::ostringstream desc;
        desc << "ÊÓÆµÐòÁÐ" << sequence->getName();
        m_sequencesListBox.addSourceString(desc.str().c_str());
        std::list<Camera*> cameraList = sequence->getCameras();
		BOOST_FOREACH(Camera* i, cameraList)
        {
            desc.clear();
            desc.str("");
            desc << "ÉãÏñÍ·" << i->getName();
            m_sequencesListBox.addSourceString(desc.str().c_str());
        }
    }

    void VideoDisplayManagerDlg::UpdateSequenceTreeCtrl(Sequence* sequence)
    {
        m_sequenceTreeCtrl.videoInputUpdated(sequence);
    }


    void VideoDisplayManagerDlg::GetSchematicCRET(CRect& markerRect)
    {
        CWnd * marker = GetDlgItem(IDC_GF_OCC_STATION_PIC);
        marker->GetWindowRect(markerRect);
    }
    HWND VideoDisplayManagerDlg::GetWndHandle()
    {
        return GetSafeHwnd();
    }
    unsigned long VideoDisplayManagerDlg::GetSelectedSequenceId()
    {
        return 0;
    }

    unsigned long VideoDisplayManagerDlg:: GetSelectedStation()
    {
        int curItemIdx = m_stationListForSEQ.GetCurSel();
        unsigned long locationId  = 0;
        if(CB_ERR != curItemIdx)
        {
            locationId = m_stationListForSEQ.GetItemData(curItemIdx);
        }
        return locationId;
    }

    void VideoDisplayManagerDlg::onEditSequence()
    {
        VideoInput * input = m_sequenceTreeCtrl.getSelectedInput();
        if(input != NULL)
        {
            unsigned long seqId = input->getKey();
            m_presenter.editSequence(seqId);
        }
    }

    void VideoDisplayManagerDlg ::EnableEditSeqButton(bool enabled)
    {
        m_editSequenceBtn.EnableWindow(enabled);
    }

    MonitorDisplayItem* VideoDisplayManagerDlg::getMonitorDisplayitem(int index , Monitor* monitor)
    {
        HBITMAP standard = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SINGLEVMON));
        HBITMAP highlight = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SINGLEVMON_HILITE));

        MonitorDisplayItem* item = new MonitorDisplayItem();
		unsigned long consoleKey = monitor->getConsoleKey();
        item->setParentWnd(this);
        if(index > 0)
        {
			if(consoleKey > 0)
			{	//OCC Quad
				switch(index)
				{
					//get local monitor
				case 1 :
					item->setIndex(index);
					item->setTextLabel(&m_quad1_Text);
                    item->setIndexLabel(&m_quad1_Index);
					m_quad1_Text.SetTransparent(TRUE);
					m_quad1_Index.SetTransparent(TRUE);
					item->setMonitorwnd(createMonitorWnd(&m_quad1, standard, highlight));
					item->SetMonitor(monitor);
					item->setLocalMonitor(false);
					break;
				case 2:
					item->setIndex(index);
					item->setTextLabel(&m_quad2_Text);
                    item->setIndexLabel(&m_quad2_Index);
					m_quad2_Text.SetTransparent(TRUE);
					m_quad2_Index.SetTransparent(TRUE);
					item->setMonitorwnd(createMonitorWnd(&m_quad2, standard, highlight));
					item->SetMonitor(monitor);
					item->setLocalMonitor(false);
					break;
				case 3 :
					item->setIndex(index);
					item->setTextLabel(&m_quad3_Text);
                    item->setIndexLabel(&m_quad3_Index);
					m_quad3_Text.SetTransparent(TRUE);
					m_quad3_Index.SetTransparent(TRUE);
					item->setMonitorwnd(createMonitorWnd(&m_quad3, standard, highlight));
					item->SetMonitor(monitor);
					item->setLocalMonitor(false);
					break;
				case 4:
					item->setIndex(index);
					item->setTextLabel(&m_quad4_Text);
                    item->setIndexLabel(&m_quad4_Index);
					m_quad4_Text.SetTransparent(TRUE);
					m_quad4_Index.SetTransparent(TRUE);
					item->setMonitorwnd(createMonitorWnd(&m_quad4, standard, highlight));
					item->SetMonitor(monitor);
					item->setLocalMonitor(false);
					break;
				default:
					delete item;
					item = NULL;
					break;
				}
			}
			else
			{	//OPS
				char indexStr[3];
				itoa(index, indexStr, 10);
				item->setIndex(index);
				item->setTextLabel(&m_wallMonitorsText[index - 1]);
                item->setIndexLabel(&m_wallMonitorsIndex[index - 1]);
				m_wallMonitorsText[index-1].SetTransparent(TRUE);
				m_wallMonitorsIndex[index-1].SetTransparent(TRUE);
				m_wallMonitorsIndex[index-1].SetText(indexStr);
				item->setMonitorwnd(createMonitorWnd(&m_wallMonitors[index-1], HBITMAP(m_wallMonitorBitmap), HBITMAP(m_wallMonitorHighLightBitmap)));
				item->SetMonitor(monitor);
				item->setLocalMonitor(false);
			}
        }
        else if(index == 0) // Local Monitor
        {
            item->setIndex(index);
            item->setTextLabel(&m_localMonitorText);
            m_localMonitorText.SetTransparent(TRUE);
			item->setMonitorwnd(createMonitorWnd(&m_localMonitor, standard, highlight));
            item->SetMonitor(monitor);
            item->setLocalMonitor(true);
        }
        return item;
    }

    void VideoDisplayManagerDlg::paintStandControl(MonitorWnd * item)
    {
        item->m_wnd->SetBitmap(item->m_standardBitmap);
        item ->m_wnd->Invalidate();
        IGFVideoSwitchDisplayView::OnPaint();
    }

    /*
    void VideoDisplayManagerDlg::refreshIndexLable(int index, bool isQuad)
    {
        if(index > 0)
        {
			if (isQuad)
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
			else
			{
				m_wallMonitorsIndex[index-1].Invalidate();
			}
        }
    }
    */


    void VideoDisplayManagerDlg::paintHightedControl(MonitorWnd * item)
    {
        item->m_wnd->SetBitmap(item->m_highlightBitmap);
        item ->m_wnd->Invalidate();
        IGFVideoSwitchDisplayView::OnPaint();
    }

    void VideoDisplayManagerDlg::assignMonitor(MonitorDisplayItem * item)
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


    void VideoDisplayManagerDlg::endDrage()
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

    void VideoDisplayManagerDlg:: handleDragImageOnMouseMove(BOOL begin , CPoint& point)
    {
        if(! begin)
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

    void VideoDisplayManagerDlg::loadSequenceSubMenu(CPoint& point , Monitor * monitor, Sequence* seq, bool bIsLocalMonitor)
    {
        CMenu contextMenu;
        contextMenu.LoadMenu(IDR_MONSEQCONTEXT_MENU);
        m_contextActive = TRUE;
        CMenu* pContextMenu = contextMenu.GetSubMenu(0);
        ClientToScreen(&point);
        int nCmd = (int) pContextMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
                                                      point.x, point.y, AfxGetMainWnd());
        // Process the appropriate context menu command.
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
                m_presenter.editSequence ( seq->getKey () );
                break;
            case ID_VIDEO_PLAY:
                m_presenter.playVideo();
                contextMenu.EnableMenuItem ( ID_VIDEO_PLAY, MF_GRAYED );
                contextMenu.EnableMenuItem ( ID_VIDEO_STOP, MF_ENABLED );
                break;
            case ID_VIDEO_STOP:
                m_presenter.stopVideo();
                contextMenu.EnableMenuItem ( ID_VIDEO_PLAY, MF_ENABLED );
                contextMenu.EnableMenuItem ( ID_VIDEO_STOP, MF_GRAYED );
                break;
            */
            default:
                break;
        }
    }
    void VideoDisplayManagerDlg::loadCameraSubMenu(CPoint& point , Monitor* monitor, bool bIsLocalMonitor)
    {
        CMenu contextMenu;
        contextMenu.LoadMenu(IDR_MONCAMCONTEXT_MENU);
        CMenu* pContextMenu = contextMenu.GetSubMenu(0);
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
                break;
            }
            /*
            case ID_NEW_EDIT_SEQUENCE:
                loadSequenceController(seq);
                break;
            case ID_VIDEO_PLAY:
                m_presenter.playVideo();
                contextMenu.EnableMenuItem ( ID_VIDEO_PLAY, MF_GRAYED );
                contextMenu.EnableMenuItem ( ID_VIDEO_STOP, MF_ENABLED );
                break;
            case ID_VIDEO_STOP:
                m_presenter.stopVideo();
                contextMenu.EnableMenuItem ( ID_VIDEO_PLAY, MF_ENABLED );
                contextMenu.EnableMenuItem ( ID_VIDEO_STOP, MF_GRAYED );
                break;
            */
            default:
                break;
        }
    }

    void VideoDisplayManagerDlg::displayErrorMessage(CString& message)
    {
        AfxMessageBox(message , MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION);
    }

    void VideoDisplayManagerDlg::displayStaionCameraDiagram(std::string& fullFileName)
    {
    }

	void VideoDisplayManagerDlg::enablePTZButton(BOOL enabled)
	{
		BOOST_FOREACH(CPTZButton* i, m_ptzButtons)
		{
			i->EnableWindow(enabled);
		}
	}

    void VideoDisplayManagerDlg::enableStationSelection(BOOL enable)
    {
        m_stationListForSEQ.EnableWindow(enable);
        m_stationListForCamera.EnableWindow(enable);
    }

    void VideoDisplayManagerDlg::setSelectedStation(unsigned int index)
    {
        m_stationListForSEQ.SetCurSel(index);
        m_stationListForCamera.SetCurSel(index);
    }

    void VideoDisplayManagerDlg::OnTimer(UINT nEventID)
    {
        m_presenter.OnTimer(nEventID);
    }

    void VideoDisplayManagerDlg::OnLevelSelChange()
    {
        m_presenter.loadStationDiagram(m_schematicLevesList.getCurrentSelLevel());
    }

    void VideoDisplayManagerDlg::OnLButtonDblClk( UINT nFlags, CPoint point )
    {
        MonitorDisplayItem* monitor = m_presenter.getOverMonitor(point);
        if(NULL == monitor)
        {
            return;
        }

        // Return if not wall monitor
        if(monitor->isLocalMonitor() || monitor->isQuad())
        {
            return;
        }

        int ctrlId = monitor->getMonitorWnd()->m_wnd->GetDlgCtrlID();
        if(ctrlId >= SINGLE_WALLMONITOR_START_ID && ctrlId < SINGLE_WALLMONITOR_START_ID + NUMBER_SINGLE_WALL_MONITORS)
        {
            // Current is single monitor mode, restore to split screen
            m_presenter.setMonitorPicCount(monitor->getMonitor(), 4);

            if(monitor->getMonitor()->isSingleWallMonitor())
            {
                // Failed set to split screen
                return;
            }

            changeWallMontior(monitor, true);
        }
        else
        {
            // Should be in split screen, change to single monitor mode
            m_presenter.setMonitorPicCount(monitor->getMonitor(), 1);

            if(!monitor->getMonitor()->isSingleWallMonitor())
            {
                // Failed set to single screen
                return;
            }

            changeWallMontior(monitor, false);
        }

    }

    void VideoDisplayManagerDlg::changeWallMontior( MonitorDisplayItem * monitor, bool split )
    {
        int ctrlId = monitor->getMonitorWnd()->m_wnd->GetDlgCtrlID();
        if(ctrlId >= SINGLE_WALLMONITOR_START_ID && ctrlId < SINGLE_WALLMONITOR_START_ID + NUMBER_SINGLE_WALL_MONITORS)
        {
            // Current is single monitor mode, restore to split screen
            if(!split)
            {
                return;
            }

            const int singleWallMonitorIndex = ctrlId - SINGLE_WALLMONITOR_START_ID;
            m_singleWallMonitors[singleWallMonitorIndex].ShowWindow(SW_HIDE);
            m_singleWallMonitorsText[singleWallMonitorIndex].ShowWindow(SW_HIDE);
            m_singleWallMonitorsIndex[singleWallMonitorIndex].ShowWindow(SW_HIDE);

            const int splitMonitorIndex = monitor->getIndex() - 1;
			const int fstMonitorIndex = (splitMonitorIndex / 4)*4;
			for(int i = fstMonitorIndex; i < fstMonitorIndex + 4; ++i)
            {
                m_wallMonitors[i].ShowWindow(SW_SHOW);
                m_wallMonitorsText[i].ShowWindow(SW_SHOW);
                m_wallMonitorsIndex[i].ShowWindow(SW_SHOW);
            }

            CString windowText;
            m_singleWallMonitorsText[singleWallMonitorIndex].GetWindowText(windowText);
            m_wallMonitorsText[splitMonitorIndex].SetText(windowText);
            //m_wallMonitors[splitMonitorIndex].SetBitmap(m_wallMonitorHighLightBitmap);

            monitor->setMonitorwnd(createMonitorWnd(&m_wallMonitors[splitMonitorIndex], HBITMAP(m_wallMonitorBitmap), HBITMAP(m_wallMonitorHighLightBitmap)));
            monitor->setTextLabel(&m_wallMonitorsText[splitMonitorIndex]);
            monitor->setIndexLabel(&m_wallMonitorsIndex[splitMonitorIndex]);
        }
        else
        {
            // Should be in split screen, change to single monitor mode
            if(split)
            {
                return;
            }

            const int splitMonitorIndex = monitor->getIndex() - 1;
			const int fstMonitorIndex = (splitMonitorIndex / 4)*4;
            for(int i = fstMonitorIndex; i < fstMonitorIndex + 4; ++i)
            {
                m_wallMonitors[i].ShowWindow(SW_HIDE);
                m_wallMonitorsText[i].ShowWindow(SW_HIDE);
                m_wallMonitorsIndex[i].ShowWindow(SW_HIDE);
            }

            const int singleWallMonitorIndex = splitMonitorIndex / 4;
            m_singleWallMonitors[singleWallMonitorIndex].ShowWindow(SW_SHOW);
            m_singleWallMonitorsText[singleWallMonitorIndex].ShowWindow(SW_SHOW);
            m_singleWallMonitorsIndex[singleWallMonitorIndex].ShowWindow(SW_SHOW);

            CString windowText, label;
            m_wallMonitorsText[splitMonitorIndex].GetWindowText(windowText);
            m_wallMonitorsIndex[splitMonitorIndex].GetWindowText(label);

            m_singleWallMonitorsText[singleWallMonitorIndex].SetTransparent(TRUE);
            m_singleWallMonitorsText[singleWallMonitorIndex].SetText(windowText);

            m_singleWallMonitorsIndex[singleWallMonitorIndex].SetTransparent(TRUE);
            m_singleWallMonitorsIndex[singleWallMonitorIndex].SetText(label);
            //m_singleWallMonitors[singleWallMonitorIndex].SetBitmap(m_singleWallMonitorHighLightBitmap);

            monitor->setMonitorwnd(createMonitorWnd(&m_singleWallMonitors[singleWallMonitorIndex], HBITMAP(m_singleWallMonitorBitmap), HBITMAP(m_singleWallMonitorHighLightBitmap)));
            monitor->setTextLabel(&m_singleWallMonitorsText[singleWallMonitorIndex]);
            monitor->setIndexLabel(&m_singleWallMonitorsIndex[singleWallMonitorIndex]);
        }
    }

    MonitorWnd* VideoDisplayManagerDlg::createMonitorWnd( CStatic* wnd, HBITMAP standard, HBITMAP highlight )
    {
        if(m_wndToMonitorWndMap.find(wnd) == m_wndToMonitorWndMap.end())
        {
            m_wndToMonitorWndMap[wnd] = new MonitorWnd(wnd, standard, highlight);
        }
        return m_wndToMonitorWndMap[wnd];
    }

} // TA_IRS_App
