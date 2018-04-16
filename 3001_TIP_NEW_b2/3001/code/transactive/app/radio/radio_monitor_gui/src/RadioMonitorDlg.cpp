/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/radio/radio_monitor_gui/src/RadioMonitorDlg.cpp $
  * @author Jade Lee
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * Main Dialog class for the Radio Monitor application
  *
  */

#include "app/radio/radio_monitor_gui/src/stdafx.h"

#include <math.h>
#include <algorithm>

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitor.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitorDlg.h"
#include "app/radio/radio_monitor_gui/src/SearchPageHelper.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "app/radio/radio_monitor_gui/src/RightsChangedWindowNotificationHandler.h"
#include "bus/mfc_extensions/src/selection_helpers/SelectionHelper.h"
#include "bus/generic_gui/src/HelpLauncher.h"
//#include "app/radio/radio_monitor_gui/src/RadioMonitorTestData.h"
#include "app/radio/radio_monitor_gui/src/REBProgressManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define WM_USER_INIT (WM_USER + 1001)

namespace TA_IRS_App
{

    static const char ONE_SHOT_MONITOR_TEXT[] = "Status: One Shot";
    static const char CONTINUOUS_MONITOR_TEXT[] = "Status: Monitoring";
    static const char IDLE_MONITOR_TEXT[] = "Status: Monitoring stopped";

    int CALLBACK RadioMonitorDlg::SelectedListSort(LPARAM data1, LPARAM data2, LPARAM listCtrl)
    {
        SelectedEntity* e1 = reinterpret_cast<SelectedEntity*>(data1);
        SelectedEntity* e2 = reinterpret_cast<SelectedEntity*>(data2);
        
        // Sort alphabetically by ID (contained in column)      
        return strcmp(e1->id.c_str(), e2->id.c_str());
    }     


    /////////////////////////////////////////////////////////////////////////////
    // RadioMonitorDlg dialog
    RadioMonitorDlg::RadioMonitorDlg(TA_Base_Bus::IGUIAccess& controlClass) : 
      TransActiveDialog(controlClass, RadioMonitorDlg::IDD, NULL),
      m_wantToShow( false ),
      m_initialDisplay( false ),
      m_rightsChangeHandler(0),
      m_searchPanel(SearchPageHelper::getInstance()),
	  m_initReady(false)
    {      
	    //{{AFX_DATA_INIT(RadioMonitorDlg)
	    //}}AFX_DATA_INIT
    }


    RadioMonitorDlg::~RadioMonitorDlg()
    {        
        TA_ASSERT(0 == m_rightsChangeHandler, "Rights change handler should be deleted in OnDestroy");

        try
        {
            // Avoid any destruction here, at such a late stage - core framework is mostly destroyed
        }     
        catch (...)
        {
             LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    void RadioMonitorDlg::DoDataExchange(CDataExchange* pDX)
    {
	    TransActiveDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(RadioMonitorDlg)
        DDX_Control(pDX, IDC_SELECTED_LIST, m_selectedList);
        
        DDX_Control(pDX, IDC_TEXT_MONITORSTACK, m_monitorStackText);
        DDX_Control(pDX, IDC_MONITOR_LIST, m_monitoredCallStack);
        DDX_Control(pDX, IDC_LISTEN_LIST, m_listenCallStack);

        DDX_Control(pDX, IDC_REMOVE_BUTTON, m_removeButton);
        DDX_Control(pDX, IDC_ONE_SHOT_BUTTON, m_oneShotButton);
        DDX_Control(pDX, IDC_START_MONITORING_BUTTON, m_startMonitoringButton);
        DDX_Control(pDX, IDC_STOP_MONITORING_BUTTON, m_stopMonitoringButton);
        DDX_Control(pDX, IDC_START_LISTENING_BUTTON, m_startListeningButton);
        DDX_Control(pDX, IDC_STOP_LISTENING_BUTTON, m_stopListeningButton);
        
        DDX_Control(pDX, IDC_JOIN_CALL_BUTTON_M, m_joinMonitorCallButton);
        DDX_Control(pDX, IDC_END_CALL_BUTTON_M, m_endMonitorCallButton);

        DDX_Control(pDX, IDC_JOIN_CALL_BUTTON_L, m_joinListenCallButton);
        DDX_Control(pDX, IDC_END_CALL_BUTTON_L, m_endListenCallButton);
        //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(RadioMonitorDlg, TransActiveDialog)
	    //{{AFX_MSG_MAP(RadioMonitorDlg)
	    ON_MESSAGE(WM_RM_ERROR_MESSAGE, OnErrorMessage)
	    ON_WM_DESTROY()
	    ON_WM_SHOWWINDOW()
	    ON_WM_SIZE()
	    ON_WM_MOVE()
        ON_WM_ACTIVATEAPP( )
        ON_MESSAGE(WM_RM_RIGHTS_CHANGE, OnRightsChanged)
       
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_SELECTED_LIST, OnItemChangedSelectedList)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_MONITOR_LIST, OnItemChangedMonitorList)
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_LISTEN_LIST, OnItemChangedListenList)
        	
        ON_BN_CLICKED(IDC_ABOUTBUTTON, OnAboutbutton)
	    
        ON_BN_CLICKED(IDC_REMOVE_BUTTON, OnRemoveButton)
        
        ON_BN_CLICKED(IDC_ONE_SHOT_BUTTON, OnOneShotButton)
        ON_BN_CLICKED(IDC_START_MONITORING_BUTTON, OnStartMonitoringButton)
        ON_BN_CLICKED(IDC_STOP_MONITORING_BUTTON, OnStopMonitoringButton)

        ON_BN_CLICKED(IDC_START_LISTENING_BUTTON, OnStartListeningButton)
        ON_BN_CLICKED(IDC_STOP_LISTENING_BUTTON, OnStopListeningButton)
        ON_BN_CLICKED(IDC_END_CALL_BUTTON_M, OnEndMonitorCallButton)
        ON_BN_CLICKED(IDC_JOIN_CALL_BUTTON_M, OnJoinMonitorCallButton)
        ON_BN_CLICKED(IDC_END_CALL_BUTTON_L, OnEndListenCallButton)
        ON_BN_CLICKED(IDC_JOIN_CALL_BUTTON_L, OnJoinListenCallButton)
        
        ON_BN_CLICKED(IDC_CLOSE_BTN, OnCloseBtn)
        ON_BN_CLICKED(IDC_HELP_BUTTON, OnHelpBtn)

		ON_WM_SHOWWINDOW()
		ON_MESSAGE(WM_USER_INIT, OnUserInit)

        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    BOOL RadioMonitorDlg::OnInitDialog() 
    {
        m_rightsChangeHandler =  new RightsChangedWindowNotificationHandler(m_hWnd);

        //
        // C830 SPECIFIC!!!
        // Resize the window to width:1280, height: 803
        //
        SetWindowPos(NULL,0,0,1280,803,SWP_NOMOVE|SWP_NOZORDER);

        TA_Base_Bus::ResizingProperties rp;
        rp.canMaximise = false;
        rp.maxWidth = 1280;
        rp.maxHeight = 1024;

        // The minimum allowed dimensions are initial (startup) dimensions
        CRect rc;
        GetClientRect(rc);
        rp.minWidth = rc.Width();
        rp.minHeight = rc.Height();

        setResizingProperties(rp);

        setupSearchPage();

        // Go and create dialog
        TransActiveDialog::OnInitDialog();
        
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }
	LRESULT RadioMonitorDlg::OnUserInit( WPARAM wParam, LPARAM lParam)
	{
		CWaitCursor cos;
		UserInit();
		cos.Restore();
		return 0;
	}

	void RadioMonitorDlg::UserInit()
	{
		//show the initialising dialog
		m_initReady = true;

		REBProgressManager mgr;

		//mgr.Init(this->m_hWnd);	//
		mgr.Init(GetDesktopWindow()->m_hWnd);
		mgr.EnableProgress(false);
		mgr.SetVisible(TRUE);	
		mgr.EnableCancel(FALSE);
		mgr.BeginProgressDialog();

		mgr.SetStaticText(0, "Initializing the Radio Monitor...");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Created Init Dialog");

		//initialise the search page data
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Before m_searchPanel.Init()");

		m_searchPanel.Init();
		m_monitoredCallStack.Init();
		m_listenCallStack.Init();

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"after m_searchPanel.Init()");

        // Start up maximised
        m_wantToShow = true;

        // Set window handle AFTER responding to any display tags - if
        // an AfxMessageBox is present while we areminimized (result from
        // Display=HIDE message), the message box will (has in past) disappear
        // off the screen (and shift into non-screen area at top left).
        // Probably as a result of the minimize call being dispatched to
        // the currently active message box
        RadioMonitorErrorHandler::getInstance().setPrimaryWindowHandle(m_hWnd);
        
        // Must apply these flags to correctly intercept selchanging states
        m_monitoredCallStack.setItemChangeRequestForwarding(TRUE);

        //m_mlSelectionController.registerList(&m_monitoredCallStack);
        //m_mlSelectionController.registerList(&m_listenCallStack);
    
        refreshButtonEnabledStates();
            
        m_selectedList.InsertColumn(0, "Name");
        m_selectedList.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
        m_selectedList.SetExtendedStyle(LVS_EX_FULLROWSELECT);

        m_monitorStackText.SetWindowText(IDLE_MONITOR_TEXT);

        /*        
        RadioMonitor::RadioMonitorTestData::getInstance().beginThread(
                                &m_monitoredCallStack.getSubscriber(),
                                &m_listenCallStack.getSubscriber(),
                                &m_searchPanel.DEBUG_getSubscriber());
        */

	}


    void RadioMonitorDlg::setupSearchPage()
    {            
        CWnd* marker = GetDlgItem(IDC_SEARCH_PANEL);

        if (0 == marker)
        {
            throw TA_Base_Core::TransactiveException("Search panel marker was null");
        }                  
	    
        CRect markerRect;
	    marker->GetWindowRect(markerRect);
	    marker->DestroyWindow();

        // Convert into RadioMonitorDlg client co-ordinates
	    ScreenToClient(markerRect);
              
        // We use our own resource definition for this panel (size is different)
        // Additionally the text on the add button is different..
        // Note that we can't use the resource provided by CSearchPage, because
        // it hasn't been linked to this project
	    m_searchPanel.Create(IDD_RADIO_MONITOR_FACILITYSEARCH, this);
        m_searchPanel.ShowWindow(SW_SHOW);
        m_searchPanel.MoveWindow(markerRect);
    }


    void RadioMonitorDlg::OnActivateApp( BOOL bActive, /*HTASK*/DWORD hTask )
    {
        if (!bActive)
        {
            // De-activation, no longer want to show the application when it's de-activated
            if (m_initialDisplay)
            {
                m_wantToShow = false;
            }
        }

        TransActiveDialog::OnActivateApp( bActive, (DWORD)hTask );
    }


    afx_msg LRESULT RadioMonitorDlg::OnErrorMessage(UINT wParam, LONG lParam)
    {
        // This is called by RadioMonitorErrorHandler class, to allow modal error
        // windows to be displayed from the main loop.
        // Serious potential problems exist if a modal error dialog pops up 
        // (with notifications continuing to be processed), since while
        // the modal error message is display, further execution may lead
        // to the same position in code, breaking NonReEntrantThreadlockable locks..
        RadioMonitorErrorHandler::getInstance().displayModalError(
                RadioMonitorErrorHandler::EErrorId(wParam));

        // Confirm afterwards (to avoid repetitive displaying of errors)
        RadioMonitorErrorHandler::getInstance().confirmReceiptOfRequest(wParam);
    
        return 0;
    }


    BOOL RadioMonitorDlg::DestroyWindow() 
    {		
        FUNCTION_ENTRY("DestroyWindow()");


        FUNCTION_EXIT;

	    return TransActiveDialog::DestroyWindow();
    }


    void RadioMonitorDlg::OnRemoveButton()
    {
        // Before removing items, gather currently selected items for later usage
        RadioMonitorModel::TSISequence tsiSelection = getSelectedListTSIs(SELECTED_ONLY);

        // Go back through in reverse order, removing all selected items
        for (int nItem = m_selectedList.GetItemCount() - 1; nItem >= 0; nItem --)
        {
            if (m_selectedList.GetItemState(nItem, LVIS_SELECTED) == LVIS_SELECTED)
            {
                m_selectedList.DeleteItem(nItem);
            }
        }

        if (m_model.isCurrentlyMonitoring())
        {
            ////////////
            // Send through a cease monitoring request for the specific items that were 
            // just removed, iff actively monitoring
            bool leaveMonitoringState = false;

            if (0 == m_selectedList.GetItemCount())
            {
                // Once all items are removed from the selected list, automatically
                // leave the monitoring state (so if more items are added again, we
                // don't automatically begin monitoring them)
                leaveMonitoringState = true;
            }

            m_model.requestStopMonitoring(tsiSelection, leaveMonitoringState); 

            // When an item is removed from selected list, forcibly clear
            // it from the model (discard its state) - the other option would
            // be to disallow its removal from the selected list if can't
            // cancel it's monitoring state (but this could be annoying)
            m_model.forceMonitorSetRemoval(tsiSelection);
        }

        refreshButtonEnabledStates();
    }


    void RadioMonitorDlg::OnOneShotButton()
    { 
        CWaitCursor wait;
        
        // Clear the monitored call stacks immediately in anticipation of response from agent
        m_monitoredCallStack.resetContents();
        m_listenCallStack.resetContents();

        // Send users through to agent requesting one-shot monitoring
        RadioMonitorModel::TSISequence tsiSequence = getSelectedListTSIs();
        std::set<std::string> success = m_model.requestOneShotMonitoring(tsiSequence);

        // Highlight in red all those items that weren't successful
        highlistSelectedListItems(success, SUCCESS);

        m_monitorStackText.SetWindowText(ONE_SHOT_MONITOR_TEXT);

        refreshButtonEnabledStates();
    }


    void RadioMonitorDlg::OnStartMonitoringButton()
    {
        CWaitCursor wait;        

        // Clear the monitored call stacks immediately in anticipation of response from agent
        m_monitoredCallStack.resetContents();
        m_listenCallStack.resetContents();

        RadioMonitorModel::TSISequence tsiSequence = getSelectedListTSIs();
        m_model.requestStartMonitoring(tsiSequence, true);

        // Highlight all items that failed to be monitored
        highlistSelectedListItems(m_model.getMonitoredSet(), SUCCESS);

        m_monitorStackText.SetWindowText(CONTINUOUS_MONITOR_TEXT);
    
        refreshButtonEnabledStates();
    }


    void RadioMonitorDlg::OnStopMonitoringButton()
    {     
        CWaitCursor wait;
        
        RadioMonitorModel::TSISequence tsiSequence = getSelectedListTSIs();
        m_model.requestStopMonitoring(tsiSequence, true); 

        // All items still being monitored failed to have their monitored state cancelled
        highlistSelectedListItems(m_model.getMonitoredSet(), FAILURE);
          
        m_monitorStackText.SetWindowText(IDLE_MONITOR_TEXT);

		//TD15928
		std::set<std::string> tsiList = m_model.getMonitoredSet();
		if (tsiList.size()==0 && m_monitoredCallStack.m_hWnd !=NULL)
		{
			m_monitoredCallStack.DeleteAllItems();
		}
		else
		{
			for (DWORD i=0; i < m_monitoredCallStack.GetItemCount();i++)
			{
				const MonitoredCallStackListCtrl::MonitorUpdate& updateData = m_monitoredCallStack.GetItemDataRef(i);
				std::set<std::string>::const_iterator itr1 = tsiList.find(updateData.calledTSI);
				std::set<std::string>::const_iterator itr2 = tsiList.find(updateData.callerTSI);
				if ( (itr1 != tsiList.end()) || (itr2 != tsiList.end()))
				{
					m_monitoredCallStack.DeleteItem(i);
				}
			}
		}
		//TD15928

        refreshButtonEnabledStates();
    }


    void RadioMonitorDlg::OnStartListeningButton()
    {     
        CWaitCursor wait;
        
        try
        {
            // Selection must be in monitored call stack, to have listen button enabled
            long callId = getSelectedCall(MONITOR_STACK);
            m_model.requestStartListening(callId); 
        }
        catch (const TA_Base_Core::ValueNotSetException&)
        {
            RadioMonitorErrorHandler::getInstance().displayError(
                        RadioMonitorErrorHandler::GENERIC_REQUEST_FAILURE);            
        }
          
        refreshButtonEnabledStates();
    }


    void RadioMonitorDlg::OnStopListeningButton()
    {   
        CWaitCursor wait;
        
        try
        {
            // Selection must be present in listening call stack, to have stop listening button enabled
            long callId = getSelectedCall(LISTEN_STACK);
            m_model.requestStopListening(callId); 
        }
        catch (const TA_Base_Core::ValueNotSetException&)
        {
            RadioMonitorErrorHandler::getInstance().displayError(
                        RadioMonitorErrorHandler::GENERIC_REQUEST_FAILURE);            
        }
          
        refreshButtonEnabledStates();      
    }


    void RadioMonitorDlg::OnEndMonitorCallButton()
    {        
        CWaitCursor wait;
        
        try
        {
            long callId = getSelectedCall(MONITOR_STACK);
            m_model.requestEndCall(callId);
        }
        catch (const TA_Base_Core::ValueNotSetException&)
        {
            RadioMonitorErrorHandler::getInstance().displayError(
                        RadioMonitorErrorHandler::GENERIC_REQUEST_FAILURE);            
        }
              
        refreshButtonEnabledStates();      
    }


    void RadioMonitorDlg::OnJoinMonitorCallButton()
    {        
        CWaitCursor wait;
        
        try
        {
            long callId = getSelectedCall(MONITOR_STACK);
            m_model.requestJoinCall(callId);
        }
        catch (const TA_Base_Core::ValueNotSetException&)
        {
            RadioMonitorErrorHandler::getInstance().displayError(
                        RadioMonitorErrorHandler::GENERIC_REQUEST_FAILURE);            
        }
              
        refreshButtonEnabledStates();      
    }


    void RadioMonitorDlg::OnEndListenCallButton()
    {        
        CWaitCursor wait;
        
        try
        {
            long callId = getSelectedCall(LISTEN_STACK);
            m_model.requestEndCall(callId);
        }
        catch (const TA_Base_Core::ValueNotSetException&)
        {
            RadioMonitorErrorHandler::getInstance().displayError(
                        RadioMonitorErrorHandler::GENERIC_REQUEST_FAILURE);            
        }
              
        refreshButtonEnabledStates();      
    }


    void RadioMonitorDlg::OnJoinListenCallButton()
    {        
        CWaitCursor wait;
        
        try
        {
            long callId = getSelectedCall(LISTEN_STACK);
            m_model.requestJoinCall(callId);
        }
        catch (const TA_Base_Core::ValueNotSetException&)
        {
            RadioMonitorErrorHandler::getInstance().displayError(
                        RadioMonitorErrorHandler::GENERIC_REQUEST_FAILURE);            
        }
              
        refreshButtonEnabledStates();      
    }


    void RadioMonitorDlg::OnHelpBtn()
    {        
        TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
    }


    void RadioMonitorDlg::OnCloseBtn() 
    {
        //hongzhi, TD18479: PISF/TST/XXXX-it is observed that Clicking close button on radio monitor GUI does not end radio monitor GUI process. The process is still observed running on windows task manager but the GUI is closed
	    DestroyWindow();	
    }


    long RadioMonitorDlg::getSelectedCall(DWORD category)
    {
        try
        {
            if (MONITOR_STACK == (category & MONITOR_STACK) &&
                1 == m_monitoredCallStack.GetSelectedCount())
            {
                int nItem = m_monitoredCallStack.GetNextItem(-1, LVNI_SELECTED);
    
                const RadioMonitor::RmMonitorUpdate& call = m_monitoredCallStack.GetItemDataRef(nItem);
                return call.callID;
            }  
            else if (LISTEN_STACK == (category & LISTEN_STACK) &&
                1 == m_listenCallStack.GetSelectedCount())
            {   
                int nItem = m_listenCallStack.GetNextItem(-1, LVNI_SELECTED);
                return m_listenCallStack.getCallIdFromIndex(nItem);                    
            }   
            else
            {
                // Otherwise nothing selected in target category
            } 
            
            // Initially disallowed selection in both lists, but changed to allow this situation (above)
            /*
            if (0 == m_listenCallStack.GetSelectedCount() &&
                1 == m_monitoredCallStack.GetSelectedCount())
            {
                // Single selection is in monitor stack
                if (MONITOR_STACK == (category & MONITOR_STACK))
                {
                    int nItem = m_monitoredCallStack.GetNextItem(-1, LVNI_SELECTED);
    
                    const RadioMonitor::RmMonitorUpdate& call = m_monitoredCallStack.GetItemDataRef(nItem);
                    return call.callID;
                }            
            }
            else if (0 == m_monitoredCallStack.GetSelectedCount() &&
                     1 == m_listenCallStack.GetSelectedCount())
            {
                // Selection, if any, must be in list stack
                if (LISTEN_STACK == (category & LISTEN_STACK))
                {   
                    int nItem = m_listenCallStack.GetNextItem(-1, LVNI_SELECTED);
                    return m_listenCallStack.getCallIdFromIndex(nItem);                    
                }            
            }
            else
            {
                // Get in here if no selection, multiple selection in a single list, or
                // selection in both lists, this is illegal (fallthrough to exception)
            }
            */
        }
        catch (...)
        {
            // Probably bad cast (wrong type in list - programming error)
            RadioMonitorErrorHandler::getInstance().displayError(
                RadioMonitorErrorHandler::INTERNAL_FAILURE);
        }
        throw TA_Base_Core::ValueNotSetException("getSelectedCall: invalid selection");
    }


    LRESULT RadioMonitorDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
    {
        LRESULT result = 0;

        // We shouldn't get any CORBA exceptions at this level, not necessarily
        // true for TransactiveExceptions though..
        try
        {
            result = TransActiveDialog::WindowProc(message, wParam, lParam);
        }
        catch (const TA_Base_Core::TransactiveException& te)
        {
            RadioMonitorErrorHandler::getInstance().displayError(
                RadioMonitorErrorHandler::getInstance().UNKNOWN_ERROR);

            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unhandled exception", te.what());        
        }
        catch (const CORBA::Exception& ce)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unhandled exception", ce._name());
            RadioMonitorErrorHandler::getInstance().displayError(
                RadioMonitorErrorHandler::getInstance().UNKNOWN_ERROR);

            TA_ASSERT(FALSE, "Unhandled CORBA comms exception");
        }

        return result;
    }


    void RadioMonitorDlg::OnDestroy() 
    {
        //RadioMonitor::RadioMonitorTestData::getInstance().cancelThread();

		OnStopMonitoringButton();

        // Delete the rights change notification object before window handle becomes invalid    
        delete m_rightsChangeHandler;
        m_rightsChangeHandler = 0;

	    TransActiveDialog::OnDestroy();

        m_searchPanel.DestroyWindow();
	    
        // Reset error handler window reference before we're destroyed
        RadioMonitorErrorHandler::getInstance().setPrimaryWindowHandle(0);

        try
        {    	 
            // Must attempt cleanup of singletons here before
            // core servants are de-activated
            RadioMonitorApp::cleanupSingletons();
        }
        catch (...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in OnDestroy");  
        }
    }


    void RadioMonitorDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
    {
	    TransActiveDialog::OnShowWindow(bShow, nStatus);	   	    
		if (!m_initReady)
		{
			PostMessage(WM_USER_INIT, 0, 0);
		}
    }


    void RadioMonitorDlg::OnSize(UINT nType, int cx, int cy) 
    {
        // If the 'Launch...' button has been pressed
	    // only then should the app be 
	    if(( m_wantToShow ) && ( nType == 0 ))
	    {
		    TransActiveDialog::OnSize(nType, cx, cy);
	        
            // Each time the window moves, must reposition main tab display
            // and items aligned below tab (the buttons)
	    }
        else
	    {	
		    if( nType == 1 ) // Minimise
		    {
			    m_wantToShow = false;  // make sure the window doesn't pop up on PID selections now it is minimised
			    return;
		    }
	    } 
    }


    void RadioMonitorDlg::OnAboutbutton() 
    {
	    TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
    }


    void RadioMonitorDlg::OnMove(int x, int y) 
    {    	    
	    LOG( SourceInfo, 
             DebugUtil::GenericLog, 
             DebugUtil::DebugInfo, 
             "Window moved to (%d,%d)", x, y);   

	    TransActiveDialog::OnMove(x, y);
    }


    afx_msg LRESULT RadioMonitorDlg::OnRightsChanged(UINT wParam, LONG lParam)
    {
        refreshButtonEnabledStates();
        
        // Get search page component to refresh state
        m_searchPanel.onUpdateRights();
        return 0;
    }


    bool RadioMonitorDlg::isSelectionChange(NMHDR* pNMHDR)
    {
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

	    // Only state changes made be a change in selection
	    if (LVIF_STATE == (pNMListView->uChanged & LVIF_STATE)) 
	    {
    		return true;
    	}
        else
        {
            return false;
        }
    }


    void RadioMonitorDlg::OnItemChangedListenList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        if (!isSelectionChange(pNMHDR))
        {
            return;
        }
        //m_mlSelectionController.onListItemStateChange((NM_LISTVIEW*)pNMHDR, &m_listenCallStack);    
    	
        refreshButtonEnabledStates();
    	*pResult = FALSE;
    }
        

    void RadioMonitorDlg::OnItemChangedSelectedList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        if (!isSelectionChange(pNMHDR))
        {
            return;
        }
        refreshButtonEnabledStates();
    	*pResult = FALSE;
    }


    void RadioMonitorDlg::OnItemChangedMonitorList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        if (!isSelectionChange(pNMHDR))
        {
            return;
        }
        //m_mlSelectionController.onListItemStateChange((NM_LISTVIEW*)pNMHDR, &m_monitoredCallStack);    
    	
        refreshButtonEnabledStates();
    	*pResult = FALSE;
    }

    
    RadioMonitorModel::TSISequence RadioMonitorDlg::getSelectedListTSIs(EFilter filter) const
    {
        RadioMonitorModel::TSISequence tsiSequence;

        for (int i = 0; i < m_selectedList.GetItemCount(); i ++)
        {
            switch (filter)
            {
            case ALL:
                break;
            case SELECTED_ONLY:
                if (m_selectedList.GetItemState(i, LVIS_SELECTED) != LVIS_SELECTED)
                {
                    // Ignore any items that aren't selected
                    continue;
                }
                break;
            default:
                TA_ASSERT(FALSE, "Unhandled case");
            };

            try
            {
                const SelectedEntity& identifier = m_selectedList.GetItemDataRef(i);
                tsiSequence.push_back(identifier.tsi);
            }
            catch (...)
            {
                // Probably bad cast (wrong type in list - programming error)
                RadioMonitorErrorHandler::getInstance().displayError(
                    RadioMonitorErrorHandler::INTERNAL_FAILURE);
            }                             
        }
        
        return tsiSequence;
    }

    
    void RadioMonitorDlg::addToSelectionList(std::vector<SelectedEntity>& subscriberSet)
    {
        if( AfxGetApp()->m_nThreadID != GetCurrentThreadId())
        {
            // This function is not thread safe!
            RadioMonitorErrorHandler::getInstance().displayError(
                    RadioMonitorErrorHandler::INTERNAL_FAILURE);
            return;
        }

        // subscriberSet defines new items to be added
        RadioMonitorModel::TSISequence currentContents = getSelectedListTSIs();
        RadioMonitorModel::TSISequence addedItems;

        // there may be duplicates..
        for (std::vector<SelectedEntity>::iterator itr = subscriberSet.begin(); itr != subscriberSet.end(); itr ++)
        {
            SelectedEntity& entity = (*itr);

            // We can ignore adding this item, if already in the list
            RadioMonitorModel::TSISequence::iterator itrFind = 
                    std::find(currentContents.begin(), currentContents.end(), entity.tsi);

            if (itrFind != currentContents.end())
            {
                // Found matching TSI already in list, disregard
                continue;
            }
            
            addedItems.push_back(entity.tsi);
                                
            m_selectedList.InsertItemPtr(0, entity.id, entity);        
        }
        
        if (addedItems.size() > 0 && m_model.isCurrentlyMonitoring())
        {
            // Start listening out for these newly added items
            m_model.requestStartMonitoring(addedItems);       
            
            // Highlight all items that failed to be monitored
            highlistSelectedListItems(m_model.getMonitoredSet(), SUCCESS);      
        }

        // Sort the list alphabetically
        m_selectedList.SortItems(SelectedListSort, (LPARAM)(&m_selectedList));

        refreshButtonEnabledStates();
    }


    void RadioMonitorDlg::refreshButtonEnabledStates()
    {
        // Must check list selections, as well as rights..
        bool hasMonitorRight = 
            CachedConfig::getInstance().isFunctionPermitted(TA_Base_Bus::aca_RADIO_MONITOR_CALLS);

        // Remove button is dependent on current selection
        // and RADIO_MONITOR_CALLS - removing will undo any active monitors..        
        if (m_selectedList.GetSelectedCount() && hasMonitorRight)            
        {
            m_removeButton.EnableWindow(TRUE);
        }
        else
        {
            m_removeButton.EnableWindow(FALSE);
        }
        
        // == one shot, start / stop monitoring ==
        // If no entries in list, or insufficient privelege, all buttons will be disabled
        BOOL enableOneShot = FALSE;
        BOOL enableStartMonitoring = FALSE;
        BOOL enableStopMonitoring = FALSE;

        if (hasMonitorRight && m_selectedList.GetItemCount() > 0)
        {
            if (!m_model.isCurrentlyMonitoring())
            {
                // start monitoring / one shot only if not currently monitoring
                enableOneShot = TRUE;
                enableStartMonitoring = TRUE;
            }
            else
            {
                // if currently monitoring, allowed to request stop monitoring
                enableStopMonitoring = TRUE;
            }
        }
        
        m_oneShotButton.EnableWindow(enableOneShot);
        m_startMonitoringButton.EnableWindow(enableStartMonitoring);
        m_stopMonitoringButton.EnableWindow(enableStopMonitoring);        
        
        // Start listening only if allowed, and item selected in monitor list
        if (m_monitoredCallStack.GetSelectedCount() > 0 &&
            CachedConfig::getInstance().isFunctionPermitted(TA_Base_Bus::aca_RADIO_MONITOR_LISTEN))
        {
            m_startListeningButton.EnableWindow(TRUE);
        }
        else
        {
            m_startListeningButton.EnableWindow(FALSE);
        }

        // Stop listening only if allowed, and VALID item selected in listen list
        if (m_listenCallStack.isParentNodeSelected() &&
            CachedConfig::getInstance().isFunctionPermitted(TA_Base_Bus::aca_RADIO_MONITOR_LISTEN))
        {
            m_stopListeningButton.EnableWindow(TRUE);
        }
        else
        {
            m_stopListeningButton.EnableWindow(FALSE);
        }
        
        BOOL enableJoinMonitorButton    = FALSE;
        BOOL enableEndMonitorButton     = FALSE;
        BOOL enableJoinListenButton     = FALSE;
        BOOL enableEndListenButton  = FALSE;

        if (1 == m_monitoredCallStack.GetSelectedCount())
        {
            // Item selected in monitor list
            if (CachedConfig::getInstance().isFunctionPermitted(TA_Base_Bus::aca_RADIO_MONITOR_JOIN))
            {   
                enableJoinMonitorButton = TRUE; 
            }

            if (CachedConfig::getInstance().isFunctionPermitted(TA_Base_Bus::aca_RADIO_MONITOR_TERMINATE))
            {
                enableEndMonitorButton = TRUE;
            }
        }
        
        if (m_listenCallStack.isParentNodeSelected())
        {
            // Item selected in listening list
            if (CachedConfig::getInstance().isFunctionPermitted(TA_Base_Bus::aca_RADIO_MONITOR_JOIN))
            {   
                enableJoinListenButton = TRUE; 
            }

            if (CachedConfig::getInstance().isFunctionPermitted(TA_Base_Bus::aca_RADIO_MONITOR_TERMINATE))
            {
                enableEndListenButton = TRUE;
            }
        }

        m_joinMonitorCallButton.EnableWindow(enableJoinMonitorButton);
        m_endMonitorCallButton.EnableWindow(enableEndMonitorButton);             
        m_joinListenCallButton.EnableWindow(enableJoinListenButton);
        m_endListenCallButton.EnableWindow(enableEndListenButton);            
    }    	

    
    void RadioMonitorDlg::highlistSelectedListItems(const std::set<std::string>& tsiList, EListType updateType)
    {
        // Go through each item and register it for highlighting as required
        m_selectedList.clearItemColourSettings();

        try
        {
            for (DWORD i = 0; i < m_selectedList.GetItemCount(); ++i )
            {
                const SelectedEntity& identifier = m_selectedList.GetItemDataRef(i);

                std::set<std::string>::const_iterator itr = tsiList.find(identifier.tsi);
                bool shouldHighlight = false;

                if (SUCCESS == updateType && itr == tsiList.end())
                {
                    // This item not in succeeded list, must have failed
                    shouldHighlight = true;
                }
                else if (FAILURE == updateType && itr != tsiList.end())
                {
                    // This item is in failed list, must have failed
                    shouldHighlight = true;
                }

                if (shouldHighlight)
                {
                    m_selectedList.setItemColour(m_selectedList.GetItemData(i),
                                                 RGB(255, 0, 0), 
                                                 TA_Base_Bus::ListCtrlSelNoFocus::I_ITEMDATA);
                }
            }
        }
        catch (...)
        {
            // Probably bad cast (wrong type in list - programming error)
            RadioMonitorErrorHandler::getInstance().displayError(
                 RadioMonitorErrorHandler::INTERNAL_FAILURE);
        }

        m_selectedList.Invalidate();
    }

} // end namespace TA_IRS_App
