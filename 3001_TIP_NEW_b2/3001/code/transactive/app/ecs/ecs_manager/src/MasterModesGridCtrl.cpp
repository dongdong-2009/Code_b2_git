/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ecs/ecs_manager/src/MasterModesGridCtrl.cpp $
  * @author:  Robert Young
  * @version: $Revision: #11 $
  *
  * Last modification: $DateTime: 2015/05/25 16:26:54 $
  * Last modified by:  $Author: qi.huang $
  *
  * <description>
  *
  */

#include "app/ecs/ecs_manager/src/stdafx.h"
#include "app/ecs/ecs_manager/src/ecsmanager.h"
#include "app/ecs/ecs_manager/src/MasterModesGridCtrl.h"
#include "app/ecs/ecs_manager/src/ECSManagerConfigurationAccess.h"
#include "app/ecs/ecs_manager/src/ECSColourAccessor.h"

#include "core/utilities/src/DebugUtil.h"
#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

namespace TA_IRS_App
{
    const char* const TYPEFACE_FOR_CONTROLS = "Arial Bold";
    const int     FONTSIZE_FOR_CONTROLS = 100;
    const int SELECTED_CELL_FONT_WEIGHT = 600;   // weight of text for selected items
    const int NUM_FIXED_COLUMNS_FOR_NORMAL = 1;
    const int NUM_FIXED_COLUMNS_FOR_EMERGENCY = 6;
    const int NUM_FIXED_ROWS = 2;
    const int LOCATION_ROW = 0;
    const int CURRENT_STATION_MODE_ROW = 1;
    const int INITIAL_LOCATION_COLUMN_WIDTH = 80;
    const char* const FIXED_ROW_LABELS[ NUM_FIXED_COLUMNS_FOR_EMERGENCY ] =
    {
        "Master Mode", 
        "Operation",  
        "Track", 
        "Location", 
        "Evacuation Direction",
        "Airflow Direction",
    };
    const int INITIAL_FIXED_COLUMN_WIDTHS[ NUM_FIXED_COLUMNS_FOR_EMERGENCY ] =
    {
        97,
        100,
        60,
        150,
        147,
        135,
    };

    //
    // Constructor
    //
    MasterModesGridCtrl::MasterModesGridCtrl()
    : GridCtrlBorderSelection(), m_flashToggle( false ), m_isFlashing( false ), m_aveCharWidth( 0 )
    {
        FUNCTION_ENTRY( "Constructor" );

        FUNCTION_EXIT;
    }   


    //
    // Destructor
    //
    MasterModesGridCtrl::~MasterModesGridCtrl()
    {
        FUNCTION_ENTRY( "Destructor" );

        //
        // nothing to clean up.
        //

        FUNCTION_EXIT;
    }


    //
    // compareMasterModeGridCellCoordinates
    //
    bool compareMasterModeGridCellCoordinates( std::pair< int, int > cellCoord1, std::pair< int, int > cellCoord2 )
    {
        if ( cellCoord1.first != cellCoord2.first )
        {
            return ( cellCoord1.first < cellCoord2.first );
        }
        else
        {
            return ( cellCoord1.second < cellCoord2.second );
        }
    }


    //
    // setUpGridCtrl
    //
    void MasterModesGridCtrl::setUpGridCtrl()
    {
        FUNCTION_ENTRY( "setUpGridCtrl" );

        // Set the font on the control.
        CFont arialBold;
        arialBold.CreatePointFont(FONTSIZE_FOR_CONTROLS, TYPEFACE_FOR_CONTROLS);
        SetFont(&arialBold);

        // Determine max char width.
        // Used for workaround for drawing issue of centered text in columns when resized to 
        // width less than character size.
        TEXTMETRIC tm;
        CDC * pDC = GetDC(); 
        pDC->SelectObject( GetFont() );
        pDC->GetTextMetrics( &tm );
        m_aveCharWidth = tm.tmAveCharWidth;
        ReleaseDC(pDC);


        SetFixedRowCount( NUM_FIXED_ROWS );
        if ( NORMAL_MASTER_MODES == ECSManagerConfigurationAccess::getInstance().getDialogType() )
        {
            SetFixedColumnCount( NUM_FIXED_COLUMNS_FOR_NORMAL );
        }
        else
        {
            SetFixedColumnCount( NUM_FIXED_COLUMNS_FOR_EMERGENCY );
        }


        for ( int col = 0; col < GetColumnCount(); col++ )
        {			
		  
            SetItemText( CURRENT_STATION_MODE_ROW, col, FIXED_ROW_LABELS[ col ] );
            SetColumnWidth( col, INITIAL_FIXED_COLUMN_WIDTHS[ col ] );
            for ( int row = 0; row < NUM_FIXED_ROWS; row++ )
            {
                SetItemFormat( row, col, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
            }
        }

        // Make sure initialised with at least one non-fixed row/column 
        // (else potential GridCtrl Asserts Fail if no Master Modes are later added)
        SetRowCount( GetRowCount() + 1 ); 
        SetColumnCount( GetColumnCount() + 1 );
        m_isCenteredTextVector = IsCenteredTextVector( 1, true );

        FUNCTION_EXIT;
    }


    //
    // setLocations
    //
    void MasterModesGridCtrl::setLocations( const LocationNames& locationNames )
    {
        FUNCTION_ENTRY( "setLocations" );

		m_numLoadedLocations = locationNames.size();

        // safe to remove our existing non-fixed columns as
        // definitely inserting at least one newbie.
        for ( int nonFixedIndex = GetFixedColumnCount(); nonFixedIndex < GetColumnCount(); nonFixedIndex++ )
        {
            DeleteColumn( nonFixedIndex );
        }

        int insertIndex = 0;
        for ( LocationNames::const_iterator locationNamesIt = locationNames.begin(); 
            locationNamesIt != locationNames.end(); ++locationNamesIt )
        {
            insertIndex = InsertColumn( _T( ( *locationNamesIt ).c_str() ), 
                DT_CENTER | DT_VCENTER | DT_SINGLELINE, -1 );
            SetColumnWidth( insertIndex, INITIAL_LOCATION_COLUMN_WIDTH );
            SetItemBkColour( CURRENT_STATION_MODE_ROW, insertIndex, GetTextBkColor() );
            RedrawColumn( insertIndex );
        }

        // Used for workaround for drawing issue of centered text in columns when resized to 
        // width less than character size.
        m_isCenteredTextVector = IsCenteredTextVector( GetColumnCount(), true );

        RedrawWindow();

        FUNCTION_EXIT;
    }


    //
    // populateMasterModeDetails
    //
    void MasterModesGridCtrl::populateMasterModeDetails( const MasterModeDetails& masterModeDetails )
    {
        FUNCTION_ENTRY( "populateMasterModeDetails" );

        // Clear any existing.
        for ( int nonFixedIndex = NUM_FIXED_ROWS; nonFixedIndex <= GetRowCount(); nonFixedIndex++ )
        {
            DeleteRow( nonFixedIndex ); 
        }

        if ( masterModeDetails.empty() )
        {
            // Leave Grid in safe state (i.e. with at least one non fixed row )
            // else potential ASSERTs in base class will fail)
            SetRowCount( NUM_FIXED_ROWS + 1 );
            FUNCTION_EXIT;
            return;
        }

        int insertIndex = 0;
        int associatedModeIndex = 0;
        StationModesAsStrings::const_iterator stationModesAsStrIt;

        for (MasterModeDetails::const_iterator masterModeDetailsIt = masterModeDetails.begin(); masterModeDetailsIt != masterModeDetails.end(); ++masterModeDetailsIt)
        {
            insertIndex = InsertRow( _T( ( *masterModeDetailsIt ).masterModeAsString.c_str() ), -1 );
            SetItemBkColour( insertIndex, 0, GetTextBkColor() );

            if ( NUM_FIXED_COLUMNS_FOR_EMERGENCY == GetFixedColumnCount() )
            {
                SetItemText( insertIndex, 1, _T( ( *masterModeDetailsIt ).condition.c_str() ) );				
                SetItemBkColour( insertIndex, 1, GetTextBkColor() );

                SetItemText( insertIndex, 2, _T( ( *masterModeDetailsIt ).zoneTrack.c_str() ) );
                SetItemBkColour( insertIndex, 2, GetTextBkColor() );

				std::string strLocation = ( *masterModeDetailsIt ).situation.empty() ? ( *masterModeDetailsIt ).zoneLocation : (( *masterModeDetailsIt ).zoneLocation + ": (" + ( *masterModeDetailsIt ).situation + ")");
                SetItemText( insertIndex, 3, _T( strLocation.c_str() ) );
                SetItemBkColour( insertIndex, 3, GetTextBkColor() );

                SetItemText( insertIndex, 4, _T( ( *masterModeDetailsIt ).evacuationDirection.c_str() ) );
                SetItemBkColour( insertIndex, 4, GetTextBkColor() );

                SetItemText( insertIndex, 5, _T( ( *masterModeDetailsIt ).airflowDirection.c_str() ) );
                SetItemBkColour( insertIndex, 5, GetTextBkColor() );
            }

            const StationModesAsStrings& stationModesAsStr = masterModeDetailsIt->associatedStationModesAsStrings;
            for ( stationModesAsStrIt = stationModesAsStr.begin(), associatedModeIndex = GetFixedColumnCount(); 
                associatedModeIndex < GetColumnCount() && ( stationModesAsStr.end() != stationModesAsStrIt ); ++associatedModeIndex, ++stationModesAsStrIt)
            {
                SetItemText( insertIndex, associatedModeIndex, _T( ( *stationModesAsStrIt ).c_str() ) );
            }

            RedrawRow( insertIndex );
        }

		EnableSelection( TRUE );

        RedrawWindow();

        FUNCTION_EXIT;
    }


    //
    // setCurrentStationMode
    //
    void MasterModesGridCtrl::setCurrentStationMode( int locationIndex, const std::string& stationMode, 
                               ECurrentStationModeType currentStationModeType )
    {
        FUNCTION_ENTRY( "setCurrentStationMode" );

        CellCoordinate cellCoord( CURRENT_STATION_MODE_ROW, ( GetFixedColumnCount() + locationIndex ) );

		// change the color if MODE_FAILED
		//if (currentStationModeType == TA_IRS_App::CURRENT_STATION_MODE_FAILED)
		//	SetFixedTextColor(0x00FFFFFF);
		//else
			SetFixedTextColor(0x00000000);
				
        SetItemText( cellCoord.first, cellCoord.second, _T( stationMode.c_str() ) );
        SetItemData( cellCoord.first, cellCoord.second, static_cast< LPARAM >( currentStationModeType ) );

		// marvin++
		if(stationMode != "#" && stationMode != "0")
		{
			//zn++ CL-20840
			//unsigned int modeIndex = atoi(stationMode.c_str());
			int modeIndex = atoi(stationMode.c_str());
			//++zn
			SetItemBkColour( cellCoord.first, cellCoord.second, 
				TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(modeIndex) );
		}
		else
		{
			SetItemBkColour( cellCoord.first, cellCoord.second, 
				TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(currentStationModeType) );
		}
		// ++marvin

		

        RedrawCell( cellCoord.first, cellCoord.second );
				
		// restore the color
		//SetFixedTextColor(0x00000000);
		
		// 10745b-Start
		if(IsCellVisible(cellCoord.first, cellCoord.second) == FALSE)
			EnsureVisible(cellCoord.first, cellCoord.second);		
		// 10745b-End

		// ISSUE
		// Changing the fixed text color to an individual cell works fine, until this grid
		// looses control and regains it..then it tries to redraw in OnKillFocus() and OnSetFocus()
		// redrawing with the wrong column text color.
		// must retrieve the cell data and check for the mode then change the colour, then redraw
		// to fix the problem

        FUNCTION_EXIT;
    }


    //
    // setCurrentStationModeInProgress
    //
    void MasterModesGridCtrl::setCurrentStationModeInProgress( int locationIndex, BOOL doShowAsInProgress )
    {
        FUNCTION_ENTRY( "setCurrentStationModeInProgress" );

        CellCoordinate cellCoord( CURRENT_STATION_MODE_ROW, ( GetFixedColumnCount() + locationIndex ) );
        bool isInCellsRequiringFlashList = std::binary_search( m_cellsRequiringFlash.begin(), 
                m_cellsRequiringFlash.end(), cellCoord, compareMasterModeGridCellCoordinates );
        if ( doShowAsInProgress && ( false == isInCellsRequiringFlashList ) )
        {
            m_cellsRequiringFlash.push_back( cellCoord );
            std::sort( m_cellsRequiringFlash.begin(), m_cellsRequiringFlash.end(), compareMasterModeGridCellCoordinates );
            if ( false == m_isFlashing )
            {
                SetTimer( FLASH_TIMER_ID, FLASH_INTERVAL, NULL );
                m_isFlashing = true;
            }
            if ( m_flashToggle )
            {
                SetItemBkColour( cellCoord.first, cellCoord.second, GetTextBkColor() );
            }
            else if ( false == m_flashToggle )
            {
				//zn++ CL-20840
// 				ECurrentStationModeType stationModeType = static_cast< ECurrentStationModeType >( GetItemData(cellCoord.first, cellCoord.second));
//                 SetItemBkColour( cellCoord.first, cellCoord.second, 
// 					TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(stationModeType) );				
				
				std::string modeString = GetItemText(cellCoord.first, cellCoord.second);
				int modeValue = atoi(modeString.c_str());
				SetItemBkColour(cellCoord.first, cellCoord.second, TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(modeValue));
				//++zn
            }

            RedrawCell( cellCoord.first, cellCoord.second );
        }
        else if ( ( false == doShowAsInProgress ) && isInCellsRequiringFlashList )
        {
            m_cellsRequiringFlash.erase( std::lower_bound(  m_cellsRequiringFlash.begin(), 
                m_cellsRequiringFlash.end(), cellCoord, compareMasterModeGridCellCoordinates ) );
            if ( ( 0 == m_cellsRequiringFlash.size() ) && m_isFlashing )
            {
                KillTimer( FLASH_TIMER_ID );
                m_isFlashing = false;
                m_flashToggle = false;
            }
            // Perhaps should check if visible also (probably no more efficient perhaps less efficient, so safer just to
            // set bk color and redraw any how)
			//zn++ CL-20840
// 			ECurrentStationModeType stationModeType = static_cast< ECurrentStationModeType >( GetItemData(cellCoord.first, cellCoord.second));
//             SetItemBkColour( cellCoord.first, cellCoord.second, 
// 				TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(stationModeType) );

			std::string modeString = GetItemText(cellCoord.first, cellCoord.second);
			int modeValue = atoi(modeString.c_str());
			SetItemBkColour(cellCoord.first, cellCoord.second, TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(modeValue));

			RedrawCell( cellCoord.first, cellCoord.second );
			//++zn
        }

        FUNCTION_EXIT;
    }


    //
    // setCurrentProposedMasterMode
    //
    void MasterModesGridCtrl::setCurrentProposedMasterMode( int masterModeIndex, EModeProposalState proposalState )
    {
        FUNCTION_ENTRY( "setCurrentProposedMasterMode" );

        CellCoordinate cellCoord( ( NUM_FIXED_ROWS + masterModeIndex ), 0 );
        bool isInCellsRequiringFlashList = std::binary_search( m_cellsRequiringFlash.begin(), m_cellsRequiringFlash.end(), cellCoord, compareMasterModeGridCellCoordinates );
    
        if ( proposalState != NOT_CURRENTLY_PROPOSED )
        {
            if ( !isInCellsRequiringFlashList )
            {
                m_cellsRequiringFlash.push_back( cellCoord );
                std::sort( m_cellsRequiringFlash.begin(), m_cellsRequiringFlash.end(), compareMasterModeGridCellCoordinates );
                if ( !m_isFlashing )
                {
                    SetTimer( FLASH_TIMER_ID, FLASH_INTERVAL, NULL );
                    m_isFlashing = true;
                }
            }
            for ( int col = 0; col < GetColumnCount(); col++ )
            {
                if ( m_flashToggle )
                {
                    SetItemBkColour( cellCoord.first, col, GetTextBkColor() );
                }
                else if ( !m_flashToggle )
                {
                    if ( CURRENTLY_PROPOSED == proposalState )
                    {
                        SetItemBkColour( cellCoord.first, col, 
							TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(CURRENTLY_PROPOSED_COLORREF) );
                    }
                    else if ( CURRENTLY_OVERRIDING_PROPOSED == proposalState )
                    {
                        SetItemBkColour( cellCoord.first, col,
                            TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(CURRENTLY_OVERRIDING_PROPOSED_COLORREF) );
                    }
                }
            }
        }
        else if ( proposalState == NOT_CURRENTLY_PROPOSED )
        {
            if ( isInCellsRequiringFlashList )
            {
                m_cellsRequiringFlash.erase( std::lower_bound(  m_cellsRequiringFlash.begin(), 
                            m_cellsRequiringFlash.end(), cellCoord, compareMasterModeGridCellCoordinates ) );
                if ( ( 0 == m_cellsRequiringFlash.size() ) && m_isFlashing )
                {
                    KillTimer( FLASH_TIMER_ID );
                    m_isFlashing = false;
                    m_flashToggle = false;
                }
            }
            for ( int col = 0; col < GetColumnCount(); col++ )
            {
                SetItemBkColour( cellCoord.first, col, GetTextBkColor() );
            }
        }

        SetItemData( cellCoord.first, cellCoord.second, static_cast< LPARAM >( proposalState ) );

        for ( int col = 0; col < GetColumnCount(); col++ )
        {
            RedrawCell( cellCoord.first, col );
        }

        FUNCTION_EXIT;
    }



    BEGIN_MESSAGE_MAP(MasterModesGridCtrl, CGridCtrl)
        //{{AFX_MSG_MAP(MasterModesGridCtrl)
        ON_WM_KILLFOCUS()
        ON_WM_KEYDOWN()
        ON_WM_SETFOCUS()
        ON_WM_MOUSEMOVE()
        ON_WM_TIMER()
        ON_WM_LBUTTONDOWN()
        ON_WM_LBUTTONDBLCLK()
        ON_WM_LBUTTONUP()
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    //
    // PreSubclassWindow
    //
    void MasterModesGridCtrl::PreSubclassWindow() 
    {
        FUNCTION_ENTRY( "PreSubclassWindow" );

        EnableSelection( FALSE );
        SetEditable( FALSE );
        EnableDragAndDrop( FALSE );
        SetRowResize( FALSE );
        SetColumnResize( TRUE );

        // Set the background colour of the control so we cannot see where the background if the columns and rows are
        // smaller than the control.
        SetBkColor( GetSysColor(COLOR_3DFACE) );

        CGridCtrl::PreSubclassWindow();

        FUNCTION_EXIT;
    }


    //
    // OnTimer
    //
    void MasterModesGridCtrl::OnTimer(UINT nIDEvent) 
    {
        switch ( nIDEvent )
        {
        case FLASH_TIMER_ID:
            {
                m_flashToggle = !m_flashToggle;

                CellCoordinateVector::iterator cellsReqFlashIt = m_cellsRequiringFlash.begin();
                while ( cellsReqFlashIt != m_cellsRequiringFlash.end() )
                {
                    if ( m_flashToggle )
                    {
                        if ( ( *cellsReqFlashIt ).first >= NUM_FIXED_ROWS )
                        {
                            for ( int col = 0; col < GetColumnCount(); col++ )
                            {
                                SetItemBkColour( ( *cellsReqFlashIt ).first, col, GetTextBkColor() );
                            }
                        }
                        else
                        {
                            SetItemBkColour( ( *cellsReqFlashIt ).first, ( *cellsReqFlashIt ).second, GetTextBkColor() );
                        }

                    }
                    else if ( ( *cellsReqFlashIt ).first == CURRENT_STATION_MODE_ROW )
                    {
						std::string modeString = GetItemText(( *cellsReqFlashIt ).first, ( *cellsReqFlashIt ).second);
						int modeValue = atoi(modeString.c_str());
						SetItemBkColour(( *cellsReqFlashIt ).first, ( *cellsReqFlashIt ).second, TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(modeValue));
                    }
                    else 
                    {
                        EModeProposalState proposalState = static_cast<EModeProposalState>( GetItemData( ( *cellsReqFlashIt ).first, ( *cellsReqFlashIt ).second ) );
                        if ( CURRENTLY_PROPOSED == proposalState )
                        {
                            for ( int col = 0; col < GetColumnCount(); col++ )
                            {
                                SetItemBkColour( ( *cellsReqFlashIt ).first, col, 
                                    TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(CURRENTLY_PROPOSED_COLORREF) );
                            }
                        }
                        else if ( CURRENTLY_OVERRIDING_PROPOSED == proposalState )
                        {
                            for ( int col = 0; col < GetColumnCount(); col++ )
                            {
                                SetItemBkColour( ( *cellsReqFlashIt ).first, col,
                                    TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(CURRENTLY_OVERRIDING_PROPOSED_COLORREF) );
                            }
                        }

                    }

                    if ( ( *cellsReqFlashIt ).first >= NUM_FIXED_ROWS )
                    {
                        for ( int col = 0; col < GetColumnCount(); col++ )
                        {
                            RedrawCell( ( *cellsReqFlashIt ).first, col );
                        }
                    }
                    else
                    {
                        RedrawCell( ( *cellsReqFlashIt ).first, ( *cellsReqFlashIt ).second );
                    }

                    cellsReqFlashIt++;
                }
            }
            break;
        default:
            break;
        }
        // Do nothing (we want to nullify the Timer Behaviour of the Parent so send to CWnd not CGridCtrl ) 
        CWnd::OnTimer(nIDEvent);
    }



    //////////////////////////////////////////////////////////
    // The functions below are merely slightly altered reimplementations 
    // of some of the CGridCtrl's virtual functions and message 
    // handling so as to change the appearance of the grid to
    // suit this application.
    ///////////////////////////////////////////////////////////


    //
    // DrawCell
    //
    BOOL MasterModesGridCtrl::DrawCell( CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk )
    {
        if (!m_bAllowDraw) 
            return FALSE;

        GV_ITEM Item;
        Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_STATE | GVIF_IMAGE | GVIF_BKCLR | GVIF_FGCLR;
        Item.row = nRow;
        Item.col = nCol;
        if (!GetItem(&Item))
            return FALSE;

        COLORREF TextBkClr = (Item.crBkClr == CLR_DEFAULT)? GetTextBkColor() : Item.crBkClr;
        COLORREF TextClr   = (Item.crFgClr == CLR_DEFAULT)? GetTextColor()   : Item.crFgClr;

        bEraseBk = TRUE;

        int nSavedDC = pDC->SaveDC();

        pDC->SetBkMode(TRANSPARENT);

        if (Item.state & GVIS_FOCUSED 
    #if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
            && !pDC->IsPrinting()
    #endif
            ) 
        {
         /*   rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
            if (bEraseBk) 
            {
                CBrush brush(TextBkClr);
                pDC->FillRect(rect, &brush);
            }
            if ( GetFocus() == this )
            {
                pDC->FillSolidRect(rect, ::GetSysColor(COLOR_HIGHLIGHT));
                pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
            }
            else
            {
                pDC->FillSolidRect(rect, ::GetSysColor(COLOR_BTNFACE));
                pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT));
            }
            rect.right--; rect.bottom--; */
			if(!m_selectedRow->isRowSelected)
			{
				rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
				if (bEraseBk) 
				{
					CBrush brush(TextBkClr);					
					pDC->FillRect(rect, &brush);
					
				}

				rect.right--; rect.bottom--;  
				CPen pen;
				TRY {
					
					// 10745j- Start
					// Select a blue pen instead of a black pen
					//pen.CreatePen(PS_SOLID, 3, RGB(256, 256, 256));
					pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
					// 10745j- End
				
				}
				CATCH (CResourceException, e)
				{
					e->Delete();
					return FALSE;
				}
				END_CATCH
				pDC->SelectObject(&pen);

				pDC->SelectStockObject(NULL_BRUSH);				
				pDC->Rectangle(rect);				
				pDC->SetTextColor(TextClr);
			}
        }
        else if ( Item.row == m_idCurrentCell.row
    #if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
            && !pDC->IsPrinting()
    #endif
            ) 
        {
          /*  rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
            if ( GetFocus() == this )
            {
                pDC->FillSolidRect(rect, ::GetSysColor(COLOR_HIGHLIGHT));
                pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
            }
            else
            {
                pDC->FillSolidRect(rect, ::GetSysColor(COLOR_BTNFACE));
                pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT));
            }
            rect.right--; rect.bottom--;*/
			if(nCol <= m_numLoadedLocations + NUM_FIXED_COLUMNS_FOR_EMERGENCY - 1 )
			{
				// Don't delete rowCell because the GridControl does this!
				CGridCell* rowCell = GetCell(nRow, nCol);
				rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
				if(rowCell->crBkClr != CLR_DEFAULT)
				{
					
					pDC->FillSolidRect(rect, rowCell->crBkClr);
					//pDC->FillSolidRect(rect, RGB(255,0,0));					
				}
				
				rect.right--; rect.bottom--;
				pDC->SetTextColor(rowCell->crFgClr);
			}
        } else {
            rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
            if (bEraseBk) 
            {
                CBrush brush(TextBkClr);				
                pDC->FillRect(rect, &brush);				
            }
            rect.right--; rect.bottom--;
            pDC->SetTextColor(TextClr);
        }

        if (Item.state & GVIS_DROPHILITED 
    #if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
            && !pDC->IsPrinting()
    #endif
            )
        {
            pDC->SelectStockObject(BLACK_PEN);
            pDC->SelectStockObject(NULL_BRUSH);		
            pDC->Rectangle(rect);			
        }

        // Create the appropriate font and select into DC
        LOGFONT *pLF = GetItemFont(nRow, nCol);
   
        CFont Font;
        Font.CreateFontIndirect(pLF);
        pDC->SelectObject(&Font);

        rect.DeflateRect(m_nMargin, 0);

        if (m_pImageList && Item.iImage >= 0)
        {
            IMAGEINFO Info;
            if (m_pImageList->GetImageInfo(Item.iImage, &Info))
            {
                int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
                m_pImageList->Draw(pDC, Item.iImage, rect.TopLeft(), ILD_NORMAL);
                rect.left += nImageWidth+m_nMargin;
            }
        }

        DrawText(pDC->m_hDC, Item.szText, -1, rect, Item.nFormat);

        pDC->RestoreDC(nSavedDC);
        Font.DeleteObject();

        return TRUE;
    }


    //
    // DrawFixedCell
    //
    BOOL  MasterModesGridCtrl::DrawFixedCell( CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk )
    {
        if (!m_bAllowDraw) 
            return FALSE;

        GV_ITEM Item;
        Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_IMAGE | GVIF_BKCLR | GVIF_FGCLR;
        Item.row = nRow;
        Item.col = nCol;
        if (!GetItem(&Item))
            return FALSE;

        bEraseBk = TRUE;

        CCellID FocusCell = GetFocusCell();

        if (bEraseBk
    #if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
            && !pDC->IsPrinting()
    #endif
            )
        {
            if ( FocusCell.row == nRow )
            {
                if ( GetFocus() == this )
                {
                    //CBrush brush( ::GetSysColor(COLOR_HIGHLIGHT) );
                    //pDC->FillRect(rect, &brush);
                    //pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));


					CGridCell* rowCell = GetCell(nRow, nCol);
					
					if(rowCell->crBkClr != CLR_DEFAULT)
					{					
						pDC->FillSolidRect(rect, rowCell->crBkClr);
					}

					pDC->SetTextColor(RGB(0, 0, 0));

                }
                else
                {
                    CBrush brush( ::GetSysColor(COLOR_BTNFACE) );
                    pDC->FillRect(rect, &brush);
                    pDC->SetTextColor(::GetSysColor(COLOR_BTNTEXT));
                }
            }
            else
            {
                CBrush brush((Item.crBkClr == CLR_DEFAULT)? GetFixedBkColor() : Item.crBkClr);
                pDC->FillRect(rect, &brush);
                pDC->SetTextColor((Item.crFgClr == CLR_DEFAULT)? GetFixedTextColor() : Item.crFgClr);
            }
        }

        int nSavedDC = pDC->SaveDC();

        // Create the appropriate font and select into DC
        LOGFONT *pLF = GetItemFont(nRow, nCol);

        if (FocusCell.row == nRow || FocusCell.col == nCol)
        {
            static LOGFONT lf;
            memcpy(&lf, pLF, sizeof(LOGFONT));
            lf.lfWeight = SELECTED_CELL_FONT_WEIGHT;
            pLF = &lf;
        }


        CFont Font;
        Font.CreateFontIndirect(pLF);
        pDC->SelectObject(&Font);

        if ( IsValid(FocusCell) && (FocusCell.row == nRow || FocusCell.col == nCol) &&
            nRow >= NUM_FIXED_ROWS  ) // Don't want to sink the locations above focus cell for the Master Modes GridCtrl
        {
            rect.right++; rect.bottom++;
            pDC->DrawEdge(rect, BDR_SUNKENINNER /*EDGE_RAISED*/, BF_RECT);
            rect.DeflateRect(1,1);
        }
        else
        {
            CPen lightpen(PS_SOLID, 1,  ::GetSysColor(COLOR_3DHIGHLIGHT)),
                  darkpen(PS_SOLID,  1, ::GetSysColor(COLOR_3DDKSHADOW)),
                 *pOldPen = pDC->GetCurrentPen();

            pDC->SelectObject(&lightpen);
            pDC->MoveTo(rect.right, rect.top);
            pDC->LineTo(rect.left, rect.top);
            pDC->LineTo(rect.left, rect.bottom);

            pDC->SelectObject(&darkpen);
            pDC->MoveTo(rect.right, rect.top);
            pDC->LineTo(rect.right, rect.bottom);
            pDC->LineTo(rect.left, rect.bottom);

            pDC->SelectObject(pOldPen);
            rect.DeflateRect(1,1);
        }

        pDC->SetBkMode(TRANSPARENT);
        rect.DeflateRect(m_nMargin, 0);

        if (m_pImageList && Item.iImage >= 0)
        {
            IMAGEINFO Info;
            if (m_pImageList->GetImageInfo(Item.iImage, &Info))
            {
                //CRgn rgn;
                //rgn.CreateRectRgnIndirect(rect);
                //pDC->SelectClipRgn(&rgn);
                //rgn.DeleteObject();

                int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
                m_pImageList->Draw(pDC, Item.iImage, rect.TopLeft(), ILD_NORMAL);
                rect.left += nImageWidth+m_nMargin;
            }
        }

        DrawText(pDC->m_hDC, Item.szText, -1, rect, Item.nFormat);

        pDC->RestoreDC(nSavedDC);
        Font.DeleteObject();

        return TRUE;
    }


    //
    // OnKillFocus
    //
    void MasterModesGridCtrl::OnKillFocus(CWnd* pNewWnd) 
    {
        CGridCtrl::OnKillFocus(pNewWnd);
        CCellID focusCell = GetFocusCell();
        for ( int col = 0; col < GetColumnCount(); col++ )
        {
            RedrawCell( focusCell.row, col );
        }
    }


    //
    // OnKeyDown
    //
    void MasterModesGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
    {
        switch ( nChar )
        {
        case VK_TAB:
            {
                // Desired Tab behavior (instead of moving to next cell as in CridCtrl) 
                // is to set focus to next control in tab order.
                GetParent()->PostMessage( WM_NEXTDLGCTL);
                return;
            }
        case VK_RIGHT:
            {
                int scrollPos = GetScrollPos( SB_HORZ );
                int scrollMin = 0, scrollMax = 0; 
                GetScrollRange( SB_HORZ, &scrollMin, &scrollMax );
                if ( scrollPos < scrollMax )
                {
                    PostMessage( WM_HSCROLL, SB_LINERIGHT, 0 );
                }
                return;
            }
        case VK_LEFT:
            {
                int scrollPos = GetScrollPos( SB_HORZ );
                int scrollMin = 0, scrollMax = 0; 
                GetScrollRange( SB_HORZ, &scrollMin, &scrollMax );
                if ( scrollPos > scrollMin )
                {
                    PostMessage( WM_HSCROLL, SB_LINELEFT, 0 );
                }
                return;
            }
        default:
            {
                CGridCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
				CCellID focusCell = GetFocusCell();
				//MasterModesGridCtrl::OnLButtonDown(nFlags, focusCell);
				Invalidate();
				SelectRows(focusCell);
				CDC* pDC = GetDC();
				if(m_selectedRow->isRowSelected)
				{
					CPen pen;
					TRY
					{
						pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 255));	
					}
					CATCH (CResourceException, e)
					{
						e->Delete();
						return;
					}
					END_CATCH
					pDC->SelectObject(&pen);

					CRect selectedRect;
					GetCellRangeRect(m_selectedRow->selectedRange, selectedRect);
					pDC->SelectStockObject(NULL_BRUSH);
					selectedRect.left = 0;
					pDC->Rectangle(selectedRect);
				}
            }
        }
    }


    //
    // OnSetFocus
    //
    void MasterModesGridCtrl::OnSetFocus(CWnd* pOldWnd) 
    {
        CGridCtrl::OnSetFocus(pOldWnd);
        CCellID focusCell = GetFocusCell();

        for ( int col = 0; col < GetColumnCount(); col++ )
        {
            RedrawCell( focusCell.row, col );
        }
    }


    //
    // OnMouseMove
    //
    void MasterModesGridCtrl::OnMouseMove(UINT nFlags, CPoint point) 
    {
        CRect rect;
        GetClientRect(rect);

    #ifndef GRIDCONTROL_NO_DRAGDROP
        // If outside client area, return (unless we are drag n dropping)
        if (m_MouseMode != MOUSE_DRAGGING && !rect.PtInRect(point))
            return;
    #endif

        // If the left mouse button is up, then test to see if row/column sizing is imminent
        if (!(nFlags & MK_LBUTTON) 
            || (m_MouseMode == MOUSE_NOTHING && (nFlags & MK_LBUTTON)))
        {
            if (m_bAllowColumnResize && MouseOverColumnResizeArea(point))
            {
                if (m_MouseMode != MOUSE_OVER_COL_DIVIDE)
                {
                    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
                    m_MouseMode = MOUSE_OVER_COL_DIVIDE;
                }
            }
            else if (m_bAllowRowResize && MouseOverRowResizeArea(point))
            {
                if (m_MouseMode != MOUSE_OVER_ROW_DIVIDE)
                {
                    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
                    m_MouseMode = MOUSE_OVER_ROW_DIVIDE;
                }
            }
            else if (m_MouseMode != MOUSE_NOTHING)
            {
                SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
                m_MouseMode = MOUSE_NOTHING;
            }

    #ifndef GRIDCONTROL_NO_TITLETIPS
            if (m_MouseMode == MOUSE_NOTHING && m_bTitleTips)
            {
                CCellID idCurrentCell = GetCellFromPt(point);
                CRect TextRect, CellRect;
                if (GetTextRect(idCurrentCell.row, idCurrentCell.col, TextRect) &&
                    GetCellRect(idCurrentCell.row, idCurrentCell.col, CellRect))
                    m_TitleTip.Show(TextRect, 
                                    GetItemText(idCurrentCell.row, idCurrentCell.col), 
                                    0,
                                    CellRect,
                                    GetItemFont(idCurrentCell.row, idCurrentCell.col));
            }
    #endif

            m_LastMousePoint = point;
            return;
        }

        if (!IsValid(m_LeftClickDownCell))
        {
            m_LastMousePoint = point;
            return;
        }

        // If the left mouse button is down, then process appropriately
        if (nFlags & MK_LBUTTON) 
        {
            switch(m_MouseMode)
            {
                case MOUSE_SELECT_ALL:        
                    break;

                 case MOUSE_SIZING_COL:   
                    {
                        CDC* pDC = GetDC();
                        if (!pDC) break;
                    
                        CRect oldInvertedRect(m_LastMousePoint.x, rect.top, 
                            m_LastMousePoint.x + 2, rect.bottom);
                        pDC->InvertRect(&oldInvertedRect);
                        CRect newInvertedRect(point.x, rect.top, 
                            point.x + 2, rect.bottom);
                        pDC->InvertRect(&newInvertedRect);
                        ReleaseDC(pDC);
                    }
                    break;
                
                case MOUSE_SIZING_ROW:        
                    {
                        CDC* pDC = GetDC();
                        if (!pDC) break;
                    
                        CRect oldInvertedRect(rect.left, m_LastMousePoint.y, 
                            rect.right, m_LastMousePoint.y + 2);
                        pDC->InvertRect(&oldInvertedRect);
                        CRect newInvertedRect(rect.left, point.y, 
                            rect.right, point.y + 2);
                        pDC->InvertRect(&newInvertedRect);
                        ReleaseDC(pDC);
                    }
                    break;

    #ifndef GRIDCONTROL_NO_DRAGDROP
                case MOUSE_PREPARE_DRAG:   
                    OnBeginDrag();    
                    break;
    #endif
            }    
        }

        m_LastMousePoint = point;
    }


    //
    // OnLButtonDown
    //
   void MasterModesGridCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
    {
        CCellID clickDownCell = GetCellFromPt(point);
        if (!IsValid(clickDownCell)) return;

        SetFocus();
  
        if ( ( m_MouseMode != MOUSE_OVER_COL_DIVIDE ) && ( m_MouseMode != MOUSE_OVER_ROW_DIVIDE ) )
        {
            if ( clickDownCell.row < NUM_FIXED_ROWS )
            {
                return; // Don't want clicks on headers to change row selection.
            }
            if ( clickDownCell.row == GetFocusCell().row )
            {
                return; // Row selection won't change so no point changing focus cell.
            }
        }   
    
		HWND hOldFocusWnd = ::GetFocus();

		m_LeftClickDownPoint = point;
		m_LeftClickDownCell = GetCellFromPt(point);

		m_SelectionStartCell = (nFlags & MK_SHIFT)? m_idCurrentCell : m_LeftClickDownCell;

		// If the user clicks on the current cell, then prepare to edit it.
		// (If the user moves the mouse, then dragging occurs)
		if (m_LeftClickDownCell == m_idCurrentCell)
		{
			m_MouseMode = MOUSE_PREPARE_EDIT;
			return;
		}   
		else if (m_MouseMode != MOUSE_OVER_COL_DIVIDE &&
				 m_MouseMode != MOUSE_OVER_ROW_DIVIDE)        
		{
			if(m_LeftClickDownCell.col != 0)
			{
				if(m_selectedRow->isRowSelected)
				{
					// Clear the previous selection cell..
					for(int i=0; i<m_nCols-1; i++)
					{
						RedrawCell((m_selectedRow->selectedRange).GetMinRow(), i+1);
					}
				}

				m_selectedRow->isRowSelected = false;

				Invalidate();
				SelectRows(m_LeftClickDownCell);

			}	

			//SetFocusCell(-1,-1);
			SetFocusCell(max(m_LeftClickDownCell.row, m_nFixedRows),
                     0 );
		}

	#ifndef GRIDCONTROL_NO_DRAGDROP
		// If the user clicks on a selected cell, then prepare to drag it.
		// (If the user moves the mouse, then dragging occurs)
		if (m_bAllowDragAndDrop && hOldFocusWnd == GetSafeHwnd() && 
			GetItemState(m_LeftClickDownCell.row, m_LeftClickDownCell.col) & GVNI_SELECTED)
		{
			m_MouseMode = MOUSE_PREPARE_DRAG;
			return;
		}
	#endif

		SetCapture();

		if (m_MouseMode == MOUSE_NOTHING)
		{
			if (m_bAllowColumnResize && MouseOverColumnResizeArea(point))
			{
				if (m_MouseMode != MOUSE_OVER_COL_DIVIDE)
				{
					SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
					m_MouseMode = MOUSE_OVER_COL_DIVIDE;
				}
			}
			else if (m_bAllowRowResize && MouseOverRowResizeArea(point))
			{
				if (m_MouseMode != MOUSE_OVER_ROW_DIVIDE)
				{
					SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
					m_MouseMode = MOUSE_OVER_ROW_DIVIDE;
				}
			}
			//else if (m_MouseMode != MOUSE_NOTHING)
			//{
			//    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			//    m_MouseMode = MOUSE_NOTHING;
			//}
		}

		if (m_MouseMode == MOUSE_OVER_COL_DIVIDE) // sizing column
		{
			m_MouseMode = MOUSE_SIZING_COL;
			CPoint start;
			if (!GetCellOrigin(0, m_LeftClickDownCell.col, &start)) return;

			CRect rect;
			GetClientRect(rect);
			CRect invertedRect(point.x, rect.top, point.x + 2, rect.bottom);

			CDC* pDC = GetDC();
			if (pDC) {
				pDC->InvertRect(&invertedRect);
				ReleaseDC(pDC);
			}

			if (point.x - start.x <= m_nResizeCaptureRange)        // clicked right of border
				if (!GetCellOrigin(0, --m_LeftClickDownCell.col, &start)) return;

			rect.left = start.x;
			ClientToScreen(rect);
	#ifndef _WIN32_WCE_NO_CURSOR
			ClipCursor(rect);
	#endif
		}
		else if (m_MouseMode == MOUSE_OVER_ROW_DIVIDE) // sizing row
		{
			m_MouseMode = MOUSE_SIZING_ROW;
			CPoint start;
			if (!GetCellOrigin(m_LeftClickDownCell, &start)) return;

			CRect rect;
			GetClientRect(rect);
			CRect invertedRect(rect.left, point.y, rect.right, point.y + 2);

			CDC* pDC = GetDC();
			if (pDC) {
				pDC->InvertRect(&invertedRect);
				ReleaseDC(pDC);
			}

			if (point.y - start.y <= m_nResizeCaptureRange)            // clicked below border
				if (!GetCellOrigin(--m_LeftClickDownCell.row, 0, &start)) return;

			rect.top = start.y;
			ClientToScreen(rect);
	#ifndef _WIN32_WCE_NO_CURSOR
			ClipCursor(rect);
	#endif
		}
		else // not sizing or editing -- selecting
		{    
			// If Ctrl pressed, save the current cell selection. This will get added
			// to the new cell selection at the end of the cell selection process
			m_PrevSelectedCellMap.RemoveAll();
			if (nFlags & MK_CONTROL) {
				for (POSITION pos = m_SelectedCellMap.GetStartPosition(); pos != NULL; )
				{
					DWORD key;
					CCellID cell;
					m_SelectedCellMap.GetNextAssoc(pos, key, (CCellID&)cell);
					m_PrevSelectedCellMap.SetAt(key, cell);
				}
			}
        
			if (m_LeftClickDownCell.row < GetFixedRowCount())
				OnFixedRowClick(m_LeftClickDownCell);
			else if (m_LeftClickDownCell.col < GetFixedColumnCount())
				OnFixedColumnClick(m_LeftClickDownCell);
			
			else
			{
				m_MouseMode = m_bListMode? MOUSE_SELECT_ROW : MOUSE_SELECT_CELLS;
				//OnSelecting(m_LeftClickDownCell);
				SelectRows(m_LeftClickDownCell);
			}
			

			m_nTimerID = SetTimer(WM_LBUTTONDOWN, m_nTimerInterval, 0);			

		}   
		m_LastMousePoint = point;
    }


    //
    // OnLButtonDblClk
    //
    void MasterModesGridCtrl::OnLButtonDblClk(UINT nFlags, CPoint point) 
    {
        if ( ( m_MouseMode != MOUSE_OVER_COL_DIVIDE ) && ( m_MouseMode != MOUSE_OVER_ROW_DIVIDE ) )
        {
            // Don't want the default CGridCtrl behaviour of scrolling towards focus cell.
            return;
        }

        // Adjust text centering to work around problem of artifacts appearing when
        // columns get smaller character width (while text is set to DT_CENTER)

        // Flag whether a column resize will occur (and if so which column).
        bool doCheck = false;
        CCellID resizingCell;
        if (m_MouseMode == MOUSE_OVER_COL_DIVIDE) 
        {
            doCheck = true;
            resizingCell = GetCellFromPt(point);
			if ( !IsValid(resizingCell) )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "IsValidCell( GetCellFromPt() ) failed while mouse poition flagged as being over Column divide." );
				return;
			}

            CPoint start;
            if ( !( GetCellOrigin(0, resizingCell.col, &start) ) ) 
            {
                return;
            }
            if ( ( point.x - start.x ) <= m_nResizeCaptureRange )     // Clicked right of border
            {
                resizingCell.col--;
            }
        } 

        // Call the superclass which will do any resizing of columns.
        CGridCtrl::OnLButtonDblClk(nFlags, point);

        // Now adjust format as necessary.
        if ( doCheck )
        {

            if ( ( GetColumnWidth( resizingCell.col ) >= m_aveCharWidth ) 
                && ( false == m_isCenteredTextVector[ resizingCell.col ] ) )
            {
                for ( int row = 0; row < GetRowCount(); row ++ )
                {
                    SetItemFormat( row, resizingCell.col, DT_CENTER );
                }
                m_isCenteredTextVector[ resizingCell.col ] = true;
            }
        }
    }


    //
    // SetFocusCell
    //
 /*   CCellID MasterModesGridCtrl::SetFocusCell(CCellID cell)
    {
        if (cell == m_idCurrentCell) 
            return m_idCurrentCell;

        CCellID idPrev = m_idCurrentCell;
        m_idCurrentCell = cell;

        if (IsValid(idPrev)) 
        {
            SendMessageToParent(idPrev.row, idPrev.col, GVN_SELCHANGING); 

            SetItemState(idPrev.row, idPrev.col, 
                         GetItemState(idPrev.row, idPrev.col) & ~GVIS_FOCUSED);

            if ( idPrev.row != m_idCurrentCell.row )
            {
                RedrawRow( idPrev.row );
            }

        }

        if (IsValid(m_idCurrentCell)) {
            SetItemState(m_idCurrentCell.row, m_idCurrentCell.col, 
                         GetItemState(m_idCurrentCell.row, m_idCurrentCell.col) | GVIS_FOCUSED);

            if ( idPrev.row != m_idCurrentCell.row )
            {
                RedrawRow( m_idCurrentCell.row );
            }

            SendMessageToParent(m_idCurrentCell.row, m_idCurrentCell.col, GVN_SELCHANGED); 
        }

        return idPrev;
    }*/


    //
    // OnLButtonUp
    //
    void MasterModesGridCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
    {
        // Adjust text centering to work around problem of artifacts appearing when
        // columns get smaller character width (while text is set to DT_CENTER)

        // Flag whether a column resize will occur (and if so which column).
        bool doCheck = false;
        int colBeingResized = -1;
        if ( (m_MouseMode == MOUSE_SIZING_COL) && (m_LeftClickDownPoint != point) )
        {
            doCheck = true;
            colBeingResized = m_LeftClickDownCell.col;
			if ( colBeingResized >= static_cast< int >( m_isCenteredTextVector.size() )  )
			{
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Column being resized has no corresponding entry in m_isCenteredTextVector." );
				return;
			}
        }

        // Call the superclass which will do any resizing of columns.
        CGridCtrl::OnLButtonUp(nFlags, point);

        // Now adjust format as necessary.
        if ( doCheck )
        {
            if ( GetColumnWidth( colBeingResized ) < m_aveCharWidth ) 
            {
                if ( m_isCenteredTextVector[ colBeingResized ] )
                {
                    for ( int row = 0; row < GetRowCount(); row ++ )
                    {
                        SetItemFormat( row, colBeingResized, DT_LEFT );
                    }
                    m_isCenteredTextVector[ colBeingResized ] = false;
                }
                // Also limit the resizing so can't make all cells dissapear.
                if ( colBeingResized == ( GetColumnCount() -1 ) )
                {
                    CRect rect, thisRect;
                    CCellID cell( 0, colBeingResized );
                    GetCellRect( cell, &rect );
                    GetWindowRect( &thisRect );
                    ScreenToClient( &thisRect );
                    if ( rect.right == thisRect.left )
                    {
                        SetColumnWidth( colBeingResized, 1 );
                    }
                }
            }
            else if ( ( GetColumnWidth( colBeingResized ) >= m_aveCharWidth ) 
                && ( false == m_isCenteredTextVector[ colBeingResized ] ) )
            {
                for ( int row = 0; row < GetRowCount(); row ++ )
                {
                    SetItemFormat( row, colBeingResized, DT_CENTER );
                }
                m_isCenteredTextVector[ colBeingResized ] = true;
            }
        }
    }


	void MasterModesGridCtrl::setCurrentStationModeLock(int locationIndex, bool isStationLocked)
	{
		FUNCTION_ENTRY( "setCurrentStationModeLock" );

		unsigned int NUM_COLUMNS = 0;

		if(NORMAL_MASTER_MODES == ECSManagerConfigurationAccess::getInstance().getDialogType())
		{
			NUM_COLUMNS = NUM_FIXED_COLUMNS_FOR_NORMAL;
		}
		else
		{
			NUM_COLUMNS = NUM_FIXED_COLUMNS_FOR_EMERGENCY;
		}

		CellCoordinate cellCoord( LOCATION_ROW, ( NUM_COLUMNS + locationIndex ) );
		// failed mode is drawn as dark blue and therefore text is made white to be visible
        
		if(isStationLocked)
		{	// Set the Lock Station Colour.
			SetItemBkColour( cellCoord.first, cellCoord.second, 
				TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(TA_IRS_App::CURRENT_STATION_LOCK) );
		}
		else // Otherwise set the colour back to normal.
		{
			SetItemBkColour(cellCoord.first, cellCoord.second, ::GetSysColor(COLOR_BTNFACE));
		}

        RedrawCell( cellCoord.first, cellCoord.second );

		FUNCTION_EXIT;
	}
	// 10745j-Start
	// Modified for Selection Behaviour Bug
	// After a master mode has been applied the current selection should be deselected			
	void MasterModesGridCtrl::resetSelectionAfterApplyButtonPressed()
	{
		FUNCTION_ENTRY( "resetSelectionAfterApplyButtonPressed" );
		if(m_selectedRow->isRowSelected)
		{
			// Clear the previous selection cell..
			for(int i=0; i<m_nCols-1; i++)
			{
				RedrawCell((m_selectedRow->selectedRange).GetMinRow(), i+1);		
			}
		}
		
		m_selectedRow->isRowSelected = false;
		
		Invalidate();
		SetFocusCell(-1,-1);
		FUNCTION_EXIT;
	}
	// 10745j-End	
	
	// 10745c-Start
	// Changed the color of the selection rectangle to blue instead of black
	void MasterModesGridCtrl::OnDraw(CDC* pDC)
	{
		CRect rect;
		int row,col;

		CRect clipRect;
		if (pDC->GetClipBox(&clipRect) == ERROR) 
			return;

		EraseBkgnd(pDC);            // OnEraseBkgnd does nothing, so erase bkgnd here.
									// This necessary since we may be using a Memory DC.

		int nFixedRowHeight = GetFixedRowHeight();
		int nFixedColWidth  = GetFixedColumnWidth();

		CCellID idTopLeft = GetTopleftNonFixedCell();
		int minVisibleRow = idTopLeft.row,
			minVisibleCol = idTopLeft.col;

		CRect VisRect;
		CCellRange VisCellRange = GetVisibleNonFixedCellRange(VisRect);
		int maxVisibleRow = VisCellRange.GetMaxRow(),
			maxVisibleCol = VisCellRange.GetMaxCol();


		// draw top-left cells 0..m_nFixedRows-1, 0..m_nFixedCols-1
		rect.bottom = -1;
		for (row = 0; row < m_nFixedRows; row++)
		{
			rect.top = rect.bottom+1;
			rect.bottom = rect.top + GetRowHeight(row)-1;
			rect.right = -1;
			for (col = 0; col < m_nFixedCols; col++)
			{
				rect.left = rect.right+1;
				rect.right = rect.left + GetColumnWidth(col)-1;  
            
				DrawFixedCell(pDC, row, col, rect);
			}
		}
     
		// draw fixed column cells:  m_nFixedRows..n, 0..m_nFixedCols-1
		rect.bottom = nFixedRowHeight-1;
		for (row = minVisibleRow; row <= maxVisibleRow; row++)
		{
			rect.top = rect.bottom+1;
			rect.bottom = rect.top + GetRowHeight(row)-1;

			// rect.bottom = bottom pixel of previous row
			if (rect.top > clipRect.bottom) 
				break;                // Gone past cliprect
			if (rect.bottom < clipRect.top)
				continue;             // Reached cliprect yet?

			rect.right = -1;
			for (col = 0; col < m_nFixedCols; col++)
			{
				rect.left = rect.right+1;
				rect.right = rect.left + GetColumnWidth(col)-1;

				if (rect.left > clipRect.right) 
					break;            // gone past cliprect
				if (rect.right < clipRect.left) 
					continue;         // Reached cliprect yet?

				DrawFixedCell(pDC, row, col, rect);
			}
		}
    
		// draw fixed row cells  0..m_nFixedRows, m_nFixedCols..n
		rect.bottom = -1;
		for (row = 0; row < m_nFixedRows; row++)
		{
			rect.top = rect.bottom+1;
			rect.bottom = rect.top + GetRowHeight(row)-1;

			// rect.bottom = bottom pixel of previous row
			if (rect.top > clipRect.bottom) 
				break;                // Gone past cliprect
			if (rect.bottom < clipRect.top) 
				continue;             // Reached cliprect yet?

			rect.right = nFixedColWidth-1;
			for (col = minVisibleCol; col <= maxVisibleCol; col++)
			{                                       
				rect.left = rect.right+1;
				rect.right = rect.left + GetColumnWidth(col)-1;

				if (rect.left > clipRect.right)
					break;        // gone past cliprect
				if (rect.right < clipRect.left) 
					continue;     // Reached cliprect yet?

				DrawFixedCell(pDC, row, col, rect);
			}
		}
    
		// draw rest of non-fixed cells
		rect.bottom = nFixedRowHeight-1;
		for (row = minVisibleRow; row <= maxVisibleRow; row++)
		{
			rect.top = rect.bottom+1;
			rect.bottom = rect.top + GetRowHeight(row)-1;

			// rect.bottom = bottom pixel of previous row
			if (rect.top > clipRect.bottom) 
				break;                // Gone past cliprect
			if (rect.bottom < clipRect.top) 
				continue;             // Reached cliprect yet?

			rect.right = nFixedColWidth-1;
			for (col = minVisibleCol; col <= maxVisibleCol; col++)
			{
				rect.left = rect.right+1;
				rect.right = rect.left + GetColumnWidth(col)-1;

				if (rect.left > clipRect.right)
					break;        // gone past cliprect
				if (rect.right < clipRect.left) 
					continue;     // Reached cliprect yet?

				DrawCell(pDC, row, col, rect);
			}
		}


		CPen pen;
		TRY {
			pen.CreatePen(PS_SOLID, 0, m_crGridColour);
		}
		CATCH (CResourceException, e)
		{
			e->Delete();
			return;
		}
		END_CATCH
		pDC->SelectObject(&pen);

		// draw vertical lines (drawn at ends of cells)
		if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT) 
		{
			int x = nFixedColWidth;
			for (col = minVisibleCol; col <= maxVisibleCol; col++) {
				x += GetColumnWidth(col);
				pDC->MoveTo(x-1, nFixedRowHeight);
				pDC->LineTo(x-1, VisRect.bottom);   
			}
		}
    
		// draw horizontal lines (drawn at bottom of each cell)
		if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ) 
		{
			int y = nFixedRowHeight;
			for (row = minVisibleRow; row <= maxVisibleRow; row++) {
				y += GetRowHeight(row);
				pDC->MoveTo(nFixedColWidth, y-1);    
				pDC->LineTo(VisRect.right,  y-1);
			}
		}

		if(m_selectedRow->isRowSelected)
		{
			CPen pen;
			TRY {
				// 10745j- Modify Start
				// Select a blue pen instead of a black pen
				//pen.CreatePen(PS_SOLID, 3, RGB(256, 256, 256));
				  pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 255));	
				// 10745j- Modify End
			}
			CATCH (CResourceException, e)
			{
				e->Delete();
				return;
			}
			END_CATCH
			pDC->SelectObject(&pen);

			CRect selectedRect;
			
			//zn++ CL-20924: topleft cell will change according to the horizontal scroll bar
			// , will make "GetCellRangeRect()" failed(bug in GridCtrl), so use current 
			// topleft(non-fixed cell) col to get the rect
			// instead of the min col which fixed as 1 for Normal Master Mode GUI.
			
			//GetCellRangeRect(m_selectedRow->selectedRange, selectedRect);
			if (FALSE == GetCellRangeRect(m_selectedRow->selectedRange, selectedRect))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "m_selectedRow->selectedRange: %d, %d, %d, %d", 
					m_selectedRow->selectedRange.GetMinRow(), 
					m_selectedRow->selectedRange.GetMinCol(), 
					m_selectedRow->selectedRange.GetMaxRow(), 
					m_selectedRow->selectedRange.GetMaxCol());

				CCellID idTopLeft = GetTopleftNonFixedCell();
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "topleft: %d, %d", idTopLeft.row, idTopLeft.col);

				if (m_selectedRow->selectedRange.GetMinRow() == m_selectedRow->selectedRange.GetMaxRow())
				{
					int rowNum = m_selectedRow->selectedRange.GetMinRow();

					if (idTopLeft.row <= rowNum)
					{
						// TODO: should cover the row is out of the display area check
						CCellRange fixedCellRange(m_selectedRow->selectedRange.GetMinRow(),
							idTopLeft.col, 
							m_selectedRow->selectedRange.GetMaxRow(), 
							m_selectedRow->selectedRange.GetMaxCol());
						
						if (FALSE == GetCellRangeRect(fixedCellRange, selectedRect))
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Still can not get accurate rect to print a bold frame!!! Unknown result.");
						}
					}
					else
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Selected range is above the topleft cell, should not paint the selected area with bold frame");
					}
				}
				else
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There is not only a whole row been selected!!! Unknow result.");
				}
			}			
			//++zn 
			
			pDC->SelectStockObject(NULL_BRUSH);
			selectedRect.left = 0;
			pDC->Rectangle(selectedRect);
		}


	#ifdef USE_MEMDC                        // Use a memDC for flicker free update
	}
	#else                                   // Use normal DC - this helps in debugging
	}
	#endif
	// 10745c-End













} // namespace TA_IRS_App
