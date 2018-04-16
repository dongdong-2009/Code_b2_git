/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/app/radio/radio_monitor_gui/src/RadioMonitorDlg.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Main Dialog class for the Radio Monitor application
  *
  */

#pragma once

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RadioMonitorDlg.h : header file
//

#include "app/radio/radio_monitor_gui/src/resource.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "core/data_access_interface/src/PrimitiveWrapper.h"
#include "bus/mfc_extensions/src/mfc_extensions.h"
#include "app/radio/radio_monitor_gui/src/MonitoredCallStackListCtrl.h"
#include "app/radio/radio_monitor_gui/src/ListeningCallStackListCtrl.h"
#include "bus/radio/RadioSearchPage/src/SearchPage.h"
#include "app/radio/radio_monitor_gui/src/RadioMonitorModel.h"
#include "app/radio/radio_monitor_gui/src/SelectedListCtrl.h"

namespace TA_IRS_App
{
    class RightsChangedWindowNotificationHandler;

    /////////////////////////////////////////////////////////////////////////////
    // RadioMonitorDlg dialog
    class RadioMonitorDlg 
    : 
    public TA_Base_Bus::TransActiveDialog
    {
    // Construction
    public:
    	//RadioMonitorDlg(CWnd* pParent = NULL);   // standard constructor
    	RadioMonitorDlg(TA_Base_Bus::IGUIAccess& controlClass);	// standard constructor
        
        virtual ~RadioMonitorDlg();
        typedef RadioMonitor::SelectedEntity SelectedEntity;
    
        bool wantToShow() { return m_wantToShow; };
        
        /**
         * addToSelectionList
         *
         * This is called when new subscribers are selected, which
         *  should be added to the selected list         
         * 
         */
        void addToSelectionList(std::vector<SelectedEntity>& newSubscribers);

        static int CALLBACK SelectedListSort(LPARAM data1, LPARAM data2, LPARAM listCtrl);
		
		void UserInit();
		
    // Overrides
    	// ClassWizard generated virtual function overrides
    	//{{AFX_VIRTUAL(RadioMonitorDlg)
    	public:
    	    virtual BOOL DestroyWindow();
    	protected:
    	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    	//}}AFX_VIRTUAL
      
    protected:
    
    	// Generated message map functions
    	//{{AFX_MSG(RadioMonitorDlg)
    	virtual BOOL OnInitDialog();
    	afx_msg LRESULT OnErrorMessage(UINT wParam, LONG lParam);
    	afx_msg void OnDestroy();
    	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    	afx_msg void OnSize(UINT nType, int cx, int cy);
    	afx_msg void OnMove(int x, int y);
        afx_msg void OnActivateApp( BOOL bActive, /*HTASK*/DWORD hTask );
        afx_msg LRESULT OnRightsChanged(UINT wParam, LONG lParam);

        afx_msg void OnItemChangedSelectedList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnItemChangedListenList(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnItemChangedMonitorList(NMHDR* pNMHDR, LRESULT* pResult);

     	afx_msg void OnAboutbutton();
    	afx_msg void OnCloseBtn();
        afx_msg void OnRemoveButton();
        afx_msg void OnOneShotButton();
        afx_msg void OnStartMonitoringButton();
        afx_msg void OnStopMonitoringButton();
        afx_msg void OnStartListeningButton();
        afx_msg void OnStopListeningButton();        
        afx_msg void OnEndMonitorCallButton();
        afx_msg void OnJoinMonitorCallButton();
        afx_msg void OnEndListenCallButton();
        afx_msg void OnJoinListenCallButton();
        afx_msg void OnHelpBtn();
		afx_msg LRESULT OnUserInit( WPARAM wParam, LPARAM lParam);

    	//}}AFX_MSG
    	DECLARE_MESSAGE_MAP()
    
        /**
         * Intercept all messages to allow provision of top level exception handler
         *
         */
    	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

    private:

        enum EFilter 
        {
            ALL,
            SELECTED_ONLY           
        };

        enum EListType
        {
            SUCCESS,
            FAILURE
        };

        /**
         * highlistSelectedListItems
         *
         * Updates the selected items list control to highlight specified items
         *  defined by the input criteria
         *
         * @param tsiList defines the TSIs of items we want to update based on
         * @param updateType defines what 'tsiList' represents, which can be failed or
         *          successful items (successful items normal colour, all failed items in red)
         *
         */
        void highlistSelectedListItems(const std::set<std::string>& tsiList, EListType updateType);

        /**
         * setupSearchPage
         *
         * Called once off to initialise the search page panel area
         *
         */
        void setupSearchPage();

        /**
         * isSelectionChange (helper function)
         *
         * @return true if the data contained in input argument suggests that
         *  this list view notification message was due to a change in selection
         */
        bool isSelectionChange(NMHDR* pNMHDR);
        
        /**
         * refreshButtonEnabledStates
         *
         * Called whenever there is a change that could affect the enabled
         *  state of a button (so call this to have the changes reflected in the UI)         
         */
        void refreshButtonEnabledStates();

        /**
         * getSelectedListTSIs
         *
         * @return an array of the TSI identifiers, as extracted from selected list
         *
         * @param filter (optional) defines filtering behaviour
         */
        RadioMonitorModel::TSISequence getSelectedListTSIs(EFilter filter = ALL) const;
    
        enum { MONITOR_STACK = 1, LISTEN_STACK = 2 };
        /**
         * getSelectedCall
         *
         * @param category combination of MONITOR_STACK or LISTEN_STACK defining list(s) to check
         *
         * @return callId identifying the singly selected item in specified category
         *
         * @exception ValueNotSetException if there is not exactly one selected item in the
         *                  monitor / listen controls, or is selection, but not in specified category
         *
         */
         long getSelectedCall(DWORD category);    

    // Dialog Data
    	//{{AFX_DATA(RadioMonitorDlg)
    	enum { IDD = IDD_RADIOMONITOR_DIALOG };
        
        TA_IRS_Bus::CSearchPage     m_searchPanel;
        SelectedListCtrl        m_selectedList;
        
        CEdit m_monitorStackText;
        MonitoredCallStackListCtrl m_monitoredCallStack;
        ListeningCallStackListCtrl m_listenCallStack;

        CButton     m_removeButton;
        
        CButton     m_oneShotButton;
        CButton     m_startMonitoringButton;
        CButton     m_stopMonitoringButton;

        CButton     m_startListeningButton;
        CButton     m_stopListeningButton;

    	CButton     m_endMonitorCallButton;
        CButton     m_joinMonitorCallButton;        
        CButton     m_endListenCallButton;
        CButton     m_joinListenCallButton;        
        //}}AFX_DATA
    
        // Indicates whether the GUI is meant to be visible at the moment
    	bool m_wantToShow;
        bool m_initialDisplay;
    
        RightsChangedWindowNotificationHandler* m_rightsChangeHandler;

        // Can have multiple selections now..
        //TA_Bus::XorListSelectionController m_mlSelectionController;

        RadioMonitorModel m_model;       
		bool m_initReady;
    };

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // end namespace TA_IRS_App
