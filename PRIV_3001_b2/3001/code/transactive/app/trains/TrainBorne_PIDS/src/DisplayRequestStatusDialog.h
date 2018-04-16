/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#pragma once

#include "resource.h"
#include "bus/generic_gui/src/StdAfx.h"
#include "bus/mfc_extensions/src/fixed_column_header_ctrl/FixedHeaderCtrl.h"
#include "bus/mfc_extensions/src/list_ctrl_selection_without_focus/ListCtrlSelNoFocus.h"
#include "bus/trains/TrainCommonLibrary/src/CommonTypes.h"
#include "bus/trains/TrainCommonLibrary/src/TtisTypes.h"
#include "bus/trains/TrainSelectionComponent/src/TrainList.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/timers/src/ITimeoutCallback.h"
#include "core/timers/src/SingletonTimerUtil.h"

namespace TA_IRS_App
{
    /**
     * @author adamr
     * @version 1.0
     * @created 04-Apr-2008 1:48:11 PM
     */
    class DisplayRequestStatusDialog : public CDialog,
        public TA_Base_Core::ITimeoutCallback
    {
    public:

        enum {IDD = IDD_CLEAR_STATUS_DIALOG};
        /**
         * Constructor
         */
        DisplayRequestStatusDialog( std::string dialogName = "Clear Status", UINT nIDTemplate = DisplayRequestStatusDialog::IDD );


        /**
         * Destructor
         */
        virtual ~DisplayRequestStatusDialog();


        /**
         * Sets the details of a display request about to be submitted, so this dialog can listen for the results and display them
         *
         * @param trainList    The list of trains the request will be submitted with
         * @param timestamp    The time the request will be submitted with
         * @param sessionId    The session ID the request will be submitted with
         * @param originalCommand    The command that will be submitted
         * @param maximumRequestTime    The maximum request time in milliseconds
         */
        void setDisplayRequest( const TA_IRS_Bus::TrainInformationTypes::TrainDetailsList& trainDetailsList,
                                time_t timestamp,
                                const std::string& sessionId,
                                TA_Base_Core::TisCommandType originalCommand,
                                unsigned long maximumRequestTime );


        /**
         * This will display the modal dialog.
         * The operator cant click the OK button until the status for all the trains has been received.
         */
        void displayModalWindow();


        /**
         * This is an incoming response from a train.
         * It may or may not be for this display request.
         *
         * @param displayResult    The result of a TTIS command. May not be for the current dialog's display request.
         */
		void handleDisplayResult( const TA_Base_Core::TTISDisplayResult& returnStatus);


        /**
         * This is called when the timer waiting for train response expires.
         * It means all trains no result was received for are considered failed.
         *
         * @param timerId
         * @param userData
         */
        virtual void timerExpired( long timerId, void* userData );


        /**
         * Called when the close button is clicked or enter is pressed
         */
        virtual void OnCloseBtn();

    protected:

        virtual void clearViewArea();
        virtual void updateViewArea();
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(DisplayRequestStatusDialog)
        virtual BOOL OnInitDialog();
        virtual void DoDataExchange( CDataExchange* pDX );	// DDX/DDV support
        //}}AFX_VIRTUAL

        // Generated message map functions
        //{{AFX_MSG(DisplayRequestStatusDialog)

        /**
         * This is a handler for the WM_CLEAR_TRAIN_LIST message.
         * It will clear the train status list
         */
        afx_msg LRESULT onClearTrainList( WPARAM wParam, LPARAM lParam );


        /**
         * This is a handler for the WM_ENABLE_OK_BUTTON message.
         * It will enable or disable the ok button
         *
         * @param wParam    the flag to disable or enable the button. cast to BOOL, it will contain TRUE or FALSE
         */
        afx_msg LRESULT onEnableOkButton( WPARAM wParam, LPARAM lParam );


        /**
         * This is a handler for the WM_UPDATE_TRAIN_STATUS message.
         * It will update the status of the given train in the train status list
         *
         * @param wParam    Contains a pointer to a StatusEntry structure
         */
        afx_msg LRESULT onUpdateTrainList( WPARAM wParam, LPARAM lParam );

        afx_msg virtual void OnItemchangedProcessList( NMHDR* pNMHDR, LRESULT* pResult );
        afx_msg void OnDestroy();
		afx_msg void OnClose();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()


        /**
         * This is sent to populate the list control
         * @author adamr
         * @version 1.0
         * @created 04-Apr-2008 1:48:11 PM
         */
        struct StatusEntry
        {
            TA_IRS_Bus::CommonTypes::TrainIdType trainId;

            std::string trainIdSeviceString;

            std::string statusString;
        };


        /**
         * Show the hidden dialog
         */
        virtual void showDialog();


        /**
         * Hide the visible dialog
         */
        virtual void hideDialog();


        /**
         * This will clear the status list
         * It will post a WM_CLEAR_TRAIN_LIST to the window to update the GUI in the GUI thread.
         */
        void clearStatusList();


        /**
         * This will add/ipdate a trains status in the status list
         * It will post a WM_UPDATE_TRAIN_STATUS to the window to update the GUI in the GUI thread.
         *
         * @param trainId    The train to update
         * @param statusString    The status to display
         */
        void updateTrainStatus( const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails, const std::string& statusString );


        /**
         * This will enable or disable the OK button.
         * It will post a WM_ENABLE_OK_BUTTON to the window to update the GUI in the GUI thread.
         *
         * @param enable    true to enable false to disable
         */
        void enableOkButton( bool enable );


        /**
         * Schedules, or reschedules a timer
         */
        void scheduleTimer();


        /**
         * Cancels any scheduled timer
         */
        void cancelTimer();



        CButton m_closeBtn; // The OK button to close the dialog
        TA_Base_Bus::ListCtrlSelNoFocus m_statusList;   // The status list


        /**
         * This is the header control that doesnt resize, to be used for the status list
         */
        TA_Base_Bus::FixedHeaderCtrl m_fixedHeader;


        /**
         * Used to schedule timers
         */
        TA_Base_Core::SingletonTimerUtil& m_timerUtil;


        /**
         * The trains involved in the request that responses havent been received for.*/

        TA_IRS_Bus::TrainInformationTypes::TrainDetailsList m_trainDetailsList;


        /**
         * The time the request was submitted with
         */
        time_t m_timestamp;


        /**
         * The session ID the request was submitted with
         */
        std::string m_sessionId;


        /**
         * The command that was submitted
         */
        TA_Base_Core::TisCommandType m_originalCommand;


        /**
         * The maximum time the request can take to execute (this is the longest this object will wait for a response)
         */
        unsigned long m_waitTimeMillis;


        /**
         * The scheduled timer - if there is an ongoing request
         */
        long m_timerId;


        /**
         * Lock for the data members
         */
        TA_Base_Core::NonReEntrantThreadLockable m_dataLock;


        /**
         * This is the message posted to update the train list with new status
         */
        static const int WM_UPDATE_TRAIN_STATUS;


        /**
         * This is the message posted to clear the train list
         */
        static const int WM_CLEAR_TRAIN_LIST;


        /**
         * This is the message posted to enable or disable the OK button
         */
        static const int WM_ENABLE_OK_BUTTON;


        /**
         * The index of the train ID column
         */
        static const int TRAIN_ID_COLUMN;


        /**
         * The index of the train status column
         */
        static const int STATUS_COLUMN;
    private:
        TA_IRS_Bus::TrainInformationTypes::TrainDetailsList::iterator getTrainDetailsById( TA_IRS_Bus::CommonTypes::TrainIdType trainId );
        std::string m_dialogName;
		HICON m_hIcon;
    };
}

