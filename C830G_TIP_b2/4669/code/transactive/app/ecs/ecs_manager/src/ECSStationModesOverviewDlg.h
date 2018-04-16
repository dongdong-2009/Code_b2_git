#if !defined(AFX_ECSSTATIONMODESOVERVIEWDLG_H__58BFECBA_3D97_48D7_BEDE_AF7E031E5674__INCLUDED_)
#define AFX_ECSSTATIONMODESOVERVIEWDLG_H__58BFECBA_3D97_48D7_BEDE_AF7E031E5674__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ecs/ecs_manager/src/ECSStationModesOverviewDlg.h $
  * @author:  Robert Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/04/23 11:15:20 $
  * Last modified by:  $Author: builder $
  *
  * This class is a derivation of TransActiveDialog and implementation of the IECSStationModesOverviewView 
  * interface. It provides the actual ECS Station Modes Overview user interface, but defers the responsibility 
  * of it's logic and data to the ECSStationModesOverview member (which manipulates this object via 
  * IECSStationModesOverviewView).
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/generic_gui/src/TransActiveDialog.h"

#include "app/ecs/ecs_manager/src/IECSStationModesOverviewView.h"
#include "app/ecs/ecs_manager/src/StationModesOverviewGridCtrl.h"
#include "core/threads/src/Thread.h"

#include <memory>


namespace TA_Bus
{
    class IGUIAccess;
}
namespace TA_IRS_App
{
    class ECSStationModesOverview;
}

namespace TA_IRS_App
{
    class ECSStationModesOverviewDlg : public TA_Base_Bus::TransActiveDialog, 
									   public virtual IECSStationModesOverviewView,
									   TA_Base_Core::Thread
    {

    public:

		void init();
        /** 
          * ECSStationModesOverviewDlg
          *
          * description
          *
          * @param genericGUICallback
          * @param pParent
          */
        ECSStationModesOverviewDlg( TA_Base_Bus::IGUIAccess& genericGUICallback,CWnd* pParent = NULL );


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
          * aquireUserDecision
          *
          * Present user with number of options and return user's decision.
          *
          * @param decisionMessage - Message to display along with choices.
          * @param choices         - Container of choices to present user.
          *
          * @return int corresponding to user's choice (-1 used to indicate no choice)
          */
        virtual int aquireUserDecision( const std::string& decisionMessage, const std::vector< std::string >& choices );


        /** 
          * terminateApplication
          *
          * Terminate the application.
          */
        virtual void terminateApplication();


        /** 
          * enableApplyStationModeControl
          *
          * Enable/disable apply Station Mode control.
          *
          * @param doEnableApplyStationModeControl - true if should enable, false otherwise.
          */
        virtual void enableApplyStationModeControl( bool doEnableApplyStationModeControl );


        /** 
          * enableApplyAllCongestedStationModesControl
          *
          * Enable/disable apply all Station Modes for congestion control.
          *
          * @param doEnableApplyStationModeControl - true if should enable, false otherwise.
          */
        virtual void enableApplyAllCongestedStationModesControl( bool doEnableApplyAllCongestedStationModesControl );


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


        virtual void setCurrentStationAutoMode( int locationIndex, bool autoModeStatus );

		virtual void setCurrentStationAutoModeControlState( int locationIndex, TA_Base_Bus::EDataPointControlState controlState);
      
		
		// 7196-Start
		void refreshHealthPollInterval();
		// 7196-End
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


		virtual void setCurrentStationVisualLock(int locationIndex, bool isStationLocked );


        /** 
          * setCurrentProposedStationMode
          *
          * Update user interface with new proposed Station Mode (or clear existing one).
          *
          * @param locationIndex           - Index of location the "in progress" setting is for (index relative to
          *                                  vector of locations passed using setLocations).
          * @param proposedStationModeType - Enum corresponding to type of proposed Station Mode.
          * @param proposedStationMode     - String value of proposed Station Mode.
          * @param proposalState           - Enum corresponding to proposal State.
          *
          */
        virtual void setCurrentProposedStationMode( int locationIndex, EProposedStationModeType proposedStationModeType, 
            const std::string& proposedStationMode, EModeProposalState proposalState );


		virtual void addStatusLine(const std::string& statusText);
        // Dialog Data
        //{{AFX_DATA(ECSStationModesOverviewDlg)
        enum { IDD = IDD_ECSSTATIONMODESOVERVIEW_DIALOG };
	CEdit	m_statusEditControl;
        CButton m_applyAllCongestedStationModesButton;
        CButton m_applyStationModeButton;
		CButton m_closeButton;
        //}}AFX_DATA

        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(ECSStationModesOverviewDlg)
        protected:
        virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV support
        //}}AFX_VIRTUAL

    protected:
        HICON m_hIcon;

        // Generated message map functions
        //{{AFX_MSG(ECSStationModesOverviewDlg)
        virtual BOOL OnInitDialog();
        afx_msg void onInitialise();
        afx_msg void onStationModeOverviewGrid1SelectionChanged(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onStationModeOverviewGrid2SelectionChanged(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void onStationModeOverviewGrid3SelectionChanged(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg LRESULT onEnableApplyStationModeControl(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onEnableApplyAllCongestedStationModesControl(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onSetCurrentStationMode(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onSetCurrentStationModeInProgress(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onSetCurrentProposedStationMode(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onSetCurrentStationModeLock(WPARAM wParam, LPARAM lParam);

		afx_msg LRESULT onAddStatusLine(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onSetLocations(WPARAM wParam, LPARAM lParam);

        afx_msg void onCloseButton();
        afx_msg void onAppAbout();
		afx_msg void OnAppHelp();
        afx_msg void OnDestroy();
        afx_msg void onApplyProposedStationModeButton();
        afx_msg void onApplyAllProposedCongestionStationModesButton();
		afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg LRESULT onSetCurrentStationAutoMode(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onSetCurrentStationAutoModeControlState(WPARAM wParam, LPARAM lParam);
		afx_msg void OnHelpButton(); //TD16730
	//}}AFX_MSG
        DECLARE_MESSAGE_MAP()


    private:

        /** 
          * createGridCtrls
          *
          * Helper to create the Station Modes Overview Grid Ctrls and position as defined 
          * by resource file markers.
          */
        void createGridCtrls();

    private:
		virtual void run();
		virtual void terminate();

        std::auto_ptr< ECSStationModesOverview >    m_pECSStationModesOverview; // Smart class that performs all the logic for this view.

        StationModesOverviewGridCtrl m_stationModesOverviewGridCtrl1; // The three Grid Ctrls to display overview of Station Modes for all 
        StationModesOverviewGridCtrl m_stationModesOverviewGridCtrl2; // location simulataneously.
        StationModesOverviewGridCtrl m_stationModesOverviewGridCtrl3;

		//zn++ - CL-20790
		bool isSelectedAutoMode();
		//++zn

    };
} // namespace TA_IRS_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECSSTATIONMODESOVERVIEWDLG_H__58BFECBA_3D97_48D7_BEDE_AF7E031E5674__INCLUDED_)
