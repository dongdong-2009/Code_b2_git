/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/cctv/video_display_manager/src/VideoDisplayManagerDlg.cpp $
  * @author:  Robert Young
  * @version: $Revision: #9 $
  *
  * Last modification: $DateTime: 2011/03/29 11:31:40 $
  * Last modified by:  $Author: builder $
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
#include "app/cctv/video_display_manager/src/TrainborneControllerDlg.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManager.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerDlg.h"
#include "app/cctv/video_display_manager/src/LocationSelectionDialog.h"

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
// like++ CL-19410
#include "bus\security\authentication_library\src\AuthenticationLibrary.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
// ++like

#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SM_CXVIRTUALSCREEN      78		//zhongjie++
#define MINVALUE				2400	//zhongjie++
#define NUMSCREENS				3		//zhongjie++

using TA_Base_Bus::TransActiveDialog;
using TA_Base_Core::RunParams;

namespace TA_IRS_App
{
		const std::string CONSSOLEMONITOR3STR = std::string("Consolemonitor3Inservice");
        const char* NODEVICE = "No item is currently selected";
        const unsigned int TRAIN_COMMAND_OFFSET = 1;
        /////////////////////////////////////////////////////////////////////////////
        // VideoDisplayManagerDlg dialog

        VideoDisplayManagerDlg::VideoDisplayManagerDlg(TA_Base_Bus::IGUIAccess& controlClass)
            : TransActiveDialog(controlClass, VideoDisplayManagerDlg::IDD, NULL),
              m_camerasPage(NULL),
              m_quadsPage(NULL),
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
              m_dragAdjustPoint(0,0),
              m_pQuadControllerDlg(0),
              m_paramCameraAddress(""),
              m_paramMonitorName(""),
              m_pSequenceEditorDlg(0),
              m_isLaunched(false),
              m_isAssignedBySchematic(false),
              m_overriddenLocation( 0 )
        {
            FUNCTION_ENTRY( "VideoDisplayManagerDlg" );

			//for m_thirdMonitorInservice get from runparameter;			
			if ( TA_Base_Core::RunParams::getInstance().isSet(CONSSOLEMONITOR3STR.c_str()) )
			{
				std::string statuVal = TA_Base_Core::RunParams::getInstance().get(CONSSOLEMONITOR3STR.c_str());
				if(   (statuVal.find("yes") != std::string::npos)||(statuVal.find("YES") != std::string::npos)
					||(statuVal.find("Yes") != std::string::npos) )
				{
					m_thirdMonitorInservice = true;
				}
				else
				{
					m_thirdMonitorInservice = false;
				}
			}
			else
			{
				m_thirdMonitorInservice = true;
			}

            //{{AFX_DATA_INIT(CGenericGUIDialogTestDlg)
                // NOTE: the ClassWizard will add member initialization here
            //}}AFX_DATA_INIT
            // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
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
                    TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey( console->getLocation() );

                locationType = location->getLocationType();
            }
            catch (DataException& ex)
            {

                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << ex.what();
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_010029);
                //CString str("Caught DataException trying to determine site from console session: ");
                //str += ex.what();
                //AfxMessageBox(str);
            }
            catch (DatabaseException& ex)
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
            m_videoSourcesPropertySheet = new VideoSourcesPropertySheet(IDS_PROPSHT_CAPTION, this, 0);
            ASSERT (m_videoSourcesPropertySheet);
            // construct pages
            m_camerasPage    = new CamerasPage();
            ASSERT (m_camerasPage);
            m_quadsPage    = new QuadsPage();
            ASSERT (m_quadsPage);
            m_sequencesPage    = new SequencesPage();
            ASSERT (m_sequencesPage);

            if (locationType != TA_Base_Core::ILocation::DPT)
            {
                m_trainsPage    = new TrainsPage;
                ASSERT (m_trainsPage);
            }

            // add in all the CPropertyPage's
            m_videoSourcesPropertySheet->AddPage(m_camerasPage);
            m_videoSourcesPropertySheet->AddPage(m_quadsPage);
            m_videoSourcesPropertySheet->AddPage(m_sequencesPage);

            if (m_trainsPage)
            {
                m_videoSourcesPropertySheet->AddPage(m_trainsPage);
            }

            FUNCTION_EXIT;
        }


        VideoDisplayManagerDlg::~VideoDisplayManagerDlg()
        {
            FUNCTION_ENTRY( "~VideoDisplayManagerDlg" );

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::setDragImage(CImageList* dragImage)
        {
            FUNCTION_ENTRY( "setDragImage" );

            if ( m_dragImage != NULL )
            {
                delete m_dragImage;
                m_dragImage = NULL;
            }
            m_dragImage = dragImage;

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::DoDataExchange(CDataExchange* pDX)
        {
            FUNCTION_ENTRY( "DoDataExchange" );

            TransActiveDialog::DoDataExchange(pDX);
            //{{AFX_DATA_MAP(VideoDisplayManagerDlg)
            DDX_Control(pDX, IDC_CONSOLE_PREVIOUSELEMENT_BUTTON, m_consolePreviousElement);
            DDX_Control(pDX, IDC_CONSOLE_PLAY_CHECK, m_consolePlay);
            DDX_Control(pDX, IDC_CONSOLE_PAUSE_CHECK, m_consolePause);
            DDX_Control(pDX, IDC_CONSOLE_NEXTELEMENT_BUTTON, m_consoleNextElement);
            DDX_Control(pDX, IDC_COMMON_PREVIOUSELEMENT_BUTTON, m_commonPreviousElement);
            DDX_Control(pDX, IDC_COMMON_PLAY_CHECK, m_commonPlay);
            DDX_Control(pDX, IDC_COMMON_PAUSE_CHECK, m_commonPause);
            DDX_Control(pDX, IDC_COMMON_NEXTELEMENT_BUTTON, m_commonNextElement);
            DDX_Control(pDX, IDC_DETAILS_EDIT, m_detailsBox);
            //}}AFX_DATA_MAP

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
            ON_BN_CLICKED(IDC_COMMON_NEXTELEMENT_BUTTON, OnCommonNextelementButton)
            ON_BN_CLICKED(IDC_COMMON_PAUSE_CHECK, OnCommonPauseCheck)
            ON_BN_CLICKED(IDC_COMMON_PLAY_CHECK, OnCommonPlayCheck)
            ON_BN_CLICKED(IDC_COMMON_PREVIOUSELEMENT_BUTTON, OnCommonPreviouselementButton)
            ON_BN_CLICKED(IDC_CONSOLE_NEXTELEMENT_BUTTON, OnConsoleNextelementButton)
            ON_BN_CLICKED(IDC_CONSOLE_PAUSE_CHECK, OnConsolePauseCheck)
            ON_BN_CLICKED(IDC_CONSOLE_PLAY_CHECK, OnConsolePlayCheck)
            ON_BN_CLICKED(IDC_CONSOLE_PREVIOUSELEMENT_BUTTON, OnConsolePreviouselementButton)
            ON_MESSAGE(WM_MONITOR_UPDATED, onMonitorUpdated)
            ON_REGISTERED_MESSAGE(TA_Base_Bus::TA_GenericGui::WM_SET_WINDOW_POSITION, OnSetWindowPosition)
            ON_MESSAGE(TA_INIT_CAMERAS, OnInitCameras)
            ON_MESSAGE(TA_INIT_QUADS, OnInitQuads)
            ON_MESSAGE(TA_INIT_SEQUENCES, OnInitSequences)
            ON_MESSAGE(TA_INIT_STAGES, OnInitStages)
            ON_MESSAGE(TA_INIT_MONITORS, OnInitMonitors)
            ON_MESSAGE(TA_INIT_CCTV, OnInitDisplayManager)
            //}}AFX_MSG_MAP
        END_MESSAGE_MAP()

        /////////////////////////////////////////////////////////////////////////////
        // VideoDisplayManagerDlg message handlers

        BOOL VideoDisplayManagerDlg::OnInitDialog()
        {
            FUNCTION_ENTRY( "OnInitDialog" );

            CRect windowSize;
            GetWindowRect(&windowSize);
            TA_Base_Bus::ResizingProperties properties;
            properties.canMaximise = false;
            properties.minHeight = windowSize.bottom - windowSize.top;
            properties.minWidth = windowSize.right - windowSize.left;
            properties.maxHeight = properties.minHeight;
            properties.maxWidth = properties.minWidth;
            setResizingProperties(properties);

            TransActiveDialog::OnInitDialog();

            // Set the icon for this dialog.  The framework does this automatically
            //  when the application's main window is not a dialog
            SetIcon(m_hIcon, TRUE);        // Set big icon

            //
            // Build the main dialog
            //

            // get the location of the GUI
            TA_Base_Core::ILocation::ELocationType locationType =
                ( (VideoDisplayManagerApp*)(AfxGetApp()) )->getLocationType();


            // set up the GUI layout
            configureGUILayout(locationType);


            //
            // Disable the GUI items until they have been loaded
            //
            m_camerasPage->m_cameraTreeCtrl.EnableWindow( FALSE );
            m_videoSourcesPropertySheet->EnableItem( 1, FALSE );
            
            m_quadsPage->m_quadTreeCtrl.EnableWindow( FALSE );
            m_videoSourcesPropertySheet->EnableItem( 2, FALSE );
            
            m_sequencesPage->m_sequenceTreeCtrl.EnableWindow( FALSE );
            m_videoSourcesPropertySheet->EnableItem( 3, FALSE );

            if (m_trainsPage)
            {
                m_trainsPage->m_inboundTrainsListCtrl.EnableWindow( FALSE );
                m_trainsPage->m_outboundTrainsListCtrl.EnableWindow( FALSE );
                
                m_videoSourcesPropertySheet->EnableItem( 4, FALSE );
            }

            AfxGetApp()->DoWaitCursor(1);

            // load and initialise display items
            DisplayItemManager::getInstance().initialiseDisplayItems( this );

			//resize the window, refer to PAManager
			// ModifyWindowPos();

            FUNCTION_EXIT;
            return TRUE;  // return TRUE  unless you set the focus to a control
        }


        LRESULT VideoDisplayManagerDlg::OnInitCameras( WPARAM wParam, LPARAM lParam)
        {
            FUNCTION_ENTRY( "OnInitCameras" );

            AfxGetApp()->DoWaitCursor(0);

            m_camerasPage->Init();
            m_camerasPage->m_cameraTreeCtrl.setDetailsPointer(&m_detailsBox);

            m_camerasPage->m_cameraTreeCtrl.EnableWindow( TRUE );

            m_videoSourcesPropertySheet->EnableItem( 1, TRUE );

            FUNCTION_EXIT;
            return 0;
        }


        LRESULT VideoDisplayManagerDlg::OnInitQuads( WPARAM wParam, LPARAM lParam)
        {
            FUNCTION_ENTRY( "OnInitQuads" );

            AfxGetApp()->DoWaitCursor(0);

            m_quadsPage->Init();
            m_quadsPage->m_quadTreeCtrl.setDetailsPointer(&m_detailsBox);

            m_quadsPage->m_quadTreeCtrl.EnableWindow( TRUE );

            m_videoSourcesPropertySheet->EnableItem( 2, TRUE );

            FUNCTION_EXIT;
            return 0;
        }


        LRESULT VideoDisplayManagerDlg::OnInitSequences( WPARAM wParam, LPARAM lParam)
        {
            FUNCTION_ENTRY( "OnInitSequences" );

            AfxGetApp()->DoWaitCursor(0);
            
            m_sequencesPage->Init();
            m_sequencesPage->m_sequenceTreeCtrl.setDetailsPointer(&m_detailsBox);

            m_sequencesPage->m_sequenceTreeCtrl.EnableWindow( TRUE );

            m_videoSourcesPropertySheet->EnableItem( 3, TRUE );

            FUNCTION_EXIT;
            return 0;
        }


        LRESULT VideoDisplayManagerDlg::OnInitStages( WPARAM wParam, LPARAM lParam)
        {
            FUNCTION_ENTRY( "OnInitStages" );

            AfxGetApp()->DoWaitCursor(0);

            if (m_trainsPage)
            {
				TA_IRS_Bus::TrainAgentProxyFactory::getInstance().guiInitialise(TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID));
				createCctvProxy();

                m_trainsPage->Init();

                m_trainsPage->m_inboundTrainsListCtrl.setDetailsPointer(&m_detailsBox);
                m_trainsPage->m_outboundTrainsListCtrl.setDetailsPointer(&m_detailsBox);

                m_trainsPage->m_inboundTrainsListCtrl.EnableWindow( TRUE );
                m_trainsPage->m_outboundTrainsListCtrl.EnableWindow( TRUE );

                m_videoSourcesPropertySheet->EnableItem( 4, TRUE );
            }

            FUNCTION_EXIT;
            return 0;
        }


        LRESULT VideoDisplayManagerDlg::OnInitMonitors( WPARAM wParam, LPARAM lParam)
        {
            FUNCTION_ENTRY( "OnInitMonitors" );

            AfxGetApp()->DoWaitCursor(0);

            TA_Base_Core::ILocation::ELocationType locationType =
                ( (VideoDisplayManagerApp*)(AfxGetApp()) )->getLocationType();

            setupMonitors(locationType);

            DisplayItemManager::getInstance().registerForMonitorChanges(this);

            FUNCTION_EXIT;
            return 0;
        }


        LRESULT VideoDisplayManagerDlg::OnInitDisplayManager( WPARAM wParam, LPARAM lParam)
        {
            FUNCTION_ENTRY( "OnInitDisplayManager" );

            AfxGetApp()->DoWaitCursor(0);

            LoadButtonIcons();

            m_detailsBox.SetWindowText(NODEVICE);

            // for each monitor on this page
            for (std::map<std::string, int>::iterator iter = m_monitorNameToResourceIdMap.begin();
            iter != m_monitorNameToResourceIdMap.end(); iter++)
            {
                Monitor* monitor = getMonitor(iter->second);
                if (monitor != NULL)
                {
                    onMonitorUpdated(NULL, reinterpret_cast<LPARAM>(monitor));
                }
            }

            //  Check the commandline parameters for a camera-to-monitor switch request (TD 13455).
            if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_ENTITY) &&
                TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DISPLAYMONITOR))
            {
                //  Display Manager has been started to handle a switch request.
                //  Set the camera-parameter "trigger" and call onRunParamChange() with the other
                //  monitor-parameter "trigger" : when both triggers are set the switch is made.
                m_paramCameraAddress = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITY);
                onRunParamChange( RPARAM_DISPLAYMONITOR, TA_Base_Core::RunParams::getInstance().get(RPARAM_DISPLAYMONITOR));
                ShowWindow( SW_MINIMIZE);
                m_isAssignedBySchematic = false;
            }

            //  Register for camera-to-monitor switch requests (eg. coming from the schematic).
            //  This enters these parameter names into the map of runtime parameters, so
            //  RunParams::isSet(RPARAM_ENTITY) and ::isSet(RPARAM_DISPLAYMONITOR) will now
            //  always return true, even if they were not in the commandline).

            RunParams::getInstance().registerRunParamUser(this, RPARAM_ENTITY);
            RunParams::getInstance().registerRunParamUser(this, RPARAM_DISPLAYMONITOR);

            m_isLaunched = true;

            AfxGetApp()->DoWaitCursor(-1);

            FUNCTION_EXIT;
            return 0;
        }


        void VideoDisplayManagerDlg::OnDestroy()
        {
            FUNCTION_ENTRY( "OnDestroy" );

            DisplayItemManager::getInstance().deregisterForMonitorChanges(this);

            DisplayItemManager::removeInstance();

            CDialog::OnDestroy();

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::configureGUILayout(TA_Base_Core::ILocation::ELocationType locationType)
        {
            FUNCTION_ENTRY( "configureGUILayout" );

            //
            // Set-up dialog for OCR, Station, or Depot
            //


            // If at the OCC or depot - remove one of the console monitors
            if ( locationType != TA_Base_Core::ILocation::STATION )
            {
                // hide console monitor 2 and its labels
                CWnd* dlgItem = NULL;
                dlgItem = GetDlgItem(IDC_CONSOLE_MONITOR_2);
                dlgItem->ShowWindow(SW_HIDE);
                dlgItem = GetDlgItem(IDC_LABEL_CONSOLE_MONITOR_2);
                dlgItem->ShowWindow(SW_HIDE);
                dlgItem = GetDlgItem(IDC_TEXT_CONSOLE_MONITOR_2);
                dlgItem->ShowWindow(SW_HIDE);
				
				//bohong++ 
                dlgItem = GetDlgItem(IDC_CONSOLE_MONITOR_3);
                dlgItem->ShowWindow(SW_HIDE);
                dlgItem = GetDlgItem(IDC_LABEL_CONSOLE_MONITOR_3);
                dlgItem->ShowWindow(SW_HIDE);
                dlgItem = GetDlgItem(IDC_TEXT_CONSOLE_MONITOR_3);
                dlgItem->ShowWindow(SW_HIDE);
            }


            // position the property pages
            CWnd* marker = GetDlgItem(IDC_TABMARKER);
            CRect markerRect;
            marker->GetWindowRect(markerRect);
            marker->DestroyWindow();
            ScreenToClient(markerRect);
            m_videoSourcesPropertySheet->Create(this, WS_VISIBLE | WS_CHILD, WS_EX_CONTROLPARENT);
            m_videoSourcesPropertySheet->SetWindowPos(NULL,markerRect.left-7,markerRect.top-8,markerRect.Width(),markerRect.Height(),SWP_NOZORDER|SWP_NOSIZE);


            // if not at the OCC
            if ( locationType != TA_Base_Core::ILocation::OCC )
            {
                //
                // hide the common monitors
                //

                int adjust( 0 );

                CWnd* dlgItem = NULL;
                dlgItem = GetDlgItem(IDC_COMMON_BOX);
                CRect rect = NULL;
                dlgItem->GetWindowRect(rect);
                ScreenToClient(rect);
                adjust = rect.Height();
                int cuttoff = rect.bottom;
                dlgItem->ShowWindow(SW_HIDE);
                dlgItem = GetDlgItem(IDC_CONSOLE_BOX);
                dlgItem->GetWindowRect(rect);
                ScreenToClient(rect);
                adjust += (rect.top - cuttoff);
                dlgItem = GetWindow(GW_CHILD);
                while (dlgItem != NULL)
                {
                    dlgItem->GetWindowRect(rect);
                    ScreenToClient(rect);
                    if (rect.bottom > cuttoff)
                    {
                        if ( rect.top > cuttoff )
                        {
                            dlgItem->MoveWindow( rect.left, (rect.top - adjust), rect.Width(), rect.Height() );
                        }
                    }
                    else if (dlgItem->GetDlgCtrlID() != IDC_MENU_LINE)
                    {
                        dlgItem->ShowWindow(SW_HIDE);
                    }
                    dlgItem = dlgItem->GetNextWindow();
                }

                // increase the height of the property sheet
                m_videoSourcesPropertySheet->increaseHeight( adjust );

                // increase the height of the console box
                dlgItem = GetDlgItem(IDC_CONSOLE_BOX);
                dlgItem->GetWindowRect(rect);
                ScreenToClient(rect);
                dlgItem->MoveWindow( rect.left, rect.top, rect.Width(), rect.Height() + adjust );


                //
                // now move the console monitors
                // they will be centered vertically
                //
				

                // the vertical center of the console monitor box
                int centerPoint = rect.top + ( (rect.Height() + adjust) / 2);


                // work out the height (and spacing) of each item

                // monitor
                dlgItem = GetDlgItem(IDC_CONSOLE_MONITOR_1);
                dlgItem->GetWindowRect(rect);
                ScreenToClient(rect);
                int monitorHeight = rect.Height();
                int monitorLabelSpacing = rect.bottom;

                // label
                dlgItem = GetDlgItem(IDC_LABEL_CONSOLE_MONITOR_1);
                dlgItem->GetWindowRect(rect);
                ScreenToClient(rect);
                int labelHeight = rect.Height();
                monitorLabelSpacing = rect.top - monitorLabelSpacing;
                int labelSequenceSpacing = rect.bottom;

                // sequence buttons and sequence label
                dlgItem = GetDlgItem(IDC_CONSOLE_PREVIOUSELEMENT_BUTTON);
                dlgItem->GetWindowRect(rect);
                ScreenToClient(rect);
                int sequenceHeight = rect.Height();
                labelSequenceSpacing = rect.top - labelSequenceSpacing;

                dlgItem = GetDlgItem(IDC_CONSOLE_PLAYBACK_LABEL);
                dlgItem->GetWindowRect(rect);
                ScreenToClient(rect);
                sequenceHeight += rect.Height();

                // work out the total height of all items to be displayed

                // one monitor
                int totalHeight = monitorHeight +
                                  monitorLabelSpacing +
                                  labelHeight +
                                  labelSequenceSpacing +
                                  sequenceHeight;

                if ( locationType == TA_Base_Core::ILocation::STATION )
                {
                    // two monitors
                    totalHeight += 2*(monitorHeight +
                                   monitorLabelSpacing +
                                   labelHeight +
                                   labelSequenceSpacing);
                }

                // the top of monitor 1
                int newTopPosition = centerPoint - (totalHeight / 2);

                // Move console monitor 1
                dlgItem = GetDlgItem(IDC_CONSOLE_MONITOR_1);
                dlgItem->GetWindowRect(rect);
                ScreenToClient(rect);
                int offset = newTopPosition - rect.top;
				AddMonitorToConsole(IDC_CONSOLE_MONITOR_1, IDC_LABEL_CONSOLE_MONITOR_1, IDC_TEXT_CONSOLE_MONITOR_1, offset);

                // if there are 2 monitors, move the second below the first
                if ( locationType == TA_Base_Core::ILocation::STATION )
                {
					offset = offset + monitorHeight + monitorLabelSpacing + labelHeight + labelSequenceSpacing;
					AddMonitorToConsole(IDC_CONSOLE_MONITOR_2, IDC_LABEL_CONSOLE_MONITOR_2, IDC_TEXT_CONSOLE_MONITOR_2, offset);
					offset = 2*offset;
					AddMonitorToConsole(IDC_CONSOLE_MONITOR_3, IDC_LABEL_CONSOLE_MONITOR_3, IDC_TEXT_CONSOLE_MONITOR_3, offset);
					
                }

                // move the sequence controls
				AddSequenceControlToConsole(IDC_CONSOLE_PREVIOUSELEMENT_BUTTON, IDC_CONSOLE_PLAY_CHECK, IDC_CONSOLE_NEXTELEMENT_BUTTON, IDC_CONSOLE_PAUSE_CHECK, IDC_CONSOLE_PLAYBACK_LABEL, offset);
            }

            FUNCTION_EXIT;
        }

		
		void VideoDisplayManagerDlg::AddMonitorToConsole(int consoleMonitorId, int labelConsoleMonitorId, int textConsoleMonitorId, int offset)
		{
			//for grey off console monitor 3;

			CWnd *dlgItem = NULL;
			CRect rect;
			dlgItem = GetDlgItem(consoleMonitorId);
			dlgItem->GetWindowRect(rect);
			ScreenToClient(rect);
			dlgItem->MoveWindow( rect.left, rect.top + offset, rect.Width(), rect.Height() );

			if (consoleMonitorId == IDC_CONSOLE_MONITOR_3)
			{
				dlgItem->EnableWindow(m_thirdMonitorInservice);
			}	

			dlgItem = GetDlgItem(labelConsoleMonitorId);
			dlgItem->GetWindowRect(rect);
			ScreenToClient(rect);
			dlgItem->MoveWindow( rect.left, rect.top + offset, rect.Width(), rect.Height() );
			if (consoleMonitorId == IDC_CONSOLE_MONITOR_3)
			{
				dlgItem->EnableWindow(m_thirdMonitorInservice);
			}

			dlgItem = GetDlgItem(textConsoleMonitorId);
			dlgItem->GetWindowRect(rect);
			ScreenToClient(rect);
			dlgItem->MoveWindow( rect.left, rect.top + offset, rect.Width(), rect.Height() );
			if (consoleMonitorId == IDC_CONSOLE_MONITOR_3)
			{
				dlgItem->EnableWindow(m_thirdMonitorInservice);
			}

		}

		void VideoDisplayManagerDlg::AddSequenceControlToConsole(int consolePreviousElementButton, int consolePlayCheck, int consoleNextElementButton, int consolePauseCheck, int consolePlaybackLabel, int offset)
		{  
			CWnd *dlgItem = NULL;
			CRect rect;
			dlgItem = GetDlgItem(IDC_CONSOLE_PREVIOUSELEMENT_BUTTON);
			dlgItem->GetWindowRect(rect);
			ScreenToClient(rect);
			dlgItem->MoveWindow( rect.left, rect.top + offset, rect.Width(), rect.Height() );
			dlgItem = GetDlgItem(IDC_CONSOLE_PLAY_CHECK);
			dlgItem->GetWindowRect(rect);
			ScreenToClient(rect);
			dlgItem->MoveWindow( rect.left, rect.top + offset, rect.Width(), rect.Height() );
			dlgItem = GetDlgItem(IDC_CONSOLE_NEXTELEMENT_BUTTON);
			dlgItem->GetWindowRect(rect);
			ScreenToClient(rect);
			dlgItem->MoveWindow( rect.left, rect.top + offset, rect.Width(), rect.Height() );
			dlgItem = GetDlgItem(IDC_CONSOLE_PAUSE_CHECK);
			dlgItem->GetWindowRect(rect);
			ScreenToClient(rect);
			dlgItem->MoveWindow( rect.left, rect.top + offset, rect.Width(), rect.Height() );
			dlgItem = GetDlgItem(IDC_CONSOLE_PLAYBACK_LABEL);
			dlgItem->GetWindowRect(rect);
			ScreenToClient(rect);
            dlgItem->MoveWindow( rect.left, rect.top + offset, rect.Width(), rect.Height() );
		}

        void VideoDisplayManagerDlg::setupMonitors(TA_Base_Core::ILocation::ELocationType locationType)
        {
            FUNCTION_ENTRY( "setupMonitors" );

            VideoDisplayManagerApp* application = dynamic_cast<VideoDisplayManagerApp*>(AfxGetApp());
            TA_ASSERT(application != NULL, "Application is NULL");

            // get all console monitors
            std::map<std::string, Monitor*> allMonitors =
                DisplayItemManager::getInstance().getAllMonitors();

            std::vector<std::string> consoleMonitorNames;

            for (std::map<std::string, Monitor*>::iterator iter = allMonitors.begin();
            iter != allMonitors.end(); iter++)
            {
                if (iter->second->isConsoleMonitor())
                {
                    consoleMonitorNames.push_back(iter->first);
                }
            }

            // get the keys to the wall monitor positioning
            std::vector<unsigned long> wallMonitorKeys = application->getVideoWallMonitorKeys();


            // set up the monitor ranges - initially they contain no monitors
            unsigned int firstConsoleMonitor = IDC_CONSOLE_MONITOR_1;
            unsigned int lastConsoleMonitor = IDC_CONSOLE_MONITOR_1 - 1;

            unsigned int firstWallMonitor = IDC_WALL_MONITOR_1;
            unsigned int lastWallMonitor = IDC_WALL_MONITOR_1 - 1;


            if ( locationType == TA_Base_Core::ILocation::OCC )
            {
                // occ has all wall monitors
                // and 1 console monitor
                lastConsoleMonitor = IDC_CONSOLE_MONITOR_1;
                lastWallMonitor = IDC_WALL_MONITOR_6;
            }
            else if ( locationType == TA_Base_Core::ILocation::STATION )
            {
                // station has no wall monitors
                // and 3 console monitors			
				lastConsoleMonitor = IDC_CONSOLE_MONITOR_3; //zhiqiang				              
            }
            else
            {
                // depot has no wall monitors
                // and 1 console monitor
                lastConsoleMonitor = IDC_CONSOLE_MONITOR_1;
            }

            // register wall monitors
			unsigned int i = firstWallMonitor;
            for( ; i <= lastWallMonitor; i++ )
            {
                // create the static
                boost::shared_ptr<CStatic> pStatic( new CStatic );
                m_monitors.push_back( pStatic );
                m_monitors[i - firstWallMonitor]->SubclassDlgItem( i, this );

                // create the overlay
                boost::shared_ptr<CLabel> pLabel( new CLabel );
                m_labels.push_back( pLabel );
                m_labels[i - firstWallMonitor]->SubclassDlgItem( (i + MONITOR_TEXT_DIFF), this );
                m_labels[i - firstWallMonitor]->SetTransparent(TRUE);

                GetDlgItem(i + MONITOR_LABEL_DIFF)->SetWindowText("Not Configured");

                // associate this item with a wall monitor
                if ( (i - firstWallMonitor) < wallMonitorKeys.size() )
                {
                    // see if there is a monitor with that key
                    Monitor* monitor =
                        DisplayItemManager::getInstance().getMonitor(wallMonitorKeys[i - firstWallMonitor]);

                    if (monitor != NULL)
                    {
                        m_monitorNameToResourceIdMap[monitor->getName()] = i;
                        m_resourceIdToMonitorNameMap[i] = monitor->getName();

                        // and set the static label text
                        GetDlgItem(i + MONITOR_LABEL_DIFF)->SetWindowText(monitor->getDescription().c_str());
                    }
                }
            }

            // register console monitors
            int index = m_monitors.size();  // carries on from i above
            for( i = firstConsoleMonitor; i <= lastConsoleMonitor; i++, index++ )
            {
                // create the static
                boost::shared_ptr<CStatic> pStatic( new CStatic );
                m_monitors.push_back( pStatic );
                m_monitors[index]->SubclassDlgItem( i, this );

                // create the overlay
                boost::shared_ptr<CLabel> pLabel( new CLabel );
                m_labels.push_back( pLabel );
                m_labels[index]->SubclassDlgItem( (i + MONITOR_TEXT_DIFF), this );
                m_labels[index]->SetTransparent(TRUE);

                // associate this item with a console monitor
                if ( (i - firstConsoleMonitor) < consoleMonitorNames.size() )
                {
                    m_monitorNameToResourceIdMap[consoleMonitorNames[i - firstConsoleMonitor]] = i;
                    m_resourceIdToMonitorNameMap[i] = consoleMonitorNames[i - firstConsoleMonitor];

                    // and set the static label text
                    Monitor* monitor =
                        DisplayItemManager::getInstance().getMonitor(consoleMonitorNames[i - firstConsoleMonitor]);
                    TA_ASSERT(monitor != NULL, "The monitor was just returned - it cannot be null");
                    GetDlgItem(i + MONITOR_LABEL_DIFF)->SetWindowText(monitor->getDescription().c_str());
                }
                else
                {
                    // set the static label text
                    GetDlgItem(i + MONITOR_LABEL_DIFF)->SetWindowText("Not Configured");
					GetDlgItem(i)->EnableWindow(false);
                }
            }

            FUNCTION_EXIT;
        }


        Monitor* VideoDisplayManagerDlg::getMonitor(int controlId)
        {
            FUNCTION_ENTRY( "getMonitor" );

            // find and return the item in the map
            std::map<int, std::string>::iterator findIter = m_resourceIdToMonitorNameMap.find(controlId);
            if (findIter != m_resourceIdToMonitorNameMap.end())
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
            FUNCTION_ENTRY( "getControlId" );

            // find and return the item in the map
            std::map<std::string, int>::iterator findIter = m_monitorNameToResourceIdMap.find(monitorName);
            if (findIter != m_monitorNameToResourceIdMap.end())
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
            FUNCTION_ENTRY( "LoadButtonIcons" );

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
                // Invalidate both appropriate control labels.
                invalidateControlLabels();

                TransActiveDialog::OnPaint();

                // Paint non-hilited control.
                if ( ( m_prvHilitedControlID != 0 ) && ( m_normalisationPerformed == FALSE ) )
                {
                    paintStandardControl();
                }

                // Paint hilited control.
                if ( ( m_hilitedControlID != 0 ) && ( m_hilitingPerformed == FALSE ) )
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
            FUNCTION_ENTRY( "OnQueryDragIcon" );
            FUNCTION_EXIT;
            return (HCURSOR) m_hIcon;
        }


        BOOL VideoDisplayManagerDlg::DestroyWindow()
        {
            FUNCTION_ENTRY( "DestroyWindow" );

            if (m_dragImage != NULL)
            {
                delete m_dragImage;
            }

            if (m_videoSourcesPropertySheet != NULL)
            {
                delete m_videoSourcesPropertySheet;
                m_videoSourcesPropertySheet = NULL;
            }

            // delete pages
            if (m_camerasPage != NULL)
            {
                delete m_camerasPage;
                m_camerasPage = NULL;
            }
            if (m_quadsPage != NULL)
            {
                delete m_quadsPage;
                m_quadsPage = NULL;
            }
            if (m_sequencesPage != NULL)
            {
                delete m_sequencesPage;
                m_sequencesPage = NULL;
            }
            if (m_trainsPage != NULL)
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
                    if ( pMsg->wParam == SC_CLOSE )
                    {
                        ::ReleaseCapture();
                        if ( m_mouseTracking && (m_dragImage != NULL) )
                        {
                            m_dragImage->DragLeave(this);
                            m_dragImage->EndDrag();
                            delete m_dragImage;
                            m_dragImage = NULL;
                            SetCursor( LoadCursor( NULL, IDC_ARROW ) );
                        }
                    }
                }
                break;
            default:
                {
                    //do nothing
                }
                break;
            }

            FUNCTION_EXIT;
            return TransActiveDialog::PreTranslateMessage(pMsg);
        }


        void VideoDisplayManagerDlg::OnLButtonDown(UINT nFlags, CPoint point)
        {
            FUNCTION_ENTRY( "OnLButtonDown" );

            TransActiveDialog::OnLButtonDown( nFlags, point );

            CPoint nullPt( 0,0 );

            // point == nullPt if the user clicked in the Video Source region.
            if( point == nullPt )
            {
                // Make sure that the user selected a list box item.
                if ( TRUE )
                {
                    SetCapture();
                    m_mouseTracking = TRUE;
                    CPoint cursorLocation;
                    ::GetCursorPos(&cursorLocation);

                    m_dragImage->BeginDrag(0, m_dragAdjustPoint);
                    m_dragImage->DragEnter(NULL,cursorLocation);
                }
            }
            // The user clicked outside the Video Sources region.
            else
            {
                // Reset the hilited and non-hilited monitor.
                if ( isOverMonitor(point, true) )
                {
                    updateDialog();
                    m_monitors[0]->SetFocus();
                    updateDetailsBox();

                    FUNCTION_EXIT;
                    return;
                }
                else
                {
                    CPoint negPt( -1,-1 );
                    if ( (m_hilitedControlID != 0) && (point == negPt) )
                    {
                        m_prvHilitedControlID = m_hilitedControlID;
                        m_normalisationPerformed = FALSE;
                        m_hilitedControlID = 0;
                        updateDialog();
                        enableSequencePlaybackButtons(NULL);
                    }
                }
            }

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::OnLButtonUp(UINT nFlags, CPoint point)
        {
            FUNCTION_ENTRY( "OnLButtonUp" );

            TransActiveDialog::OnLButtonUp(nFlags, point);

            // Release mouse and load standard arrow cursor.
            if ( TRUE == m_mouseTracking )
            {
                ::ReleaseCapture();
                endDragging();
                m_mouseTracking = FALSE;
                m_contextActive = FALSE;
                SetCursor( LoadCursor( NULL, IDC_ARROW ) );
                m_dragImage->DragLeave(this);
                m_dragImage->EndDrag();
                if ( NULL != m_dragImage )
                {
                    delete m_dragImage;
                    m_dragImage = NULL;
                }
                if ( isOverMonitor(point) )
                {                	
                	 //ignore consolemonitor3 when it is not in service
                	 if (m_hilitedControlID == IDC_CONSOLE_MONITOR_3)
					         {
						           if (!m_thirdMonitorInservice)
						           {
							             return;
						           }
					         }                	
                	
                    // get the monitor object from the highlighted control id
                    Monitor* monitor = getMonitor(m_hilitedControlID);
                    // get the video source that was dropped
                    VideoInput* input = getSelectedVideoSource( true );

                    // if there are both monitor and video input objects
                    if ( (monitor != NULL) && (input != NULL) )
                    {
                        // special case - check if the new input is a stage
                        BvsStage* newStage = dynamic_cast<BvsStage*>(input);
                        unsigned short newTrainId = 0;
                        if (newStage != NULL)
                        {
                            // get the selected train
                            newTrainId = getSelectedTrainId();
                        }

                        m_monitors[0]->SetFocus();

                        // get the current source for this monitor
                        VideoInput* currentInput = monitor->getInput();

                        //
                        // deal with the existing input
                        //

                        if (currentInput != NULL)
                        {
                            // if there is a train assigned to this monitor
                            BvsStage* stage = dynamic_cast<BvsStage*>(currentInput);
                            if ( stage != NULL )
                            {
                                // if there is a train assigned,
                                // and it isnt the same train on the same stage
                                unsigned short trainID = stage->getActiveTrain();
                                if ( (0 != trainID) &&
                                     !( (newTrainId == trainID) && (stage == newStage) ) )
                                {
                                    deactivateTrain(stage, monitor);
                                }
                            }
                        }

                        //
                        // Deal with the new input
                        //

                        // if it is a train
                        if (newStage != NULL)
                        {
                            unsigned short currentActiveTrainForStage = newStage->getActiveTrain();

                            // if there is already an active train for the new stage
                            //if ( ( 0 != currentActiveTrainForStage ) && ( newTrainId != currentActiveTrainForStage ) )
                            //{
                            //    std::stringstream ss;
                            //    ss << "The Stage " << newStage->getName() << " already has an active train.";
                            //    ss << " This train must be deactivated before Train" << static_cast<int>(newTrainId)
                            //       << " can be activated.";
                            //    AfxMessageBox( ss.str().c_str() );
                            //    return;
                            //}

                            try
                            {
                                // if the train needs to be activated
                                if ( currentActiveTrainForStage != newTrainId )
                                {
                                    // first check localisation
                                    VideoSourcesListCtrl::TrainDetails* details = m_trainsPage->getTrainDetails( newTrainId );

                                    // check the ats vailidity and location validity
                                    if ( ( VideoSourcesListCtrl::Valid == details->atsValid ) &&
                                         ( true == details->isLocalisationValid ) )
                                    {
                                        // activate the train
                                        m_trainCctvCorbaProxy->setCarrierOn( (newTrainId%0x0100), TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) );
                                    }
                                    else // this is a delocalised train
                                    {
                                        m_trainCctvCorbaProxy->setCarrierOnDelocalisedTrain( (newTrainId%0x0100), m_overriddenLocation, TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) );
                                    }
                                }

                                // set the active train for the current stage
                                TA_IRS_App::SwitchAgentCommunicator::getInstance().setActiveTrainID( newStage, newTrainId );

                                // assign the stage to the monitor
                                try
                                {
                                    TA_IRS_App::SwitchAgentCommunicator::getInstance().assignMonitor(monitor, input);
                                }
                                catch(...)
                                {
                                    FUNCTION_EXIT;
                                    throw;
                                }

                                newStage->setActiveTrain( newTrainId );
                                DisplayItemManager::getInstance().notifyGUIofDisplayItemUpdate(newStage);
                            }
                            catch( TA_Base_Core::TransactiveException& te)
                            {
                                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", te.what() );

                                CString errorNumber1 = "UE-090047";
                                CString errorNumber2 = "UE-090048";
                                CString errorNumber3 = "IDS_UE_090083";
                                CString errorNumber4 = "UE-020056";
                                CString errorNumber5 = "UE-010021";
                                CString errorNumber6 = "IDS_UE_090089";
                                CString errorNumber7 = "IDS_UE_090090";
                                // TD16879
                                CString errorNumber8 = "UE-090037";
                                // TD16879
                                CString message = "Switching of sequence inputs";
                                TA_Base_Bus::TransActiveMessage userMsg;
                                if(te.what() == errorNumber1)
                                {
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090047);
                                }
                                else if(te.what() == errorNumber2)
                                {
                                    userMsg << message;
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090048);
                                }
                                else if(te.what() == errorNumber3)
                                {
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090083);
                                }
                                else if(te.what() == errorNumber4)
                                {
                                    userMsg << message;
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_020056);
                                }
                                else if(te.what() == errorNumber5)
                                {
									//like++ CL-19410 
									std::string userName;
									std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
									TA_Base_Bus::AuthenticationLibrary authLibrary;
									TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
									boost::shared_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
									userName = iOperator->getName();
									userMsg << userName;
									userMsg << message;
									//CL-19410 ++like
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
                                }
                                else if(te.what() == errorNumber6)
                                {
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090089);
                                }
                                else if(te.what() == errorNumber7)
                                {
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090090);
                                }
                                // TD16879
                                else if(te.what() == errorNumber8)
                                {
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090037);
                                }
                                // TD16879
                                else
                                {
                                    std::ostringstream trainStream;
                                    trainStream << "PV" << std::setw(2) << std::setfill('0') << newTrainId;
                                    trainStream << ". " << te.what();
                
                                    TA_Base_Bus::TransActiveMessage userMsg;
                                    userMsg << trainStream.str();
                                    UINT selectedButton = userMsg.showMsgBox( IDS_UE_090075 );
                                }

                                FUNCTION_EXIT;
                                return;
                            }
                        }
                        else // this is a camera, sequence, or quad
                        {
                            try
                            {
                                TA_IRS_App::SwitchAgentCommunicator::getInstance().assignMonitor(monitor, input);
                            }
                            catch(TA_Base_Core::TransactiveException& te)
                            {
                                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
                                Monitor* monitor = getMonitor(m_hilitedControlID);
                                CString errorNumber1 = "UE-090047";
                                CString errorNumber2 = "UE-090048";
                                CString errorNumber3 = "UE-020056";
                                CString errorNumber4 = "UE-010021";
                                CString errorNumber5 = "IDS_UE_090089";
                                CString errorNumber6 = "IDS_UE_090090";
                                TA_Base_Bus::TransActiveMessage userMsg;
                                // TD16879
                                CString errorNumber7 = "UE-090037";
                                // TD16879
                                CString message = "Assigning monitor";

                                if(te.what() == errorNumber1)
                                {
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090047);
                                }
                                else if(te.what() == errorNumber2)
                                {
                                    userMsg << message;
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090048);
                                }
                                else if(te.what() == errorNumber3)
                                {
                                    userMsg << message;
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_020056);
                                }
                                else if(te.what() == errorNumber4)
                                {
									//like++ CL-19410 
									std::string userName;
									std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
									TA_Base_Bus::AuthenticationLibrary authLibrary;
									TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
									boost::shared_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
									userName = iOperator->getName();
									userMsg << userName;
									userMsg << message;
									//CL-19410 ++like
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
                                }
                                else if(te.what() == errorNumber5)
                                {
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090089);
                                }
                                else if(te.what() == errorNumber6)
                                {
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090090);
                                }
                                // TD16879
                                else if(te.what() == errorNumber7)
                                {
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090037);
                                }
                                // TD16879
                                else
                                {
                                    userMsg << message;
                                    userMsg << te.what();
                                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090080);
                                }
                                // update the details for the monitor - in case it changed
                                updateDetailsBox();
                                // enable/disable the sequence control buttons if the input changed
                                enableSequencePlaybackButtons(monitor);
                                //AfxMessageBox( te.what() );
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
            FUNCTION_ENTRY( "OnMouseMove" );

            short hilite = m_hilitedControlID;
            short prev = m_prvHilitedControlID;

            // If left mouse button is held down change to drag cursor
            // and keep resetting the current hilited monitor.
            if( m_mouseTracking )
            {
                SetCursor( AfxGetApp()->LoadCursor( IDC_DRAG_CURSOR ) );
                // Repaint the dialog as the hilited / non-hilited monitors change.
                if( FALSE == isOverMonitor( point ) )
                {
                    m_prvHilitedControlID = m_hilitedControlID;
                    m_hilitedControlID = 0;
                }

                if ( (hilite != m_hilitedControlID ) || (prev != m_prvHilitedControlID ) )
                {
                    m_dragImage->DragLeave(NULL);
                    m_dragImage->EndDrag();
                    m_contextActive = TRUE;
                    if (prev != m_prvHilitedControlID )
                    {
                        m_normalisationPerformed = FALSE;
                    }
                    updateDialog();
                }
                else
                {
                    ClientToScreen(&point);
                    if ( TRUE == m_contextActive )
                    {
                        m_dragImage->BeginDrag(0, m_dragAdjustPoint);
                        m_dragImage->DragEnter(NULL,point);
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
            FUNCTION_ENTRY( "updateDialog" );

            SendMessage( WM_PAINT, NULL, NULL );

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
            FUNCTION_ENTRY( "invalidateControlLabels" );

            if ( ( m_normalisationPerformed && m_hilitingPerformed ) )//&&
                //(FALSE == m_mouseTracking ) )
            {
                //nothing to do
                FUNCTION_EXIT;
                return;
            }

            // Declare an iterator for the vector.
            std::vector< boost::shared_ptr< CLabel > >::iterator theIterator;

            // Iterate through the vector and invalidate both of the appropriate labels.
            int i = 0;
            for ( theIterator = m_labels.begin(); theIterator != m_labels.end(); theIterator++ )
            {
                if( m_hilitedControlID == ( * theIterator )->GetDlgCtrlID() - MONITOR_TEXT_DIFF )
                {
                    if ( ( FALSE == m_hilitingPerformed ) )//|| ( m_mouseTracking ) )
                    {
                        ( *theIterator )->Invalidate();
                    }
                }
                if( m_prvHilitedControlID  == ( * theIterator )->GetDlgCtrlID() - MONITOR_TEXT_DIFF )
                {
                    if ( ( FALSE == m_normalisationPerformed ) )//|| ( m_mouseTracking ) )
                    {
                        ( *theIterator )->Invalidate();
                    }
                }
                i++;
            }

            FUNCTION_EXIT;
        }


        /**
          * IsOverMonitor
          *
          * TRUE if the mouse is over a monitor.
          */
        BOOL VideoDisplayManagerDlg::isOverMonitor( CPoint& pt, bool doCheckForSequence )
        {
            FUNCTION_ENTRY( "isOverMonitor" );

            std::vector< boost::shared_ptr< CStatic > >::iterator theIterator;
            CRect wndRect;

            for ( theIterator = m_monitors.begin(); theIterator != m_monitors.end(); theIterator++ )
            {
                if ( (*theIterator)->m_hWnd == NULL)
                {
                    // not drawn yet
                    continue;
                }

                ( *theIterator )->GetWindowRect( &wndRect );
                ScreenToClient( &wndRect );

                if( wndRect.PtInRect( pt ) )
                {
                    if ( (* theIterator )->GetDlgCtrlID() != m_hilitedControlID )
                    {
                        // m_prvHilitedMonitorID and m_hilitedMonitorID are reinitialized here.
                        m_prvHilitedControlID = m_hilitedControlID;
                        if ( 0 != m_prvHilitedControlID )
                        {
                            m_normalisationPerformed = FALSE;
                        }
                        m_hilitedControlID = (* theIterator )->GetDlgCtrlID();
                        m_hilitingPerformed = FALSE;

                        // check for sequence so as to enable controls

                        Monitor* monitor = getMonitor(m_hilitedControlID);
                        if (doCheckForSequence)
                        {
                            enableSequencePlaybackButtons(monitor);
                        }

                        FUNCTION_EXIT;
                        return TRUE;
                    }
                    else if ( (* theIterator )->GetDlgCtrlID() == m_hilitedControlID )
                    {
                        // still over monitor
                        FUNCTION_EXIT;
                        return TRUE;
                    }
                }
            }

            FUNCTION_EXIT;
            return FALSE;
        }


        /**
          * PaintStandardControl
          *
          * Paints the non-hilited bitmap in the control that lost focus.
          */
        void VideoDisplayManagerDlg::paintStandardControl()
        {
            FUNCTION_ENTRY( "paintStandardControl" );

            m_normalisationPerformed = TRUE;

            std::vector< boost::shared_ptr< CStatic > >::iterator theIterator;

            // If in OCC check for ODS or Wall Monitors
            TA_Base_Core::ILocation::ELocationType locationType =
                ( (VideoDisplayManagerApp*)(AfxGetApp()) )->getLocationType();

            if ( locationType == TA_Base_Core::ILocation::OCC )
            {
                // Overview Display System
                if ( m_prvHilitedControlID == IDC_OVERVIEW_DISPLAY )
                {
                    for ( theIterator = m_monitors.begin(); theIterator != m_monitors.end(); theIterator++ )
                    {
                        if( ( *theIterator )->GetDlgCtrlID() == m_prvHilitedControlID )
                        {
                            ( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                            MAKEINTRESOURCE( IDB_ODS ) ) );

                            FUNCTION_EXIT;
                            return;
                        }
                    }
                }
                // one of the Wall Monitors
                else if ( (m_prvHilitedControlID >= IDC_WALL_MONITOR_1)
                        && (m_prvHilitedControlID <= IDC_WALL_MONITOR_6) )
                {
                    for ( theIterator = m_monitors.begin(); theIterator != m_monitors.end(); theIterator++ )
                    {
                        if( ( *theIterator )->GetDlgCtrlID() == m_prvHilitedControlID )
                        {
                            ( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                            MAKEINTRESOURCE( IDB_SINGLEVMON ) ) );

                            FUNCTION_EXIT;
                            return;
                        }
                    }
                }
            }

            // Console Controls
            if ( m_prvHilitedControlID <= IDC_CONSOLE_MONITOR_3 ) //zhiqiang
            {

                for ( theIterator = m_monitors.begin(); theIterator != m_monitors.end(); theIterator++ )
                {
                    if( ( *theIterator )->GetDlgCtrlID() == m_prvHilitedControlID )
                    {
                        ( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                        MAKEINTRESOURCE( IDB_VMON ) ) );
                        FUNCTION_EXIT;
                        return;
                    }
                }
            }

            FUNCTION_EXIT;
        }


        /**
          * PaintHilitedStandardControl
          *
          * Paints the hilited bitmap in the control that lost focus.
          */
        void VideoDisplayManagerDlg::paintHilitedControl()
        {
            FUNCTION_ENTRY( "paintHilitedControl" );

            m_hilitingPerformed = TRUE;
            std::vector< boost::shared_ptr< CStatic > >::iterator theIterator;

            // If in OCC check for ODS or Wall Monitors
            TA_Base_Core::ILocation::ELocationType locationType =
                ( (VideoDisplayManagerApp*)(AfxGetApp()) )->getLocationType();

            if ( locationType == TA_Base_Core::ILocation::OCC )
            {
                // Overview Display System
                if ( m_hilitedControlID == IDC_OVERVIEW_DISPLAY )
                {
                    for ( theIterator = m_monitors.begin(); theIterator != m_monitors.end(); theIterator++ )
                    {
                        if( ( *theIterator )->GetDlgCtrlID() == m_hilitedControlID )
                        {
                            ( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                                MAKEINTRESOURCE( IDB_ODS_HILITE ) ) );

                            FUNCTION_EXIT;
                            return;
                        }
                    }
                }
                // one of the Wall Monitors
                else if ( (m_hilitedControlID >= IDC_WALL_MONITOR_1)
                        && (m_hilitedControlID <= IDC_WALL_MONITOR_6) )
                {
                    for ( theIterator = m_monitors.begin(); theIterator != m_monitors.end(); theIterator++ )
                    {
                        if( ( *theIterator )->GetDlgCtrlID() == m_hilitedControlID )
                        {
                            ( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
                            MAKEINTRESOURCE( IDB_SINGLEVMON_HILITE ) ) );

                            FUNCTION_EXIT;
                            return;
                        }
                    }
                }
            }

            // Console Controls
            if ( m_hilitedControlID <= IDC_CONSOLE_MONITOR_3 ) //zhiqiang
            {
				
                for ( theIterator = m_monitors.begin(); theIterator != m_monitors.end(); theIterator++ )
                {
                    if( ( *theIterator )->GetDlgCtrlID() == m_hilitedControlID )
                    {
						if (GetDlgItem(m_hilitedControlID)->IsWindowEnabled())
						{
							( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
							MAKEINTRESOURCE( IDB_VMON_HILITE ) ) );
						}
						else
						{
							( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
							MAKEINTRESOURCE( IDB_VMON ) ) );
						}
						
						
						if (m_hilitedControlID == IDC_CONSOLE_MONITOR_3)
						{
							if (!m_thirdMonitorInservice)
							{
								( *theIterator )->SetBitmap( ::LoadBitmap( AfxGetInstanceHandle(),
									MAKEINTRESOURCE( IDB_VMON ) ) );
							}
						}
					}
					
                }
            }

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::OnContextMenu(CWnd* pWnd, CPoint point)
        {
            FUNCTION_ENTRY( "OnContextMenu" );

            // if context menu currently displayed
            if ( (TRUE == m_contextActive) || (TRUE == m_mouseTracking) )
            {
                FUNCTION_EXIT;
                return;
            }

            ScreenToClient(&point);
            // Load the context menu.
            CMenu contextMenu;

            // Only display context menu if the monitor is hilited and has an associated camera
            // or sequence.
            if( isOverMonitor( point ) )
            {
                updateDialog();

                // get the monitor
                Monitor* monitor = getMonitor(m_hilitedControlID);

                // get the associated input
                VideoInput* input = NULL;
                if (monitor != NULL)
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
                Quad* quad = dynamic_cast<Quad*>(input);
                Camera* camera = dynamic_cast<Camera*>(input);
                BvsStage* stage = dynamic_cast<BvsStage*>(input);

                std::vector<VideoOutput::TrainCamera> stages;

                if ( input == NULL)
                {
                    if(monitor->isInAlarmMode())
                    {
                        stages = monitor->getTrainCamerasInAlarmStack();
                        if(stages.size() > 0)
                        {
                            contextMenu.LoadMenu( IDR_MONALARMCONTEXT_MENU );
                            buildTrainContextMenu(contextMenu.GetSubMenu(0), stages);
                        }
                        else if (monitor->getUnassignedStagesInAlarmStack().size() > 0)
                        {
                            // allow the monitor to be cleared
                            contextMenu.LoadMenu( IDR_MONCAMCONTEXT_MENU );
                        }
                        // TD16690
                        else
                        {
                            FUNCTION_EXIT;
                            return;
                        }
                        // TD16690
                    }
                    else
                    {
                        FUNCTION_EXIT;
                        return;
                    }
                }
                else
                {
                    if (seq != NULL)
                    {
                        contextMenu.LoadMenu( IDR_MONSEQCONTEXT_MENU );
                    }
                    else if (quad != NULL)
                    {
                        contextMenu.LoadMenu( IDR_MONQUADCONTEXT_MENU );
                    }
                    else if (camera != NULL)
                    {
                        contextMenu.LoadMenu( IDR_MONCAMCONTEXT_MENU );
                    }
                    else if (stage != NULL)
                    {
                        // get the active train for the stage
                        unsigned short trainID = stage->getActiveTrain();

                        // if there is one
                        if ( trainID > 0 )
                        {
                            contextMenu.LoadMenu( IDR_MONTRAINCONTEXT_MENU );
                        }
                        else
                        {
                            contextMenu.LoadMenu( IDR_MONCAMCONTEXT_MENU );
                        }
                    }
                    else
                    {
                        TA_ASSERT(false, "Unknown video input type assigned to monitor");
                    }
                }

                CMenu* pContextMenu = contextMenu.GetSubMenu( 0 );

                // Display the context menu.
                m_contextActive = TRUE;
                ClientToScreen(&point);
                int nCmd = ( int ) pContextMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
                    point.x, point.y, AfxGetMainWnd() );
                // Process the appropriate context menu command.
                m_contextActive = FALSE;

                // check the result
                if(nCmd > static_cast<long>(stages.size()) || nCmd==0)
                {
                    switch ( nCmd )
                    {
                    case ID_CLEAR_MONITOR:
                        {
                            if (false == monitor->isInAlarmMode())
                            {
                                clearMonitor(monitor);
                            }
                            else
                            {
                                // this is alarm mode without any active train
                                // must allow the monitor to be cleared
                                std::vector<BvsStage*> alarmStages = monitor->getUnassignedStagesInAlarmStack();
                                SwitchAgentCommunicator::getInstance().clearStages(alarmStages);
                            }
                        }
                        break;
                    case ID_LOAD_CONTROLLER:
                        if ( quad != NULL )
                        {
                            loadQuadController(quad);
                        }
                        else if ( stage != NULL )
                        {
                            loadTrainController(stage);
                        }
                        break;
                    case ID_NEW_EDIT_SEQUENCE:
                        loadSequenceController(seq);
                        break;
                    case ID_DEACTIVATE_TRAIN:
                        // a monitor with an associated stage was clicked on
                        // clear the monitor - it will prompt whether to clear other users of this stage as well
                        // get the current input
                        deactivateTrain(stage, monitor);
                        break;
                    default:
                        //do nothing
                        break;
                    }

                    FUNCTION_EXIT;
                    return;
                }
                else
                {
                    try
                    {
                        m_trainCctvCorbaProxy->removeAlarmImage( static_cast<unsigned char>(stages[nCmd-TRAIN_COMMAND_OFFSET].trainId),
                                                                 static_cast<TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera>(stages[nCmd-TRAIN_COMMAND_OFFSET].cameraId ),
                                                                 TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) );
                    }
                    catch( TA_Base_Core::TransactiveException& e)
                    {
                        LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

                        std::ostringstream trainStream;
                        trainStream << "PV" << std::setw(2) << std::setfill('0') << stages[nCmd-TRAIN_COMMAND_OFFSET].trainId;
                        trainStream << ". " << e.what();
                
                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << trainStream.str();
                        UINT selectedButton = userMsg.showMsgBox( IDS_UE_090076 );
                    }
                    FUNCTION_EXIT;
                    return;
                }
            }


            //
            // It wasnt a monitor that was clicked on
            // It was one of the source pages
            //


            // get the video source that was dropped
            VideoInput* input = getSelectedVideoSource();

            ClientToScreen(&point);
            HWND hwnd = WindowFromPoint(point)->m_hWnd;

            // if one of the list controls is active
            if ( (hwnd == m_camerasPage->m_cameraTreeCtrl.GetSafeHwnd() ) ||
                (hwnd == m_quadsPage->m_quadTreeCtrl.GetSafeHwnd() ) ||
                (hwnd == m_sequencesPage->m_sequenceTreeCtrl.GetSafeHwnd() ) ||
                (hwnd == m_trainsPage->m_inboundTrainsListCtrl.GetSafeHwnd() ) ||
                (hwnd == m_trainsPage->m_outboundTrainsListCtrl.GetSafeHwnd() ) )
            {
                Sequence* seq = dynamic_cast<Sequence*>(input);
                Quad* quad = dynamic_cast<Quad*>(input);
                Camera* camera = dynamic_cast<Camera*>(input);
                BvsStage* stage = dynamic_cast<BvsStage*>(input);
                if (seq != NULL)
                {
                    contextMenu.LoadMenu( IDR_PAGESEQCONTEXT_MENU );
                }
                else if (quad != NULL)
                {
                    contextMenu.LoadMenu( IDR_PAGEQUADCONTEXT_MENU );
                }
                else if (camera != NULL)
                {
                    contextMenu.LoadMenu( IDR_MONCAMCONTEXT_MENU );
                }
                else if (stage != NULL)
                {
                    // the stage is just the stage the train is in
                    // so the train id is not necessarily the active train for the stage
                    // get the selected train id
                    unsigned short trainID = getSelectedTrainId();

                    contextMenu.LoadMenu( IDR_PAGETRAINCONTEXT_MENU );

                    // if the train is not the active train for its stage
                    if (trainID != stage->getActiveTrain())
                    {
                        // grey the menu items out
                        CMenu* pContextMenu = contextMenu.GetSubMenu( 0 );
                        //TD12786 - enable context menu regardless of train state
                        //pContextMenu->EnableMenuItem(ID_DEACTIVATE_TRAIN, MF_GRAYED);
                        pContextMenu->EnableMenuItem(ID_LOAD_CONTROLLER, MF_GRAYED);
                    }
                }
                else
                {
                    // special case for delocalised trains
                    if ( hwnd == m_trainsPage->m_outboundTrainsListCtrl.GetSafeHwnd() )
                    {
                        VideoSourcesListCtrl::TrainDetails* details = m_trainsPage->getSelectedTrainDetails();

                        if ( NULL != details )
                        {
                            contextMenu.LoadMenu( IDR_PAGETRAINCONTEXT_MENU );

                            // grey the menu items out
                            CMenu* pContextMenu = contextMenu.GetSubMenu( 0 );
                            pContextMenu->EnableMenuItem(ID_LOAD_CONTROLLER, MF_GRAYED);
                        }
                    }
                    else
                    {
                        FUNCTION_EXIT;
                        return;
                    }
                }

                CMenu* pContextMenu = contextMenu.GetSubMenu( 0 );
                // Display the context menu.
                m_contextActive = TRUE;
                int nCmd = ( int ) pContextMenu->TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
                        point.x, point.y, AfxGetMainWnd() );
                m_contextActive = FALSE;

                // Process the appropriate context menu command.
                switch ( nCmd )
                {
                case ID_NEW_EDIT_SEQUENCE:
                    loadSequenceController(seq)    ;
                    break;
                case ID_LOAD_CONTROLLER:
                    if ( quad != NULL )
                    {
                        loadQuadController(quad);
                    }
                    else if ( stage != NULL )
                    {
                        // the stage is just the stage the train is in
                        // so the train id is not necessarily the active train for the stage
                        // get the selected train id
                        unsigned short trainId = getSelectedTrainId();

                        loadTrainController(trainId);
                    }
                    break;
                case ID_DEACTIVATE_TRAIN:
                    {
                        // get the selected train Id
                        unsigned short trainId = getSelectedTrainId();

                        // there should be a selected stage
                        // and the active train in that stage should match the selected train
                        if ( (stage != NULL) && (trainId == stage->getActiveTrain()) )
                        {
                            // deactivate the train
                            // this will prompt whether to clear off all monitors
                            deactivateTrain(stage);
                        }
                        else
                        {
                            // just send the carrier off to the train, because it is not assigned to anything
                            sendCarrierOff( trainId );
                        }
                    }
                    break;
                default:
                    //do nothing
                    break;
                }

                FUNCTION_EXIT;
                return;
            }

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::clearMonitor(Monitor* monitor)
        {
            FUNCTION_ENTRY( "clearMonitor" );

            TA_ASSERT(monitor != NULL, "Cannot clear a NULL monitor - menu item should be disabled");

            // get the current input
            VideoInput* input = monitor->getInput();

            if (input == NULL)
            {
                // monitor is clear
                FUNCTION_EXIT;
                return;
            }

            // if a train is assigned to the monitor - then deactivate the train first
            BvsStage* stage = dynamic_cast<BvsStage*>(input);
            if ( stage != NULL )
            {
                // deactivate the train if it is unique in the system
                // prompt if it is used elsewhere
                if(stage->getNumActiveMonitors() <= 1 && !SwitchAgentCommunicator::getInstance().isBvsInAlarmStack(stage))
                {
                    deactivateTrain(stage, monitor);
                }
            }

            try
            {
                TA_IRS_App::SwitchAgentCommunicator::getInstance().clearMonitor( monitor );
            }
            catch(TA_Base_Core::TransactiveException& te)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
                CString errorNumber1 = "UE-090047";
                CString errorNumber2 = "UE-090048";
                CString errorNumber3 = "UE-020056";
                CString errorNumber4 = "UE-010021";
                CString errorNumber5 = "IDS_UE_090089";
                CString errorNumber6 = "IDS_UE_090090";
                TA_Base_Bus::TransActiveMessage userMsg;
                // TD16879
                CString errorNumber7 = "UE-090037";
                // TD16879
                CString message = "Clear Monitor";
                if(te.what() == errorNumber1)
                {
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090047);
                }
                else if(te.what() == errorNumber2)
                {
                    userMsg << message;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090048);
                }
                else if(te.what() == errorNumber3)
                {
                    userMsg << message;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_020056);
                }
                else if(te.what() == errorNumber4)
                {
					std::string userName;
					std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
					TA_Base_Bus::AuthenticationLibrary authLibrary;
					TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
					boost::shared_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
					userName = iOperator->getName();
					userMsg << userName;
					userMsg << message;
					UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
                }
                else if(te.what() == errorNumber5)
                {
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090089);
                }
                else if(te.what() == errorNumber6)
                {
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090090);
                }
                // TD16879
                else if(te.what() == errorNumber7)
                {
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090037);
                }
                // TD16879
                else
                {
                    userMsg << message;
                    userMsg<< te.what();
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090080);
                }
                //AfxMessageBox( te.what() );
            }

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::loadQuadController(Quad* quad)
        {
            FUNCTION_ENTRY( "loadQuadController" );

            TA_ASSERT(quad != NULL, "Cannot load a controller for a NULL quad");
            QuadControllerDlg quadContDlg;
            m_pQuadControllerDlg = &quadContDlg;
            quadContDlg.DoModal(quad);
            m_pQuadControllerDlg = NULL;

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::loadTrainController(BvsStage* stage)
        {
            FUNCTION_ENTRY( "loadTrainController" );

            TA_ASSERT(stage != NULL, "Cannot load a controller for a NULL stage");
            unsigned short trainId = stage->getActiveTrain();

            loadTrainController(trainId);

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::loadTrainController(unsigned short trainId)
        {
            FUNCTION_ENTRY( "loadTrainController" );

            if (trainId == 0)
            {
                FUNCTION_EXIT;
                return;
            }

            VideoSourcesListCtrl::TrainDetails* details = m_trainsPage->getTrainDetails( trainId );
            TrainborneControllerDlg trainContDlg;
            trainContDlg.DoModal( m_trainCctvCorbaProxy, details );

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::loadSequenceController(Sequence* sequence)
        {
            FUNCTION_ENTRY( "loadSequenceController" );

            TA_ASSERT(sequence != NULL, "Cannot launch sequence editor for null sequence - menu item should not be accessible");
            // launch Sequence Editor
            SequenceEditorDlg sequenceEditorDlg(this, &m_sequencesPage->m_sequenceTreeCtrl);
            m_pSequenceEditorDlg = &sequenceEditorDlg;
            sequenceEditorDlg.DoModal(sequence);
            m_pSequenceEditorDlg = NULL;

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::deactivateTrain(BvsStage* source, Monitor* monitor)
        {
            FUNCTION_ENTRY( "deactivateTrain" );

            TA_ASSERT(source != NULL, "Cannot deactivate a train for a NULL stage");
            if(monitor!=NULL)
            {
                if(SwitchAgentCommunicator::getInstance().isBvsInAlarmStack(source))
                {
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090082);
                    clearMonitor(monitor);

                    FUNCTION_EXIT;
                    return;
                }
                else if(source->getNumActiveMonitors() > 1)
                {
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UI_090073);
                    if(IDNO == selectedButton) // TD16052
                    {
                        clearMonitor(monitor);

                        FUNCTION_EXIT;
                        return;
                    }
                }
            }

            unsigned short trainId = source->getActiveTrain();

            if (trainId == 0)
            {
                // dont need to deactivate anything
                FUNCTION_EXIT;
                return;
            }

            try
            {
                //TD 15954
                //zhou yuan++
                m_trainCctvCorbaProxy->setCarrierOff( (trainId%0x0100), TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) );

                // deactivate it at the switch agent
                TA_IRS_App::SwitchAgentCommunicator::getInstance().setActiveTrainID( source, 0x0 );

                source->setActiveTrain( 0 );

            }
            catch( TA_Base_Core::TransactiveException& e)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

                std::ostringstream trainStream;
                trainStream << "PV" << std::setw(2) << std::setfill('0') << trainId;
                trainStream << ". " << e.what();
                
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << trainStream.str();
                UINT selectedButton = userMsg.showMsgBox( IDS_UE_090076 );
            }

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::sendCarrierOff( unsigned short trainId )
        {
            try
            {
                m_trainCctvCorbaProxy->setCarrierOff( (trainId%0x0100), TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID) );
            }
            catch( TA_Base_Core::TransactiveException& e)
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

                std::ostringstream trainStream;
                trainStream << "PV" << std::setw(2) << std::setfill('0') << trainId;
                trainStream << ". " << e.what();
                
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << trainStream.str();
                UINT selectedButton = userMsg.showMsgBox( IDS_UE_090076 );
            }
        }


        void VideoDisplayManagerDlg::OnRButtonDown(UINT nFlags, CPoint point)
        {
            FUNCTION_ENTRY( "OnRButtonDown" );

            // Invoke the left button down functionality to ensure
            // that the hilited and non hilited monitors are updated.
            if (FALSE == m_contextActive)
            {
                LPARAM lParam = 0;
                lParam = ( ( point.x ) + (point.y << 16 ) );
                SendMessage( WM_LBUTTONDOWN, MK_RBUTTON, lParam );
            }
            TransActiveDialog::OnRButtonDown(nFlags, point);

            FUNCTION_EXIT;
        }


        VideoInput* VideoDisplayManagerDlg::getSelectedVideoSource( bool allowDelocalisedTrains )
        {
            FUNCTION_ENTRY( "getSelectedVideoSource" );

            // Hopefully when the back end for train operations is
            // properly implemented - this can all be encapsulated in
            // VideoSourcesPropertySheet

            VideoInput* source = NULL;

            // get the source based on the active sheet
            int focus = m_videoSourcesPropertySheet->GetActiveIndex();
            switch ( focus )
            {
            case 0:
                source = m_camerasPage->m_cameraTreeCtrl.getSelectedInput();
                break;
            case 1:
                source = m_quadsPage->m_quadTreeCtrl.getSelectedInput();
                break;
            case 2:
                source = m_sequencesPage->m_sequenceTreeCtrl.getSelectedInput();
                break;
            case 3:
                {
                    VideoSourcesListCtrl::TrainDetails* details = m_trainsPage->getSelectedTrainDetails();

                    if ( NULL != details )
                    {
                        // first using the existing location check if the train is already active
                        BvsStage* possibleStage = DisplayItemManager::getInstance().getBvsStageById( details->currentCctvZone );

                        if ( NULL != possibleStage )
                        {
                            unsigned short activeTrain = possibleStage->getActiveTrain();
                            if ( activeTrain == details->trainID )
                            {
                                source = possibleStage;
                            }
                        }
                        
                        if ( NULL == source )
                        {
                            // check the ats vailidity and location validity
                            if ( ( VideoSourcesListCtrl::Valid == details->atsValid ) &&
                                 ( true == details->isLocalisationValid ) )
                            {
                                // get the stage based on the stage ID in the details
                                source = DisplayItemManager::getInstance().getBvsStageById( details->currentCctvZone );
                            }
                            else if ( true == allowDelocalisedTrains )
                            {
                                // prompt the user for the train location
                                LocationSelectionDialog locationSelectionDialog( details->currentLocation, this );

                                if ( IDOK == locationSelectionDialog.DoModal() )
                                {
                                    m_overriddenLocation = locationSelectionDialog.getSelectedLocation();
                                    source = DisplayItemManager::getInstance().getBvsStageForLocation( m_overriddenLocation );
                                }
                            }
                            else if ( 0 != details->currentCctvZone )
                            {
                                // use delocalised location
                                source = DisplayItemManager::getInstance().getBvsStageById( details->currentCctvZone );
                            }
                        }
                    }
                }
                break;
            default:
                {
                    // do nothing
                }
            }

            FUNCTION_EXIT;
            return source;
        }


        unsigned short VideoDisplayManagerDlg::getSelectedTrainId()
        {
            FUNCTION_ENTRY( "getSelectedTrainId" );

            if (m_videoSourcesPropertySheet->GetActiveIndex() == 3)
            {
                FUNCTION_EXIT;
                return m_trainsPage->getSelectedTrainId();
            }

            FUNCTION_EXIT;
            return 0;
        }


        void VideoDisplayManagerDlg::endDragging()
        {
            FUNCTION_ENTRY( "endDragging" );

            int focus = m_videoSourcesPropertySheet->GetActiveIndex();
            switch ( focus )
            {
            case 0:
                {
                    m_camerasPage->m_cameraTreeCtrl.endDragging();

                    FUNCTION_EXIT;
                    return;
                }
                break;
            case 1:
                {
                    m_quadsPage->m_quadTreeCtrl.endDragging();

                    FUNCTION_EXIT;
                    return;
                }
                break;
            case 2:
                {
                    m_sequencesPage->m_sequenceTreeCtrl.endDragging();

                    FUNCTION_EXIT;
                    return;
                }
                break;
            default:
                {
                    //do nothing
                }
            }

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::OnHelpDisplaymanagerhelp()
        {
            FUNCTION_ENTRY( "OnHelpDisplaymanagerhelp" );

            TA_Base_Bus::HelpLauncher::getInstance().displayHelp();

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::OnAppAbout()
        {
            FUNCTION_ENTRY( "OnAppAbout" );

            TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
        {
            FUNCTION_ENTRY( "OnLButtonDblClk" );

            // Check the double click occured over a hilited monitor
            if ( isOverMonitor( point ) )
            {
                updateDialog();

                // get the monitor
                Monitor* monitor = getMonitor(m_hilitedControlID);

                // get the associated input
                VideoInput* input = NULL;
                if (monitor != NULL)
                {
                    input = monitor->getInput();
                }

                // if there is an input
                if (input != NULL)
                {
                    // load the context menu based on the type of assignment
                    Sequence* seq = dynamic_cast<Sequence*>(input);
                    Quad* quad = dynamic_cast<Quad*>(input);
                    Camera* camera = dynamic_cast<Camera*>(input);
                    BvsStage* stage = dynamic_cast<BvsStage*>(input);
                    if (seq != NULL)
                    {
                        loadSequenceController(seq);
                    }
                    else if (quad != NULL)
                    {
                        loadQuadController(quad);
                    }
                    else if (camera != NULL)
                    {
                        // do nussing
                    }
                    else if (stage != NULL)
                    {
                        loadTrainController(stage);
                    }
                    else
                    {
                        TA_ASSERT(false, "Unknown video input type assigned to monitor");
                    }
                }
            }

            TransActiveDialog::OnLButtonDblClk(nFlags, point);

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::OnClose()
        {
            FUNCTION_ENTRY( "OnClose" );

            //TD18095, jianghp, to fix the performance of showing manager application
            TransActiveDialog::DestroyWindow();

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::updateDetailsBox()
        {
            FUNCTION_ENTRY( "updateDetailsBox" );

            // get the selected item
            Monitor* monitor = getMonitor(m_hilitedControlID);

            // if a monitor is selected
            if (monitor != NULL)
            {
                // populate the details box with the descriptive text
                m_detailsBox.SetWindowText(monitor->getDetails().c_str());
            }
            else
            {
                m_detailsBox.SetWindowText(NODEVICE);
            }

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::OnCommonNextelementButton()
        {
            FUNCTION_ENTRY( "OnCommonNextelementButton" );

            sendSequenceCommand( TA_IRS_App::SwitchAgentCommunicator::SEQUENCE_SKIP_FORWARD );
            m_commonNextElement.SetCheck(0);

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::OnCommonPauseCheck()
        {
            FUNCTION_ENTRY( "OnCommonPauseCheck" );

            sendSequenceCommand( TA_IRS_App::SwitchAgentCommunicator::SEQUENCE_HOLD );
            m_commonPause.SetCheck(0);

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::OnCommonPlayCheck()
        {
            FUNCTION_ENTRY( "OnCommonPlayCheck" );

            sendSequenceCommand( TA_IRS_App::SwitchAgentCommunicator::SEQUENCE_PLAY );
            m_commonPlay.SetCheck(0);

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::OnCommonPreviouselementButton()
        {
            FUNCTION_ENTRY( "OnCommonPreviouselementButton" );

            sendSequenceCommand( TA_IRS_App::SwitchAgentCommunicator::SEQUENCE_SKIP_BACKWARD );
            m_commonPreviousElement.SetCheck(0);

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::OnConsoleNextelementButton()
        {
            FUNCTION_ENTRY( "OnConsoleNextelementButton" );

            sendSequenceCommand( TA_IRS_App::SwitchAgentCommunicator::SEQUENCE_SKIP_FORWARD );
            m_consoleNextElement.SetCheck(0);

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::OnConsolePauseCheck()
        {
            FUNCTION_ENTRY( "OnConsolePauseCheck" );

            sendSequenceCommand( TA_IRS_App::SwitchAgentCommunicator::SEQUENCE_HOLD );
            m_consolePause.SetCheck(0);

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::OnConsolePlayCheck()
        {
            FUNCTION_ENTRY( "OnConsolePlayCheck" );

            sendSequenceCommand( TA_IRS_App::SwitchAgentCommunicator::SEQUENCE_PLAY );
            m_consolePlay.SetCheck(0);
			if(TA_Base_Core::RunParams::getInstance().isSet("IsServer")) // --is-server
			{

			}


            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::OnConsolePreviouselementButton()
        {
            FUNCTION_ENTRY( "OnConsolePreviouselementButton" );

            sendSequenceCommand( TA_IRS_App::SwitchAgentCommunicator::SEQUENCE_SKIP_BACKWARD );
            m_consolePreviousElement.SetCheck(0);

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::enableSequencePlaybackButtons(Monitor* selectedMonitor)
        {
            FUNCTION_ENTRY( "enableSequencePlaybackButtons" );

            bool enableConsole = false;
            bool enableCommon = false;

            if (selectedMonitor != NULL)
            {
                // get the monitor input
                Sequence* sequence = dynamic_cast<Sequence*>( selectedMonitor->getInput() );

                // if a sequence is asssigned
                if (sequence != NULL || selectedMonitor->getAlarmStackItems().size() > 1)
                {
                    if ( selectedMonitor->isConsoleMonitor() )
                    {
                        enableConsole = true;
                    }
                    else
                    {
                        enableCommon = true;
                    }
                }

            }

            m_consoleNextElement.EnableWindow(enableConsole);
            m_consolePause.EnableWindow(enableConsole);
            m_consolePlay.EnableWindow(enableConsole);
            m_consolePreviousElement.EnableWindow(enableConsole);
            GetDlgItem(IDC_CONSOLE_PLAYBACK_LABEL)->EnableWindow(enableConsole);

            m_commonNextElement.EnableWindow(enableCommon);
            m_commonPause.EnableWindow(enableCommon);
            m_commonPlay.EnableWindow(enableCommon);
            m_commonPreviousElement.EnableWindow(enableCommon);
            GetDlgItem(IDC_COMMON_PLAYBACK_LABEL)->EnableWindow(enableCommon);

            FUNCTION_EXIT;
        }


        void VideoDisplayManagerDlg::sendSequenceCommand( SwitchAgentCommunicator::ESequencePlaybackCommandType playbackCmd )
        {
            FUNCTION_ENTRY( "sendSequenceCommand" );

            // get the selected monitor
            Monitor* monitor = getMonitor(m_hilitedControlID);
            TA_ASSERT(monitor != NULL, "The seqence control buttons should be disabled if a monitor is not selected");
            // get the sequence for that monitor

            try
            {
                TA_IRS_App::SwitchAgentCommunicator::getInstance().controlSequencePlayback( monitor, playbackCmd );
            }
            catch(TA_Base_Core::TransactiveException& te)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
                CString errorNumber1 = "UE-090047";
                CString errorNumber2 = "UE-090048";
                CString errorNumber3 = "UE-020056";
                CString errorNumber4 = "UE-010021";
                CString errorNumber5 = "IDS_UE_090089";
                CString errorNumber6 = "IDS_UE_090090";
                // TD16879
                CString errorNumber7 = "UE-090037";
                // TD16879
                CString msgHold = "Pause sequence playback";
                CString msgSkipForward = "Cycle sequence forward";
                CString msgSkipBackward = "Cycle sequence backward";
                CString msgPlay = "Resume sequence playback";

                TA_Base_Bus::TransActiveMessage userMsg;
				CString message;

                switch(playbackCmd)
                {
                    case  SwitchAgentCommunicator::ESequencePlaybackCommandType::SEQUENCE_HOLD:
                        userMsg << msgHold;
						message = msgHold;
                        break;
                    case SwitchAgentCommunicator::ESequencePlaybackCommandType::SEQUENCE_SKIP_FORWARD:
                        userMsg << msgSkipForward;
						message = msgSkipForward;
                        break;
                    case SwitchAgentCommunicator::ESequencePlaybackCommandType::SEQUENCE_SKIP_BACKWARD:
                        userMsg << msgSkipBackward;
						message = msgSkipBackward;
                        break;
                    case SwitchAgentCommunicator::ESequencePlaybackCommandType::SEQUENCE_PLAY:
                        userMsg << msgPlay;
						message = msgPlay;
                        break;
                }
                if(te.what() == errorNumber1)
                {
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090047);
                }
                else if(te.what() == errorNumber2)
                {
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090048);
                }
                else if(te.what() == errorNumber3)
                {
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_020056);
                }
                else if(te.what() == errorNumber4)
                {
					std::string userName;
					std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
					TA_Base_Bus::AuthenticationLibrary authLibrary;
					TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
					boost::shared_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
					userName = iOperator->getName();
					userMsg.clearArguments();
					userMsg << CString(userName.c_str());
					userMsg << message;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
                }
                else if(te.what() == errorNumber5)
                {
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090089);
                }
                else if(te.what() == errorNumber6)
                {
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090090);
                }
                // TD16879
                else if(te.what() == errorNumber7)
                {
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090037);
                }
                // TD16879
                else
                {
                    userMsg<< te.what();
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_090080);
                }
                //AfxMessageBox( te.what() );
            }

            FUNCTION_EXIT;
        }


        LRESULT VideoDisplayManagerDlg::onMonitorUpdated(WPARAM wParam, LPARAM lParam)
        {
            FUNCTION_ENTRY( "onMonitorUpdated" );

            // this is called when
            // - A monitor assignment is updated
            // - Something assigned to one of the monitors is updated

            // lParam holds the DisplayItem* that was updated
            DisplayItem* item = reinterpret_cast<DisplayItem*>(lParam);
            TA_ASSERT(item != NULL, "Monitor update received for NULL display item");
            Monitor* monitor = dynamic_cast<Monitor*>(item);
            TA_ASSERT(monitor != NULL, "Monitor update received for not a monitor");

            // update the monitor on the screen
            int updatedControl = getControlId(monitor->getName());

            if (updatedControl > 0)
            {
                CString text("");
                // update the monitor assignment label
                if (monitor->getAlarmStackItems().size() > 0)
                {
                    text = "ALARM MODE";
                }
                else
                {
                    VideoInput* input = monitor->getInput();
                    if (input != NULL)
                    {
                        text = input->getName().c_str();
                    }

                    // if the input is a stage - then set the assignment text to the train ID
                    BvsStage* stage = dynamic_cast<BvsStage*>(input);
                    if (stage != NULL)
                    {
                        std::ostringstream temp;
                        temp << "PV" << std::setw(2) << std::setfill('0') << stage->getActiveTrain();


                        text = temp.str().c_str();
                    }
                }
                text = text.GetBufferSetLength(CAMERA_TEXT_LENGTH);
                // set the overlay monitor label
                CString oldText;
                GetDlgItem(updatedControl + MONITOR_TEXT_DIFF)->GetWindowText(oldText);
                if (text != oldText)
                {
                    GetDlgItem(updatedControl + MONITOR_TEXT_DIFF)->SetWindowText(text);
                    // make sure the updated monitor gets redrawn
                    GetDlgItem(updatedControl)->Invalidate();
                    GetDlgItem(updatedControl + MONITOR_TEXT_DIFF)->Invalidate();
                }
            }

            // if the updated monitor is the selected monitor
            if (monitor == getMonitor(m_hilitedControlID))
            {
                // update the details for the monitor - in case it changed
                updateDetailsBox();

                // enable/disable the sequence control buttons if the input changed
                enableSequencePlaybackButtons(monitor);
            }

            FUNCTION_EXIT;
            return 0;
        }


        void VideoDisplayManagerDlg::buildTrainContextMenu(CMenu* menu, std::vector<VideoOutput::TrainCamera> stages)
        {
            FUNCTION_ENTRY( "buildTrainContextMenu" );

            CMenu* subMenu = menu->GetSubMenu(0);
            subMenu->RemoveMenu(0,MF_BYPOSITION);

            unsigned long previousTrainId = 0;

            std::vector<VideoOutput::TrainCamera>::iterator itr;
            int i=TRAIN_COMMAND_OFFSET;
            for(itr = stages.begin(); itr!=stages.end(); itr++, i++)
            {
                if(    previousTrainId!=0 && previousTrainId!=(*itr).trainId)
                {
                    subMenu->AppendMenu(MF_SEPARATOR, i, "");
                }
                previousTrainId=(*itr).trainId;
                std::stringstream train;
                train << "Train ";
                if((*itr).serviceNum!="")
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


        void VideoDisplayManagerDlg::onRunParamChange(const std::string& name, const std::string& value)
        {
            FUNCTION_ENTRY( "onRunParamChange" );

            CWaitCursor waitcursor;

            if ( name == RPARAM_ENTITY )
            {
                m_paramCameraAddress = value;
            }

            if ( name == RPARAM_DISPLAYMONITOR)
            { //TD19471  lichao++
                std::vector<std::string> consoleMonitors = DisplayItemManager::getInstance().getConsoleMonitors();
                std::vector<std::string>::iterator  it;
                for ( it = consoleMonitors.begin(); it != consoleMonitors.end(); it++ )
                {
                    std::string monitorName = *it;
                    if (monitorName.find(value) != std::string::npos)
                    {
                        m_paramMonitorName = monitorName;
                    }

                }

                //m_paramMonitorName = value;
            }

            //  When both runParam strings are filled then the camera-to-monitor switch
            //  can be performed.

            if ( ! (m_paramMonitorName.empty() || m_paramCameraAddress.empty() ))
            {
                Monitor* monitor = DisplayItemManager::getInstance().getMonitor(m_paramMonitorName);
                Camera* camera = DisplayItemManager::getInstance().getCameraByAddress(m_paramCameraAddress);

                CString sMsg("");

                if (monitor == NULL)
                {
                    sMsg.Format ( "Specified monitor (%s) is invalid", m_paramMonitorName.c_str());
                }
                else if (camera == NULL)
                {
                    sMsg.Format ( "Specified camera (%s) is invalid", m_paramCameraAddress.c_str());
                }

                //  This runParam switch command has been triggered, so reset the trigger strings.
                m_paramCameraAddress = "";
                m_paramMonitorName = "";

                if ( (monitor == NULL) || (camera == NULL) )
                {
                    AfxMessageBox( sMsg, MB_OK | MB_APPLMODAL | MB_ICONEXCLAMATION );

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

                    AfxMessageBox( te.what() );
                }
            }

            FUNCTION_EXIT;
        }


        // TD 16824
        LRESULT VideoDisplayManagerDlg::OnSetWindowPosition(WPARAM wParam, LPARAM lParam)
        {
            FUNCTION_ENTRY( "OnSetWindowPosition" );

            if (m_isLaunched && m_isAssignedBySchematic && (lParam == TA_Base_Bus::TA_GenericGui::REPOSITION))
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
        }


        // TD 16824


        void VideoDisplayManagerDlg::createCctvProxy()
        {
            FUNCTION_ENTRY( "createCctvProxy" );

            m_trainCctvCorbaProxy = TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainCctvCorbaProxy();

            FUNCTION_EXIT;
        }

		//resize the window, refer to PAManager
		void VideoDisplayManagerDlg::ModifyWindowPos()
		{
			
			int left,top,width,height;
			int screenWidth,currentScreen;
			int numScreens = 1;
			int x_pos;
			
			CPoint   pt;   
			GetCursorPos(&pt);   
			
			RECT boundary = TA_Base_Bus::AppLauncher::getInstance().getRect(TA_Base_Bus::TA_GenericGui::SCREEN_CURRENT, 
				TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,
				pt.x);
			x_pos = pt.x;
			screenWidth = GetSystemMetrics(SM_CXVIRTUALSCREEN);
			if(screenWidth >= MINVALUE)
				numScreens = NUMSCREENS;
			currentScreen = x_pos / (screenWidth / numScreens);
			left	= currentScreen * (screenWidth / numScreens)+1;
			width	= screenWidth / numScreens;
			top		= boundary.top;
			height	= boundary.bottom - boundary.top;
			
			this->MoveWindow(left,top,width,height,NULL);

		}
} // TA_IRS_App
