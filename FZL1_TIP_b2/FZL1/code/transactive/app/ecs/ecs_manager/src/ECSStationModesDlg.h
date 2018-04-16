#if !defined(AFX_ECSSTATIONMODESDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_)
#define AFX_ECSSTATIONMODESDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/ECSStationModesDlg.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class is a derivation of TransActiveDialog and implementation of the IECSStationModesView 
  * interface. It provides the actual ECS Station Modes user interface, but defers the responsibility 
  * of it's logic and data to the ECSStationModes member (which manipulates this object via 
  * IECSStationModesView).
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//  -Prasanna- Prevent Warning C4786 being generated.
#pragma warning(disable: 4503) 
//  -Prasanna- 

#include <memory>

#include "app/ecs/ecs_manager/src/IECSStationModesView.h"
#include "app/ecs/ecs_manager/src/StationModesListCtrl.h"
#include "app/ecs/ecs_manager/src/TextDisplay.h"

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/mfc_extensions/src/coloured_controls/ColorComboBox.h"

/////////////////////////////////////////////////////////////////////////////
// ECSStationModesDlg dialog


//
// Forward declarations
//
namespace TA_Bus
{
    class IGUIAccess;
}
namespace TA_IRS_App
{
    class ECSStationModes;
}


namespace TA_IRS_App
{
    class ECSStationModesDlg : public TA_Base_Bus::TransActiveDialog, public virtual IECSStationModesView
    {

    public:

        /** 
          * ECSStationModesDlg
          *
          * description
          *
          * @param genericGUICallback
          * @param pParent
          *
          */
        ECSStationModesDlg( TA_Base_Bus::IGUIAccess& genericGUICallback, CWnd* pParent = NULL ); // standard constructor

		//TD18095, jianghp, to fix the performance of showing manager application
		virtual void init();
        /** 
          * setLocations
          *
          * Populate user interface with list of location (Station) names.
          *
          * @param locationNames - Vector of location (station) names in inbound order.
          */
        virtual void setLocations( const TA_ECSManagerCommonDefs::LocationNames& locations );

    
        /** 
          * setSelectedLocationIndex
          *
          * Set location selection control specific location.
          *
          * @param selectedLocationIndex - index of location to set as selected (index relative
          *                                to container of locations passed from setLocations)
          */
        virtual void setSelectedLocationIndex( unsigned int selectedLocationIndex );


        /** 
          * enableLocationSelectionControl
          *
          * Enable/disable location selection control.
          *
          * @param doEnableLocationSelectionControl - true if should enable, false otherwise.
          */
        virtual void enableLocationSelectionControl( bool doEnableLocationSelectionControl );

        
        /** 
          * enableApplyStationModeControl
          *
          * Enable/disable apply Station Mode control.
          *
          * @param doEnableApplyStationModeControl - true if should enable, false otherwise.
          */
        virtual void enableApplyStationModeControl( bool doEnableApplyStationModeControl );


        /** 
          * setStationModeDetails
          *
          * Populate user interface from vector of StationModeDetails.
          *
          * @param stationModeDetails - Vector of stationodeDetails ordered by mode.
          */
        virtual void setStationModeDetails( const TA_ECSManagerCommonDefs::StationModeDetails& stationModeDetails );


        /** 
          * setCurrentStationMode
          *
          * Populate user interface with current Station Mode and type for 
          * currently selected location.
          *
          * @param stationMode     - Current station mode value as string.
          * @param stationModeType - ECurrentStationModeType indicating type of current station mode.
          */
        virtual void setCurrentStationMode( const std::string& stationMode, TA_ECSManagerCommonDefs::ECurrentStationModeType currentStationModeType );


        /** 
          * setCurrentStationModeInProgress
          *
          * Set display of current Station Mode for currently selected location as being
          * in progress, or not in progress.
          *
          * @param doShowAsInProgress - true to show as in progress, false otherwise. 
          */
        virtual void setCurrentStationModeInProgress( bool doShowAsInProgress );


		virtual void setCurrentStationVisualLock( bool isStationLocked, unsigned int index );


        /** 
          * setStationModeProposalState
          *
          * Update display of Station Mode for currently selected location with new
          * proposal state.
          *
          * @param stationModeIndex         - Index of Station Mode the update is for (index relative
          *                                   to container of Station Modes passed from setStationModeDetails).
          * @param stationModeProposalState - Enum corresponding to updated proposal state.
          *
          */
        virtual void setStationModeProposalState( unsigned int stationModeIndex, TA_ECSManagerCommonDefs::EModeProposalState stationModeProposalState );


        /** 
          * setInboundLocation
          *
          * Set display of location adjacently inbound to currently selected location (Station).
          *
          * @param inboundLocation - Name of inbound location.
          */
        virtual void setInboundLocation( const std::string& inboundLocation );

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
          * setOutboundLocation
          *
          * Set display of location adjacently outbound to currently selected location (Station).
          *
          * @param outboundLocation - Name of outbound location.
          */
        virtual void setOutboundLocation( const std::string& outboundLocation );

        /** 
          * setInboundStationLock
          *
          * Set visual lock status of location adjacently inbound station.
          *
          * @param boolean which indicates if station is locked or not.
          */
		virtual void setInboundStationLock( bool isStationLocked );

        /** 
          * setOutboundStationLock
          *
          * Set visual lock status of location adjacently outbound station.
          *
          * @param boolean which indicates if station is locked or not.
          */
		virtual void setOutboundStationLock( bool isStationLocked );
        
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
          * addStatusLine
          *
          * Adds some text to the status text box.
          *
          * @param - Text which indicates current status of various ECS Modes at the current location.
          */
		virtual void addStatusLine(const std::string& statusText);

        /** 
          * terminateApplication
          *
          * Terminate the application.
          */
        virtual void terminateApplication();

		virtual void setColourMap(unsigned int index, COLORREF colour);

		
		// 7196-Start
		/** 
          * dataPollHealthPointIntervalEntityUpdated
          *
          * Update the DataPollHealthPointInterval Entity in the Station data point manager
          */
		void refreshHealthPollInterval();
		// 7196-End	


		virtual void setCurrentAppliedMode(int index, bool isOverriding);


        // Dialog Data
        //{{AFX_DATA(ECSStationModesDlg)
        enum { IDD = IDD_ECSSTATIONMODES_DIALOG };
		CEdit	m_statusEditControl;
        TextDisplay m_currentStationModeDisplay;
        StationModesListCtrl    m_stationModeSelectionListCtrl;
        TextDisplay m_outboundLocationDisplay;
        TextDisplay m_inboundLocationDisplay;
        CColorComboBox   m_locationSelectionCombo;
        CButton m_applyButton;
		CButton m_closeButton;
        //}}AFX_DATA

        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(ECSStationModesDlg)
        protected:
        virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV support
		//virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
        //}}AFX_VIRTUAL


    protected:
        HICON m_hIcon;

        // Generated message map functions
        //{{AFX_MSG(ECSStationModesDlg)
        virtual BOOL OnInitDialog();
        afx_msg void onCloseButton();
        afx_msg void onLocationSelectionChanged();
        afx_msg void onStationModeDeselected();
        afx_msg void onStationModeSelectionItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg LRESULT onSetCurrentStationMode(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onSetCurrentStationModeInProgress(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onEnableApplyStationModeControl(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onEnableCloseButton(WPARAM wParam, LPARAM lParam);
        afx_msg LRESULT onSetStationModeProposalState(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onSetCurrentStationModeLock(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onSetInboundStationLock(WPARAM wParam, LPARAM lParam);
		afx_msg LRESULT onSetOutboundStationLock(WPARAM wParam, LPARAM lParam);
        afx_msg void onInitialise();
        afx_msg void onAppAbout();
        afx_msg void OnSize(UINT nType, int cx, int cy);
        afx_msg void OnTimer(UINT nIDEvent);
        afx_msg void OnClose();
        afx_msg void onApplyStationModeButton();
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

    private:
    
        std::auto_ptr< ECSStationModes > m_pECSStationModes; // Smart class that performs all the logic for this view

        bool m_areOffsetsSetup; // Flag so early WM_SIZE message/s (OnSize) won't do anything until
                                // offsets calculated.

        int m_rightRelativeOffset; // Offset to maintain dialog geometry during resize.
        int m_bottomRelativeOffset; // Offset to maintain dialog geometry during resize.

        bool m_flashToggle; // bool to toggle on WM_TIMER to support flashing.
        COLORREF m_currentStationModeDisplayColour; // Colour of cuurent Station Mode display .

		bool m_isInProgressBlinking;

		/////////////////////////////////////////////////////////////////////
		//Launch Manager modified by: zhangjunsheng 2008/09/01 11:00:00
		
		static DWORD WINAPI	ThreadFunction(LPVOID lpThreadParameter);
		DWORD m_thread_id;
		HANDLE m_thread_hd;
		
		//Launch Manager/////////////////////////////////////////////////////
    };
} // namespace TA_IRS_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ECSSTATIONMODESDLG_H__B1FC56F1_91F3_4680_9193_F5B9A34FE350__INCLUDED_)
