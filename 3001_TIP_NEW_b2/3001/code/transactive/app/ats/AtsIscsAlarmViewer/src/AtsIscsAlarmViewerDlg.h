#if !defined(AFX_ATSISCSALARMVIEWERDLG_H__FF31778B_500E_4F60_9035_E90F4FB4C06E__INCLUDED_)
#define AFX_ATSISCSALARMVIEWERDLG_H__FF31778B_500E_4F60_9035_E90F4FB4C06E__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/AtsIscsAlarmViewer/src/AtsIscsAlarmViewerDlg.h $
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

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>

#include "core/utilities/src/RunParams.h"

#include "bus/alarm/alarm_list_control/src/AlarmListCtrl.h"
#include "bus/alarm/alarm_list_control/src/IAlarmColumn.h"
#include "bus/alarm/alarm_list_control/src/IAlarmListObserver.h"
#include "bus/alarm/alarm_list_control/src/Sort.h"
#include "bus/alarm/alarm_list_control/src/Filter.h"
#include "bus/generic_gui/src/TransactiveDialog.h"
#include "bus/generic_gui/src/IGUIAccess.h"
#include "bus/generic_gui/src/ISystemStateUpdateObserver.h"
#include "bus/alarm/alarm_list_control/src/ListHeaderCtrl.h" //TD15005

//{{AFX_INCLUDES()
#include "webbrowser2.h"
//}}AFX_INCLUDES


namespace TA_Base_Bus
{
    // Forward declaration
    class SystemControllerGroupUpdateSubscriber;
}

namespace TA_IRS_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CAtsIscsAlarmViewerDlg dialog

    class CAtsIscsAlarmViewerDlg 
    : public TA_Base_Bus::TransActiveDialog,
      public TA_Base_Core::RunParamUser,
      public TA_Base_Bus::IAlarmListObserver,
      public TA_Base_Bus::ISystemStateUpdateObserver
    {
    // Construction
    public:
	    CAtsIscsAlarmViewerDlg(TA_Base_Bus::IGUIAccess* controlClass);	// standard constructor

    // Dialog Data
	    //{{AFX_DATA(CAtsIscsAlarmViewerDlg)
	    enum { IDD = IDD_ATSISCSALARMVIEWER_DIALOG };
        CStatic    m_statusBar;
		    // NOTE: the ClassWizard will add data members here
	    //}}AFX_DATA

	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CAtsIscsAlarmViewerDlg)
	    public:
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
	    HICON m_hIcon;

	    // Generated message map functions
	    //{{AFX_MSG(CAtsIscsAlarmViewerDlg)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnPaint();
	    afx_msg HCURSOR OnQueryDragIcon();
	    afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	    afx_msg void OnSize(UINT nType, int cx, int cy);
	    afx_msg void OnClose();
        afx_msg LRESULT  OnSystemInDegradedMode(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT  OnSystemInNormalMode(WPARAM wParam, LPARAM lParam);
		afx_msg void OnTimer(UINT nIDEvent); //TD17081
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    public:

      /**
	    * onRunParamChange
	    *
	    * This implements the virtual function from AtsIscsAlarmViewerGUI. It 
	    * handles the situation in an event when the main server 
	    * reestablishes comms. 
	    *
	    * @param The RunParam name.
	    *
	    * @param The new RunParam value.
        */
        virtual void onRunParamChange(const std::string& name, const std::string& value);

      /**
	    * serverUp
	    *
	    * This implements the virtual function from AtsIscsAlarmViewerGUI. It 
	    * handles the situation in an event when the main server 
	    * reestablishes comms. 
        */
        void serverUp();

      /**
	    * serverDown
	    *
	    * This implements the virtual function from AtsIscsAlarmViewerGUI. It 
	    * handles the situation in an event when the main server goes down.
        */
        void serverDown();

      /**
	    * updateAlarmCounts
	    * 
	    * The virtual method that updates the container's total acknowledge and unacknowledge values.
        * This method is not used by the AtsIscsALarmViewer.
	    *
	    * @param The number of alarms in the system.
	    *
	    * @param The number of unacknowledged alarms in the system.
	    */
	    virtual void updateAlarmCounts(unsigned int totAlarms, unsigned int totUnacknowledgedAlarms, bool exceededMaxDisplayAlarm, bool isLoadingAlarms);


       /**
        * updateAlarmSound
        * 
        * The virtual method that updates the container's alarm severity tones.
        * This method is not used by the AtsIscsALarmViewer.
        *
        * @param The severity tone to update as an unsigned long.
        * @param True if new alarms were inserted, and false otherwise.
        */
        virtual void updateAlarmSound(unsigned long severity, bool wereAlarmsAdded);


        /**
         * updateAvailableAlarmActions
         *
         * This will pass in the current availability of each alarm action. This will be called whenever
         * the alarm selection changes or the current alarms change.
         * This method is not used by the AtsIscsALarmViewer.
         *
         * @param ItemAvailability - This should pass the availability of every possible alarm action currently
         *                           being used.
         */
        virtual void updateAvailableAlarmActions(const ItemAvailability& availability);

    
        /**
         * setCurrentDescriptionText
         *
         * This will pass some appropriate text to be shown by the observer in a status bar or the like.
         * This string will hold the description of the selected alarm. If multiple alarms are selected it
         * will indicate how many alarms are selected
         *
         * @param CString - The text to be displayed
         */
        virtual void setCurrentDescriptionText(const CString& description);


       /**
	    * getProgressBar
	    * 
	    * The virtual method that retrieves the progress bar (if any) and then updates it. Once
        * the progress bar is finished with the method finishWithProgressBar() is called.
	    * This method is not used by the AtsIscsALarmViewer.
        *
	    * @return 
	    */
	    virtual CProgressCtrl* getProgressBar();


       /**
	    * finishedWithProgressBar
	    * 
	    * This will be called after a progress bar has finished updating. This will only be called if
        * the call to getProgressBar does not return NULL.
	    * This method is not used by the AtsIscsALarmViewer.
        */
        virtual void finishedWithProgressBar();

        /**
	    * notifyNoMatchingAlarms
	    * 
	    * This will be called if no alarms match the current filter. 
	    */
        virtual void notifyNoMatchingAlarms();

        /**
        * notifySystemStateDegraded
        *
        * This is called when the system is in degraded mode.
        *
        */
        virtual void notifySystemStateDegraded();


       /**
        * notifySystemStateNormal
        *
        * This is called when the system has recovered from degraded mode 
        * (ie. system is running normally).
        *
        */
        virtual void notifySystemStateNormal();

    private:

        CAtsIscsAlarmViewerDlg(const CAtsIscsAlarmViewerDlg&);
        CAtsIscsAlarmViewerDlg& operator=(const CAtsIscsAlarmViewerDlg&);

        void initialiseIscsAlarmListCtrl();
        void initPreFiltering(TA_Base_Bus::Filter& preFilter, std::vector<unsigned long>& locations);
        std::vector<TA_Base_Bus::IAlarmAction*> initListCtrlActions();
        void initListCtrlColumns();
        TA_Base_Bus::Sort reverseSort(const TA_Base_Bus::Sort& sort);
        void resizeDialogElements(int cx, int cy);
        void updatePreFilterAccordingToSystemState(TA_Base_Bus::Filter& preFilter);
		
		//TD18980, hongzhi,ATS-ISCS Alarm Viewer Issues In IFAT Platform	
		bool fileExists(const std::string& fileName);
		void BuildATSFileName();

    private:

        CWebBrowser2 m_atsAlarmListCtrl;                            // This will contain the ATS alarm list
        std::string m_atsAlarmListFilename;                         // Path and filename of ATS alarm list mimic file

        TA_Base_Bus::AlarmListCtrl m_iscsAlarmList;                 // This will contain the ISCS alarm list
        std::vector<TA_Base_Bus::IAlarmColumn*> m_alarmColumns;     // Columns displayed in the ISCS alarm list

        std::string m_sessionId;                                    // Session ID of current session

        bool m_internalRepaint;                                     // Flag to avoid infinite loop in OnPaint()

        bool m_showStatusBar;                                       // Indicates if status bar is displayed
        bool m_showTitleBar;                                        // Indicates if title bar is displayed

        static const ULONG PROFILE_LOCAL;                           // Local profile type key

        static const int VERTICAL_OFFSET;                           // Vertical offset between ATS and ISCS lists
    
        static const char* RPARAM_NO_STATUS_BAR;
        static const char* RPARAM_NO_TITLE_BAR;
        static const char* RPARAM_MAXIMISED;
        static const char* RPARAM_ATS_FILE_NAME_PATH;

		static const int ALARMVIEWER_INITIALIZE_TIMER; //TD17081
        bool m_initialised;                                         // Indicates that all initialisation is complete

		TA_Base_Bus::SystemControllerGroupUpdateSubscriber* m_groupUpdateSubscriber;

		TA_Base_Bus::ListHeaderCtrl   m_listHeader; //TD15005
    };

} // end TA_IRS_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ATSISCSALARMVIEWERDLG_H__FF31778B_500E_4F60_9035_E90F4FB4C06E__INCLUDED_)
