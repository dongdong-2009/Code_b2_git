#if !defined(AFX_ECSMASTERMODESDLG_H__B0329441_02F1_465F_854D_569BAB13DC74__INCLUDED_)
#define AFX_ECSMASTERMODESDLG_H__B0329441_02F1_465F_854D_569BAB13DC74__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ecs_manager/src/ECSMasterModesDlg.h $
  * @author:  Robert Young
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/03/14 17:05:46 $
  * Last modified by:  $Author: huangjian $
  *
  * This class is a derivation of TransActiveDialog and implementation of the IECSMasterModesView 
  * interface. It provides the actual ECS Master Modes user interface, but defers the responsibility 
  * of it's logic and data to the ECSMasterModes member (which manipulates this object via 
  * IECSMasterModesView).
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "app/ecs/ecs_manager/src/IECSMasterModesView.h"
#include "app/ecs/ecs_manager/src/ECSMasterModes.h"
#include "app/ecs/ecs_manager/src/MasterModesGridCtrl.h"
#include "app/ecs/ecs_manager/src/StationModesOverviewGridCtrl.h"

#include "bus/generic_gui/src/TransActiveDialog.h"

/////////////////////////////////////////////////////////////////////////////
// ECSMasterModesDlg dialog

namespace TA_Bus
{
    class IGUIAccess;
}

namespace TA_IRS_App
{
    class ECSMasterModesDlg : public TA_Base_Bus::TransActiveDialog, public virtual IECSMasterModesView
    {

    public:

        /** 
          * Constructor
          *
          * @param genericGUICallback
          * @param pParent
          */
        ECSMasterModesDlg( TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent = NULL );  // standard constructor

		//TD18095, jianghp, to fix the performance of showing manager application
		void init();
        /** 
          * setLocations
          *
          * Populate user interface with list of location (Station) names.
          *
          * @param locationNames - Vector of location (station) names in inbound order.
          */
        virtual void setLocations( const LocationNames& locationNames );


        /** 
          * displayErrorMessage
          *
          * Display error message to user.
          *
          * @param errorMessage - Error message to display.
          */
        virtual void displayErrorMessage( const std::string& errorMessage );


        /** 
          * displayWarningMessage
          *
          * Display error message to user.
          *
          * @param warningMessage - Warning message to display.
          */
        virtual void displayWarningMessage( const std::string& warningMessage );


        /** 
          * aquireConfirmation
          *
          * Acquire confirmation from user.
          *
          * @param confirmationMessage - Description of action requiring confirmation.
          *
          * @return true if confirmed, false otherwise.
          */
        virtual bool aquireConfirmation( const std::string& confirmationMessage );


        /** 
          * terminateApplication
          *
          * Terminate the application.
          */
        virtual void terminateApplication();


        /** 
          * setMasterModeDetails
          *
          * Populate user interface from vector of MasterModeDetails.
          *
          * @param masterModeDetails - Vector of MasterModeDetails ordered by mode.
          */
        virtual void setMasterModeDetails( const MasterModeDetails& masterModeDetails );


        /** 
          * enableApplyMasterModeControl
          *
          * Enable or diable user interface control which applies selected MasterMode. 
          *
          * @param doEnableApplyMasterModeControl - true if should enable, 
          *                                         false if should disable.
          */
        virtual void enableApplyMasterModeControl( bool doEnableApplyMasterModeControl );


        /** 
          * setCurrentStationMode
          *
          * Populate user interface with current Station Mode and type for 
          * particular location.
          *
          * @param locationIndex   - Index of location Station Mode is for (index relative to
          *                          vector of locations passed using setLocations).
          * @param stationMode     - Current station mode value as string.
          * @param stationModeType - ECurrentStationModeType indicating type of current station mode.
          */
        virtual void setCurrentStationMode( int locationIndex, const std::string& stationMode, ECurrentStationModeType stationModeType );


        /** 
          * setCurrentStationModeInProgress
          *
          * Set display of current Station Mode (for particular location) as being currently
          * in progress, or not in progress.
          *
          * @param locationIndex      - Index of location the "in progress" setting is for (index relative to
          *                             vector of locations passed using setLocations).
          * @param doShowAsInProgress - true to show as in progress, false otherwise. 
          */
        virtual void setCurrentStationModeInProgress( int locationIndex, bool doShowAsInProgress );


		virtual void setCurrentStationVisualLock( int locationIndex, bool isStationLocked ); 


        /** 
          * setCurrentProposedMasterMode
          *
          * Set display of Master Mode according to current proposal state.
          *
          * @param masterModeIndex - Index of Master Mode the proposal state is for (index relative to
          *                          vector of Master Mode Details passed using setMasterModeDetails).
          * @param proposalState   - Current proposal state to set display of MasterMode to.
          */
        virtual void setCurrentProposedMasterMode( int masterModeIndex, EModeProposalState proposalState );

		/** 
          * addStatusLine
          *
          * Adds some text to the status text box.
          *
          * @param - Text which indicates current status of various ECS Modes at the current location.
          */
		virtual void addStatusLine(const std::string& statusText);
    // Dialog Data
        //{{AFX_DATA(ECSMasterModesDlg)
        enum { IDD = IDD_ECSMASTERMODES_DIALOG };
	CEdit	m_masterModeEditControl;
        CButton m_applyMasterModeButton;
		CButton m_closeButton;
    //}}AFX_DATA

        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(ECSMasterModesDlg)
        protected:
        virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV support

        //}}AFX_VIRTUAL

    // Implementation
    protected:
        HICON m_hIcon;

        // Generated message map functions
        //{{AFX_MSG(ECSMasterModesDlg)
        virtual BOOL OnInitDialog();
        afx_msg void onMasterModesGridSelectionChanged(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg LRESULT onSetCurrentStationMode(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onSetCurrentStationModeInProgress(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onSetCurrentProposedMasterMode(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onEnableCloseButton(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onEnableApplyMasterModeControl(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onSetStationLockStatus(WPARAM wParam, LPARAM lParam);
        afx_msg void onInitialise();
        afx_msg void onCloseButton();
        afx_msg void OnClose();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void onAppAbout();
		afx_msg void OnAppHelp();
        afx_msg void onApplyMasterModeButton();
		afx_msg void OnHelpButton(); //TD16730
	//}}AFX_MSG

        DECLARE_MESSAGE_MAP()


    private:

        /** 
          * setupOffsets
          *
          * Helper to calculate the offsets between controls and dialog borders (using initial 
          * dimensions as set in resource file) to be used for resize adjustments.
          */
        void setupOffsets();


        /** 
          * createGridCtrl
          *
          * Helper to create the Master Modes Grid Ctrl and position as defined by resource file
          * marker.
          */
        void createGridCtrl();


        /** 
          * setInitialSize
          *
          * Helper to set an appropriate default initial size for the dialog (based on type). This
          * is called before TransActiveDialog::OnInitDialog so as to allow subsequent user-settings
          * to override the default size set here.
          */
        void setInitialSize();

    private:
        
        std::auto_ptr< ECSMasterModes > m_pECSMasterModes;  // Smart class that performs all the logic for this view.

        MasterModesGridCtrl m_masterModesGridCtrl;          // Master Modes Grid Control.
        
        bool m_areOffsetsSetup;                             // Flag so early WM_SIZE message/s (OnSize) won't do anything until
                                                            // offsets calculated.
        int m_rightRelativeOffset;                          // Offset to maintain dialog geometry during resize.
        int m_bottomRelativeOffset;                         // Offset to maintain dialog geometry during resize.

		// Reduce launching manager time
		static DWORD WINAPI	ThreadFunction(LPVOID lpThreadParameter);
		DWORD m_thread_id;
		HANDLE m_thread_hd;
		// Reduce launching manager time
    };
} // namespace TA_IRS_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECSMASTERMODESDLG_H__B0329441_02F1_465F_854D_569BAB13DC74__INCLUDED_)
