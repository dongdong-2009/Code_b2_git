#if !defined(AFX_MASTERMODESGRIDCTRL_H__05AFACD7_192D_478A_A840_1EEAC31434AB__INCLUDED_)
#define AFX_MASTERMODESGRIDCTRL_H__05AFACD7_192D_478A_A840_1EEAC31434AB__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/MasterModesGridCtrl.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Specialised derivation of TA_Bus::CGridCtrl to display the current ECS Station Mode of all locations
  * as well as available Master Modes and their proposal state.
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"

#define IMPORT_EXTENSIONS
//#include "bus/mfc_extensions/src/grid_control/GridCtrl.h"
#include "bus/mfc_extensions/src/grid_control_border_selection/GridCtrlBorderSelection.h"

//  -Prasanna- Prevent Warning C4786 being generated.
#pragma warning(disable: 4786) 
//  -Prasanna- 

#include <vector>
#include <string>

namespace TA_IRS_App
{
    class MasterModesGridCtrl : public GridCtrlBorderSelection  
    {

    public:

        typedef std::pair< int, TA_IRS_App::TA_ECSManagerCommonDefs::EProposedStationModeType > LocationIndexAndProposedType;
		
        /** 
          * Constructor
          */
        MasterModesGridCtrl();
    

        /** 
          * Destructor
          */
        virtual ~MasterModesGridCtrl();
    

        /** 
          * setUpGridCtrl
          *
          * Set up grid control.
          */
        void setUpGridCtrl();

    
        /** 
          * setLocations
          *
          * Populate Grid with vector of location names (Stations).
          *
          * @param locationNames - Vector location names (Stations) to display.
          */
        void setLocations( const TA_IRS_App::TA_ECSManagerCommonDefs::LocationNames& locationNames );


        /** 
          * populateMasterModeDetails
          *
          * Populate Grid with vector of Master Mode Details.
          *
          * @param masterModeDetails - Vector of Master Mode Details to populate.
          */
        void populateMasterModeDetails( const TA_IRS_App::TA_ECSManagerCommonDefs::MasterModeDetails& masterModeDetails );


        /** 
          * setCurrentStationMode
          *
          * Set display of current Station Mode for particular location.
          *
          * @param locationIndex          - Index Location to set display for (relative to vector passed using setLocations)
          * @param stationMode            - Value of Station Mode to display.
          * @param currentStationModeType - Enum specifying type of current Station Mode to display.
          */
        void setCurrentStationMode( int locationIndex, const std::string& stationMode,
            TA_IRS_App::TA_ECSManagerCommonDefs::ECurrentStationModeType currentStationModeType );


        /** 
          * setCurrentStationModeInProgress
          *
          * Set display of current Station Mode as being In Progress or not In Progress for particular location.
          *
          * @param locationIndex      - Index Location to set display for (relative to vector passed using setLocations)
          * @param doShowAsInProgress - TRUE if should show as in progress, false otherwise.
          */
        void setCurrentStationModeInProgress( int locationIndex, BOOL doShowAsInProgress );

    
        /** 
          * setCurrentProposedMasterMode
          *
          * Set display of Master Mode according to proposal state.
          *
          * @param masterModeIndex - Index of Master Mode to set display of proposal state for (index relative to vecor of
          *                          Master Mode Details passed using setMasterModeDetails.
          * @param proposalState   - Enum corresponding to proposal state to set diaply of Master Mode to.
          */
        void setCurrentProposedMasterMode( int masterModeIndex, TA_IRS_App::TA_ECSManagerCommonDefs::EModeProposalState proposalState );


		void setCurrentStationModeLock( int locationIndex, bool isStationLocked);

		// 10745j-Start
		// Modified for Selection Behaviour Bug		
		/** 
          * resetSelectionAfterApplyButtonPressed
          *
          * After the current station mode has been applied,reset the current selection
          */
		void resetSelectionAfterApplyButtonPressed();
		// 10745j-End

        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(MasterModesGridCtrl)
        protected:
        virtual void PreSubclassWindow();		
        //}}AFX_VIRTUAL

        // Generated message map functions
    protected:
        //{{AFX_MSG(MasterModesGridCtrl)
        afx_msg void OnKillFocus(CWnd* pNewWnd);
        afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
        afx_msg void OnSetFocus(CWnd* pOldWnd);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg void OnTimer(UINT nIDEvent);
        afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
        afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
        afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()


        virtual BOOL  DrawCell(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk=FALSE);

        virtual BOOL DrawFixedCell(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk=FALSE);

		// 10745c-Start
		virtual void OnDraw(CDC* pDC);
		// 10745c-End
        //virtual CCellID SetFocusCell(CCellID cell);

    private:

        typedef std::pair< int, int > CellCoordinate;
        typedef std::vector< CellCoordinate > CellCoordinateVector;
        typedef std::vector< bool > IsCenteredTextVector; 

        CellCoordinateVector m_cellsRequiringFlash;

        // Used for workaround for drawing issue of centered text in columns when resized to 
        // width less than character size.
        IsCenteredTextVector m_isCenteredTextVector;
        int m_aveCharWidth;

        bool m_flashToggle;
        bool m_isFlashing;

		int m_numLoadedLocations;

        /////////////////////////////////////////////////////////////////////////////

        //{{AFX_INSERT_LOCATION}}
        // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
    };
} // namespace TA_IRS_App

#endif  // !defined(AFX_MASTERMODESGRIDCTRL_H__05AFACD7_192D_478A_A840_1EEAC31434AB__INCLUDED_)
