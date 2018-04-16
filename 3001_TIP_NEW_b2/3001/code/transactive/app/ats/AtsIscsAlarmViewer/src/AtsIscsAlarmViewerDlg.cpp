/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsIscsAlarmViewer/src/AtsIscsAlarmViewerDlg.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This is a dialog containing the integrated alarm list. It has two separate lists, the top list displaying
  * the ATS alarms and the bottom list displaying the ISCS alarms. There are a number of command line parameter
  * options available for displaying this dialog. These are:
  *
  * --ats-file-name-path    URL/path to the ats mimic file.
  * --no-status-bar         Removes the status bar from the dialog 
  * --no-title-bar          Removes the title bar from the dialog
  * --maximise              Launches the dialog in the maximised state
  *
  */

#include "stdafx.h"
#include "AtsIscsAlarmViewer.h"
#include "AtsIscsAlarmViewerDlg.h"

#include "bus/alarm/alarm_list_control/src/Filter.h"

#include  "bus/alarm/alarm_common/src/AlarmConstants.h"
#include "bus/alarm/alarm_list_control/src/AlarmActionFactory.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnComment.h"
#include "bus/alarm/alarm_list_control/src/Sort.h"
#include "bus/generic_gui/src/TransactiveMessage.h"
#include "bus/generic_gui/src/SystemControllerGroupUpdateSubscriber.h"
#include "bus/security/authentication_library/src/SessionInfo.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"

#include "bus/alarm/alarm_list_control/src/actions/ActionDisplayComment.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionSeparator.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionAcknowledgeAlarm.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedSchematic.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionCloseAndAckAlarm.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionIncidentLogSheet.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionJobRequest.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionPostponeDSS.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedPlanInstance.h" //TD15736
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedDSS.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedEvents.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedInspector.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedTrend.h"
//#include "bus/alarm/alarm_list_control/src/actions/ActionAcknowledgeAllAlarms.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedAvalancheEvents.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionSuppressAlarm.h"

#include "bus/alarm/alarm_list_control/src/columns/ColumnAck.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnState.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnSourceTimestamp.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnSeverity.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnAsset.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnDescription.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnValue.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnDecisionSupport.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnAvalanche.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnMms.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnComment.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnAckBy.h"

#include "core/exceptions/src/ObjectResolutionException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/DataException.h"

#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/ats/src/AtsMssZoneAccessFactory.h"
#include "core/data_access_interface/ats/src/IAtsMssZone.h"

#include "core/utilities/src/DebugUtil.h"
#include <afxole.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Bus::Sort;

namespace TA_IRS_App
{

	const int CAtsIscsAlarmViewerDlg::ALARMVIEWER_INITIALIZE_TIMER = 1;
    const ULONG CAtsIscsAlarmViewerDlg::PROFILE_LOCAL = 0;
    const int CAtsIscsAlarmViewerDlg::VERTICAL_OFFSET = 5;

    const char* CAtsIscsAlarmViewerDlg::RPARAM_NO_STATUS_BAR = "NoStatusBar";
    const char* CAtsIscsAlarmViewerDlg::RPARAM_NO_TITLE_BAR = "NoTitleBar";
    const char* CAtsIscsAlarmViewerDlg::RPARAM_MAXIMISED = "Maximise";
    const char* CAtsIscsAlarmViewerDlg::RPARAM_ATS_FILE_NAME_PATH = "AtsFileNamePath";

    /////////////////////////////////////////////////////////////////////////////
    // CAtsIscsAlarmViewerDlg dialog

    CAtsIscsAlarmViewerDlg::CAtsIscsAlarmViewerDlg(TA_Base_Bus::IGUIAccess* controlClass)
	    : TransActiveDialog(*controlClass, CAtsIscsAlarmViewerDlg::IDD, NULL),
          m_atsAlarmListFilename(""),
          m_sessionId(""),
          m_initialised(false),
          m_showStatusBar(true),
          m_showTitleBar(true),
          m_groupUpdateSubscriber(NULL)
    {
	    //{{AFX_DATA_INIT(CAtsIscsAlarmViewerDlg)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
	    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

        TA_Base_Bus::ResizingProperties properties;
        properties.canMaximise = true;
        properties.maxHeight = -1;
        properties.maxWidth = -1;
        properties.minHeight = 300;
        properties.minWidth = 500;
        setResizingProperties(properties);

        m_groupUpdateSubscriber = new TA_Base_Bus::SystemControllerGroupUpdateSubscriber(*this);
    }

    void CAtsIscsAlarmViewerDlg::DoDataExchange(CDataExchange* pDX)
    {
	    TransActiveDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(CAtsIscsAlarmViewerDlg)
		    // NOTE: the ClassWizard will add DDX and DDV calls here
        DDX_Control(pDX, IDC_ISCS_ALARM_LIST, m_iscsAlarmList);
        DDX_Control(pDX, IDC_EXPLORER, m_atsAlarmListCtrl);
        DDX_Control(pDX, IDC_STATUS_BAR, m_statusBar);
	    //}}AFX_DATA_MAP
    }

    BEGIN_MESSAGE_MAP(CAtsIscsAlarmViewerDlg, TransActiveDialog)
	    //{{AFX_MSG_MAP(CAtsIscsAlarmViewerDlg)
	    ON_WM_PAINT()
	    ON_WM_QUERYDRAGICON()
	    ON_WM_SHOWWINDOW()
	    ON_WM_SIZE()
	    ON_WM_CLOSE()
	    ON_WM_SYSCOMMAND()
        ON_MESSAGE(WM_ON_SYSTEM_DEGRADED,OnSystemInDegradedMode)
        ON_MESSAGE(WM_ON_SYSTEM_RECOVERED,OnSystemInNormalMode)
		ON_WM_TIMER() //TD17081
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CAtsIscsAlarmViewerDlg message handlers

    BOOL CAtsIscsAlarmViewerDlg::OnInitDialog()
    {
        FUNCTION_ENTRY("OnInitDialog");

	    TransActiveDialog::OnInitDialog();
		//TD20131,hongzhi,An "Server Busy" dialog is launched each time client Builder is launch from ISCS navigator
		COleMessageFilter* pFilter = AfxOleGetMessageFilter();
		pFilter->SetMessagePendingDelay(1000000); //permit to delay the apparition of the message box , the default seem to be 5000 milliseconds 

        // Set the icon for this dialog.  The framework does this automatically
        //  when the application's main window is not a dialog
        SetIcon(m_hIcon, TRUE);            // Set big icon
        SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);        // Set small icon

	    // Register for the SessionId run param
	    RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

        // Get the session id
        m_sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);

        // Get filename for ATS list and set m_atsAlarmListFilename here...
		//m_atsAlarmListFilename = RunParams::getInstance().get(RPARAM_ATS_FILE_NAME_PATH);
		//TD18980, hongzhi,ATS-ISCS Alarm Viewer Issues In IFAT Platform
		BuildATSFileName();
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "OnInitDialog()::ATS Alarm List Filename: %s", m_atsAlarmListFilename.c_str());

        // Check if the status bar is to be displayed
        if(RunParams::getInstance().isSet(RPARAM_NO_STATUS_BAR))
        {
            m_showStatusBar = false;
            m_statusBar.ShowWindow(SW_HIDE);
        }

        // Check if the title bar is to be displayed
        if(RunParams::getInstance().isSet(RPARAM_NO_TITLE_BAR))
        {
            m_showTitleBar = false;
            ModifyStyle(WS_CAPTION, 0);
        
            // Fake a resize to repaint the dialog (invalidate doesn't work here for some reason)
            CRect client;
            GetClientRect(client);
            ClientToScreen(client);
            MoveWindow(client.left, client.top, client.Width(), client.Height(), true); 
        }

        // Check if the dialog is to be maximised
        if(RunParams::getInstance().isSet(RPARAM_MAXIMISED))
        {
            ShowWindow(SW_SHOWMAXIMIZED);

            // Send a Show Window message here because one isn't sent when the window has the
            // MAXIMIZED style. OnShowWindow() needs to be called because this is where the URL
            // is loaded into the web browser control.
            PostMessage(WM_SHOWWINDOW, true, 0);
        }

        // TD 6950: Start receiving message informing GUI when system is degraded/recovered
        m_groupUpdateSubscriber->startNotifications();

        //TD17081++ 
		initListCtrlColumns();
		m_listHeader.SubclassDlgItem(0, &m_iscsAlarmList);		
		m_iscsAlarmList.setupAlarmList(*this, m_alarmColumns, TA_Base_Bus::COL_STATE, true);		
		m_listHeader.expandColumnToFitListWidth(m_iscsAlarmList, 4, true);

		SetTimer(ALARMVIEWER_INITIALIZE_TIMER, 0, NULL);
        //++TD17081        
        m_initialised = true;

        // Force a resize so the dialog elements are adjusted within the dialog
        PostMessage(WM_SIZE, NULL, NULL);
    
        FUNCTION_EXIT;
	    return TRUE;  // return TRUE  unless you set the focus to a control
    }

//TD17081
	void CAtsIscsAlarmViewerDlg::OnTimer(UINT nIDEvent)
	{

		if (nIDEvent == ALARMVIEWER_INITIALIZE_TIMER)
		{
			CWaitCursor waitCursor;

			setCurrentDescriptionText("Loading of alarms is in progress.");
	        // Initialise ISCS Alarm list
			initialiseIscsAlarmListCtrl();
			KillTimer(ALARMVIEWER_INITIALIZE_TIMER);
		}
	}

    // If you add a minimize button to your dialog, you will need the code below
    //  to draw the icon.  For MFC applications using the document/view model,
    //  this is automatically done for you by the framework.

    void CAtsIscsAlarmViewerDlg::OnPaint() 
    {
        FUNCTION_ENTRY("OnPaint");

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

            // Must flag internally to indicate the OnPaint notification
            // received within this block should not cause recursive OnPaint notifications
            if (!m_internalRepaint)
            {
                // Prompt an internal redraw (flag as such, so this redraw doesn't cause another, which
                // would cause an infinite loop)
                m_internalRepaint = true;

                CRect rc;    
                m_atsAlarmListCtrl.GetWindowRect(rc);        
                ScreenToClient(rc);
                // Lock redraw so don't see the pixel height increase
                m_atsAlarmListCtrl.SetRedraw(FALSE);
                // Perform a dummy resize to force a redraw condition - note that
                // Invalidate(TRUE), InvalidateRect(0, TRUE) do not cause a proper redraw
                // for these web browser controls (for whatever reason)
                m_atsAlarmListCtrl.MoveWindow(rc.left, rc.top, rc.Width(), rc.Height() + 1);
                // Unlock prior to restoring original size (hence will be drawn with proper dimensions -
                // so user doesn't see any flicker between the dummy resize and proper dimensions)
                m_atsAlarmListCtrl.SetRedraw(TRUE);
                // Restore original size
                m_atsAlarmListCtrl.MoveWindow(rc);    
            }
            else
            {
                // Need to flip internal paint states to avoid infinite loop in paint
                m_internalRepaint = false;
            }
	    }

        FUNCTION_EXIT;
    }

    // The system calls this to obtain the cursor to display while the user drags
    //  the minimized window.
    HCURSOR CAtsIscsAlarmViewerDlg::OnQueryDragIcon()
    {
	    return (HCURSOR) m_hIcon;
    }

    void CAtsIscsAlarmViewerDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
    {
        FUNCTION_ENTRY("OnShowWindow");

	    TransActiveDialog::OnShowWindow(bShow, nStatus);
	    
	    if (bShow == TRUE)
	    {
			if(fileExists(m_atsAlarmListFilename))
			{
				try
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ATS Alarm List Filename: %s", m_atsAlarmListFilename.c_str());

					VARIANT flags, targetFrameName, postData, headers;
					m_atsAlarmListCtrl.Navigate(m_atsAlarmListFilename.c_str(),	&flags, &targetFrameName, &postData, &headers);
				}
				catch(std::exception& ex)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what());
				}
				catch(...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown exception caught while initialising CWebBrowser controls");
				}
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "File doesn't exist: %s", m_atsAlarmListFilename.c_str());
			}
	    }

        FUNCTION_EXIT;
    }

    void CAtsIscsAlarmViewerDlg::OnSize(UINT nType, int cx, int cy) 
    {
        FUNCTION_ENTRY("OnSize");

	    TransActiveDialog::OnSize(nType, cx, cy);
	    
        if(m_initialised)
        {
            resizeDialogElements(cx, cy);
        }
    
        FUNCTION_EXIT;
    }

    void CAtsIscsAlarmViewerDlg::OnClose() 
    {
        FUNCTION_ENTRY("OnClose");

        if(m_groupUpdateSubscriber != NULL)
        {
            delete m_groupUpdateSubscriber;
            m_groupUpdateSubscriber = NULL;
        }

	    RunParams::getInstance().deregisterRunParamUser(this);
		//m_atsAlarmListCtrl.DestroyWindow();		//hongzhi

		//TD18095, jianghp, to fix the performance of showing manager application
		TransActiveDialog::DestroyWindow();
        FUNCTION_EXIT;
    }


    LRESULT CAtsIscsAlarmViewerDlg::OnSystemInDegradedMode(WPARAM wParam, LPARAM lParam)
    {
	    LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnSystemInDegradedMode" );

        // Retrieve the locations related to the current profile. We still only want alarms
        // from these locations.
        TA_Base_Bus::Filter preFilter;
	    std::vector<unsigned long> locations;
	    initPreFiltering(preFilter, locations);

        // Clear pre-filter as we don't want rights to be checked before an alarm is displayed.
        preFilter.removeAll();
	    m_iscsAlarmList.updatePreFilterLocationsAndSession(preFilter, locations, m_sessionId);
    
	    LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnSystemInDegradedMode" );
        return NULL;	// This line must be included. Everything above is application specific
    }


    LRESULT CAtsIscsAlarmViewerDlg::OnSystemInNormalMode(WPARAM wParam, LPARAM lParam)
    {
	    LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnSystemInNormalMode" );

	    // Reset Pre-Filter
        TA_Base_Bus::Filter preFilter;
	    std::vector<unsigned long> locations;
	    initPreFiltering(preFilter, locations);
	    m_iscsAlarmList.updatePreFilterLocationsAndSession(preFilter, locations, m_sessionId);
		    
	    LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnSystemInNormalMode" );
        return NULL;	// This line must be included. Everything above is application specific

    }

    void CAtsIscsAlarmViewerDlg::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY("onRunParamChange");

        if(name.compare(RPARAM_SESSIONID) == 0)
	    {
		    // Store the new session id.
		    m_sessionId = value;

		    // Update the pre-filter, locations and session ID.
		    TA_Base_Bus::Filter preFilter;
		    std::vector<unsigned long> locations;
		    initPreFiltering(preFilter, locations);
		    m_iscsAlarmList.updatePreFilterLocationsAndSession(preFilter, locations, m_sessionId);

            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "RunParamChange received for session ID. New sesion ID is %s", m_sessionId.c_str());
	    }
	    
        FUNCTION_EXIT;
    }

    void CAtsIscsAlarmViewerDlg::serverUp()
    {
        FUNCTION_ENTRY("serverUp");

        m_statusBar.SetWindowText("Server connection re-established.");

	    TA_Base_Bus::TransActiveMessage userMsg;
	    UINT selectedButton = userMsg.showMsgBox(IDS_UI_040029);    // TODO: Check this message

        FUNCTION_EXIT;
    }

    void CAtsIscsAlarmViewerDlg::serverDown()
    {
        FUNCTION_ENTRY("serverDown");

        m_iscsAlarmList.DeleteAllItems();
	    m_statusBar.SetWindowText("Server down.");

	    TA_Base_Bus::TransActiveMessage userMsg;
	    UINT selectedButton = userMsg.showMsgBox(IDS_UI_040028);    // TODO: Check this message

        FUNCTION_EXIT;
    }


    void CAtsIscsAlarmViewerDlg::updateAlarmCounts(unsigned int totAlarms, unsigned int totUnacknowledgedAlarms, bool exceededMaxDisplayAlarm, bool isLoadingAlarms)
    {
        // Not used
    }


    void CAtsIscsAlarmViewerDlg::updateAlarmSound(unsigned long severity, bool wereAlarmsAdded)
    {
        // Not used
    }


    void CAtsIscsAlarmViewerDlg::updateAvailableAlarmActions(const ItemAvailability& availability)
    {
        // Not used
    }

    
    void CAtsIscsAlarmViewerDlg::setCurrentDescriptionText(const CString& description)
    {
        FUNCTION_ENTRY("setCurrentDescriptionText");

        m_statusBar.SetWindowText(description);

        FUNCTION_EXIT;
    }


    CProgressCtrl* CAtsIscsAlarmViewerDlg::getProgressBar()
    {
        return NULL;
    }


    void CAtsIscsAlarmViewerDlg::finishedWithProgressBar()
    {
        // Not used
    }

    
    void CAtsIscsAlarmViewerDlg::notifyNoMatchingAlarms()
    {
        // Not used
    }


    void CAtsIscsAlarmViewerDlg::notifySystemStateDegraded()
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "notifySystemStateDegraded" );
	    PostMessage(WM_ON_SYSTEM_DEGRADED, NULL, NULL);
	    LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "notifySystemStateDegraded" );
    }


    void CAtsIscsAlarmViewerDlg::notifySystemStateNormal()
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "notifySystemStateNormal" );
	    PostMessage(WM_ON_SYSTEM_RECOVERED, NULL, NULL);
	    LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "notifySystemStateNormal" );
    }


    void CAtsIscsAlarmViewerDlg::initialiseIscsAlarmListCtrl()
    {
        FUNCTION_ENTRY("initialiseIscsAlarmListCtrl");

        TA_Base_Bus::Filter preFilter;
	    std::vector<unsigned long> locations;    
	    initPreFiltering(preFilter, locations);

        // TD 6950: Check if the system is currently in degraded mode. If so, clear the pre-filter.
        updatePreFilterAccordingToSystemState(preFilter);

        TA_Base_Bus::Filter filter;

        TA_Base_Bus::AlarmActionFactory* factory = 
		    new TA_Base_Bus::AlarmActionFactory(initListCtrlActions());

		//TD17081 - move to OnInitDialog
	    // initListCtrlColumns();
		
		//TD15005++
		//TD 3354: To take control of the column resize by subclass the m_listHeader class to the alarm list ctrl
		// m_listHeader.SubclassDlgItem(0, &m_iscsAlarmList);
		//++TD15005

		// m_iscsAlarmList.startAlarmList(*this, *factory, m_alarmColumns, preFilter, locations, TA_Base_Bus::COL_STATE, true, filter);
		m_iscsAlarmList.startAlarmList( *factory, preFilter, locations, filter);

		//TD17081 - move to OnInitDialog
		//TD15005++
		//From the m_alarmColumns.push_back() calls, COL_DESCRIPTION is 5th (4th???) item
		//m_listHeader.expandColumnToFitListWidth(m_iscsAlarmList, 4, true);
		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "TD15005: expandColumnToFitListWidth()" );
		//++TD15005        
		
        FUNCTION_EXIT;
    }

    void CAtsIscsAlarmViewerDlg::initPreFiltering(TA_Base_Bus::Filter& preFilter, std::vector<unsigned long>& locations)
    {
        FUNCTION_ENTRY("initPreFiltering");

        locations.clear();

	    TA_Base_Bus::SessionInfo sessionInfo;

	    try
	    {
		    // Get some info about the current session.
		    TA_Base_Bus::AuthenticationLibrary authLibrary(true);
		    sessionInfo = authLibrary.getSessionInfo(m_sessionId, m_sessionId);
	    }
	    catch(TA_Base_Core::AuthenticationAgentException&)
	    {
		    LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationAgentException", "Could not retrieve session information");
		    return;
	    }
	    catch(TA_Base_Core::ObjectResolutionException&)
	    {
		    LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve Authentication Agent");
		    return;
	    }

	    // The prefilter requires the session id.
	    preFilter.addFilter(TA_Base_Bus::Filter::FILTER_PROFILE, 
						    TA_Base_Bus::Filter::COMPARE_EQUAL, m_sessionId);

	    bool isCentralProfile = false;
	    TA_Base_Core::IProfile* profile;

	    for (std::vector<unsigned long>::iterator iter = sessionInfo.ProfileId.begin();
		    iter != sessionInfo.ProfileId.end(); iter++)
	    {
		    try
		    {
				profile = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(*iter);

			    if (profile->getType() == PROFILE_LOCAL)
			    {
				    locations.push_back(sessionInfo.LocationId);
			    }
			    else  // PROFILE_CENTRAL
			    {
				    isCentralProfile = true;
			    }

			    delete profile;
		    }
		    catch(TA_Base_Core::DataException&)
		    {
			    delete profile;
			    LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve profile information");
		    }
		    catch(TA_Base_Core::DatabaseException&)
		    {
			    delete profile;
			    LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve profile information");
		    }
	    }

	    if (isCentralProfile)
	    {
		    // Central profiles require all locations.
		    locations.clear();
	    }

        FUNCTION_EXIT;
    }

    std::vector<TA_Base_Bus::IAlarmAction*> CAtsIscsAlarmViewerDlg::initListCtrlActions()
    {
        FUNCTION_ENTRY("initListCtrlActions");

        std::vector<TA_Base_Bus::IAlarmAction*> actions;

	    // Add actions available from the context menu.
        actions.push_back( new TA_Base_Bus::ActionAcknowledgeAlarm() );
	    actions.push_back( new TA_Base_Bus::ActionDisplayComment() );
        actions.push_back( new TA_Base_Bus::ActionSeparator() );
        actions.push_back( new TA_Base_Bus::ActionRelatedSchematic() );
        actions.push_back( new TA_Base_Bus::ActionRelatedInspector() );
        actions.push_back( new TA_Base_Bus::ActionRelatedEvents() );
        actions.push_back( new TA_Base_Bus::ActionSeparator() );
        actions.push_back( new TA_Base_Bus::ActionRelatedDSS() );
        actions.push_back( new TA_Base_Bus::ActionPostponeDSS() );
		actions.push_back( new TA_Base_Bus::ActionRelatedPlanInstance() ); 	//TD15736
        actions.push_back( new TA_Base_Bus::ActionSeparator() );
        actions.push_back( new TA_Base_Bus::ActionJobRequest() );
        actions.push_back( new TA_Base_Bus::ActionRelatedTrend() );
        actions.push_back( new TA_Base_Bus::ActionRelatedAvalancheEvents() );
        actions.push_back( new TA_Base_Bus::ActionIncidentLogSheet() );
        actions.push_back( new TA_Base_Bus::ActionSeparator() );
        actions.push_back( new TA_Base_Bus::ActionCloseAndAckAlarm() );
	    actions.push_back( new TA_Base_Bus::ActionSuppressAlarm() );

	    // Add actions not available from the context menu.
	    //actions.push_back( new TA_Base_Bus::ActionAcknowledgeAllAlarms(this->GetSafeHwnd(),false) );

	    FUNCTION_EXIT;
	    return actions;
    }

    void CAtsIscsAlarmViewerDlg::initListCtrlColumns()
    {
        FUNCTION_ENTRY("initListCtrlColumns");

        TA_Base_Bus::Sort descSort;
        TA_Base_Bus::Sort ascSort;

        //as per TD 12743/12744
        //we now use nuber of characters to specify initial width of coulmns
        //for Ack By and Asset columns
        CString widestCharacter = 'W';
        unsigned long widestCharWidth = m_iscsAlarmList.GetStringWidth (widestCharacter);
		    
		// Add disabled state and ack columns so that default sort will work.
		//marvin++ wrong default sort
	/*	ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		*/
		//TD 3354: This col is added to list but hidden from user to remove the black line affecting 1st col 
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnState(0, true, NOSORT_BY_SUBSYSTEM, NULL));
	//	m_alarmColumns.push_back(new TA_Base_Bus::ColumnAck(0, false, SORT_BY_ACK));

		// Source timestamp
	/*	ascSort.removeAll();
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		descSort = reverseSort(ascSort);*/
		unsigned long ulTimestampWidth = TA_Base_Bus::AlarmConstants::DEFAULT_TIMESTAMPCOLWIDTH;
	/*	if (RunParams::getInstance().isSet(TA_Base_Bus::AlarmConstants::RPARAM_TIMESTAMPCOLWIDTH.c_str()))
		{
			ulTimestampWidth = strtoul(RunParams::getInstance().get(TA_Base_Bus::AlarmConstants::RPARAM_TIMESTAMPCOLWIDTH.c_str()).c_str(), NULL, 0);
		}*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnSourceTimestamp(ulTimestampWidth, true,SORT_BY_DATETIME, NULL));
	        		
		// Severity
	/*	ascSort.removeAll();
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,	Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnSeverity(40, true, SORT_BY_SEVERITY, NULL)); //TD15005

		// Asset
	/*	ascSort.removeAll();
		ascSort.addSort(Sort::SORT_ASSET_NAME,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);	*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnAsset(210, true, SORT_BY_ASSET, NULL)); //TD15005

		// Description
	/*	ascSort.removeAll();
		ascSort.addSort(Sort::SORT_DESCRIPTION, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);*/
		//TD12744 / TD12743 / TD15005(4th) ++
		//will automatically be adjusted by expandColumnToFitListWidth
		//m_alarmColumns.push_back(new TA_Base_Bus::ColumnDescription(504, true, &ascSort, &descSort, NULL)); 
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnDescription(504, true, SORT_BY_DESCRIPTION, NULL));  //tianxiang change column width
		//++ TD12744 / TD12743 / TD15005(4th)

		// Value
	/*	ascSort.removeAll();
		ascSort.addSort(Sort::SORT_VALUE, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnValue(140, true, SORT_BY_VALUE, NULL));

		// TD #11287
		// Changed column order from DAM to MDA
		// MMS	
	/*	ascSort.removeAll();
		ascSort.addSort(Sort::SORT_MMS, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnMms(24, true, SORT_BY_MMS, NULL));

		// DSS	
	/*	ascSort.removeAll();
		ascSort.addSort(Sort::SORT_DSS, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnDecisionSupport(22, true, SORT_BY_DSS, NULL));

		// Avalanche	
		/*ascSort.removeAll();
		ascSort.addSort(Sort::SORT_AVALANCHE, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnAvalanche(22, true, SORT_BY_AVALANCHE, NULL));

		// Acknowledged by
	/*	ascSort.removeAll(); 
		ascSort.addSort(Sort::SORT_ACK_BY,		Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnAckBy(10 * widestCharWidth, true, SORT_BY_ACK, NULL));

		// Comment	
	/*	ascSort.removeAll();
		//TD #12719++
		ascSort.addSort(Sort::SORT_COMMENT,		Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);
		//TD ++#12719
		TA_Base_Bus::IAlarmAction* doubleClickAction = new TA_Base_Bus::ActionDisplayComment();*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnComment(24, true, SORT_BY_COMMENTS, NULL));
        FUNCTION_EXIT;
    }


    TA_Base_Bus::Sort CAtsIscsAlarmViewerDlg::reverseSort(const TA_Base_Bus::Sort& sort)
    {
        using TA_Base_Bus::Sort;

	    Sort reverseSort;
	    Sort::SortDirection oppDirection;

	    std::vector<Sort::SortDetails> sortDetails = sort.getSort();

	    for (std::vector<Sort::SortDetails>::iterator iter = sortDetails.begin();
		    iter != sortDetails.end(); iter++)
	    {
		    oppDirection = (iter->direction == Sort::SORT_ASCENDING ? 
							    Sort::SORT_DESCENDING : Sort::SORT_ASCENDING);
		    reverseSort.addSort(iter->type, oppDirection);
	    }

	    return reverseSort;
    }


    void CAtsIscsAlarmViewerDlg::resizeDialogElements(int cx, int cy)
    {
        // Get coordinates of main window
        CRect mainWindowRect;
        GetClientRect(mainWindowRect);        
    
        int statusBarHeight = 0;
        if(m_showStatusBar)
        {
            // Get the size of the status bar control (note that we just need the size, not the position)
            CRect statusBarRect;
            if (m_statusBar.m_hWnd != NULL)
            {
                m_statusBar.GetWindowRect(statusBarRect);
                statusBarHeight = statusBarRect.Height();
                m_statusBar.MoveWindow(0, mainWindowRect.Height() - statusBarHeight, mainWindowRect.Width(), statusBarHeight, true);
            }
        }

        int titleBarHeight = 0;
        if(m_showTitleBar)
        {
            // Get coordinates of title bar
            TITLEBARINFO ti;
//             GetTitleBarInfo(this->m_hWnd, &ti);
			GetTitleBarInfo( &ti );
            CRect titleBarRect = ti.rcTitleBar;
            ScreenToClient(&titleBarRect);

            titleBarHeight = titleBarRect.Height();
        }

        int width = mainWindowRect.right;
//tianxiang fix release version do not display alarms			
        int height = ((mainWindowRect.Height() - statusBarHeight) / 2) - (0.5 * VERTICAL_OFFSET);
    
//        if(m_atsAlarmListCtrl)
//       {
            int atsX = mainWindowRect.left; 
            int atsY = mainWindowRect.top;
			try
			{
				m_atsAlarmListCtrl.MoveWindow(atsX, atsY, width, height);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"move ATS window to -- %d -- %d the size is (%d) (%d)" , 
							atsX,atsY,width,height);
			}
			catch(...)
			{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"unknow error when move ats");
			}

//        }

//        if(m_iscsAlarmList)
//        {
           int iscsX = mainWindowRect.left;
           int iscsY = mainWindowRect.bottom - statusBarHeight - height;
			try
			{
				m_iscsAlarmList.MoveWindow(iscsX, iscsY, width, height);
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"move ISCS window to -- %d -- %d the size is (%d) (%d)" , 
							iscsX,iscsY,width,height);
			}
			
			catch(...)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"unknow error when move iscs");
			}
//        }
//tianxiang fix release version do not display alarms	
    }


    void CAtsIscsAlarmViewerDlg::updatePreFilterAccordingToSystemState(TA_Base_Bus::Filter& preFilter)
    {
        // TD 6950: Check if the system is currently in degraded mode. If so, clear the pre-filter.
        std::string groupsOffline = RunParams::getInstance().get(RPARAM_GROUPSOFFLINE);
        if(!groupsOffline.empty())
        {
            int pos = 0;
            while(pos != std::string::npos)
            {
                pos = groupsOffline.find(",", 0);
                std::string group = groupsOffline.substr(0, pos);

                if(group == m_groupUpdateSubscriber->getCentralServerGroupName())
                {
                    // System is degraded
                    preFilter.removeAll();
                    return;
                }

                if(pos != std::string::npos)
                {
                    groupsOffline = groupsOffline.substr(pos+1, groupsOffline.size() - (pos + 1));
                }
            }
        }
    }

	//TD18980, hongzhi,ATS-ISCS Alarm Viewer Issues In IFAT Platform
	bool CAtsIscsAlarmViewerDlg::fileExists(const std::string& fileName)
	{
		CString filePath(fileName.c_str());
    
		HANDLE fileHandle = CreateFile(
			filePath, 
			GENERIC_READ, 
			FILE_SHARE_READ, 
			NULL, 
			OPEN_EXISTING,  
			FILE_ATTRIBUTE_NORMAL, 
			0); 

		bool fileExists = false;
		if(fileHandle != INVALID_HANDLE_VALUE )
		{
			fileExists = true;
		}

		CloseHandle(fileHandle);

		return fileExists;
	}

	//TD18980, hongzhi,ATS-ISCS Alarm Viewer Issues In IFAT Platform
	void CAtsIscsAlarmViewerDlg::BuildATSFileName()
	{
		TA_Base_Core::IConsole* console = NULL;
		TA_Base_Core::ILocation* location = NULL;
		TA_IRS_Core::IAtsMssZones mssZones;

		try
		{
			std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

			console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);

			if (console != NULL)
			{
				// Get physical location
				unsigned long locationKey = console->getLocation();

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
							"Located local console location - session %s, console %s, location %d", 
							sessionId.c_str(), console->getName().c_str(), locationKey);

				location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationKey);

				std::string path = "";

				if (location != NULL)
				{
					if (location->getName() == "OCC")
					{
						path = "C:/ATS_GUI/OCC_MFT/Mimic Files/";
					}
					else if (location->getName() == "KCD")
					{
						path = "C:/ATS_GUI/DEPOT_MFT/Mimic Files/";
					}
					else
					{
						// Need to find the MSS
						mssZones = TA_IRS_Core::AtsMssZoneAccessFactory::getInstance().getAtsMssZonesWhere(false, locationKey);
						if (mssZones.size() == 1)
						{
							path = "C:/ATS_GUI/Station" + mssZones[0]->getMssZone() + "_MFT/Mimic Files/";
						}
					}
				}

				if (path != "")
				{
					m_atsAlarmListFilename = path + "AlarmsHalfList.fvm";

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
								"Located ATS Banner mimic filenames - list %s", 
								m_atsAlarmListFilename.c_str());
				}
			}
		}
		catch(TA_Base_Core::DataException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
		}
		catch(TA_Base_Core::DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
		}
		catch(std::exception& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "std::exception", ex.what());
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown exception caught while constructing ATS Alarm List");
		}

		delete console;
		delete location;

		for (TA_IRS_Core::IAtsMssZonesIt it = mssZones.begin();
			 it != mssZones.end();
			 it++)
		{
			if (*it != NULL)
			{
				delete (*it);
				(*it) = NULL;
			}
		}
	}
} // end TA_IRS_App
