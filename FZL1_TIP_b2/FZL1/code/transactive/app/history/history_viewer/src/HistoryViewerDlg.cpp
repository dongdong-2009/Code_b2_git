/**
  * HistoryViewerDlg.cpp
  *
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/history/history_viewer/src/HistoryViewerDlg.cpp $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * The main dialog class.
  */

#include <afxpriv.h>   //For AfxLoadString

#include "app/history/history_viewer/src/stdafx.h"
#include "app/history/history_viewer/src/HistoryViewer.h"
#include "app/history/history_viewer/src/HistoryViewerDlg.h"
#include "app/history/history_viewer/src/HistoryConfig.h"
#include "app/history/history_viewer/src/DataPointInfo.h"
#include "app/history/history_viewer/src/DataPointSelect.h"
#include "app/history/history_viewer/src/DataBaseConnection.h"
#include "app/history/history_viewer/src/ModifyGraph.h"
#include "app/history/history_viewer/src/PrintTitleDlg.h"
#include "app/history/history_viewer/src/DataPointAddDlg.h"
#include "app/history/history_viewer/src/DataPointOnlineUpdateSubscriber.h"
#include "app/history/history_viewer/src/RealTimeDataUpdateSubscriber.h"
#include "app/history/history_viewer/src/GraphRefreshThread.h"
#include "app/history/history_viewer/src/globals.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/runparams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/FunctionEntry.h"

#include "core/message/types/HistoryComms_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/TrendViewerAudit_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"

#include "core/exceptions/src/UserSettingsException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/NameNotFoundException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/HistoryInitialisationException.h"

#include "bus/generic_gui/src/GenericGUI.h"
#include "bus/generic_gui/src/HelpLauncher.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/entity_access/src/HistoryViewerData.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"

#include "core/naming/src/Naming.h"

#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_library/src/SessionInfo.h"

#include "cots/ACE/5_3/ACE_wrappers/ace/OS.h"

#ifdef USE_OCCI
#include <occi.h>
#endif

// TChart includes
#include <axes.h>
#include <axis.h>
#include <axisTitle.h>
#include <axisLabels.h>
#include <series.h>
#include <valuelist.h>
#include <lineseries.h>
#include <printer.h>
#include <export.h>
#include <textexport.h>
#include <zoom.h>
#include <pen.h>
#include <titles.h>
#include <strings.h>
#include <marks.h>
#include <teefont.h>
#include <canvas.h>
#include <panel.h>

#include <pointseries.h>
#include <pointer.h>

#include <axisarrowtool.h>
#include <toollist.h>
#include <tools.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Core::EntityAccessFactory;
using TA_Core::IEntityData;
using TA_Core::DataPointEntityData;
using TA_Core::HistoryViewerData;
using TA_Core::DataException;
using TA_Core::GenericGUIException;
using TA_Core::DebugUtil;
using TA_Core::RunParams;
using TA_Core::DescriptionParameters;
using TA_Core::NameValuePair;
using TA_Bus::TransActiveDialog;

using namespace TA_Core::TrendViewerAudit;

int style = 0;

// Colours for the plot lines.  Need to have a diffent number than the
// number for stacking booleans otherwise it will repeat the same colour
// on the same stack position.
/* ORIGINAL COLOURS
const COLORREF colourList[11] = { RGB( 255,   0,   0 ),		// Red
								  RGB(   0,   0, 255 ),		// Blue
								  RGB(   0, 128,   0 ),		// Dark Green
								  RGB( 255, 160,   0 ),		// Orange
								  RGB( 128,   0, 255 ),		// Purple
								  RGB( 128, 255,   0 ),		// Light Green
								  RGB( 128, 128,   0 ),		// Ochre
								  RGB(   0,   0, 128 ),		// Dark Blue
								  RGB(   0,   0,   0 ),		// Black
    							  RGB( 255, 128, 255 ),		// Pink
								  RGB( 255, 255,   0 ) };	// Yellow
*/
const COLORREF colourList[17] = { //RGB( 255,   0,   0 ),		// Red
								  RGB(   0, 128, 255 ),		// Blue
								  RGB( 255,  64, 255 ),		// Magenta
								  RGB( 103, 159, 159 ),     // Turquoise
                                  RGB( 205, 159, 159 ),     // Beige
                                  RGB( 153,   0, 102 ),     // Maroon
                                  RGB( 159, 159, 205 ),     // Violet
                                  RGB( 255, 153, 204 ),     // Pink
                                  RGB( 255, 160,   0 ),		// Orange
								  RGB(   0, 128,   0 ),		// Dark Green
								  RGB(   0, 255,   0 ),		// A Horrible Fluro Green
								  RGB( 153,   0, 204 ),     // Purple
                                  RGB( 128,   0, 255 ),		// Dark Purple
								  RGB( 192, 192,   0 ),		// Olive
								  RGB( 128, 128, 128 ),     // Grey
                                  RGB( 128,  64,   0 ),		// Brown
								  RGB( 102, 255, 153 ),     // Green
                                  RGB(   0,   0, 192 ),		// Dark Blue
                                };

const CString LOAD_DATA = "Loading data...";
const CString VIEW_MENU = "&View";
const int FONT_SIZE = 100;
const CString FONT = "Arial Bold";
const CString DATAPOINT_TYPENAME = "DataPoint";
const std::string SAMPLE_BASE_DATA_VALUE = "0";
const long ORIGINAL_PEN_STYLE = 0;
const long NO_CUSTOM_AXIS_DISPLAYED = -1;

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
    ID_LIVE_MODE
};


namespace TA_App
{
    TA_Core::ReEntrantThreadLockable CHistoryViewerDlg::m_lock;
    TA_Core::ReEntrantThreadLockable CHistoryViewerDlg::m_toolbarLock;
    const std::string CHistoryViewerDlg::DATAPOINTS_SETTING("DataPointsSetting");

    //
    // CHistoryViewerDlg
    //
    CHistoryViewerDlg::CHistoryViewerDlg(TA_Bus::IGUIAccess* controlClass)
    : TransActiveDialog(*controlClass, CHistoryViewerDlg::IDD, NULL),
      m_UnzoomReset( true ),
      m_nIDTracking( 0 ),	        // For status messages
      m_nIDLastMessage( 0 ),
      m_graphRefreshThread(NULL),
      m_dataPointOnlineUpdateSubscriber(NULL),
      m_serverDown(false),
      m_lastTimeStamp(0),
      m_customAxisDisplayed(NO_CUSTOM_AXIS_DISPLAYED),
      m_lastSeriesClicked(0)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "CHistoryViewerDlg");

        m_sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
        RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

        //{{AFX_DATA_INIT(CHistoryViewerDlg)
        //}}AFX_DATA_INIT
        // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
        m_NowCheck = TRUE;
        m_RangeSeconds = 3600;
        m_HistoryType = -1;
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
        m_rangeDays = 0;
        m_rangeTime.SetTime(1, 0, 0);
        m_StartDateTime = COleDateTime::GetCurrentTime();

        TA_Bus::ResizingProperties properties;
        properties.canMaximise = true;
        properties.maxHeight = -1;
        properties.maxWidth = -1;
        properties.minHeight = 400;
        properties.minWidth = 850;
        setResizingProperties(properties);

        m_GraphDataPointList.clear();

        m_dataPointOnlineUpdateSubscriber = new DataPointOnlineUpdateSubscriber(this);

        m_dataPointLoaderThread = new DataPointLoaderThread;

        m_auditSender = TA_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Core::TrendViewerAudit::Context );

        TA_Bus::AuthenticationLibrary authentication(true);
        TA_Bus::SessionInfo sessionInfo;
        try
        {
            sessionInfo = authentication.getSessionInfo(m_sessionId, m_sessionId);
        }
        catch(const TA_Core::AuthenticationAgentException& ex)
        {
            CString actionName, reason;
            actionName.LoadString(IDS_INITIALISE);
            reason.LoadString(IDS_SESSION_INFO_ERROR);
            TA_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120023);
            std::string msg("Error retrieving session information from Authentication Agent: ");
            msg += ex.what();
            TA_THROW(TA_Core::HistoryInitialisationException(msg));
        }
        catch(const TA_Core::ObjectResolutionException& ex)
        {
            CString actionName, agentName;
            actionName.LoadString(IDS_INITIALISE);
            agentName.LoadString(IDS_AUTHENTICATION_AGENT);
            TA_Bus::TransActiveMessage userMsg;
            userMsg << actionName << agentName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030064);
            std::string msg("Error retrieving session information from Authentication Agent: ");
            msg += ex.what();
            TA_THROW(TA_Core::HistoryInitialisationException(msg));
        }
        catch(...)
        {
            CString actionName;
            actionName.LoadString(IDS_INITIALISE);
            TA_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_250001);
            std::string msg("Unknown exception caught while retrieving session information from Authentication Agent.");
            TA_THROW(TA_Core::HistoryInitialisationException(msg));
        }

        char buffer[256];
        sprintf(buffer, "%lu", sessionInfo.UserId);
        m_operatorId = buffer;
        m_operatorName = getOperatorName(sessionInfo.UserId);

        LOG(SourceInfo, DebugUtil::FunctionExit, "CHistoryViewerDlg");
    }


    //
    // DoDataExchange
    //
    void CHistoryViewerDlg::DoDataExchange(CDataExchange* pDX)
    {
        TransActiveDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CHistoryViewerDlg)
        DDX_Control(pDX, IDC_TCHART2, m_ChartCtrl);
        //}}AFX_DATA_MAP

        //
        // Manually exchange data with controls on the toolbar
        //
        if( pDX->m_bSaveAndValidate )
        {
	        // Get the history control type
	        m_HistoryType = m_toolBar.m_HistoryTypeCtrl.GetCurSel();

	        // Get the range value
	        //m_RangeSeconds = m_toolBar.m_RangeCtrl.Get_TotalSeconds();

            // Set m_RangeSeconds
            calculateRangeSeconds();

            //
            // Note: History type must be processed before range verification
            //
	        DDV_NoRange(pDX, m_RangeSeconds);	            	// Check for no range
	        int histConfig = getDataTypeDisplayed(m_HistoryType);
	        DDV_LargeRange( pDX, m_RangeSeconds, histConfig);	// Check for range too big

            //
	        // Save and validate data depending on the Now button
            //
	        m_NowCheck = m_toolBar.GetToolBarCtrl().IsButtonChecked( ID_NOW );
	        if( m_NowCheck )
	        {
		        SetStartDateTime();
	        }
		        else
	        {
		        m_toolBar.m_StartDateTimeCtrl.GetTime( m_StartDateTime );

                //
		        // If saving and not in live mode, clear the seconds.
                //
		        int years = m_StartDateTime.GetYear();
		        int months = m_StartDateTime.GetMonth();
		        int days = m_StartDateTime.GetDay();
		        int hours = m_StartDateTime.GetHour();
		        int minutes = m_StartDateTime.GetMinute();

                m_StartDateTime.SetDateTime( years, months, days, hours, minutes, 0 );
			    m_toolBar.m_StartDateTimeCtrl.SetTime( m_StartDateTime );
            }
        }
        else
        {
            //
	        // Set the range value
            //
	        //m_toolBar.m_RangeCtrl.Set_TotalSeconds( m_RangeSeconds );
            m_toolBar.m_rangeTimeCtrl.SetTime( m_rangeTime );
            //m_toolBar.m_rangeDayCtrl.SetWindowText( ConvertIntToString(m_rangeDays).c_str() );
            COleDateTime temp;
            temp.SetTime(0, m_rangeDays, 0);
            m_toolBar.m_rangeDayCtrl.SetTime(temp);

            //
	        // Set the history data type
            //
	        m_toolBar.m_HistoryTypeCtrl.SetCurSel( m_HistoryType );

            //
	        // Set the now button
            //
	        m_toolBar.GetToolBarCtrl().CheckButton( ID_NOW, m_NowCheck );
	        m_toolBar.SetNowBitmap();

            //
	        // Set the start date/time
            //
	        m_toolBar.m_StartDateTimeCtrl.SetTime( m_StartDateTime );
        }

        SetStatusLiveMode();
    }

    BEGIN_MESSAGE_MAP(CHistoryViewerDlg, TransActiveDialog)
        //{{AFX_MSG_MAP(CHistoryViewerDlg)
        ON_WM_PAINT()
        ON_WM_QUERYDRAGICON()
        ON_BN_CLICKED(IDC_APPLYBUTTON, OnApply)
        ON_COMMAND(ID_NOW, OnNowCheck)
        ON_COMMAND(ID_VIEW_NOW, OnToggleNowCheck)
        ON_COMMAND(ID_CURRENT_TIME, OnViewCurrentTime)
        ON_COMMAND(ID_DATAPOINT_ADD, OnDataPointAdd)
        ON_COMMAND(ID_DATAPOINT_DELETE, OnDataPointDelete)
        ON_WM_SIZE()
        ON_COMMAND(ID_PRINT_GRAPH, OnPrintGraph)
        ON_COMMAND(ID_EXPORT, OnExport)
        ON_WM_CLOSE()
        ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
        ON_COMMAND(ID_HELP, OnHelp)
        ON_WM_CREATE()
        ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
        ON_MESSAGE(WM_POPMESSAGESTRING, OnPopMessageString)
        ON_WM_ENTERIDLE()
        ON_WM_INITMENUPOPUP()
        ON_WM_MENUSELECT()
        ON_COMMAND(ID_VIEW_MODIFY, OnViewModify)
        ON_WM_DESTROY()
        ON_WM_SYSCOMMAND()
        ON_COMMAND(ID_APPLY_NOW, OnApply)
	    //}}AFX_MSG_MAP
        ON_MESSAGE(WM_HISTORY_UPDATE, OnHistoryUpdate)
        ON_MESSAGE(WM_LOADING_COMPLETE, OnLoadingComplete)
        // Tool tip text
        ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
        ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
        // Standard View menu options
        ON_COMMAND(ID_VIEW_STATUS_BAR, OnStatusBarCheck)
        ON_COMMAND(ID_VIEW_TOOLBAR, OnToolBarCheck)
        ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateStatusBarMenu)
        ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateToolBarMenu)
    END_MESSAGE_MAP()


    //
    // Need to implement this to avoid application shutting down whenever
    // the user hits enter.
    //
    void CHistoryViewerDlg::OnOK()
    {
        //
        // Make the enter key apply
        //
        OnApply();
    }


    //
    // OnInitDialog
    //
    BOOL CHistoryViewerDlg::OnInitDialog()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "OnInitDialog");

        TransActiveDialog::OnInitDialog();
        short	shBtnColor = 30;

        //
        // Fix font on the data type pull down
        //
        m_toolBar.m_HistoryTypeCtrl.SetFont(GetFont());
        m_toolBar.m_StartDateTimeCtrl.SetFont(GetFont());
        m_toolBar.m_rangeTimeCtrl.SetFont(GetFont());
        m_toolBar.m_rangeDayCtrl.SetFont(GetFont());

        //
        // Set the icon for this dialog.  The framework does this automatically
        // when the application's main window is not a dialog
        //
        SetIcon(m_hIcon, TRUE);			// Set big icon
        SetIcon(m_hIcon, FALSE);		// Set small icon

        //
        // Attempt to open the database connection. This is the only point where
        // the error message will be displayed.
        //
    #ifdef USE_OCCI
        try
        {
	        DataBaseConnection::getInstance().CreateConnection();
        }
        catch ( oracle::occi::SQLException &sqlException )
        {
            TA_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010001);
        }
        catch ( DataBaseConnection::CouldNotConnectException )
        {
            TA_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }
    #else // USE_OCCI
        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugInfo, "CHistoryViewerDlg::OnInitDialog - Create SimpleDb Connection");
        try
        {
	        SimpleDBConnection::getInstance().CreateConnection();
        }
        catch ( SimpleDBConnection::CouldNotConnectException )
        {
            TA_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }
    #endif // USE_OCCI

        LoadHistoryConfig();
        LoadDataPoints();
        ParseParameters();
        SetupGraph();
        m_toolBar.DisableApply();

        restoreUserSettings();

        m_rightMarginInitial = m_ChartCtrl.GetPanel().GetMarginRight();

        // TD 2924: Send a "Size" message to resize the manager. Forcing a resize will
        // force the toolbar to display.
		this->PostMessage(WM_SIZE, 0, 0);

        LOG(SourceInfo, DebugUtil::FunctionExit, "OnInitDialog");

        return TRUE;
    }


    //
    // onRunParamChange
    //
    void CHistoryViewerDlg::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY("onRunParamChange");

        if(name == RPARAM_SESSIONID)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Session ID changed");
            m_sessionId = value;
        }

        FUNCTION_EXIT;
    }


    //
    // If you add a minimize button to your dialog, you will need the code below
    // to draw the icon.  For MFC applications using the document/view model,
    // this is automatically done for you by the framework.
    //
    void CHistoryViewerDlg::OnPaint()
    {
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
    }

    //
    // The system calls this to obtain the cursor to display while the user drags
    // the minimized window.
    //
    HCURSOR CHistoryViewerDlg::OnQueryDragIcon()
    {
        return (HCURSOR) m_hIcon;
    }


    //
    // SetStartDateTime
    //
    void CHistoryViewerDlg::SetStartDateTime()
    {
        COleDateTimeSpan timeDiff( (double)m_RangeSeconds/(double)secInDay );
        m_StartDateTime = COleDateTime::GetCurrentTime() - timeDiff;
        m_toolBar.m_StartDateTimeCtrl.SetTime( m_StartDateTime );
    }


    //
    // SetStartDateTime
    //
    void CHistoryViewerDlg::SetStartDateTime( COleDateTime startDateTime )
    {
        m_StartDateTime = startDateTime;
        m_toolBar.m_StartDateTimeCtrl.SetTime( m_StartDateTime );
    }


    //
    // SetEndDateTime
    //
    void CHistoryViewerDlg::SetEndDateTime( const COleDateTime& endDateTime )
    {
        COleDateTimeSpan timeDiff( (double)m_RangeSeconds/(double)secInDay );
        m_StartDateTime = endDateTime - timeDiff;
        m_toolBar.m_StartDateTimeCtrl.SetTime( m_StartDateTime );
    }


    //
    // OnViewCurrentTime
    //
    void CHistoryViewerDlg::OnViewCurrentTime()
    {
        //
	    // Get the range value (in case they haven't applied yet)
        //
	    calculateRangeSeconds();

        //
	    // If range is zero or too large, abort function
        //
	    int histConfig = getDataTypeDisplayed(m_HistoryType);
	    if( NoRange( m_RangeSeconds ) || LargeRange( m_RangeSeconds, histConfig ) )
	    {
            return;
	    }

	    UnzoomNoReset();

        //
	    // Move the graph to the current time
        //
	    SetStartDateTime();
        SetGraphFromValues();

        m_toolBar.DisableNow();
        //SetMenuItemCheck(VIEW_MENU, ID_VIEW_NOW, m_NowCheck);
    }


    //
    // OnApply
    //
    void CHistoryViewerDlg::OnApply()
    {
        if( UpdateData( TRUE ) == false )
        {
	        return;
        }

        displayCurrentTrendDataWarning();
        ApplyChanges();
    }


    //
    // ApplyChanges
    //
    void CHistoryViewerDlg::ApplyChanges()
    {
        //
        // Unzoom the graph (if zoomed) without reseting start and range
        //
        UnzoomNoReset();

        //
        // Set the graph start and range
        //
        SetGraphFromValues();

        //
        // Reset icon back to normal state
        //
        m_toolBar.DisableApply();

        //
        // Send an audit message that display has been modified
        //
        DescriptionParameters desc;

        int histConfig = getDataTypeDisplayed(m_HistoryType);
        HistConfigMap hcMap = HistoryConfig::getInstance().GetHistConfigMap();
        std::map <unsigned long, HistConfig> :: iterator it;
        it = hcMap.find(histConfig);
        NameValuePair param1("DataType", it->second.shortDesc);
        desc.push_back(&param1);

        CString startTime = m_StartDateTime.Format( "%d/%m/%Y %H:%M:%S" );
        NameValuePair param2("StartTime", startTime.GetBuffer(0));
        desc.push_back(&param2);

        NameValuePair param3("RangeDays", ConvertIntToString(m_rangeDays));
        desc.push_back(&param3);

        CString rangeTime = m_rangeTime.Format( "%d/%m/%Y %H:%M:%S" );
        NameValuePair param4("RangeTime", rangeTime.GetBuffer(0));
        desc.push_back(&param4);

        sendAuditMessage(TA_Core::TrendViewerAudit::TrendViewerModifyDisplay, desc);
    }


    //
    // OnNowCheck
    //
    void CHistoryViewerDlg::OnNowCheck()
    {
        m_toolBar.SetNowBitmap();
        if( (m_NowCheck = m_toolBar.GetToolBarCtrl().IsButtonChecked( ID_NOW )) )
        {
            //
	        // It is now live so disable the start time
            //
	        m_toolBar.m_StartDateTimeCtrl.EnableWindow( FALSE );

            //
	        // Get the range value (in case they haven't applied yet)
            //
	        //m_RangeSeconds = m_toolBar.m_RangeCtrl.Get_TotalSeconds();
            calculateRangeSeconds();

            //
	        // If range is zero or too large, abort function
            //
	        int histConfig = getDataTypeDisplayed(m_HistoryType);
	        if( NoRange( m_RangeSeconds ) || LargeRange( m_RangeSeconds, histConfig ) )
	        {
                //
		        // Turn button off again
                //
		        m_NowCheck = false;
		        m_toolBar.GetToolBarCtrl().CheckButton( ID_NOW, m_NowCheck );
		        m_toolBar.SetNowBitmap();
		        return;
	        }

	        UnzoomNoReset();

            //
	        // Move the graph to the current time
            //
	        SetStartDateTime();
            SetGraphFromValues();

            //
            // Subscribe to messages so the trends can do real time updates
            //
            std::vector<RealTimeDataUpdateSubscriber*>::iterator it;
            it = m_realTimeDataUpdateSubscriberList.begin();
            for(; it != m_realTimeDataUpdateSubscriberList.end(); ++it)
            {
                (*it)->subscribeToMessage();
            }

            //
            // Start the refresh thread
            //
            if(!m_GraphDataPointList.empty())
            {
                m_graphRefreshThread = new GraphRefreshThread(this);
                m_graphRefreshThread->start();
            }

            m_toolBar.DisableNow();

            //
            // Send an audit message operator has changed to Live mode
            //

            DescriptionParameters desc;

            sendAuditMessage(TA_Core::TrendViewerAudit::TrendViewerSetLive, desc);
        }
        else
        {
            //
	        // It is no longer live so enable the start time
            //
	        m_toolBar.m_StartDateTimeCtrl.EnableWindow( TRUE );

            //
            // Unsubcribe to messages as trends no longer need to do real time updates
            //
            std::vector<RealTimeDataUpdateSubscriber*>::iterator it;
            it = m_realTimeDataUpdateSubscriberList.begin();
            for(; it != m_realTimeDataUpdateSubscriberList.end(); ++it)
            {
                (*it)->unsubscribeToMessage();
            }

            //
            // Stop the refresh thread
            //
            if(m_graphRefreshThread != NULL)
            {
                if(!m_graphRefreshThread->isTerminated())
                {
                    m_graphRefreshThread->terminate();
                    delete m_graphRefreshThread;
                    m_graphRefreshThread = NULL;
                }
            }

            //
            // Check if there are any Current Trend Datapoint currently displayed. If so,
            // display a warning that these trends will no longer be updated
            //
            int histConfig = getDataTypeDisplayed(m_HistoryType);
            HistConfigMap hcMap = HistoryConfig::getInstance().GetHistConfigMap();
            std::map <unsigned long, HistConfig> :: iterator hcIt;
            hcIt = hcMap.find(histConfig);

            DataPointMap dpMap = CDataPointInfo::getInstance().GetGraphingDataPointMap();
            DataPointMap::iterator dpMapIt = dpMap.begin();
            for(; dpMapIt != dpMap.end(); ++dpMapIt )
	        {
		        if(!dpMapIt->second.trendingEnabled && (hcIt->second).baseData == SAMPLE_BASE_DATA_VALUE)
                {
                    TA_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UW_120009);
                    break;
                }
	        }

            //
            // Send an audit message operator has changed to non-Live mode
            //

            DescriptionParameters desc;

            sendAuditMessage(TA_Core::TrendViewerAudit::TrendViewerUnsetLive, desc);
        }

        SetStatusLiveMode();
        SetMenuItemCheck(VIEW_MENU, ID_VIEW_NOW, m_NowCheck);

    }


    //
    // Toggles the now button
    //
    void CHistoryViewerDlg::OnToggleNowCheck()
    {
        m_toolBar.GetToolBarCtrl().CheckButton( ID_NOW, !m_NowCheck );

        OnNowCheck();
    }


    //
    // LoadHistoryConfig
    //
    void CHistoryViewerDlg::LoadHistoryConfig()
    {
        try
        {
	        HistoryConfig::getInstance().LoadConfig();
        }
        catch ( HistoryConfig::CouldNotLoadException e )
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                "Could not load the History Configuration.");
	        CString errorMsg;
	        errorMsg.Format( "Could not load the History Configuration.\n%s",
			    e.GetErrorMsg().c_str() );
	        AfxMessageBox( errorMsg, MB_OK | MB_ICONEXCLAMATION );
	        return;
        }

        HistoryConfig::getInstance().AddShortDesc( m_toolBar.m_HistoryTypeCtrl );
        if( m_toolBar.m_HistoryTypeCtrl.GetCount() > 0 )
        {
	        m_toolBar.m_HistoryTypeCtrl.SetCurSel(0);
	        m_HistoryType = m_toolBar.m_HistoryTypeCtrl.GetCurSel();
        }
    }


    //
    // LoadDataPoints
    //
    void CHistoryViewerDlg::LoadDataPoints()
    {
        try
        {
	        //CDataPointInfo::getInstance().LoadDataPoints();
            m_dataPointLoaderThread->start();
        }
        catch ( CDataPointInfo::CouldNotLoadException e )
        {
	        CString errorMsg;
	        errorMsg.Format( "Could not load the datapoints.\n%s",
		        e.GetErrorMsg().c_str() );
	        AfxMessageBox( errorMsg, MB_OK | MB_ICONEXCLAMATION );
	        return;
        }

        // Disable option to add datapoints until loading has finished
        m_toolBar.DisableAdd();

        CMenu* topMenu = GetMenu();
        if( topMenu == NULL )
        {
            return;
        }

        CMenu* subMenu = topMenu->GetSubMenu( FindMenuItem( topMenu, "&Edit" ) );
        subMenu->EnableMenuItem(ID_DATAPOINT_ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

        OnSetMessageString(ID_LOADING_STATUS_BAR);
    }


    LRESULT CHistoryViewerDlg::OnLoadingComplete( WPARAM wParam, LPARAM lParam )
    {
        FUNCTION_ENTRY("OnLoadingComplete");

        // Still have to restore user settings - this takes a while...
        restoreUserSettings();

        // Enable add datapoint options
        m_toolBar.EnableAdd();

        CMenu* topMenu = GetMenu();
        if( topMenu == NULL )
        {
            return 0;
        }

        CMenu* subMenu = topMenu->GetSubMenu( FindMenuItem( topMenu, "&Edit" ) );
        subMenu->EnableMenuItem(ID_DATAPOINT_ADD, MF_BYCOMMAND | MF_ENABLED);

        OnSetMessageString(AFX_IDS_IDLEMESSAGE);

        return 0;
    }


    //
    // ParseParameters
    //
    // Get the parameters and use them to set the plot parameters
    // --data-points=23232,33523 --graph-type=23252 --range=0,01:00 --start-time=24/03/2002,01:00
    //
    void CHistoryViewerDlg::ParseParameters()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "ParseParameters");

        //
        // Parse graph type (Representation entity)
        //
        CString graphType = RunParams::getInstance().get("GraphType").c_str();
        if( graphType.GetLength() > 0 )
        {
            //
	        // Convert text to the entity key
            //
	        unsigned long entityKey = atoi( graphType );

            //
	        // Search through the entity key's to find the correct
            //
	        for( int i=0; i<m_toolBar.m_HistoryTypeCtrl.GetCount(); i++ )
	        {
		        if(	getDataTypeDisplayed( i ) == static_cast<long>(entityKey) )
		        {
			        m_toolBar.m_HistoryTypeCtrl.SetCurSel( i );
			        m_HistoryType = m_toolBar.m_HistoryTypeCtrl.GetCurSel();
			        break;
		        }
	        }
        }

        //
        // Parse range parameter
        //
        CString range = RunParams::getInstance().get("Range").c_str();
        if( range.GetLength() > 0 )
        {
            //
	        // Get the day range (comma marking the end)
            //
	        int commaPosition = range.Find( ',' );
	        int dayRange = abs( atoi( range.Left( commaPosition ) ) );
	        range = range.Right( range.GetLength() - (commaPosition+1) );
	        if( dayRange > 730 || dayRange < 0 )
		        dayRange = 0;

            //
	        // Get the hours in range (colon marking the end)
            //
	        int colonPosition = range.Find( ':' );
	        int hourRange = abs( atoi( range.Left( colonPosition ) ) );
	        range = range.Right( range.GetLength() - (colonPosition+1) );
	        if( hourRange > 23 || hourRange < 0 )
		        hourRange = 0;

            //
	        // Get the minutes in range - should be all that is left
            //
	        int minRange = abs( atoi( range ) );
	        if( minRange > 59 || minRange < 0 )
		        minRange = 0;

            //
	        // Set the range in seconds
            //
	        m_RangeSeconds = secInDay*dayRange + 60*60*hourRange + 60*minRange;
	        if( m_RangeSeconds <= 0 )
	        {
		        m_RangeSeconds = 3600; // Default to an hour
	        }

            //
	        // Check that the range is not too large
            //
	        long numSamples = GetNumSamples( m_RangeSeconds );
	        if( numSamples > sampleLimit )
	        {
                //
		        // Range results in too many sample.
                //
                TA_Bus::TransActiveMessage userMsg;
                userMsg << numSamples << sampleLimit;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_120014);

		        int histConfig = getDataTypeDisplayed(m_HistoryType);

                //
		        // Make it at least 1 to avoid any divide by zero errors.
                //
		        long samplePeriod = __max( 1, HistoryConfig::getInstance().GetSamplePeriod( histConfig ) );
		        m_RangeSeconds = sampleLimit*samplePeriod;
	        }

	        //m_toolBar.m_RangeCtrl.Set_TotalSeconds( m_RangeSeconds );
            m_toolBar.m_rangeTimeCtrl.SetTime( m_rangeTime );
            //m_toolBar.m_rangeDayCtrl.SetWindowText( ConvertIntToString(m_rangeDays).c_str() );

            COleDateTime temp;
            temp.SetTime(0, m_rangeDays, 0);
            m_toolBar.m_rangeDayCtrl.SetTime(temp);
        }

        //
        // Parse start time
        // NOTE: If we get a valid start date/time, we know the graph is not live.
        //       That is, if they specify the start of the graph we are looking
        //       at historic values not live values.
        //
        CString startTime = RunParams::getInstance().get("StartTime").c_str();
        if( startTime.GetLength() > 0 )
        {
	        COleDateTime startDateTime;

	        startTime.Replace( ',', ' ' );	// Replace comma with space
	        startTime += ":00";				// Add seconds

            //
	        // Parse the date and time
            //
	        if( startDateTime.ParseDateTime( startTime ) )
	        {
                //
		        // If successful, turn off "Now" and set start date/time
                //
		        m_NowCheck = FALSE;
		        m_toolBar.GetToolBarCtrl().CheckButton( ID_NOW, m_NowCheck );
		        m_toolBar.SetNowBitmap();
		        SetStartDateTime( startDateTime );
		        m_toolBar.m_StartDateTimeCtrl.EnableWindow( TRUE );
		        SetStatusLiveMode();
	        }
        }
        else
        {
	        SetStartDateTime();
        }

        //
        // Parse datapoints --data-Points
        //
        CString dataPoints = RunParams::getInstance().get("DataPoints").c_str();
        if( dataPoints.GetLength() > 0 )
        {
            //
	        // First split the the list of names up
            //
	        std::vector <unsigned long> dataPointKeyList;
	        unsigned long dataPointKey;
	        int commaPosition = 0;
	        while( (commaPosition = dataPoints.Find( ',' )) >= 0 )
	        {
		        dataPointKey = atoi( dataPoints.Left( commaPosition ) );
		        dataPoints = dataPoints.Right( dataPoints.GetLength() - (commaPosition+1) );
		        if( dataPointKey > 0 && !isDataPointAlreadyDisplayed(dataPointKey))
		        {
			        dataPointKeyList.push_back( dataPointKey );
		        }
	        }

            //
	        // Add the last one
            //
	        dataPointKey = atoi( dataPoints );

	        if( dataPointKey > 0 && !isDataPointAlreadyDisplayed(dataPointKey))
	        {
		        dataPointKeyList.push_back( dataPointKey );
	        }

            //
	        // Add the datapoint to the graph
            //
	        if( dataPointKeyList.size() > 0 )
	        {
		        AddToGraph( dataPointKeyList );
	        }
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "ParseParameters");
    }


    //
    // isDataPointAlreadyDisplayed
    //
    bool CHistoryViewerDlg::isDataPointAlreadyDisplayed(unsigned long entityKey)
    {
        std::vector <GraphDataPoint>::iterator iter = m_GraphDataPointList.begin();

        for(; iter != m_GraphDataPointList.end(); ++iter)
        {
            if(iter->GetEntityKey() == static_cast<long>(entityKey))
            {
                return true;
            }
        }

        return false;
    }


    //
    // SetupGraph
    //
    void CHistoryViewerDlg::SetupGraph()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "SetupGraph");

        //
        // Add a dummy series so that the graph lines show
        //
        long seriesNum = m_ChartCtrl.AddSeries( 0 ); // 0 for line graph
        m_ChartCtrl.Series( seriesNum ).GetXValues().SetDateTime( TRUE );
        m_ChartCtrl.Series( seriesNum ).SetShowInLegend( false );
        //m_ChartCtrl.GetHeader().SetVisible(FALSE);

        // TD 2756: Clicking the right arrow will now increase the time and left
        // arrow will decrease the time
        CToolList toolList = m_ChartCtrl.GetTools();
        CAxisArrowTool arrowTool = toolList.GetItems(1).GetAsAxisArrow();
        arrowTool.SetScrollPercent(-25);


        //
        // Set the times on the X-Axis
        //
        SetGraphFromValues();

        LOG(SourceInfo, DebugUtil::FunctionExit, "SetupGraph");
    }


    //
    // DDV_NoRange
    //
    void CHistoryViewerDlg::DDV_NoRange(CDataExchange* pDX, long rangeSeconds)
    {
        //
        // Only check when saving
        //
        if( pDX->m_bSaveAndValidate )
        {
            //
	        // If no range, then error!
            //
	        if( NoRange(rangeSeconds) )
	        {
		        pDX->Fail();
	        }
        }
    }


    //
    // NoRange
    //
    bool CHistoryViewerDlg::NoRange( const long& rangeSeconds )
    {
        if (rangeSeconds == 0)
        {
            TA_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120020);
	        return true;
        }
        return false;
    }


    //
    // DDV_LargeRange
    //
    void CHistoryViewerDlg::DDV_LargeRange(CDataExchange* pDX, long rangeSeconds, int histConfig)
    {
        if (pDX->m_bSaveAndValidate)
        {
            //
	        // Test for range being too big
            //
	        if( LargeRange(rangeSeconds, histConfig) )
	        {
		        pDX->Fail();
	        }
        }
    }


    //
    // LargeRange
    //
    bool CHistoryViewerDlg::LargeRange( const long& rangeSeconds, int histConfig )
    {
        //
        // Using the history data type, calculate the number of samepls
        // Make it at least 1 to avoid any divide by zero errors.
        //
        long samplePeriod = __max( 1, HistoryConfig::getInstance().GetSamplePeriod( histConfig ) );
        long numSamples = (long)(rangeSeconds/samplePeriod);

        //
        // If no range, then error!
        //
        if (numSamples > sampleLimit)
        {
            TA_Bus::TransActiveMessage userMsg;
            userMsg << numSamples << sampleLimit;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120014);
	        return true;
        }
        else if (numSamples > sampleWarning)
        {
            TA_Bus::TransActiveMessage userMsg;
            userMsg << numSamples;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_120010);

	        if( selectedButton == IDNO )
	        {
		        return true;
	        }
        }
        return false;
    }


    //
    // OnDataPointAdd
    //
    void CHistoryViewerDlg::OnDataPointAdd()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "OnDataPointAdd");

        CDataPointAddDlg dpAddDlg;
        dpAddDlg.DoModal();

        /*
        //CDataPointSelect DPSelect( false, this );

        //if( DPSelect.DoModal() == IDOK )
        //{
	    //    std::vector <unsigned long> dataPointKeyList = DPSelect.GetDataPointKeyList();
        std::vector <unsigned long> dataPointKeyList = dpAddDlg.GetDataPointKeyList();

            //
            // Add the datapoint to the graph
            //
	        if( dataPointKeyList.size() > 0 )
	        {
                //
                // Check if there are any trends displayed in the graph before adding
                // the new ones. If there aren't then the graph refresh thread is not
                // running. Start it now.
                //
                if(m_NowCheck && m_GraphDataPointList.empty())
                {
                    m_graphRefreshThread = new GraphRefreshThread(this);
                    m_graphRefreshThread->start();
                }

		        AddToGraph( dataPointKeyList );
	        }
        //}*/

        LOG(SourceInfo, DebugUtil::FunctionExit, "OnDataPointAdd");
    }


    //
    // addDataPoint
    //
    void CHistoryViewerDlg::addDataPoint(unsigned long dataPointKey)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "addDataPoint");

        //
        // Check if there are any trends displayed in the graph before adding
        // the new ones. If there aren't then the graph refresh thread is not
        // running. Start it now.
        //
        if(m_NowCheck && m_GraphDataPointList.empty())
        {
            m_graphRefreshThread = new GraphRefreshThread(this);
            m_graphRefreshThread->start();
        }

        std::vector <unsigned long> dataPointKeyList;
        dataPointKeyList.push_back(dataPointKey);

		AddToGraph( dataPointKeyList );

        LOG(SourceInfo, DebugUtil::FunctionExit, "addDataPoint");
    }


    //
    // OnDataPointDelete
    //
    void CHistoryViewerDlg::OnDataPointDelete()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "OnDataPointDelete");

        CDataPointSelect DPSelect( true, this );

        if( DPSelect.DoModal() == IDOK )
        {
	        std::vector <unsigned long> dataPointKeyList = DPSelect.GetDataPointKeyList();

            //
	        // Delete the datapoint(s) from the graph
            //
	        if( dataPointKeyList.size() > 0 )
	        {
		        DeleteFromGraph( dataPointKeyList );

                //
                // Check if there are anymore trends in the graph. If not, then
                // stop the refresh thread.
                //
                if(m_NowCheck && m_GraphDataPointList.empty())
                {
                    m_graphRefreshThread->terminateAndWait();
                    delete m_graphRefreshThread;
                    m_graphRefreshThread = NULL;
                }
	        }
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "OnDataPointDelete");
    }


    //
    // AddToGraph
    //
    void CHistoryViewerDlg::AddToGraph( std::vector <unsigned long> &dataPointKeyList )
    {
        FUNCTION_ENTRY("AddToGraph");

        //
        // Return if nothing to add
        //
        if( dataPointKeyList.size() == 0 )
        {
	        return;
        }

        static int colourIndex = -1;

        BeginWaitCursor();
        OnSetMessageString( ID_LOADING_DATA );

        //
        // Move data to plotted list
        //
        try
        {
            CDataPointInfo::getInstance().AddToGraph( dataPointKeyList );
        }
        catch(CDataPointInfo::CouldNotLoadException& ex)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "CouldNotLoadException", ex.GetErrorMsg().c_str());

            std::string msg("Cannot not add data point to graph: ");
            msg += ex.GetErrorMsg();
            AfxMessageBox(msg.c_str());
            return;
        }

        //
        // Calculate the end time from the start and the range
        //
        COleDateTime endTime = GetEndTime();

        //
        // Go through the list of datapoints and add them to the graph
        //
        std::vector <unsigned long>::iterator it;
        DataPoint dataPoint;
        for ( it = dataPointKeyList.begin(); it != dataPointKeyList.end(); it++ )
        {
	        try
	        {
		        dataPoint = CDataPointInfo::getInstance().GetDataPoint( *it );
	        }
	        catch ( CDataPointInfo::DatapointNotFoundException )
	        {
		        continue;
	        }

            //
	        // Add the series to the plot
            //
	        long seriesNum = m_ChartCtrl.AddSeries( 0 );
	        CSeries thisSeries = m_ChartCtrl.Series( seriesNum );
	        thisSeries.GetAsLine().GetLinePen().SetWidth( 2 );
            thisSeries.GetAsLine().GetLinePen().SetStyle(ORIGINAL_PEN_STYLE);
            thisSeries.SetTitle( dataPoint.entityName.c_str() );
            thisSeries.GetXValues().SetDateTime( TRUE );

            thisSeries.GetAsLine().GetPointer().SetVisible(TRUE);
            thisSeries.GetAsLine().GetPointer().SetStyle(1);
            thisSeries.GetAsLine().GetPointer().SetVerticalSize(3);
            thisSeries.GetAsLine().GetPointer().SetHorizontalSize(3);
            thisSeries.GetAsLine().SetColorEachLine(FALSE);
            thisSeries.GetAsLine().GetPointer().GetPen().SetVisible(FALSE);
            thisSeries.GetAsLine().SetClickableLine(TRUE);

            //
	        // Line colour
            //
            if( ++colourIndex >= (sizeof(colourList)/sizeof(COLORREF)) )
	        {
		        colourIndex = 0;
	        }
	        thisSeries.SetColor( colourList[colourIndex] );

            //
			// If this is a boolean, make it a stairs display
            //
	        if( dataPoint.dpType == DP_BOOLEAN_TYPE )
	        {
		        thisSeries.GetAsLine().SetStairs( true );
	        }

            //
	        // Load the data if it has trending enabled
            //
	        GraphDataPoint graphDP( *it, thisSeries, seriesNum, dataPoint.engLow, dataPoint.engHigh, dataPoint.scaleFactor );
            if(dataPoint.trendingEnabled)
            {
	            try
	            {
                    int histConfig = getDataTypeDisplayed(m_HistoryType);
                    graphDP.LoadData( histConfig, m_StartDateTime, endTime );
	            }
	            catch( GraphDataPoint::CouldNotLoadException e )
	            {
		            CString errorMsg;
		            errorMsg.Format( "Error loading data:\n%s",
						             e.GetErrorMsg().c_str() );
		            AfxMessageBox( errorMsg, MB_OK | MB_ICONEXCLAMATION );
	            }
            }
            else
            {
                //
                // Trending isn't enabled so this is a Current Trend Data plot.
                // Try to resolve the datapoint to find the current value - we need
                // to do this incase the datapoint doesn't change value for a while.
                // If we don't have a value and wait until the datapoint changes value
                // before a point is plotted for it, we could be waiting a while
                // before something is shown on the graph
                //

                TA_Core::CorbaName corbaName = EntityAccessFactory::getInstance().getCorbaNameOfEntity( *it );

                DataPointNamedObj dataPointNamedObj( corbaName.getAgentName(), corbaName.getObjectName());
                //TA_Bus::DataPointCorbaDef::DataPointState* dpState = NULL;
                TA_Bus::DataPointCorbaDef::ClientDataPointState* dpState = NULL;

                try
                {
					CORBA_CALL_RETURN( dpState,
					                   dataPointNamedObj,
					                   GetDataPointState,
					                   () );

                    switch ( dpState->value._d() )
	                {
		                case TA_Bus::DataPointCorbaDef::dpvFloat:
			            {
				            graphDP.setLatestValue((double) dpState->value.floatValue());
				            break;
			            }
		                case TA_Bus::DataPointCorbaDef::dpvBoolean:
			            {
				            if((bool) dpState->value.booleanValue())
                            {
                                graphDP.setLatestValue((double) 1);
                            }
                            else
                            {
                                graphDP.setLatestValue((double) 0);
                            }
				            break;
			            }
		                case TA_Bus::DataPointCorbaDef::dpvText:
			            {
				            // Shouldn't go in here because we don't trend text datapoints
				            return;
			            }
		                default:
			            {
				            return;
			            }
                    }
                }
                catch ( const TA_Core::ObjectResolutionException& )
                {
                    // TODO: Need to set bad quality flag
                    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                        "Could not resolve for Current Trend Data: %s", corbaName.getObjectName());
                    graphDP.setLatestValue(0);
                    time_t timestamp;
                    time(&timestamp);
                    graphDP.updatePlot(timestamp, 0, STATUS_UNKNOWN, GraphDataPoint::COMMS_ERROR);
                }
                catch(...)
                {
                    //TA_THROW(TA_Core::NameNotFoundException(corbaName));

                    // TODO: Need to set bad quality flag
                    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                        "Could not resolve for Current Trend Data: %s", corbaName.getObjectName());
                    graphDP.setLatestValue(0);
                    time_t timestamp;
                    time(&timestamp);
                    graphDP.updatePlot(timestamp, 0, STATUS_UNKNOWN, GraphDataPoint::COMMS_ERROR);
                }
            }

            // Add custom axis
	        // Commented out for the moment but may want it later.
	        long axisIndex = m_ChartCtrl.GetAxis().AddCustom( false );
	        CAxis theAxis = m_ChartCtrl.GetAxis().GetCustom( axisIndex );
	        //theAxis.SetPositionPercent(100);
            theAxis.GetLabels().GetFont().SetBold(true);
            theAxis.GetLabels().GetFont().SetSize(10);
            theAxis.GetLabels().GetFont().SetColor(colourList[colourIndex]);
            theAxis.GetAxisPen().SetColor(colourList[colourIndex]);
            theAxis.SetOtherside(true);
            theAxis.SetVisible( false );

            theAxis.GetTitle().SetCaption(dataPoint.engUnits.c_str());
            theAxis.GetTitle().GetFont().SetBold(true);
            theAxis.GetTitle().GetFont().SetSize(10);
            theAxis.GetTitle().GetFont().SetColor(colourList[colourIndex]);
            theAxis.GetTitle().SetAngle(90);
            theAxis.GetTitle().SetVisible(true);

            double leftAxisMin = m_ChartCtrl.GetAxis().GetLeft().GetMinimum();
            double leftAxisMax = m_ChartCtrl.GetAxis().GetLeft().GetMaximum();

            if( dataPoint.dpType != DP_BOOLEAN_TYPE )
	        {
                const double scaleFactor = (dataPoint.engHigh - dataPoint.engLow)/100;
                theAxis.SetMinMax(scaleFactor*leftAxisMin, scaleFactor*leftAxisMax);
            }
            else
            {
                //
                // These calculations are based on the way the scale factor and plotted
                // values are calculated in DataPointInfo and GraphDataPoint.
                //
                double scaleFactor = dataPoint.scaleHigh - dataPoint.scaleLow;
                double lowValue = ( 0 - dataPoint.scaleLow ) * scaleFactor;
                double highValue = ( 1 - dataPoint.scaleLow ) * scaleFactor;
                double diff = highValue - lowValue;
                double min = (1/diff)*leftAxisMin - (1/diff)*lowValue;
                double max = (1/diff)*leftAxisMax - (1/diff)*lowValue;
                theAxis.SetMinMax( min, max );

                theAxis.GetLabels().SetStyle(3);
            }

            thisSeries.SetVerticalAxisCustom( axisIndex );
            graphDP.SetAxisIndex( axisIndex );

	        m_GraphDataPointList.push_back( graphDP );

            //
            // Create a RealTimeDataUpdateSubscriber for the datapoint
            //
            RealTimeDataUpdateSubscriber* subscriber = new RealTimeDataUpdateSubscriber(this, dataPoint.entityKey);
            if(m_NowCheck)
            {
                subscriber->subscribeToMessage();
            }

            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Adding a subscriber for dp: %lu", dataPoint.entityKey);
            m_realTimeDataUpdateSubscriberList.push_back(subscriber);
        }

        OnSetMessageString(AFX_IDS_IDLEMESSAGE);
        EndWaitCursor();

        FUNCTION_EXIT;
    }


    //
    // DeleteFromGraph
    //
    void CHistoryViewerDlg::DeleteFromGraph( std::vector <unsigned long> &dataPointKeyList )
    {
        //
        // Return if nothing to delete
        //
        if( dataPointKeyList.size() == 0 )
	        return;

        CDataPointInfo::getInstance().DeleteFromGraph( dataPointKeyList );

        //
        // Go through the list of datapoints and delete them from the graph
        //
        std::vector <unsigned long>::iterator it;
        for ( it = dataPointKeyList.begin(); it != dataPointKeyList.end(); it++ )
        {
            //
	        // Go through the list of GraphDataPoints to find the one we want
            //
	        std::vector <GraphDataPoint>::iterator gdp_it;
	        for ( gdp_it = m_GraphDataPointList.begin(); gdp_it != m_GraphDataPointList.end(); gdp_it++ )
	        {
                //
                // Delete the subscriber for this datapoint
                //
                std::vector<RealTimeDataUpdateSubscriber*>::iterator subscriberIt;
                subscriberIt = m_realTimeDataUpdateSubscriberList.begin();
                for(; subscriberIt != m_realTimeDataUpdateSubscriberList.end(); ++subscriberIt)
                {
                    if((*subscriberIt)->getKey() == *it)
                    {
                        (*subscriberIt)->unsubscribeToMessage();
                        break;
                    }
                }

		        if( (*gdp_it).GetEntityKey() == static_cast<long>(*it) )
		        {
			        long seriesNum = (*gdp_it).GetSeriesNum();

			        // Commented out for the moment but may want it later.
			        // Disconnect the custom axis from the series
			        m_ChartCtrl.Series( seriesNum ).DisconnectAxisCustom( false );

                    int numCustomAxes = m_ChartCtrl.GetAxis().GetCustomCount();

			        // Remove the custom axis
                    long axisIndex = (*gdp_it).GetAxisIndex();
			        m_ChartCtrl.GetAxis().RemoveCustom( axisIndex );

                    // Set the grid back to default if the deleted trend was displaying
                    // a custom axis
                    if(m_customAxisDisplayed == axisIndex)
                    {
                        m_ChartCtrl.GetAxis().GetLeft().GetGridPen().SetVisible(true);
                        m_customAxisDisplayed = NO_CUSTOM_AXIS_DISPLAYED;
                        m_ChartCtrl.GetPanel().SetMarginRight(m_rightMarginInitial);
                    }

			        m_ChartCtrl.RemoveSeries( seriesNum );

                    //
			        // Adjust the series numbering for series added after the deleted one
                    //
			        AdjustSeriesNumbers( seriesNum );

                    //
			        // Adjust the axis numbering for series added after the deleted one
                    //
                    AdjustCustomAxes( axisIndex, seriesNum);

                    m_GraphDataPointList.erase( gdp_it );

			        break;
		        }
            }
        }
    }


    void CHistoryViewerDlg::AdjustCustomAxes(long axisIndex, long seriesNum)
    {
        if( m_GraphDataPointList.empty() )
        {
	        return;
        }

        //
        // Go through the list of GraphDataPoints to find the one we want
        //
        std::vector <GraphDataPoint>::iterator gdp_it;
        for ( gdp_it = m_GraphDataPointList.begin(); gdp_it != m_GraphDataPointList.end(); gdp_it++ )
        {
            long gdpAxisIndex = gdp_it->GetAxisIndex();
	        if(gdpAxisIndex > axisIndex)
            {
                if(m_customAxisDisplayed == gdpAxisIndex)
                {
                    --m_customAxisDisplayed;
                }

                gdp_it->SetAxisIndex(--gdpAxisIndex);
            }
        }

        if(m_lastSeriesClicked > seriesNum)
        {
            --m_lastSeriesClicked;
        }
    }


    //
    // OnSize
    //
    void CHistoryViewerDlg::OnSize(UINT nType, int cx, int cy)
    {
        TransActiveDialog::OnSize(nType, cx, cy);
        if( m_ChartCtrl && nType != SIZE_MAXHIDE && nType != SIZE_MINIMIZED  )
        {
	        ResizeGraph(cx, cy);
        }
    }


    //
    // AdjustTimeLabelFormat
    //
    void CHistoryViewerDlg::AdjustTimeLabelFormat()
    {
        //
        // Check the time scale we are using and change if appropriate
        //
        double minValue = m_ChartCtrl.GetAxis().GetBottom().GetMinimum();
        double maxValue = m_ChartCtrl.GetAxis().GetBottom().GetMaximum();
        if( (maxValue - minValue) < (15.0/(60.0*24.0)) )
        {
            // Less than 15 minutes
	        m_ChartCtrl.GetAxis().GetBottom().GetLabels().SetDateTimeFormat( "hh:nn:ss" );
        }
        else if( (maxValue - minValue) <= 1.0 )
        {
            // Less than a day
	        m_ChartCtrl.GetAxis().GetBottom().GetLabels().SetDateTimeFormat( "hh:nn" );
        }
        else if( (maxValue - minValue) <= 7.0 )
        {
            // Less than seven days
	        m_ChartCtrl.GetAxis().GetBottom().GetLabels().SetDateTimeFormat( "dd/mm/yyyy hh:nn" );
        }
        else if( (maxValue - minValue) <= 365.0 )
        {
            // Less than a year
	        m_ChartCtrl.GetAxis().GetBottom().GetLabels().SetDateTimeFormat( "dd/mm/yyyy" );
        }
        else
        {
	        m_ChartCtrl.GetAxis().GetBottom().GetLabels().SetDateTimeFormat( "mm/yyyy" );
        }
    }


    BEGIN_EVENTSINK_MAP(CHistoryViewerDlg, TransActiveDialog)
        //{{AFX_EVENTSINK_MAP(CHistoryViewerDlg)
        ON_EVENT(CHistoryViewerDlg, IDC_TCHART2, 27 /* OnZoom */, OnZoomGraph, VTS_NONE)
        ON_EVENT(CHistoryViewerDlg, IDC_TCHART2, 26 /* OnUndoZoom */, OnUndoZoomGraph, VTS_NONE)
        ON_EVENT(CHistoryViewerDlg, IDC_TCHART2, 24 /* OnScroll */, OnScrollGraph, VTS_NONE)
        ON_EVENT(CHistoryViewerDlg, IDC_TCHART2, 7 /* OnClickSeries */, OnClickSeriesPoint, VTS_I4 VTS_I4 VTS_I2 VTS_I2 VTS_I4 VTS_I4)
        ON_EVENT(CHistoryViewerDlg, IDC_TCHART2, 30 /* OnBeforeDrawAxes */, OnOnBeforeDrawAxes, VTS_NONE)
	    ON_EVENT(CHistoryViewerDlg, IDC_TCHART2, 32 /* OnGetSeriesMark */, OnGetSeriesMark, VTS_I4 VTS_I4 VTS_PBSTR)
	ON_EVENT(CHistoryViewerDlg, IDC_TCHART2, 5 /* OnClickBackground */, OnClickBackgroundTchart, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	ON_EVENT(CHistoryViewerDlg, IDC_TCHART2, 6 /* OnClickLegend */, OnClickLegend, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
	//}}AFX_EVENTSINK_MAP
    END_EVENTSINK_MAP()


    //
    // OnGetSeriesMark
    //
    void CHistoryViewerDlg::OnGetSeriesMark(long SeriesIndex, long ValueIndex, BSTR FAR* MarkText)
    {
	    //
        // Find the series we want to update and update it
        //
        char buffer[200];
        std::vector <GraphDataPoint>::iterator it;
	    for(it = m_GraphDataPointList.begin(); it != m_GraphDataPointList.end(); ++it)
	    {
            if((*it).GetSeriesNum() == SeriesIndex)
            {
                //
                // Found it. Get the actual value and engineering unit.
                // Convert all into a string
                //
                DataPoint dataPoint = CDataPointInfo::getInstance().GetDataPoint((*it).GetEntityKey());
                std::string engUnit = dataPoint.engUnits;
                if(engUnit == "Unknown")
                {
                    engUnit = "";
                }

                double value = (*it).getActualValue(ValueIndex);


                if(m_customAxisDisplayed == NO_CUSTOM_AXIS_DISPLAYED)
                {
                    sprintf(buffer, "%.4lf %s", value, engUnit.c_str());
                }
                else
                {
                    sprintf(buffer, "%.4lf", value);
                }
                break;
            }
	    }

        if(it == m_GraphDataPointList.end())
        {
            sprintf(buffer,"%s", "Unknown");
        }

        OLECHAR oleChar[100];
        int x = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, buffer, -1, oleChar, 100);
        *MarkText = SysAllocString(oleChar);
    }


    //
    // OnClickSeriesPoint
    //
    void CHistoryViewerDlg::OnClickSeriesPoint(
        long SeriesIndex,
        long ValueIndex,
        WPARAM wParam,
        LPARAM lParam,
        long X,
        long Y)
    {
        CSeries series = m_ChartCtrl.Series(SeriesIndex);

        //
        // Display the custom axis
        //
        if(m_customAxisDisplayed != NO_CUSTOM_AXIS_DISPLAYED)
        {
            CAxis oldAxis = m_ChartCtrl.GetAxis().GetCustom(m_customAxisDisplayed);
            oldAxis.SetVisible(false);

            if(SeriesIndex != m_lastSeriesClicked)
            {
                CSeries oldSeries = m_ChartCtrl.Series(m_lastSeriesClicked);
                CMarks oldMarks = oldSeries.GetMarks();
                oldMarks.SetVisible(false);
            }
        }
        else
        {
            m_ChartCtrl.GetAxis().GetLeft().GetGridPen().SetVisible(false);
        }

        long axisIndex = series.GetVerticalAxisCustom();
        CAxis theAxis = m_ChartCtrl.GetAxis().GetCustom( axisIndex );
	    theAxis.SetVisible( true );
        m_customAxisDisplayed = axisIndex;

	    //
        // Toggle the marks on and off
        //
        CMarks mark = series.GetMarks();
        mark.SetStyle(0);
        mark.GetArrow().SetVisible(FALSE);
        mark.SetArrowLength(10);
        mark.SetVisible(!series.GetMarks().GetVisible());

        m_lastSeriesClicked = SeriesIndex;

        m_ChartCtrl.GetPanel().SetMarginRight(5);
    }


    void CHistoryViewerDlg::OnClickLegend(long Button, long Shift, long X, long Y)
    {
	    // TODO: Add your control notification handler code here
	}


    void CHistoryViewerDlg::OnClickBackgroundTchart(long Button, long Shift, long X, long Y)
    {
        if(Button != 1)
        {
            return;
        }

	    if(m_customAxisDisplayed != NO_CUSTOM_AXIS_DISPLAYED)
        {
            CAxis oldAxis = m_ChartCtrl.GetAxis().GetCustom(m_customAxisDisplayed);
            oldAxis.SetVisible(false);

            CSeries oldSeries = m_ChartCtrl.Series(m_lastSeriesClicked);
            CMarks oldMarks = oldSeries.GetMarks();
            oldMarks.SetVisible(false);

            m_ChartCtrl.GetAxis().GetLeft().GetGridPen().SetVisible(true);

            m_customAxisDisplayed = NO_CUSTOM_AXIS_DISPLAYED;

            m_ChartCtrl.GetPanel().SetMarginRight(m_rightMarginInitial);
        }
	}


    //
    // OnZoomGraph
    //
    void CHistoryViewerDlg::OnZoomGraph()
    {
        if(m_serverDown)
        {
            return;
        }

        // Adjust custom axes
        double leftAxisMin = m_ChartCtrl.GetAxis().GetLeft().GetMinimum();
        double leftAxisMax = m_ChartCtrl.GetAxis().GetLeft().GetMaximum();

        std::vector <GraphDataPoint>::iterator it;
	    for(it = m_GraphDataPointList.begin(); it != m_GraphDataPointList.end(); ++it)
	    {
            DataPoint dataPoint = CDataPointInfo::getInstance().GetDataPoint(it->GetEntityKey());
            CAxis theAxis = m_ChartCtrl.GetAxis().GetCustom(it->GetAxisIndex());

            if( dataPoint.dpType != DP_BOOLEAN_TYPE )
	        {
                const double scaleFactor = (dataPoint.engHigh - dataPoint.engLow)/100;
                theAxis.SetMinMax(scaleFactor*leftAxisMin, scaleFactor*leftAxisMax);
            }
            else
            {
                double scaleFactor = dataPoint.scaleHigh - dataPoint.scaleLow;
                double lowValue = ( 0 - dataPoint.scaleLow ) * scaleFactor;
                double highValue = ( 1 - dataPoint.scaleLow ) * scaleFactor;
                double diff = highValue - lowValue;
                double min = (1/diff)*leftAxisMin - (1/diff)*lowValue;
                double max = (1/diff)*leftAxisMax - (1/diff)*lowValue;

                theAxis.SetMinMax( min, max );
            }
        }

        //
        // Adjust the date/time format of the X-axis
        //
        AdjustTimeLabelFormat();

        //
        // Turn off live updating
        //
        //TurnOffLiveUpdate();

        //
        // Set the start and range from the new zoom values
        //
        SetValuesFromGraph();

        m_toolBar.EnableNow();
    }


    //
    // OnUndoZoomGraph
    //
    void CHistoryViewerDlg::OnUndoZoomGraph()
    {
        if(m_serverDown)
        {
            return;
        }

        if( m_UnzoomReset )
        {
             // Adjust custom axes
            double leftAxisMin = m_ChartCtrl.GetAxis().GetLeft().GetMinimum();
            double leftAxisMax = m_ChartCtrl.GetAxis().GetLeft().GetMaximum();

            std::vector <GraphDataPoint>::iterator it;
	        for(it = m_GraphDataPointList.begin(); it != m_GraphDataPointList.end(); ++it)
	        {
                DataPoint dataPoint = CDataPointInfo::getInstance().GetDataPoint(it->GetEntityKey());
                CAxis theAxis = m_ChartCtrl.GetAxis().GetCustom(it->GetAxisIndex());

                if( dataPoint.dpType != DP_BOOLEAN_TYPE )
	            {
                    const double scaleFactor = (dataPoint.engHigh - dataPoint.engLow)/100;
                    theAxis.SetMinMax(scaleFactor*leftAxisMin, scaleFactor*leftAxisMax);
                }
                else
                {
                    double scaleFactor = dataPoint.scaleHigh - dataPoint.scaleLow;
                    double lowValue = ( 0 - dataPoint.scaleLow ) * scaleFactor;
                    double highValue = ( 1 - dataPoint.scaleLow ) * scaleFactor;
                    double diff = highValue - lowValue;
                    double min = (1/diff)*leftAxisMin - (1/diff)*lowValue;
                    double max = (1/diff)*leftAxisMax - (1/diff)*lowValue;

                    theAxis.SetMinMax( min, max );
                }
            }

            //
	        // Adjust the date/time format of the X-axis
            //
	        AdjustTimeLabelFormat();

            //
	        // Set the start and range from the new zoom values
            //
	        SetValuesFromGraph();
        }

        m_toolBar.DisableNow();
    }


    //
    // OnScrollGraph
    //
    void CHistoryViewerDlg::OnScrollGraph()
    {
        FUNCTION_ENTRY("OnScrollGraph");

        if(m_serverDown)
        {
            return;
        }

        //
        // Turn off live updating
        //
        //TurnOffLiveUpdate();

        //
        // Set the start and range from the new zoom values
        //
        SetValuesFromGraph();

        //
        // Load data
        //
        LoadAllData();

        m_toolBar.EnableNow();

        FUNCTION_EXIT;
    }


    //
    // TurnOffLiveUpdate
    //
    void CHistoryViewerDlg::TurnOffLiveUpdate()
    {
        if( m_NowCheck )
        {
	        m_NowCheck = FALSE;
	        m_toolBar.SetNowBitmap();
	        m_toolBar.GetToolBarCtrl().CheckButton( ID_NOW, m_NowCheck );

            //
            // Unsubcribe to messages as trends no longer need to do real time updates
            //
            std::vector<RealTimeDataUpdateSubscriber*>::iterator it;
            it = m_realTimeDataUpdateSubscriberList.begin();
            for(; it != m_realTimeDataUpdateSubscriberList.end(); ++it)
            {
                (*it)->unsubscribeToMessage();
            }

            //
            // Stop the refresh thread
            //
            if(m_graphRefreshThread != NULL)
            {
                if(!m_graphRefreshThread->isTerminated())
                {
                    m_graphRefreshThread->terminate();
                    delete m_graphRefreshThread;
                    m_graphRefreshThread = NULL;
                }
            }
        }

        //
        // Update the check on the menu
        //
        SetMenuItemCheck(VIEW_MENU, ID_VIEW_NOW, m_NowCheck);

        if( !m_toolBar.m_StartDateTimeCtrl.IsWindowEnabled() )
        {
	        m_toolBar.m_StartDateTimeCtrl.EnableWindow( TRUE );
        }

        SetStatusLiveMode();
    }


    //
    // SetValuesFromGraph
    //
    void CHistoryViewerDlg::SetValuesFromGraph()
    {
        //
        // Get the new min and max value
        //
        double minValue = m_ChartCtrl.GetAxis().GetBottom().GetMinimum();
        double maxValue = m_ChartCtrl.GetAxis().GetBottom().GetMaximum();
        long rangeInSeconds = secInDay*static_cast<long>(maxValue - minValue) + 1;

        //
        // Can only zoom down to 1 minute minimum
        //
        if( 60 > rangeInSeconds )
        {
            //
	        // Adjust to one minute
            //
	        double centreValue = (maxValue + minValue)/2.0;
	        minValue = centreValue - 30.0/(double)secInDay;
	        maxValue = centreValue + 30.0/(double)secInDay;
	        rangeInSeconds = 60;
	        m_ChartCtrl.GetAxis().GetBottom().SetMinMax( minValue, maxValue );
        }

        //
        // Set the start time
        //
        m_StartDateTime = minValue;
        m_toolBar.m_StartDateTimeCtrl.SetTime( m_StartDateTime );

        //
        // Set the range
        //
        m_RangeSeconds = rangeInSeconds;
        //m_toolBar.m_RangeCtrl.Set_TotalSeconds( m_RangeSeconds );
        m_toolBar.m_rangeTimeCtrl.SetTime( m_rangeTime );
        //m_toolBar.m_rangeDayCtrl.SetWindowText( ConvertIntToString(m_rangeDays).c_str() );

        COleDateTime temp;
        temp.SetTime(0, m_rangeDays, 0);
        m_toolBar.m_rangeDayCtrl.SetTime(temp);
    }


    //
    // SetGraphFromValues
    //
    void CHistoryViewerDlg::SetGraphFromValues()
    {
        //
        // Set the times on the X-Axis
        //
        double rangeInDays = (double)m_RangeSeconds/(double)secInDay;
        m_ChartCtrl.GetAxis().GetBottom().SetMinMax( m_StartDateTime.m_dt, m_StartDateTime.m_dt + rangeInDays );
        m_ChartCtrl.GetAxis().GetLeft().SetMinMax( 0.0, 100.0 );

        //
        // Adjust custom axes labels
        //
        std::vector <GraphDataPoint>::iterator it;
	    for(it = m_GraphDataPointList.begin(); it != m_GraphDataPointList.end(); ++it)
	    {
            long axisIndex = it->GetAxisIndex();
            CAxis axis = m_ChartCtrl.GetAxis().GetCustom(axisIndex);

            double leftAxisMin = m_ChartCtrl.GetAxis().GetLeft().GetMinimum();
            double leftAxisMax = m_ChartCtrl.GetAxis().GetLeft().GetMaximum();

            DataPoint dataPoint = CDataPointInfo::getInstance().GetDataPoint(it->GetEntityKey());
            if(dataPoint.dpType == DP_BOOLEAN_TYPE)
            {
                //
                // These calculations are based on the way the scale factor and plotted
                // values are calculated in DataPointInfo and GraphDataPoint.
                //
                double scaleFactor = dataPoint.scaleHigh - dataPoint.scaleLow;
                double lowValue = ( 0 - dataPoint.scaleLow ) * scaleFactor;
                double highValue = ( 1 - dataPoint.scaleLow ) * scaleFactor;
                double diff = highValue - lowValue;
                double min = (1/diff)*leftAxisMin - (1/diff)*lowValue;
                double max = (1/diff)*leftAxisMax - (1/diff)*lowValue;
                axis.SetMinMax(min, max);
            }
            else
            {
                const double scaleFactor = (dataPoint.engHigh - dataPoint.engLow)/100;
                axis.SetMinMax(scaleFactor*leftAxisMin, scaleFactor*leftAxisMax);
            }
        }

        AdjustTimeLabelFormat();
        LoadAllData();
    }


    //
    // GetNumSamples
    //
    long CHistoryViewerDlg::GetNumSamples( const long rangeSeconds )
    {
        int histConfig = getDataTypeDisplayed(m_HistoryType);

        //
        // Make it at least 1 to avoid any divide by zero errors.
        //
        long samplePeriod = __max( 1, HistoryConfig::getInstance().GetSamplePeriod( histConfig ) );
        return( (long)(rangeSeconds/samplePeriod) );
    }


    //
    // GetEndTime
    //
    COleDateTime CHistoryViewerDlg::GetEndTime()
    {
        COleDateTimeSpan timeDiff( (double)m_RangeSeconds/(double)secInDay );
        return( m_StartDateTime + timeDiff );
    }


    //
    // LoadAllData
    //
    void CHistoryViewerDlg::LoadAllData()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "LoadAllData");

        if( m_GraphDataPointList.empty() )
        {
	        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"LoadAllData: No data on graph");
	        return;
        }

        BeginWaitCursor();
        OnSetMessageString( ID_LOADING_DATA );

        COleDateTime endTime = GetEndTime();
        int histConfig = getDataTypeDisplayed(m_HistoryType);

        //
        // Go through the list of datapoints and load data
        //
        try
        {
	        std::vector <GraphDataPoint>::iterator it;
	        for ( it = m_GraphDataPointList.begin(); it != m_GraphDataPointList.end(); it++ )
	        {
		        if( !(*it).InRange( m_StartDateTime, endTime ) ||
			        !(*it).IsSameHistoryType( histConfig ) )
		        {
			        (*it).LoadData( histConfig, m_StartDateTime, endTime );
		        }
	        }

	        EndWaitCursor();
        }
        catch( GraphDataPoint::CouldNotLoadException e )
        {
	        EndWaitCursor();
	        CString errorMsg;
	        errorMsg.Format( "Error loading data:\n%s",
					         e.GetErrorMsg().c_str() );
	        AfxMessageBox( errorMsg, MB_OK | MB_ICONEXCLAMATION );
        }
        OnSetMessageString(AFX_IDS_IDLEMESSAGE);
    }


    //
    // OnPrintGraph
    //
    void CHistoryViewerDlg::OnPrintGraph()
    {
        TA_Bus::RightsLibrary* rl = NULL;
        bool response = false;
        CString actionName;
        actionName.LoadString(IDS_PRINT);

        try
        {
            TA_Bus::RightsLibraryFactory rlFactory;
            rl = rlFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);
            TA_Bus::EDecisionModule decisionModule = TA_Bus::UNDEFINED_DM;

            TA_Core::IEntityData* iData = TA_Core::EntityAccessFactory::getInstance().getEntity(RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str());
            unsigned long anEntityKey = iData->getKey();
            delete iData;

            TA_Core::IResource* r = TA_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( anEntityKey );
            unsigned long resourceID = r->getKey();
            delete r;

            std::string reason;

            response = rl->isActionPermittedOnResource(
                m_sessionId,
                resourceID,
                TA_Bus::aca_TREND_PRINT,
                reason,
                decisionModule);

            if(response)
            {

                if(m_ChartCtrl.GetPrinter().GetPrinterCount() <= 0)
                {
                    TA_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_120019);
                    return;
                }

                //
                // Display a dialog to give the operator the option
                // of printing a title with the graph
                //
                CPrintTitleDlg titleDlg;
                if(titleDlg.DoModal() == IDOK)
                {
                    std::vector<CString> titles = titleDlg.getTitles();

                    m_ChartCtrl.GetHeader().GetText().Clear();

                    VARIANT item;
                    VariantInit(&item);
                    item.vt = VT_BSTR;
                    OLECHAR oleChar[100];

                    for(unsigned int i = 0; i < titles.size(); ++i)
                    {
                        if(titles[i] == "")
                        {
                            break;
                        }

                        int x = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, titles[i], -1, oleChar, 100);
                        BSTR bstrMsg = SysAllocString(oleChar);
                        item.bstrVal = bstrMsg;
                        m_ChartCtrl.GetHeader().GetText().Add(item);

                        SysFreeString(oleChar);
                    }

                    m_ChartCtrl.GetHeader().SetColor(RGB(0,0,0));
                    m_ChartCtrl.GetHeader().SetVisible(TRUE);

                }
                else
                {
                    return;
                }

                //
                // Check if the date and time needs to be printed
                //
                if(titleDlg.doPrintDateTime())
                {
                    // Get the date and time
                    CTime timeNow = CTime::GetCurrentTime();
                    CString dateTime = timeNow.Format("%d/%m/%y %H:%M:%S");

                    VARIANT item;
                    VariantInit(&item);
                    item.vt = VT_BSTR;
                    OLECHAR oleChar[100];

                    int x = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, dateTime, -1, oleChar, 100);
                    BSTR bstrMsg = SysAllocString(oleChar);
                    item.bstrVal = bstrMsg;
                    m_ChartCtrl.GetHeader().GetText().Add(item);

                    SysFreeString(oleChar);
                }

                //
                // Check if the pen needs to be changed
                //
                if(titleDlg.doChangePen())
                {
                    try
                    {
                        int penStyle = 0;
                        std::vector <GraphDataPoint>::iterator gdp_it;
                        for (gdp_it = m_GraphDataPointList.begin(); gdp_it != m_GraphDataPointList.end(); gdp_it++)
	                    {
                            gdp_it->updatePenStyle(penStyle%6);
                            penStyle++;
                        }
                    }
                    catch(...)
                    {
                        TA_Bus::TransActiveMessage userMsg;
                        userMsg << actionName;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_010018);
                    }
                }

                //
                // Set the margins to 5% each
                //
                m_ChartCtrl.GetPrinter().SetMarginBottom( 5 );
                m_ChartCtrl.GetPrinter().SetMarginTop( 5 );
                m_ChartCtrl.GetPrinter().SetMarginLeft( 5 );
                m_ChartCtrl.GetPrinter().SetMarginRight( 5 );
                m_ChartCtrl.GetPrinter().SetPrintProportional(TRUE);

                //
                // Show the preview and they can print from there.
                //
                m_ChartCtrl.GetPrinter().ShowPreview();

                //
                // Remove the header and change the pen styles back to the original
                //
                m_ChartCtrl.GetHeader().SetVisible(FALSE);
                if(titleDlg.doChangePen())
                {
                    std::vector <GraphDataPoint>::iterator gdp_it;
                    for (gdp_it = m_GraphDataPointList.begin(); gdp_it != m_GraphDataPointList.end(); gdp_it++)
	                {
                        gdp_it->updatePenStyle(ORIGINAL_PEN_STYLE);
                    }
                }
            }
            else
            {
                TA_Bus::TransActiveMessage userMsg;
                userMsg << actionName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
            }

            delete rl;
        }
        catch(TA_Core::RightsException& ex)
        {
            if(ex.what() == "Indeterminate")
            {
                TA_Bus::TransActiveMessage userMsg;
                userMsg << actionName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
            }
            else if(ex.what() == "Not Applicable")
            {
            }

            delete rl;
        }
        catch(...)
        {
            TA_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010023);
            delete rl;
        }
    }


    // Loads new samples from the database if the same history type has been updated.
    // It is called (via CORBA event) when new history data has been written to the
    // database.
    LRESULT CHistoryViewerDlg::OnHistoryUpdate( WPARAM wParam, LPARAM lParam )
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "OnHistoryUpdate");

        long* entitykey = reinterpret_cast<long*>(wParam);

        DataPointInfo* dataPointInfo = reinterpret_cast<DataPointInfo*>(lParam);

        time_t timestamp = dataPointInfo->timestamp;
        unsigned long status = dataPointInfo->status;
        double value = dataPointInfo->value;

        updateRealTimePlot(*entitykey, timestamp, value, status);

        delete entitykey;
        delete dataPointInfo;

        LOG(SourceInfo, DebugUtil::FunctionExit, "OnHistoryUpdate");

        return 0;
    }


    //
    // OnExport
    //
    void CHistoryViewerDlg::OnExport()
    {
        TA_Bus::RightsLibrary* rl = NULL;
        bool response = false;
        CString actionName;
        actionName.LoadString(IDS_EXPORT);

        try
        {
            TA_Bus::RightsLibraryFactory rlFactory;
            rl = rlFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);
            TA_Bus::EDecisionModule decisionModule = TA_Bus::UNDEFINED_DM;

            TA_Core::IEntityData* iData = TA_Core::EntityAccessFactory::getInstance().getEntity(RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str());
            unsigned long anEntityKey = iData->getKey();
            delete iData;

            TA_Core::IResource* r = TA_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( anEntityKey );
            unsigned long resourceID = r->getKey();
            delete r;

            std::string reason;

            response = rl->isActionPermittedOnResource(
                m_sessionId,
                resourceID,
                TA_Bus::aca_TREND_EXPORT,
                reason,
                decisionModule);

            if(response)
            {
                m_ChartCtrl.GetExport().ShowExport();
            }
            else
            {
                TA_Bus::TransActiveMessage userMsg;
                userMsg << actionName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_010021);
            }

            delete rl;
        }
        catch(TA_Core::RightsException& ex)
        {
            if(ex.what() == "Indeterminate")
            {
                TA_Bus::TransActiveMessage userMsg;
                userMsg << actionName;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_010023);
            }
            else if(ex.what() == "Not Applicable")
            {
            }

            delete rl;
        }
        catch(...)
        {
            TA_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_010023);
            delete rl;
        }
    }


    //
    // AdjustSeriesNumbers
    //
    void CHistoryViewerDlg::AdjustSeriesNumbers( const long &seriesNum )
    {
        if( m_GraphDataPointList.empty() )
        {
	        return;
        }

        //
        // Go through the list of GraphDataPoints to find the one we want
        //
        std::vector <GraphDataPoint>::iterator gdp_it;
        for ( gdp_it = m_GraphDataPointList.begin(); gdp_it != m_GraphDataPointList.end(); gdp_it++ )
        {
	        (*gdp_it).AdjustSeriesNumber( seriesNum );
        }
    }


    //
    // UnzoomNoReset
    //
    void CHistoryViewerDlg::UnzoomNoReset()
    {
        if( m_ChartCtrl.GetZoom().GetZoomed() )
        {
	        m_UnzoomReset = false;
	        m_ChartCtrl.GetZoom().Undo();
	        m_UnzoomReset = true;
        }
    }


    //
    // DestroyWindow
    //
    BOOL CHistoryViewerDlg::DestroyWindow()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "DestroyWindow");

        RunParams::getInstance().deregisterRunParamUser(this);

        if(m_dataPointLoaderThread != NULL)
        {
            delete m_dataPointLoaderThread;
            m_dataPointLoaderThread = NULL;
        }

        if(m_graphRefreshThread != NULL)
        {
            if(!m_graphRefreshThread->isTerminated())
            {
                //
                // Haven't used the terminateAndWait here because
                // it seemed to hang on the join() for some reason...
                //
                m_graphRefreshThread->terminate();

                while(!m_graphRefreshThread->isTerminated())
                {
                    TA_Core::Thread::sleep(100);
                }
            }

            delete m_graphRefreshThread;
            m_graphRefreshThread = NULL;
        }

        //
        // Unsubcribe to messages as trends no longer need to do real time updates
        //
        std::vector<RealTimeDataUpdateSubscriber*>::iterator it;
        it = m_realTimeDataUpdateSubscriberList.begin();
        for(; it != m_realTimeDataUpdateSubscriberList.end(); ++it)
        {
            (*it)->unsubscribeToMessage();
            delete *it;
        }

        //
        // Unsubcribe to online update messages
        //
        delete m_dataPointOnlineUpdateSubscriber;
        m_dataPointOnlineUpdateSubscriber = NULL;

        //
        // Delete audit sender
        //
        delete m_auditSender;
        m_auditSender = NULL;

        //
        // Set now to false so that if OnHistoryUpdate() is called via messages,
        // it will not attempt to load new data.
        //
        m_NowCheck = false;

        //
        // Clear the connection to avoid problem with OCIAccess singleton being
        // destroyed before SimpleDBConnection singleton.
        //
    #ifndef USE_OCCI
        SimpleDBConnection::getInstance().ClearConnection();
    #endif

        LOG(SourceInfo, DebugUtil::FunctionExit, "DestroyWindow");

        return TransActiveDialog::DestroyWindow();
    }


    //
    // OnClose
    //
    void CHistoryViewerDlg::OnClose()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "OnClose");

        CDialog::OnCancel();

        LOG(SourceInfo, DebugUtil::FunctionExit, "OnClose");
    }


    //
    // OnDestroy
    //
    void CHistoryViewerDlg::OnDestroy()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "OnDestroy");

        LOG(SourceInfo, DebugUtil::FunctionExit, "OnDestroy");
    }


    //
    // OnAppAbout
    //
    void CHistoryViewerDlg::OnAppAbout()
    {
        TA_Bus::HelpLauncher::getInstance().displayAboutBox();
    }


    //
    // OnHelp
    //
    void CHistoryViewerDlg::OnHelp()
    {
        TA_Bus::HelpLauncher::getInstance().displayHelp();
    }


    //
    // OnCreate
    //
    int CHistoryViewerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
    {
        if (TransActiveDialog::OnCreate(lpCreateStruct) == -1)
        {
	        return -1;
        }

        if (!m_wndStatusBar.CreateEx( this, SBT_TOOLTIPS | SBARS_SIZEGRIP,
            WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, AFX_IDW_TOOLBAR) ||
	        !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)) )
        {
	        TRACE0("Failed to create status bar\n");
	    }

        CFont font;
        font.CreatePointFont(FONT_SIZE,FONT,NULL);
        m_wndStatusBar.SetFont(&font,FALSE);
        m_wndStatusBar.GetStatusBarCtrl().SetTipText( 0, "Ready" );
        m_wndStatusBar.SetPaneInfo(0, m_wndStatusBar.GetItemID(0),	SBPS_STRETCH, NULL );
        OnSetMessageString(AFX_IDS_IDLEMESSAGE);

        try
        {
	        CreateToolbar();
        }
        catch( ButtonNotFound )
        {
            TA_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120017);
        }
        catch( ToolbarCreationError )
        {
            TA_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120018);
        }

        return 0;
    }


    //
    // SetStatusLiveMode
    //
    void CHistoryViewerDlg::SetStatusLiveMode()
    {
        if( m_NowCheck )
        {
	        m_wndStatusBar.GetStatusBarCtrl().SetText( "LIVE", m_wndStatusBar.CommandToIndex( ID_LIVE_MODE ), 0 );
        }
        else
        {
	        m_wndStatusBar.GetStatusBarCtrl().SetText( "", m_wndStatusBar.CommandToIndex( ID_LIVE_MODE ), 0 );
        }
    }


    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnSetMessageString
    //      OnSetMessageString updates the status bar text.
    //
    //      This code is based on CFrameWnd::OnSetMessageString. We assume
    //      a string ID is always passed in wParam.
    LRESULT CHistoryViewerDlg::OnSetMessageString(WPARAM wParam, LPARAM lParam)
    {
        UINT    nIDMsg = (UINT)wParam;
        CString strMsg;

        if (nIDMsg)
        {
            if (strMsg.LoadString(nIDMsg) != 0)
	        {
                //
		        // Strip anything after the newline (newline breaks up the status bar
		        // help from the toop tip text).
                //
		        CString strStatusText;
		        AfxExtractSubString(strStatusText, strMsg, 0, '\n');
                if(m_serverDown)
                {
                    m_wndStatusBar.SetWindowText("Disconnected from server...");
                }
                else if(!m_dataPointLoaderThread->isFinishedLoading())
                {
                    m_wndStatusBar.SetWindowText("Loading...");
                }
                else
                {
		            m_wndStatusBar.SetWindowText(strStatusText);
                }
	        }
	        else
	        {
		        TRACE1("Warning: no message line prompt for ID %x%04X\n", nIDMsg);
	        }
        }

        UINT nIDLast     = m_nIDLastMessage;
        m_nIDLastMessage = nIDMsg;
        m_nIDTracking    = nIDMsg;
        return nIDLast;
    }


    /////////////////////////////////////////////////////////////////////////////
    // CModelessMain::OnPopMessageString
    //      Resets status bar message string. This code is based on
    //      CFrameWnd::OnPopMessageString
    LRESULT CHistoryViewerDlg::OnPopMessageString(WPARAM wParam, LPARAM lParam)
    {
        if (m_nFlags & WF_NOPOPMSG)
        {
	        return 0;
        }

        return SendMessage(WM_SETMESSAGESTRING, wParam, lParam);
    }


    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnEnterIdle
    //      OnEnterIdle updates the status bar when there's nothing better to do.
    //      This code is based on CFrameWnd::OnEnterIdle.
    void CHistoryViewerDlg::OnEnterIdle(UINT nWhy, CWnd* pWho)
    {
        if(m_serverDown)
        {
            OnSetMessageString(ID_SERVER_DOWN_STATUS_BAR);
            return;
        }

        if (nWhy != MSGF_MENU || m_nIDTracking == m_nIDLastMessage)
        {
	        return;
        }

        OnSetMessageString(m_nIDTracking);
        ASSERT(m_nIDTracking == m_nIDLastMessage);
    }


    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnMenuSelect
    //      OnMenuSelect updates the status bar message, based on the state
    //      of the dialog menu.
    //
    //      This code is based on CFrameWnd::OnMenuSelect.  We assume the
    //      application does not support context sensitive help.
    void CHistoryViewerDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
    {
        // set the tracking state
        if (nFlags == 0xFFFF)
        {
	        // cancel menu operation (go back to idle now)
	        m_nIDTracking = AFX_IDS_IDLEMESSAGE;
	        OnSetMessageString(m_nIDTracking);  // set string now
	        ASSERT(m_nIDTracking == m_nIDLastMessage);
        }
        else if (nItemID == 0 ||
		         nFlags & (MF_SEPARATOR|MF_POPUP|MF_MENUBREAK|MF_MENUBARBREAK))
        {
	        // nothing should be displayed
	        m_nIDTracking = 0;
        }
        else if (nItemID >= 0xF000 && nItemID < 0xF1F0)
        {
	        // special string table entries for system commands
	        m_nIDTracking = ID_COMMAND_FROM_SC(nItemID);
	        ASSERT(m_nIDTracking >= AFX_IDS_SCFIRST &&
		           m_nIDTracking < AFX_IDS_SCFIRST + 31);
        }
        else
        {
	        // track on idle
	        m_nIDTracking = nItemID;
        }
    }


    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnInitMenuPopup
    //      OnInitMenuPopup updates the state of items on a popup menu.
    //
    //      This code is based on CFrameWnd::OnInitMenuPopup.  We assume the
    //      application does not support context sensitive help.
    void CHistoryViewerDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
    {
        if (!bSysMenu)
        {
	        ASSERT(pPopupMenu != NULL);

	        // check the enabled state of various menu items
	        CCmdUI state;
	        state.m_pMenu = pPopupMenu;
	        ASSERT(state.m_pOther == NULL);

	        state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	        for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		         state.m_nIndex++)
	        {
		        state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		        if (state.m_nID == 0)
			        continue; // menu separator or invalid cmd - ignore it

		        ASSERT(state.m_pOther == NULL);
		        ASSERT(state.m_pMenu != NULL);
		        if (state.m_nID == (UINT)-1)
		        {
			        // possibly a popup menu, route to first item of that popup
			        state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			        if (state.m_pSubMenu == NULL ||
				        (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				        state.m_nID == (UINT)-1)
			        {
				        continue; // first item of popup can't be routed to
			        }
			        state.DoUpdate(this, FALSE);  // popups are never auto disabled
		        }
		        else
		        {
			        // normal menu item
			        // Auto enable/disable if command is _not_ a system command
			        state.m_pSubMenu = NULL;
                    UINT itemState = pPopupMenu->GetMenuState(state.m_nID, MF_BYCOMMAND);
                    if( !(itemState & 0x03) )
                    {
			            state.DoUpdate(this, state.m_nID < 0xF000);
                    }
		        }
	        }
        }
    }


    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnStatusBarCheck
    //      OnStatusBarCheck toggles the status of the status bar when the
    //      corresponding View option is selected from the menu.
    void CHistoryViewerDlg::OnStatusBarCheck()
    {
        m_wndStatusBar.ShowWindow(m_wndStatusBar.IsWindowVisible() ? SW_HIDE : SW_SHOWNA);
        ResizeGraph();
    }


    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnUpdateStatusBarMenu
    //      OnUpdateStatusBarMenu checks or unchecks the View option,
    //      depending on whether or not the status bar is visible.
    void CHistoryViewerDlg::OnUpdateStatusBarMenu(CCmdUI* pCmdUI)
    {
        ASSERT(pCmdUI->m_nID == ID_VIEW_STATUS_BAR);
        pCmdUI->SetCheck(m_wndStatusBar.IsWindowVisible());
    }


    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnToolBarCheck
    //      OnToolBarCheck toggles the status of the toolbar when the
    //      corresponding View option is selected from the menu.
    void CHistoryViewerDlg::OnToolBarCheck()
    {
        m_toolBar.ShowWindow(m_toolBar.IsWindowVisible() ? SW_HIDE : SW_SHOWNA);
        ResizeGraph();
    }


    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnUpdateToolBarMenu
    //      OnUpdateToolBarMenu checks or unchecks the View option,
    //      depending on whether or not the toolbar is visible.
    void CHistoryViewerDlg::OnUpdateToolBarMenu(CCmdUI* pCmdUI)
    {
        ASSERT(pCmdUI->m_nID == ID_VIEW_TOOLBAR);
        pCmdUI->SetCheck(m_toolBar.IsWindowVisible());
    }


    //
    // ResizeGraph
    //
    void CHistoryViewerDlg::ResizeGraph(int cx, int cy)
    {
        RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

        if( m_ChartCtrl )
        {
            //
	        // Resize the graph
            //
	        static bool firstTime = true;

            // TD 2924: Retrieve the client coordinates and use these to set the
            // window height instead of setting width and height to cx and cy.
            CRect client;
            GetClientRect(client);

            int windowHeight = client.Height();

            //
	        // Adjust size for status bar (if visible)
            //
	        if( firstTime || m_wndStatusBar.IsWindowVisible() )
	        {
		        CRect statusBarRect;
		        m_wndStatusBar.GetWindowRect(statusBarRect);
		        ScreenToClient(statusBarRect);
		        windowHeight -= statusBarRect.Height();
	        }

            //
	        // Adjust size for toolbar (if visible)
            //
	        int topAdjust = 0;
	        if( firstTime || m_toolBar.IsWindowVisible() )
	        {
		        CRect toolBarRect;
		        m_toolBar.GetWindowRect(toolBarRect);
		        ScreenToClient(toolBarRect);
		        topAdjust += toolBarRect.Height();
	        }
	        windowHeight -= topAdjust;

            //
	        // Resize the chart
            //
            // TD 2924: Changed width from cx to client.Width()
	        m_ChartCtrl.MoveWindow(0, topAdjust, client.Width(), windowHeight, true);

	        firstTime = false;
        }

    }


    //
    // ResizeGraph
    //
    void CHistoryViewerDlg::ResizeGraph()
    {
        CRect client;
        GetClientRect(client);
        ResizeGraph(client.Width(), client.Height());
    }


    /////////////////////////////////////////////////////////////////////////////
    // CModelessMain::OnToolTipText
    //      Handles TTN_NEEDTEXT message to display tooltips for the toolbar.
    //      This code is based on CFrameWnd::OnToolTipText
    BOOL CHistoryViewerDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
    {
        ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

        // allow top level routing frame to handle the message
        if (GetRoutingFrame() != NULL)
	        return FALSE;

        // need to handle both ANSI and UNICODE versions of the message
        TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
        TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
        TCHAR szFullText[256];
        CString strTipText;
        UINT nID = pNMHDR->idFrom;
        if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
	        pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
        {
	        // idFrom is actually the HWND of the tool
	        nID = ((UINT)(WORD)::GetDlgCtrlID((HWND)nID));
        }

        if (nID != 0) // will be zero on a separator
        {
	        AfxLoadString(nID, szFullText);
		        // this is the command id, not the button index
	        AfxExtractSubString(strTipText, szFullText, 1, '\n');
        }
    #ifndef _UNICODE
        if (pNMHDR->code == TTN_NEEDTEXTA)
	        lstrcpyn(pTTTA->szText, strTipText,
		        (sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
        else
	        _mbstowcsz(pTTTW->szText, strTipText,
		        (sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
    #else
        if (pNMHDR->code == TTN_NEEDTEXTA)
	        _wcstombsz(pTTTA->szText, strTipText,
		        (sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
        else
	        lstrcpyn(pTTTW->szText, strTipText,
		        (sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
    #endif
        *pResult = 0;

        // bring the tooltip window above other popup windows
        ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
	        SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);

        return TRUE;    // message was handled
    }


    void CHistoryViewerDlg::CreateToolbar()
    {
        //
        // Create toolbar at the top of the dialog window
        //
        if (!m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS
            | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) || !m_toolBar.LoadToolBar(IDR_MAINTOOLBAR))
        {
	        m_toolBar.LoadToolBar(IDR_MAINTOOLBAR);
	        TRACE0("Failed to create toolbar\n");
        }

        //
        // Reset the font
        //
        CFont font;
        font.CreatePointFont(FONT_SIZE,FONT,NULL);
        m_toolBar.SetFont(&font,TRUE);

        //
        // Modify the toolbar to contain controls
        //
        int index = -1;
        CRect rect;

        //
        // Add text to the buttons
        // Any exception from these functions will fall through and be caught by
        // the calling member
        //
        AddTextToButton( ID_DATAPOINT_ADD, "Add" );
        AddTextToButton( ID_DATAPOINT_DELETE, "Remove" );
        AddTextToButton( ID_EXPORT, "Export" );
        AddTextToButton( ID_PRINT_GRAPH, "Print" );
        AddTextToButton( ID_APPLY_NOW, "Apply" );
        AddTextToButton( ID_NOW, "Live" );
        AddTextToButton( ID_CURRENT_TIME, "Now" );
        AddTextToButton( ID_HELP, "Help" );
        m_toolBar.SetButtonStyle( m_toolBar.CommandToIndex( ID_NOW ), TBBS_CHECKBOX );
        m_toolBar.SetButtonStyle( m_toolBar.CommandToIndex(ID_DATAPOINT_DELETE), TBBS_AUTOSIZE );

        m_toolBar.GetToolBarCtrl().SetButtonSize( CSize( 59, 40 ) );

        //
        // Find the data type placeholder button
        //
        index = m_toolBar.CommandToIndex( ID_DATA_TYPE_HOLDER );
        if( index >=0 )
        {
            //
	        // Convert that button to a separator and get its position
            //
	        m_toolBar.SetButtonInfo(index, ID_DATA_TYPE_HOLDER, TBBS_SEPARATOR, 140);
            m_toolBar.GetItemRect(index, &rect);

            //
	        // Add text label
            //
            rect.bottom = rect.top + 16;
	        if (!m_toolBar.m_HistoryTypeText.Create("Representation:", WS_VISIBLE | WS_CHILD,
                rect, &m_toolBar, ID_DATA_TYPE_HOLDER))
	        {
		        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError,
                    "Failed to create data type text");
		        throw ToolbarCreationError();
	        }
	        //m_toolBar.m_HistoryTypeText.SetFont(m_toolBar.GetFont());
	        m_toolBar.m_HistoryTypeText.ShowWindow(SW_SHOW);

            //
	        // Expand the rectangle to allow the combo box room to drop down
            //
	        rect.top = rect.bottom;
	        rect.bottom += 200;

            //
            // Create the combo box and show it
            //
	        if (!m_toolBar.m_HistoryTypeCtrl.Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_AUTOHSCROLL| WS_DISABLED |
			    CBS_DROPDOWNLIST, rect, &m_toolBar, ID_DATA_TYPE_HOLDER))
	        {
		        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Failed to create data type combo-box");
		        throw ToolbarCreationError();
	        }
	        //m_toolBar.m_HistoryTypeCtrl.SetFont(GetFont());
	        m_toolBar.m_HistoryTypeCtrl.ShowWindow(SW_SHOW);
	        m_toolBar.m_HistoryTypeCtrl.SetItemHeight( -1, 15 );
        }
        else
        {
	        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugInfo, "Unable to find data type place holder");
	        throw ButtonNotFound();
        }

        //
        // Find the start date/time placeholder button
        //
        index = m_toolBar.CommandToIndex( ID_START_PLACEHOLDER );
        if( index >=0 )
        {
            //
	        // Convert that button to a separator and get its position
            //
	        m_toolBar.SetButtonInfo(index, ID_START_PLACEHOLDER, TBBS_SEPARATOR, 135);
   	        m_toolBar.GetItemRect(index, &rect);
            rect.bottom = rect.top + 16;
            if (!m_toolBar.m_StartText.Create("Start:", WS_VISIBLE | WS_CHILD,
               rect, &m_toolBar, ID_START_PLACEHOLDER))
	        {
		        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Failed to create start text");
		        throw ToolbarCreationError();
	        }
	        //m_toolBar.m_StartText.SetFont(m_toolBar.GetFont());
	        m_toolBar.m_StartText.ShowWindow(SW_SHOW);

            //
	        // Set the size
            //
	        rect.top = rect.bottom;
	        rect.bottom = rect.top + 24;

            //
	        // Create the time and date picker box and show it
            //
	        if (!m_toolBar.m_StartDateTimeCtrl.Create(WS_VISIBLE | WS_CHILD | WS_DISABLED
                | DTS_TIMEFORMAT, rect, &m_toolBar, ID_START_TIME))
	        {
		        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Failed to create start date/time control");
		        throw ToolbarCreationError();
	        }
	        m_toolBar.m_StartDateTimeCtrl.ShowWindow(SW_SHOW);

            //
	        // Set the start date/time format
            //
	        m_toolBar.m_StartDateTimeCtrl.SetFormat( "dd/MM/yy HH:mm" );
        }
        else
        {
	        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugInfo, "Unable to find date/time place holder");
	        throw ButtonNotFound();
        }

        //
        // Find the Duration placeholder button
        //
        index = m_toolBar.CommandToIndex( ID_DURATION_PLACEHOLDER );
        if( index >=0 )
        {
	        index = m_toolBar.CommandToIndex( ID_DURATION_PLACEHOLDER );

            //
            // Convert that button to a separator and get its position
            //
	        m_toolBar.SetButtonInfo(index, ID_DURATION_PLACEHOLDER, TBBS_SEPARATOR, 210);
	        m_toolBar.GetItemRect(index, &rect);

            CRect labelRect(rect);

            rect.bottom = rect.top + 16;
            if (!m_toolBar.m_RangeText.Create("X-Axis Range:", WS_VISIBLE | WS_CHILD,
								           rect, &m_toolBar, ID_DURATION_PLACEHOLDER))
	        {
		        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Failed to create range text");
		        throw ToolbarCreationError();
	        }
	        //m_toolBar.m_RangeText.SetFont(m_toolBar.GetFont());
	        m_toolBar.m_RangeText.ShowWindow(SW_SHOW);

	        // Set the size
	        rect.top = rect.bottom;
	        rect.bottom = rect.top + 24;
            //rect.left += 1;
            rect.right -= 165;
	        // Create the duration control and show it
	        //if (!m_toolBar.m_RangeCtrl.Create(NULL, "TEST", WS_CHILD | WS_VISIBLE,
	        //							   rect, &m_toolBar, ID_DURATION_PLACEHOLDER))
	        //{
	        //	TRACE0("Failed to create duration control\n");
	        //	LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Failed to create duration control");
	        //	throw ToolbarCreationError();
	        //}
            //m_toolBar.m_RangeCtrl.SetFont(&font, TRUE);
	        //m_toolBar.m_RangeCtrl.ShowWindow(SW_SHOW);
            //m_toolBar.m_RangeCtrl.SetFont(&font, TRUE);
	        //int oldCtrlId = m_toolBar.m_RangeCtrl.SetDlgCtrlID( ID_DURATION_PLACEHOLDER );

            //
            // Create Range Ctrl holding Days
            //
            //if(!m_toolBar.m_rangeDayCtrl.Create( ES_RIGHT | WS_VISIBLE | WS_CHILD,
            //                                rect, &m_toolBar, ID_DURATION_PLACEHOLDER ))

            //if(!m_toolBar.m_rangeDayCtrl.CreateEx(WS_EX_CLIENTEDGE, _T("EDIT"), "0",
            //    ES_RIGHT | WS_VISIBLE | WS_CHILD, rect, &m_toolBar, ID_RANGE_DAYS ))
            //{
            //    TRACE0("Failed to create day range control\n");
		    //    LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Failed to create day range control");
		    //    throw ToolbarCreationError();
            //}
            //m_toolBar.m_rangeDayCtrl.SetLimitText(2);
            //m_toolBar.m_rangeDayCtrl.ShowWindow(SW_SHOW);

            //m_toolBar.m_rangeDayCtrl.SetFont(&font, FALSE);

            if (!m_toolBar.m_rangeDayCtrl.Create(WS_VISIBLE | WS_CHILD
                | DTS_TIMEFORMAT, rect, &m_toolBar, ID_RANGE_TIME))
	        {
		        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Failed to create start date/time control");
		        throw ToolbarCreationError();
	        }
	        m_toolBar.m_rangeDayCtrl.ShowWindow(SW_SHOW);

            //
	        // Set the start date/time format
            //
	        m_toolBar.m_rangeDayCtrl.SetFormat( "mm" );
            COleDateTime temp;
            temp.SetTime(0, 0, 0);
            m_toolBar.m_rangeDayCtrl.SetTime(temp);

            //
            // Create Range Text: Days
            //
            rect.top = rect.top + 3;
            rect.bottom = rect.bottom + 3;
            rect.left = rect.right + 2;
            rect.right = rect.left + 36;
            if (!m_toolBar.m_rangeDayText.Create("Days", WS_VISIBLE | WS_CHILD,
								           rect, &m_toolBar, ID_DURATION_PLACEHOLDER))
            //if (!m_toolBar.m_rangeDayText.CreateEx(WS_EX_CLIENTEDGE, _T("STATIC"), "Day",
            //    WS_VISIBLE | WS_CHILD, rect, &m_toolBar, ID_DURATION_TEXT_PLACEHOLDER));
	        {
		        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Failed to create range day text");
		        throw ToolbarCreationError();
	        }
            m_toolBar.m_rangeDayText.ShowWindow(SW_SHOW);

            //
            // Create Range Ctrl holding time
            //
            rect.top = rect.top - 3;
            rect.bottom = rect.bottom - 3;
            rect.left = rect.right + 2;
            rect.right = rect.left + 62;
            if (!m_toolBar.m_rangeTimeCtrl.Create(WS_VISIBLE | WS_CHILD |
                                DTS_TIMEFORMAT, rect, &m_toolBar, ID_RANGE_TIME ))
	        {
		        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Failed to create range time control");
		        throw ToolbarCreationError();
	        }
            m_toolBar.m_rangeTimeCtrl.ShowWindow(SW_SHOW);
            m_toolBar.m_rangeTimeCtrl.SetFormat( "HH:mm" );


            // Create Range Text: Hrs:Mins
            //
            rect.top = rect.top + 3;
            rect.bottom = rect.bottom + 3;
            rect.left = rect.right + 2;
            rect.right = rect.left + 60;
            if (!m_toolBar.m_rangeTimeText.Create("Hrs:Mins", WS_VISIBLE | WS_CHILD,
								           rect, &m_toolBar, ID_DURATION_PLACEHOLDER))
            {
		        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Failed to create range day text");
		        throw ToolbarCreationError();
	        }
            m_toolBar.m_rangeTimeText.ShowWindow(SW_SHOW);
        }
        else
        {
	        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugInfo, "Unable to find duration place holder");
	        throw ButtonNotFound();
        }

        m_toolBar.AddNowBitmaps();
        m_toolBar.DisableApply();
        m_toolBar.DisableNow();
    }

    //
    // AddTextToButton
    //
    void CHistoryViewerDlg::AddTextToButton(UINT buttonId, CString buttonLabel )
    {
        int index = m_toolBar.CommandToIndex( buttonId );
        if( index >=0 )
        {
	        m_toolBar.SetButtonText( index, buttonLabel );
        }
        else
        {
	        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Unable to find %s button", buttonLabel);
	        throw ButtonNotFound();
        }
    }


    //
    // SetMenuItemCheck
    //
    void CHistoryViewerDlg::SetMenuItemCheck(CString menuString, const UINT& itemID, BOOL bChecked)
    {
        //
        // Get the top memu
        //
        CMenu* topMenu = GetMenu();
        if( topMenu == NULL )
	        return;

        //
        // Find the sub-menu specified by subMenuID
        //
        try
        {
            CMenu* subMenu = topMenu->GetSubMenu( FindMenuItem( topMenu, menuString ) );
            subMenu->CheckMenuItem(itemID, MF_BYCOMMAND | (bChecked ? MF_CHECKED : MF_UNCHECKED));
        }
        catch( ItemNotFoundException )
        {
	        LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugDebug,
                "SetMenuItemCheck unable to set item %d in menu %s", itemID, menuString );
	        return;
        }
    }


    // FindMenuItem() will find a menu item from the specified menu and returns
    // its position (0-based) in the specified menu.  It returns -1 if no menu
    // item is found.
    int CHistoryViewerDlg::FindMenuItem(CMenu* Menu, const CString& menuString)
    {
        ASSERT(Menu);
        ASSERT(::IsMenu(Menu->GetSafeHmenu()));

        int count = Menu->GetMenuItemCount();
        CString str;
        for (int i = 0; i < count; i++)
        {
	        if( Menu->GetMenuString(i, str, MF_BYPOSITION) && str == menuString )
	        {
		        return i;
	        }
        }

        throw ItemNotFoundException();
    }


    //
    // OnViewModify
    //
    void CHistoryViewerDlg::OnViewModify()
    {
        //CModifyGraph modifyDlg( m_NowCheck, m_HistoryType, m_StartDateTime,
        //						m_RangeSeconds, m_toolBar.m_HistoryTypeCtrl );

        CModifyGraph modifyDlg( m_NowCheck, m_HistoryType, m_StartDateTime,
						        m_rangeDays, m_rangeTime, m_toolBar.m_HistoryTypeCtrl );

        if( modifyDlg.DoModal() == IDOK )
        {
            //
	        // Update anything that has changed
            //

            //
            // Set the history data type if it has changed
            //
            if( modifyDlg.m_DataType != m_HistoryType)
            {
	            m_HistoryType = modifyDlg.m_DataType;
	            m_toolBar.m_HistoryTypeCtrl.SetCurSel( m_HistoryType );

                //
                // Display a warning if current trend data has been selected and the
                // history type is not 'Sample'
                //
                displayCurrentTrendDataWarning();
            }

            //
	        // Set the now check
            //
	        if(	m_NowCheck != modifyDlg.m_Now )
	        {
    	        m_NowCheck = modifyDlg.m_Now;
		        m_toolBar.GetToolBarCtrl().CheckButton( ID_NOW, m_NowCheck );
		        OnNowCheck();
	        }

            //
	        // Get the start time only when not in live mode
            //
            if( !modifyDlg.m_Now )
	        {
		        SetStartDateTime( modifyDlg.m_Start );
	        }

            if(modifyDlg.m_rangeDays != m_rangeDays)
            {
                COleDateTime ctrlTime;
                ctrlTime.SetTime(0, modifyDlg.m_rangeDays, 0);
                m_toolBar.m_rangeDayCtrl.SetTime(ctrlTime);
            }

            //
	        // Set the range value
            //
	        m_RangeSeconds = modifyDlg.m_RangeSeconds;

	        //m_toolBar.m_RangeCtrl.Set_TotalSeconds( m_RangeSeconds );
            m_toolBar.m_rangeTimeCtrl.SetTime( modifyDlg.m_rangeTime );
            //m_toolBar.m_rangeDayCtrl.SetWindowText( ConvertIntToString(modifyDlg.m_rangeDays).c_str() );

	        ApplyChanges();
        }
    }


    //
    // OnOnBeforeDrawAxes
    //
    void CHistoryViewerDlg::OnOnBeforeDrawAxes()
    {
        double bottomIncrement = m_ChartCtrl.GetAxis().GetBottom().CalcIncrement();

        // For 5 or 10 second label increments
        if( bottomIncrement < (10.0/(60.0*60.0*24.0) + 1e-05) )
        {
	        m_ChartCtrl.GetAxis().GetBottom().SetMinorTickCount( 4 );
        }
        // For 30 or 15 second label increments
        else if( bottomIncrement < (30.0/(60.0*60.0*24.0) + 1e-05) )
        {
	        m_ChartCtrl.GetAxis().GetBottom().SetMinorTickCount( 2 );
        }
        // For 1 minute label increments
        else if( bottomIncrement < (1.0/(60.0*24.0) + 1e-05) )
        {
	        m_ChartCtrl.GetAxis().GetBottom().SetMinorTickCount( 5 );
        }
        // For 5 or 10 minute label increments
        else if( bottomIncrement < (10.0/(60.0*24.0) + 1e-05) )
        {
	        m_ChartCtrl.GetAxis().GetBottom().SetMinorTickCount( 4 );
        }
        // For 15 or 30 minute label increments
        else if( bottomIncrement < (30.0/(60.0*24.0) + 1e-05) )
        {
	        m_ChartCtrl.GetAxis().GetBottom().SetMinorTickCount( 2 );
        }
        // For 1 hour label increments
        else if( bottomIncrement < (1.0/24.0 + 1e-05) )
        {
	        m_ChartCtrl.GetAxis().GetBottom().SetMinorTickCount( 3 );
        }
        // For 6 or 12 hour label increments
        else if( bottomIncrement < (12.0/24.0 + 1e-05) )
        {
	        m_ChartCtrl.GetAxis().GetBottom().SetMinorTickCount( 5 );
        }
        else
        {
	        m_ChartCtrl.GetAxis().GetBottom().SetMinorTickCount( 3 );
        }
    }


    //
    // serverDown
    //
    void CHistoryViewerDlg::serverDown()
    {
        m_serverDown = true;

        //
        // Notify operator server is down
        //
        OnSetMessageString(ID_SERVER_DOWN_STATUS_BAR);

        //
        // Disable all controls in toolbar
        //
        m_toolBar.DisableAll();
        m_toolBar.EnableWindow(false);

        //
        // Disable appropriate menu items
        //
        CMenu* topMenu = GetMenu();
        if( topMenu == NULL )
        {
            return;
        }

        // TODO: After demo, change this section and remove hardcoded menu items...
        CMenu* subMenu = topMenu->GetSubMenu( FindMenuItem( topMenu, "&Edit" ) );
        subMenu->EnableMenuItem(ID_EXPORT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
        subMenu->EnableMenuItem(ID_DATAPOINT_ADD, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
        subMenu->EnableMenuItem(ID_DATAPOINT_DELETE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

        subMenu = topMenu->GetSubMenu( FindMenuItem( topMenu, "&View" ) );
        subMenu->EnableMenuItem(ID_VIEW_MODIFY, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
        subMenu->EnableMenuItem(ID_VIEW_NOW, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);

        // TODO: Maybe don't need to disable these...
        subMenu = topMenu->GetSubMenu( FindMenuItem( topMenu, "&File" ) );
        subMenu->EnableMenuItem(ID_EXPORT, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
        subMenu->EnableMenuItem(ID_PRINT_GRAPH, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    }


    //
    // serverUp
    //
    void CHistoryViewerDlg::serverUp()
    {
        m_serverDown = false;

        //
        // Enable all controls in toolbar
        //
        m_toolBar.EnableAll();
        m_toolBar.EnableWindow(true);

        //
        // Resume the last status bar text just before server went down
        // (as the state should be the same as before)
        //
        OnSetMessageString(AFX_IDS_IDLEMESSAGE);

        //
        // Disable appropriate menu items
        //
        CMenu* topMenu = GetMenu();
        if( topMenu == NULL )
        {
	        return;
        }

        // TODO: After demo, change this section and remove hardcoded menu items...
        CMenu* subMenu = topMenu->GetSubMenu( FindMenuItem( topMenu, "&Edit" ) );
        subMenu->EnableMenuItem(ID_EXPORT, MF_BYCOMMAND | MF_ENABLED);
        subMenu->EnableMenuItem(ID_DATAPOINT_ADD, MF_BYCOMMAND | MF_ENABLED);
        subMenu->EnableMenuItem(ID_DATAPOINT_DELETE, MF_BYCOMMAND | MF_ENABLED);

        subMenu = topMenu->GetSubMenu( FindMenuItem( topMenu, "&View" ) );
        subMenu->EnableMenuItem(ID_VIEW_MODIFY, MF_BYCOMMAND | MF_ENABLED);
        subMenu->EnableMenuItem(ID_VIEW_NOW, MF_BYCOMMAND | MF_ENABLED);

        // TODO: Maybe don't need to disable these...
        subMenu = topMenu->GetSubMenu( FindMenuItem( topMenu, "&File" ) );
        subMenu->EnableMenuItem(ID_EXPORT, MF_BYCOMMAND | MF_ENABLED);
        subMenu->EnableMenuItem(ID_PRINT_GRAPH, MF_BYCOMMAND | MF_ENABLED);
    }


    //
    // calculateRangeSeconds
    //
    void CHistoryViewerDlg::calculateRangeSeconds()
    {
        if(!m_toolBar.m_rangeTimeCtrl.GetTime(m_rangeTime))
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                "Could not get Range Time" );
            return;
        }

        //char text[100];
        //m_toolBar.m_rangeDayCtrl.GetLine( 0, text );
        COleDateTime temp;
        m_toolBar.m_rangeDayCtrl.GetTime(temp);
        //m_rangeDays = atoi(text);
        //sprintf(text, "%d", temp.GetMinute());
        m_rangeDays = temp.GetMinute();

        int hourRange = m_rangeTime.GetHour();
        int minuteRange = m_rangeTime.GetMinute();

        m_RangeSeconds = secInDay*m_rangeDays + 60*60*hourRange + 60*minuteRange;
    }


    //
    // ConvertIntToString
    //
    std::string CHistoryViewerDlg::ConvertIntToString(int integer)
    {
        char buffer[100];
        _itoa( integer, buffer, 10 );
        return std::string(buffer);
    }


    //
    // updateRealTimePlot
    //
    void CHistoryViewerDlg::updateRealTimePlot(unsigned long entitykey, time_t timestamp, double value, unsigned long status)
    {
        TA_Core::ThreadGuard guard(m_lock);

        FUNCTION_ENTRY("updateRealTimePlot");

        //
        // NOTE: The history update is performed by doing a select for each of the
        // datapoints on the graph one at a time.  If this proves too slow, it should
        // be possible to do single select with a clause WHERE ENTITY IN (...) and
        // get all the new points for all the plotted values.  The results would then
        // need to be processed and points added to the appropriate data series.
        // A method would also need to be added to GraphDataPoint to add a single
        // datapoint and update the range.
        //

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
            "Received update from Entity key: %lu, Value: %lf and timestamp: %lu",
            entitykey, value, timestamp);

        //
        // Get the type of data currently displayed
        //
        int histConfig = getDataTypeDisplayed(m_HistoryType);

        //
        // Only update screen if we are live and looking at sample data.
        // Won't update live if looking at other representations as the
        // database does the calculations.
        //
        // Possible, but why would someone want to look at live 6 minute
        // averages??
        //
        // If this is required, would need the Data Recording Agent to send
        // events again each time the other representation tables are updated
        // and the Trend Viewer would retrieve the data and update its display
        // accordingly.
        //
        HistConfigMap hcMap = HistoryConfig::getInstance().GetHistConfigMap();
        std::map <unsigned long, HistConfig> :: iterator it;
        it = hcMap.find(histConfig);
        if(m_NowCheck && ((it->second).baseData == SAMPLE_BASE_DATA_VALUE))
        {
		    //
            // Only move the graph if the timestamp is later than the last time shown
            //
            try
            {
                moveGraph(timestamp);
            }
            catch(const TA_Core::HistoryInitialisationException& ex)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot plot point: %s", ex.what());
                FUNCTION_EXIT;
                return;
            }

            //
            // Find the series we want to update and update it
            //
            std::vector <GraphDataPoint>::iterator it;
	        for ( it = m_GraphDataPointList.begin(); it != m_GraphDataPointList.end(); ++it )
	        {
                if((*it).GetEntityKey() == static_cast<long>(entitykey))
                {
                    if(status == QUALITY_BAD_COMM_FAILURE)
                    {
                        status = GraphDataPoint::COMMS_ERROR;
                    }

                    (*it).updatePlot(timestamp, value, status);
                    break;
                }
	        }
	    }
        else
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugNorm,
                "Not updating live as not in Live mode or sample data is not displayed");
        }

        FUNCTION_EXIT;
    }


    //
    // refreshGraph
    //
    void CHistoryViewerDlg::refreshGraph(time_t timestamp)
    {
        FUNCTION_ENTRY("refreshGraph");

        TA_Core::ThreadGuard guard(m_lock);

        //
        // Get the type of data currently displayed
        //
        //int test = m_HistoryType;
        int histConfig = getDataTypeDisplayed(m_HistoryType);

        //
        // Only update screen if we are looking at sample data.
        // Won't update live if looking at other representations as the
        // database does the calculations.
        //
        // Possible, but why would someone want to look at live 6 minute
        // averages??
        //
        // If this is required, would need the Data Recording Agent to send
        // events again each time the other representation tables are updated
        // and the Trend Viewer would retrieve the data and update its display
        // accordingly.
        //
        HistConfigMap hcMap = HistoryConfig::getInstance().GetHistConfigMap();
        std::map <unsigned long, HistConfig> :: iterator it;
        it = hcMap.find(histConfig);
        if((it->second).baseData == SAMPLE_BASE_DATA_VALUE)
        {
		    //
            // Only move the graph if the timestamp is later than the last time shown
            //
            try
            {
                moveGraph(timestamp);
            }
            catch(const TA_Core::HistoryInitialisationException& ex)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot plot point: %s", ex.what());
                FUNCTION_EXIT;
                return;
            }

            //
            // Update each series in the graph
            //
            std::vector <GraphDataPoint>::iterator it;
	        for ( it = m_GraphDataPointList.begin(); it != m_GraphDataPointList.end(); ++it )
	        {
                double latestValue = (*it).getLatestValue();

                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                    "Updating series for datapoint: %lu with value: %lf", (*it).GetEntityKey(), latestValue);

                (*it).updatePlot(timestamp, latestValue, STATUS_UNKNOWN, COMMS_ERROR_UNKNOWN);
            }
        }

        FUNCTION_EXIT;
    }


    //
    // moveGraph
    //
    void CHistoryViewerDlg::moveGraph(time_t timestamp)
    {
        FUNCTION_ENTRY("moveGraph");

        //
        // One second time span used for adjustments
        //
        COleDateTimeSpan oneSecond( 1.0/(double)secInDay );

        //
        // Check if the timestamp is valid
        //
        tm ptm;
        if(ACE_OS::localtime_r(&timestamp, &ptm) == NULL)
        {
            TA_THROW(TA_Core::HistoryInitialisationException("Received an invalid timestamp"));
        }

        COleDateTime endTime(timestamp);
		endTime += oneSecond;           // Add a second so we definately get the value

        CString testTime = endTime.Format( "%d/%m/%Y %H:%M:%S" );

        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Graph update time: %s", testTime);

        //
        // Only move the graph if the timestamp is later than the last time shown
        //
        if(timestamp > m_lastTimeStamp && !m_toolBar.GetToolBarCtrl().IsButtonEnabled(ID_CURRENT_TIME))
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Moving Graph");

            //
		    // Set the new start date/time (given the end date/time)
            //
		    SetEndDateTime(endTime);

            //
		    // Move the graph to look and load the new samples
		    // Set the times on the X-Axis
            //
            double rangeInDays = (double)m_RangeSeconds/(double)secInDay;
            m_ChartCtrl.GetAxis().GetBottom().SetMinMax( m_StartDateTime.m_dt, m_StartDateTime.m_dt + rangeInDays );
            m_ChartCtrl.GetAxis().GetLeft().SetMinMax( 0.0, 100.0 );

            //
            // Adjust custom axes labels
            //
            std::vector <GraphDataPoint>::iterator it;
	        for(it = m_GraphDataPointList.begin(); it != m_GraphDataPointList.end(); ++it)
	        {
                long axisIndex = it->GetAxisIndex();
                CAxis axis = m_ChartCtrl.GetAxis().GetCustom(axisIndex);

                double leftAxisMin = m_ChartCtrl.GetAxis().GetLeft().GetMinimum();
                double leftAxisMax = m_ChartCtrl.GetAxis().GetLeft().GetMaximum();

                DataPoint dataPoint = CDataPointInfo::getInstance().GetDataPoint(it->GetEntityKey());
                if(dataPoint.dpType == DP_BOOLEAN_TYPE)
                {
                    //
                    // These calculations are based on the way the scale factor and plotted
                    // values are calculated in DataPointInfo and GraphDataPoint.
                    //
                    double scaleFactor = dataPoint.scaleHigh - dataPoint.scaleLow;
                    double lowValue = ( 0 - dataPoint.scaleLow ) * scaleFactor;
                    double highValue = ( 1 - dataPoint.scaleLow ) * scaleFactor;
                    double diff = highValue - lowValue;
                    double min = (1/diff)*leftAxisMin - (1/diff)*lowValue;
                    double max = (1/diff)*leftAxisMax - (1/diff)*lowValue;
                    axis.SetMinMax(min, max);
                }
                else
                {
                    const double scaleFactor = (dataPoint.engHigh - dataPoint.engLow)/100;
                    axis.SetMinMax(scaleFactor*leftAxisMin, scaleFactor*leftAxisMax);
                }
            }

            AdjustTimeLabelFormat();

            m_lastTimeStamp = timestamp;
        }

        FUNCTION_EXIT;
    }


    //
    // displayCurrentTrendDataWarning
    //
    void CHistoryViewerDlg::displayCurrentTrendDataWarning()
    {
        int histConfig = getDataTypeDisplayed(m_HistoryType);

        HistConfigMap hcMap = HistoryConfig::getInstance().GetHistConfigMap();
        std::map <unsigned long, HistConfig> :: iterator it;
        it = hcMap.find(histConfig);
        if((it->second).baseData != SAMPLE_BASE_DATA_VALUE)
        {
            //
            // Check if there are any Current Data Trends selected for display. These will
            // only be displayed in the graph when the "Sample" type is chosen. If there are
            // any Current Data Trends selected, then display a warning that these won't be
            // displayed.
            //
            TA_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_120009);
        }
    }


    //
    // getDataTypeDisplayed
    //
    int CHistoryViewerDlg::getDataTypeDisplayed(int type)
    {
        FUNCTION_ENTRY("getDataTypeDisplayed");

        //ThreadGuard guard(m_toolbarLock);

        int histConfig = m_toolBar.m_HistoryTypeCtrl.GetItemData( type );

        return histConfig;
    }


    //
    // processUpdateCfgEvent
    //
    void CHistoryViewerDlg::processUpdateCfgEvent(IEntityData* entityData)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "processUpdateCfgEvent");

        //
        // Retrieve the latest parameters and store
        //

        DataPointEntityData* dpData = dynamic_cast <DataPointEntityData*> (entityData);
        dpData->invalidate();

        DataPoint datapoint;
        datapoint.entityKey = dpData->getKey();
		datapoint.entityName = dpData->getName();
		datapoint.description = dpData->getDescription();
        datapoint.trendingEnabled = dpData->getEnableTrending();
        datapoint.subsystem = dpData->getSubsystemName();
        datapoint.dpType = dpData->getType();
		datapoint.engLow = dpData->getEngineeringLimitLow();
		datapoint.engHigh = dpData->getEngineeringLimitHigh();
		datapoint.engUnits = dpData->getEngineeringUnits();
		datapoint.configIsLoaded = true;

        CDataPointInfo::getInstance().UpdateDataPoint(datapoint);

        LOG(SourceInfo, DebugUtil::FunctionExit, "processUpdateCfgEvent");
    }


    //
    // processCreateCfgEvent
    //
    void CHistoryViewerDlg::processCreateCfgEvent(unsigned long entitykey, IEntityData* entityData)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "processCreateCfgEvent");

        DataPointEntityData* dpData = dynamic_cast <DataPointEntityData*> (entityData);
        dpData->invalidate();

        DataPoint datapoint;
        datapoint.entityKey = dpData->getKey();
		datapoint.entityName = dpData->getName();
		datapoint.description = dpData->getDescription();
        datapoint.trendingEnabled = dpData->getEnableTrending();
        datapoint.subsystem = dpData->getSubsystemName();
        datapoint.dpType = dpData->getType();
		datapoint.engLow = dpData->getEngineeringLimitLow();
		datapoint.engHigh = dpData->getEngineeringLimitHigh();
		datapoint.engUnits = dpData->getEngineeringUnits();
		datapoint.configIsLoaded = true;

        CDataPointInfo::getInstance().InsertDataPoint(datapoint);

        LOG(SourceInfo, DebugUtil::FunctionExit, "processCreateCfgEvent");
    }


    //
    // processDeleteCfgEvent
    //
    void CHistoryViewerDlg::processDeleteCfgEvent(unsigned long entitykey)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "processDeleteCfgEvent");

        //
        // Check if the data point currently has a trend displayed
        //
        std::vector <GraphDataPoint>::iterator gdp_it = m_GraphDataPointList.begin();
        for(; gdp_it != m_GraphDataPointList.end(); ++gdp_it)
        {
            if((*gdp_it).GetEntityKey() == static_cast<long>(entitykey))
            {
                break;
            }
        }

        if(gdp_it != m_GraphDataPointList.end())
        {
            //
            // Display warning that trend will disappear
            //
            DataPoint datapoint = CDataPointInfo::getInstance().GetDataPoint(entitykey);
            std::string name = datapoint.entityName;

            TA_Bus::TransActiveMessage userMsg;
            userMsg << name;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120021);

            //
            // Remove the trend from the graph
            //
            std::vector<unsigned long> dpList;
            dpList.push_back(entitykey);
            DeleteFromGraph(dpList);

            //
            // Check if there are anymore trends in the graph. If not, then
            // stop the refresh thread.
            //
            if(m_NowCheck && m_GraphDataPointList.empty())
            {
                m_graphRefreshThread->terminateAndWait();
                delete m_graphRefreshThread;
                m_graphRefreshThread = NULL;
            }
        }

        //
        // Remove the datapoint from the overall store
        //
        CDataPointInfo::getInstance().DeleteDataPoint(entitykey);

        LOG(SourceInfo, DebugUtil::FunctionExit, "processDeleteCfgEvent");
    }

/*
    void CHistoryViewerDlg::processEntityParameterUpdates(const std::string& parameter)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "processEntityParameterUpdates");

        //
        // Retrieve the latest graph refresh interval and store
        //

        std::string entityName = RunParams::getInstance().get(RPARAM_ENTITYNAME);
        IEntityData* entityData = TA_Core::EntityAccessFactory::getInstance().getEntity(entityName);
        entityData->invalidate();

        HistoryViewerData* hvData = dynamic_cast <HistoryViewerData*> (entityData);
        m_graphRefreshThread->setRefreshInterval(hvData->getGraphRefreshInterval());

        LOG(SourceInfo, DebugUtil::FunctionExit, "processEntityParameterUpdates");
    }
*/

    std::string CHistoryViewerDlg::getOperatorName(unsigned long operatorId)
    {
        try
        {
            TA_Core::IOperator* iOperator = TA_Core::OperatorAccessFactory::getInstance().getOperator(operatorId, false);
            std::string name = iOperator->getName();
            return name;
        }
        catch(const TA_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            return "<operator unknown>";
        }
        catch(const TA_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            return "<operator unknown>";
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown exception caught while retrieving operators");
            return "<operator unknown>";
        }
    }

    //
    // sendAuditMessage
    //
    void CHistoryViewerDlg::sendAuditMessage(const TA_Core::MessageType& messageType, const TA_Core::DescriptionParameters& descriptionParams)
    {
        FUNCTION_ENTRY("sendAuditMessage");

        try
        {
            m_auditSender->sendAuditMessage(
                messageType,
                strtoul(RunParams::getInstance().get(RPARAM_ENTITY_KEY).c_str(), NULL, 10),
                descriptionParams,
                "",
                m_sessionId,
                "",
                "",
                "");
        }
        catch(...)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "An unknown exception was caught while "
                "attempting to send an audit message");
        }

        FUNCTION_EXIT;
    }


    //
    // saveUserSettings
    //
    void CHistoryViewerDlg::saveUserSettings(std::map<std::string, std::string>& userSettings)
    {
        FUNCTION_ENTRY("saveUserSettings");

        std::ostringstream oss;

        std::vector <GraphDataPoint>::iterator it;
	    for ( it = m_GraphDataPointList.begin(); it != m_GraphDataPointList.end(); ++it )
	    {
            unsigned long entityKey = (*it).GetEntityKey();
            oss << entityKey << "\\";
        }

        std::string dataPointsStr = oss.str();

        if(!dataPointsStr.empty())
        {
            //
            // Because we're saving the datapoints for the particular operator, save the setting
            // name with the operator ID appended to it
            //
            std::string settingName(DATAPOINTS_SETTING);
            settingName += m_operatorId;
            userSettings.clear();
            userSettings[settingName] = dataPointsStr;
        }

        FUNCTION_EXIT;
    }


    //
    // loadUserSettings
    //
    void CHistoryViewerDlg::loadUserSettings(TA_Bus::SettingsMgr& settingMgr)
    {
        FUNCTION_ENTRY("loadUserSettings");

        //
        // Get the saved data points
        //
        std::ostringstream oss;
        oss << DATAPOINTS_SETTING << m_operatorId;
        std::string settingName = oss.str();

        try
        {
            m_dataPointsSettingStr = settingMgr.getSettingAsString(settingName);
        }
        catch(const TA_Core::UserSettingsException&)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "UserSettingsException");
            m_dataPointsSettingStr = "";
        }
        catch(...)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown Exception");
            m_dataPointsSettingStr = "";
        }

        FUNCTION_EXIT;
    }


    //
    // restoreUserSettings
    //
    void CHistoryViewerDlg::restoreUserSettings()
    {
        FUNCTION_ENTRY("restoreUserSettings");

        if(!m_dataPointsSettingStr.empty())
        {
            // Break the string up into a vector of strings
            std::string::size_type start = 0;
            std::string::size_type end;
            do
            {
	            end = m_dataPointsSettingStr.find('\\', start);
	            if(end != std::string::npos)
	            {
		            std::string thisValue(&m_dataPointsSettingStr[start], &m_dataPointsSettingStr[end]);
                    unsigned long dataPointKey = strtoul(thisValue.c_str(), NULL, 10);

                    if(!isDataPointAlreadyDisplayed(dataPointKey))
                    {
                        addDataPoint(dataPointKey);
                    }
                }
	            start = end + 1;
            } while(end != std::string::npos);
        }

        FUNCTION_EXIT;
    }


} // TA_App









