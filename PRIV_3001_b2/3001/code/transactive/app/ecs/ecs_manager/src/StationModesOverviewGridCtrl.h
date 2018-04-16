#if !defined(AFX_STATIONMODESOVERVIEWGRIDCTRL_H__05AFACD7_192D_478A_A840_1EEAC31434AB__INCLUDED_)
#define AFX_STATIONMODESOVERVIEWGRIDCTRL_H__05AFACD7_192D_478A_A840_1EEAC31434AB__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/StationModesOverviewGridCtrl.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Specialised derivation of TA_Bus::CGridCtrl to display the current ECS Station Mode
  * and any Proposed Station Modes for up to NUM_LOCATIONS_PER_GRID locations.
  */


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/ecs/ecs_manager/src/ECSManagerCommonDefs.h"

#define IMPORT_EXTENSIONS
#include "bus/mfc_extensions/src/grid_control_border_selection/GridCtrlBorderSelection.h"

//  Prevent Warning C4786 being generated.
#pragma warning(disable: 4786)  

#include <vector>
#include <string>

#include "bus/scada/common_library/src/CommonDefs.h"


namespace TA_IRS_App
{
	const int CURRENT_STATION_AUTO_MODE_ROW = 6;

	typedef std::pair<int, int> CellCoordinate;
    typedef std::vector<CellCoordinate> CellCoordinateVector;
	typedef std::map<int, TA_IRS_App::ECurrentStationAutoModeType> LocationIndexAndAutoModeMap;

    class StationModesOverviewGridCtrl : public GridCtrlBorderSelection  
    {

	public:
	typedef std::pair<int, TA_IRS_App::ECurrentStationAutoModeType> LocationIndexAndAutoModeType;
	

    public:
		
        /** 
          * Constructor
          */
        StationModesOverviewGridCtrl();
    

        /** 
          * Destructor
          */
        virtual ~StationModesOverviewGridCtrl();
    

        /** 
          * setUpGridCtrl
          *
          * Set up grid control.
          */
        void setUpGridCtrl();

    
        /** 
          * clearSelection
          *
          * Unselect any selected cell.
          */
        void clearSelection();

    
        /** 
          * setLocations
          *
          * Populate Grid with vector of location names (Stations).
          *
          * @param locationNames - Vector location names (Stations) to display.
          */
        void setLocations( const TA_IRS_App::LocationNames& locationNames );


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
            TA_IRS_App::ECurrentStationModeType currentStationModeType );

       
        void setCurrentStationAutoMode( int locationIndex, BOOL autoModeStatus );

		void setCurrentStationAutoModeControlState( int locationIndex, TA_IRS_App::ECurrentStationAutoModeType autoModeType);
    

        /** 
          * setCurrentStationModeInProgress
          *
          * Set display of current Station Mode as being In Progress or not In Progress for particular location.
          *
          * @param locationIndex      - Index Location to set display for (relative to vector passed using setLocations)
          * @param doShowAsInProgress - TRUE if should show as in progress, false otherwise.
          */
        void setCurrentStationModeInProgress( int locationIndex, BOOL doShowAsInProgress );


		void setCurrentStationModeLock( int locationIndex, bool isStationLocked);

    
        /** 
          * setCurrentProposedStationMode
          *
          * Set display of proposed Station Mode.
          *
          * @param locationIndex           - Index Location to set display for (relative to vector passed using setLocations)
          * @param proposedStationModeType - Type of Station Mode proposal state display is being set for.
          * @param proposedStationMode     - Value of Proposed Station Mode.
          * @param proposalState           - Proposal state to set display for.
          */
        void setCurrentProposedStationMode( int locationIndex, TA_IRS_App::EProposedStationModeType proposedStationModeType,
            const std::string& proposedStationMode, TA_IRS_App::EModeProposalState proposalState );


        /** 
          * getProposedStationModeFromCellCoordinates
          *
          * Get Proposed Station Mode (location index and type) from cell coordinates.
          *
          * @param row - Row index to get proposed Station Mode for.
          * @param col - Col index to get proposed Station Mode for.
          *
          * @return LocationIndexAndProposedType for particular cell coordinates.
          */
        LocationIndexAndProposedType getProposedStationModeFromCellCoordinates( int row, int col ); 
		
		//Prasanna
		LocationIndexAndAutoModeType getAutoModeFromCellCoordinates( int row, int col );
		//Prasanna

		bool GetRowSelectedFlag();

		void SetRowSelectedFlag(bool bSet);

		void ResetSelectedRow();


		//CL17276++
		bool GetAutoRowSelectedFlag();

		void ResetAutoSelectedRow();
		//++CL17276

		//zn++ - CL-20791
		bool GetMultiSelectedFlag();
		//++zn

		void checkCurrentModeFlashing();

		// Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(StationModesOverviewGridCtrl)
    protected:
        virtual void PreSubclassWindow();			
		virtual void OnFixedColumnClick(CCellID& cell);
        //}}AFX_VIRTUAL

        // Generated message map functions

    protected:

        virtual BOOL DrawCell( CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk=FALSE );
        //{{AFX_MSG(StationModesOverviewGridCtrl)
        afx_msg void OnKillFocus(CWnd* pNewWnd);
        afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
        afx_msg void OnSetFocus(CWnd* pOldWnd);
        afx_msg void OnMouseMove(UINT nFlags, CPoint point);
        afx_msg void OnTimer(UINT nIDEvent);
	    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		//}}AFX_MSG
        DECLARE_MESSAGE_MAP()

	public:
		std::vector<LocationIndexAndProposedType> m_selectedrowProposedTypes;
		LocationIndexAndAutoModeMap m_selectedrowAutoMode;	//CL17276
		CCellID m_CurrSelectedCell;
		static const int NUM_LOCATIONS_PER_GRID;

    private:

        
        CellCoordinateVector m_cellsRequiringFlash;
        bool m_flashToggle;
        bool m_isFlashing;

	    SelectedCell m_selectedCell;
		int m_numLoadedLocations;

		int m_proposedRow;
		bool m_ProposedRowSelected;
		bool m_isAutoModeRowSelected;	//CL17276

		bool m_isAutoModeApplied;

		bool m_isMultiSelected;		//CL-20791
		
        /////////////////////////////////////////////////////////////////////////////

        //{{AFX_INSERT_LOCATION}}
        // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
    };
} // namespace TA_IRS_App
#endif  // !defined(AFX_STATIONMODESOVERVIEWGRIDCTRL_H__05AFACD7_192D_478A_A840_1EEAC31434AB__INCLUDED_)
