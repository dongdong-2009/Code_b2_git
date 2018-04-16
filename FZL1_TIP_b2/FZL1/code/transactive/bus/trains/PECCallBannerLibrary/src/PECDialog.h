/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $Date: 2012/06/12 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(PECDialog_H)
#define PECDialog_H

#include "bus/banner_pages/call_page_library/src/stdafx.h"
#include "bus/trains/PECCallBannerLibrary/src/resource.h"

#include <string>


// forward declarations
namespace TA_IRS_Bus
{
    class PECCallEntry;
}

namespace TA_IRS_Bus
{
    
    /**
     * PECDialog dialog
     * @author Robert van Hugten
     * @version 1.0
     * @created 21-Dec-2007 3:49:57 PM
     */

    class PECDialog : public CDialog
    {

    public:

        
        /**
         * Constructs a new dialog with the given call entry being displayed
         *
         * @param callEntry
         * @param pParent
         */
        PECDialog( PECCallEntry* callEntry, CWnd* pParent = NULL );


        /**
         * Destructor
         */
        virtual ~PECDialog();


        /**
         * Sets the call entry object that is being displayed by this dialog.
         *
         * @param callEntry
         */
        void setCallEntry( PECCallEntry* callEntry );


        /**
         * gets the call entry this dialog is displaying.
         */
        PECCallEntry* getCallEntry();


        /**
         * Updates the details displayed in the PEC dialog with the details of the PEC entry.
         * Used to populate the fields, and also refresh them in case some information changed.
         */
        void updatePecDetails();


        /**
         * Show the hidden dialog
         */
        void showDialog();


        /**
         * Hide the visible dialog
         */
        void hideDialog();


        /**
         * Checks to see if the window is visible
         *
         * @return true if the window is shown
         */
        bool isShown();


        /**
         * Enables the visible dialog to allow contorls to be active
         */
        void enableDialog();


        /**
         * Disables the visible dialog to make contorls inactive
         */
        void disableDialog();


        /**
         * Shows the PEC is being answered in the status bar
         */
        void setStatusInitial();


        /**
         * Shows the PEC is being answered in the status bar
         */
        void setStatusAnswerInProgress();


        /**
         * Shows the PEC is answered and CCTV is being activated in the status bar
         */
        void setStatusActivatingCctv();


        /**
         * Shows the PEC is answered in the status bar
         */
        void setStatusPecAnswered();


        /**
         * Tells the operator that the audio call has been dropped
         */
        void setStatusCallLost();


        /**
         * Tells the operator that the PEC has been reset
         */
        void setStatusReset();


        /**
         * Tells the operator that the PEC has been reset due to mpu failure
         */
        void setStatusMpuChangeover();


        /**
         * Shows a modal message box with the given error message.
         * Blocks until acknowledged.
         *
         * @param message
         */
        void showErrorMessage( const std::string& message );


        /**
         * Shows a modal message box with the given information message.
         * Blocks until acknowledged.
         *
         * @param message
         */
        void showInformationMessage( const std::string& message );


        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(PECDialog)
        public:
        virtual BOOL OnInitDialog();
        virtual void OnCancel();
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL


    protected:


        // Generated message map functions
        //{{AFX_MSG(PECDialog)
        afx_msg HBRUSH OnCtlColor( CDC* pDC, CWnd* pWnd, UINT nCtlColor );
        afx_msg LRESULT refreshFields( WPARAM wParam, LPARAM lParam );
        afx_msg LRESULT displayPecStatus( WPARAM wParam, LPARAM lParam );
        afx_msg void onButtonCctv();
        afx_msg void onButtonReset();
        afx_msg void onButtonResetAll();
        afx_msg void onButtonHelp();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()


    private:

        /**
         * The current stage of the PEC answer process
         * @author adamr
         * @version 1.0
         * @created 21-Dec-2007 3:49:58 PM
         */
        enum EPecStatus
        {
            /**
             * The PEC is in the initial state
             */
            None = 0,
            /**
             * The PEC is being answered
             */
            AnsweringPec = 1,
            /**
             * The PEC is answered, the CCTV image is being activated
             */
            ActivatingCctv = 2,
            /**
             * The PEC is answered
             */
            Answered = 3,
            /**
             * The PEC audio call has been dropped
             */
            CallLost = 4,
            /**
             * The PEC has been reset
             */
            Reset = 5,
            /**
             * The PEC has been reset
             */
            MpuFailure = 6
        };


        /**
         * Private default constructor
         */
        PECDialog();


        /**
         * Display the PEC status info in the status bar.
         *
         * @param status    The PEC status
         */
        void setPecStatus( EPecStatus status );


        /**
         * Sets the status bar text and colour to the given values.
         *
         * @param message
         * @param colour
         */
        void setStatusBar( const std::string& message, COLORREF colour );


        /**
         * Sets the colour of the status bar.
         *
         * @param colour
         */
        void setStatusBarColour( COLORREF colour );


        /**
         * The message displayed when a call is lost
         */
        static const std::string STATUS_BAR_CALL_LOST_MESSAGE;


        /**
         * The message displayed when a call is reset
         */
        static const std::string STATUS_BAR_PEC_RESET_MESSAGE;


        /**
        * The message displayed when a call is reset due to an MPU failure
        */
        static const std::string STATUS_BAR_MPU_FAILURE_MESSAGE;


        /**
         * The message displayed when a pec is being answered
         */
        static const std::string STATUS_BAR_ANSWER_IN_PROGRESS_MESSAGE;


        /**
         * The message displayed when a CCTV is being activated
         */
        static const std::string STATUS_BAR_ACTIVATING_CCTV_MESSAGE;


        /**
         * The message displayed when a CCTV is being activated
         */
        static const std::string STATUS_BAR_PEC_ANSWERED_MESSAGE;


        /**
         * The colour to set the status bar to when there is an error
         */
        static const COLORREF STATUS_BAR_ERROR_COLOUR;


        /**
         * The colour to set the status bar to when there is no error
         */
        static const COLORREF STATUS_BAR_OK_COLOUR;


        /**
         * The windows message used to update the status bar
         */
        static const DWORD DISPLAY_PEC_STATUS;


        /**
         * The windows message used to refresh the status bar
         */
        static const DWORD REFRESH_PEC_DETAILS;


        // Dialog Data
        //{{AFX_DATA(PECDialog)
        enum { IDD = IDD_PEC };
		CEdit m_statusBar; // The status bar text field
        CEdit m_type; // The text field for the PEC details
        CEdit m_source; // The field for the PEC source
        CEdit m_destination; // The field for the PEC location details
        //}}AFX_DATA

        /**
         * The call entry whose details are currently being displayed
         */
        PECCallEntry* m_callEntry;


        /**
         * The brush that is used when the status bar background is drawn
         */
        CBrush* m_statusBarBackgroundBrush;

    };

}

#endif // !defined(PECDialog_H)
