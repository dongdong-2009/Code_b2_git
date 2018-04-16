/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/StationModesOverviewGridCtrl.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  */

#include <algorithm>

#include "app/ecs/ecs_manager/src/stdafx.h"
#include "app/ecs/ecs_manager/src/ecsmanager.h"
#include "app/ecs/ecs_manager/src/StationModesOverviewGridCtrl.h"
#include "app/ecs/ecs_manager/src/ECSColourAccessor.h"

#include "core/utilities/src/DebugUtil.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


namespace TA_IRS_App
{
    const CString TYPEFACE_FOR_CONTROLS = "Arial Bold";
    const int     FONTSIZE_FOR_CONTROLS = 100;
    const int NUM_FIXED_COLUMNS = 1;
    const int NUM_FIXED_ROWS = 1;
    const int NUM_COLUMNS = 13;
    const int NUM_ROWS = 7;
    const int CURRENT_STATION_MODE_ROW = 1;
	const int EMPTY_MODE_ROW = 2;
    const int CURRENT_PROPOSED_NORMAL_ROW = 3;
	const int CURRENT_PROPOSED_CONGESTION_ROW = 4;
    const int CURRENT_PROPOSED_EMERGENCY_ROW = 5;
	const int LOCATION_ROW = 0;
    const int FIXED_COLUMN_WIDTH = 326;
    const int COLUMN_WIDTH = 80;
	const int CONTENT_COLUMN = 0;
	const int StationModesOverviewGridCtrl::NUM_LOCATIONS_PER_GRID = NUM_COLUMNS - NUM_FIXED_COLUMNS;

    const char* const FIXED_COLUMN_LABELS[ 7 ] =
    {
        "", 
        "Current Station Mode", 
        "", 
        "Proposed Station Mode For Normal Operation", 
        "Proposed Station Mode For Congestion Operation",
        "Proposed Station Mode For Emergency Operation",
        "Auto Mode Reset",
    };
 

    //
    // Constructor
    //
    StationModesOverviewGridCtrl::StationModesOverviewGridCtrl(): 
	GridCtrlBorderSelection(), 
	m_flashToggle( false ), 
	m_isFlashing( false ),
	m_isAutoModeApplied( false )
    {
        FUNCTION_ENTRY( "Constructor" );
		m_CurrSelectedCell = -1;
		m_ProposedRowSelected = false;
		m_isAutoModeRowSelected = false;	//CL17276
		m_proposedRow = -1;
		m_isMultiSelected = false;			//CL-20791
        FUNCTION_EXIT;
    }


    //
    // Destructor
    //
    StationModesOverviewGridCtrl::~StationModesOverviewGridCtrl()
    {
        FUNCTION_ENTRY( "Destructor" );

        //
        // Nothing to clean up.
        //

        FUNCTION_EXIT;
    }


    //
    // compareCellCoordinates
    //
    bool compareCellCoordinates( std::pair< int, int > cellCoord1, std::pair< int, int > cellCoord2 )
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
    void StationModesOverviewGridCtrl::setUpGridCtrl()
    {
        FUNCTION_ENTRY( "setUpGridCtrl" );

        // set the font on the control.
        CFont arialBold;
        arialBold.CreatePointFont(FONTSIZE_FOR_CONTROLS, TYPEFACE_FOR_CONTROLS);
        SetFont(&arialBold);

        SetFixedRowCount( NUM_FIXED_COLUMNS );
        SetFixedColumnCount(NUM_FIXED_ROWS );
        SetColumnCount( NUM_COLUMNS );
        SetRowCount( NUM_ROWS );
	
        for (int row = 0; row < NUM_ROWS; row++)
        {
            SetItemText(row, 0, FIXED_COLUMN_LABELS[ row ]);

            for (int col = 0; col < NUM_COLUMNS; col++)
            {
                if (col > 0)
                {
                    SetItemFormat( row, col, DT_SINGLELINE | DT_CENTER | DT_VCENTER );
                    SetColumnWidth( col, COLUMN_WIDTH );
                }
                else
                {
                    SetItemFormat( row, col, DT_SINGLELINE | DT_LEFT | DT_VCENTER );
                    SetColumnWidth( col, FIXED_COLUMN_WIDTH );
                }
            }
        }

        ShowScrollBar( SB_BOTH, FALSE );

        FUNCTION_EXIT;
    }


    //
    // setLocations
    //
    void StationModesOverviewGridCtrl::setLocations( const LocationNames& locationNames )
    {
        FUNCTION_ENTRY( "setLocations" );
		
		int numLocations = locationNames.size();
		m_numLoadedLocations = numLocations;

		if ( static_cast< int >( locationNames.size() ) >= NUM_COLUMNS )
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "StationModesOverviewGridCtrl::setLocations called with vector of location names greater than number of columns for displaying them." );
			return;
		}

        LocationNames::const_iterator locationNamesIt; 
        for (locationNamesIt = locationNames.begin();
			 locationNamesIt != locationNames.end();
			 ++locationNamesIt)
        {
			int col = NUM_FIXED_COLUMNS + (locationNamesIt - locationNames.begin());

            SetItemText(0, col, _T( ( *locationNamesIt ).c_str() ) );
            RedrawCell(0, col);
        }
		
        FUNCTION_EXIT;
    }


	//
	// get Auto Mode
	//
	StationModesOverviewGridCtrl::LocationIndexAndAutoModeType StationModesOverviewGridCtrl::getAutoModeFromCellCoordinates( int row, int col )
    {
		FUNCTION_ENTRY( "getAutoModeFromCellCoordinates" );

		if (row == CURRENT_STATION_AUTO_MODE_ROW)
        {
			if (col >= NUM_FIXED_COLUMNS && col<NUM_COLUMNS)
			{
				ECurrentStationAutoModeType stationAutoModeType = 
							static_cast<ECurrentStationAutoModeType>(GetItemData(row,col));		
				FUNCTION_EXIT;
				return LocationIndexAndAutoModeType((col - NUM_FIXED_COLUMNS), stationAutoModeType);
			}
		}

		FUNCTION_EXIT;
		return LocationIndexAndAutoModeType(-1, CURRENT_STATION_AUTO_MODE_FAILED);
    }


    //
    // clearSelection
    //
    void StationModesOverviewGridCtrl::clearSelection()
    {
		SetFocusCell(CCellID(-1, -1));	
		m_CurrSelectedCell = -1;

		//zn++ - CL-20791
		m_isMultiSelected = false;
		m_PrevSelectedCellMap.RemoveAll();
		m_ProposedRowSelected = false;
		m_selectedrowProposedTypes.clear();
		//++zn
		Invalidate();
    }

    //
    // LocationIndexAndProposedType
    //
    LocationIndexAndProposedType StationModesOverviewGridCtrl::getProposedStationModeFromCellCoordinates( int row, int col )
    {
        FUNCTION_ENTRY( "getProposedStationModeFromCellCoordinates" );

        if (row >= CURRENT_PROPOSED_NORMAL_ROW && row <= CURRENT_PROPOSED_EMERGENCY_ROW &&
			col >= NUM_FIXED_COLUMNS && col < NUM_COLUMNS  )
        {
			FUNCTION_EXIT;
            return LocationIndexAndProposedType( (col - NUM_FIXED_COLUMNS),
                    static_cast<EProposedStationModeType>(row - CURRENT_PROPOSED_NORMAL_ROW) );
        }

        FUNCTION_EXIT;
        return LocationIndexAndProposedType(-1, PROPOSED_IS_UNKNOWN);
    }


    //
    // setCurrentStationMode
    //
    void StationModesOverviewGridCtrl::setCurrentStationMode( int locationIndex, const std::string& stationMode, 
                               ECurrentStationModeType currentStationModeType )
    {
        FUNCTION_ENTRY( "setCurrentStationMode" );

        CellCoordinate cellCoord(CURRENT_STATION_MODE_ROW, NUM_FIXED_COLUMNS + locationIndex);

		// failed mode is drawn as dark blue and therefore text is made white to be visible
		//if (currentStationModeType == TA_IRS_App::CURRENT_STATION_MODE_FAILED)
		//{
		//	SetTextColor(0x00FFFFFF);
		//}
		//else
		//{
			SetTextColor(0x00000000);
		//}

        SetItemText(cellCoord.first, cellCoord.second, _T(stationMode.c_str()));
        SetItemData(cellCoord.first, cellCoord.second, static_cast<LPARAM>(currentStationModeType));

		// marvin++
		if(stationMode != "#" && stationMode!="0")
		{
			//zn++ CL-20840
			//unsigned int modeIndex = atoi(stationMode.c_str());
			int modeIndex = atoi(stationMode.c_str());
			//++zn
			SetItemBkColour( cellCoord.first, cellCoord.second, 
				TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(modeIndex));

		}
		else
		{
			SetItemBkColour( cellCoord.first, cellCoord.second, 
				TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(currentStationModeType) );
		}
		// ++marvin

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "StationModesOverviewGridCtrl::setCurrentStationMode - row=%d col=%d stationMode=%s",
						cellCoord.first, cellCoord.second, stationMode.c_str());

		RedrawCell( cellCoord.first, cellCoord.second );
		checkCurrentModeFlashing();
		
		// restore text color
		//SetTextColor(0x00000000);

        FUNCTION_EXIT;
    }


    void StationModesOverviewGridCtrl::setCurrentStationAutoMode( int locationIndex, BOOL autoModeStatus )
    {
        FUNCTION_ENTRY( "setCurrentStationAutoMode" );

		// Add the handling if Auto Mode Status should be used later
		if ( m_selectedrowAutoMode.find( locationIndex ) != m_selectedrowAutoMode.end() )
		{
			m_selectedrowAutoMode.erase( locationIndex );		
		}

		CellCoordinate cellCoord( CURRENT_STATION_AUTO_MODE_ROW, NUM_FIXED_COLUMNS + locationIndex);
		
		// According to GUI design note1.3.6.1.3: For Auto Mode Reset, the indication
		// "On" will be shown only when selected. It will revert to a blank cell when 
		// it is deselected or after the "Apply Selections" button has been pressed
		SetItemData(cellCoord.first, cellCoord.second, static_cast< LPARAM >(autoModeStatus));
        RedrawCell( cellCoord.first, cellCoord.second );
		//zn++ 1st go thru defects list: Prob1
		clearSelection();
		ResetSelectedRow();
		//++zn
		Invalidate();

        FUNCTION_EXIT;
    }


	void StationModesOverviewGridCtrl::setCurrentStationAutoModeControlState( int locationIndex, TA_IRS_App::ECurrentStationAutoModeType autoModeType)
	{
		FUNCTION_ENTRY( "setCurrentStationAutoModeControlState" );

		CellCoordinate cellCoord(CURRENT_STATION_AUTO_MODE_ROW, NUM_FIXED_COLUMNS + locationIndex);

		// According to GUI design note1.3.6.1.3: For Auto Mode Reset, the indication
		// "On" will be shown only when selected. It will revert to a blank cell when 
		// it is deselected or after the "Apply Selections" button has been pressed
		SetItemData(cellCoord.first, cellCoord.second, static_cast< LPARAM >(autoModeType));
        RedrawCell( cellCoord.first, cellCoord.second );
		//zn++ 1st go thru defects list: Prob1
		clearSelection();
		ResetSelectedRow();
		//++zn
		Invalidate();

		FUNCTION_EXIT;

	}

    //
    // setCurrentStationModeInProgress
    //
    void StationModesOverviewGridCtrl::setCurrentStationModeInProgress( int locationIndex, BOOL doShowAsInProgress )
    {
        FUNCTION_ENTRY( "setCurrentStationModeInProgress" );

        CellCoordinate cellCoord( CURRENT_STATION_MODE_ROW, ( NUM_FIXED_COLUMNS + locationIndex ) );

        bool isInCellsRequiringFlashList = std::binary_search( m_cellsRequiringFlash.begin(), 
                m_cellsRequiringFlash.end(), cellCoord, compareCellCoordinates );

        if (TRUE==doShowAsInProgress && false==isInCellsRequiringFlashList)
        {
            m_cellsRequiringFlash.push_back( cellCoord );
            std::sort( m_cellsRequiringFlash.begin(), m_cellsRequiringFlash.end(), compareCellCoordinates );

            if ( false == m_isFlashing )
            {
                SetTimer( FLASH_TIMER_ID, FLASH_INTERVAL, NULL );
                m_isFlashing = true;
            }

            if ( true == m_flashToggle )
            {
                SetItemBkColour( cellCoord.first, cellCoord.second, GetTextBkColor() );
            }
            else 
            {
				//zn++ CL-20840
// 				ECurrentStationModeType stationModeType = static_cast<ECurrentStationModeType>( GetItemData( cellCoord.first, cellCoord.second ) );
// 				
//                 SetItemBkColour( cellCoord.first, cellCoord.second,
// 				TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(stationModeType));	
				std::string modeString = GetItemText(cellCoord.first, cellCoord.second);
				int modeValue = atoi(modeString.c_str());
				SetItemBkColour(cellCoord.first, cellCoord.second, TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(modeValue));
				//++zn
            }

            RedrawCell( cellCoord.first, cellCoord.second );
        }
        else if (false==doShowAsInProgress && true==isInCellsRequiringFlashList)
        {
            m_cellsRequiringFlash.erase( std::lower_bound(  m_cellsRequiringFlash.begin(), 
                m_cellsRequiringFlash.end(), cellCoord, compareCellCoordinates ) );

			checkCurrentModeFlashing();

            if (0 == m_cellsRequiringFlash.size() && true == m_isFlashing)
            {
                KillTimer(FLASH_TIMER_ID);
                m_isFlashing = false;
                m_flashToggle = false;
            }

            // Perhaps should check if visible also (probably no more efficient perhaps less efficient, so safer just to
            // set bk color and redraw any how)

			//zn++ CL-20840
// 			ECurrentStationModeType stationModeType = 
// 					static_cast<ECurrentStationModeType>(GetItemData(cellCoord.first, cellCoord.second));
//             SetItemBkColour( cellCoord.first, cellCoord.second,
// 					TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(stationModeType));
			std::string modeString = GetItemText(cellCoord.first, cellCoord.second);
			int modeValue = atoi(modeString.c_str());
			SetItemBkColour(cellCoord.first, cellCoord.second, TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(modeValue));
			//++zn

            RedrawCell( cellCoord.first, cellCoord.second );
        }

        FUNCTION_EXIT;
    }


	void StationModesOverviewGridCtrl::setCurrentStationModeLock(int locationIndex, bool isStationLocked)
	{
		FUNCTION_ENTRY( "setCurrentStationModeLock" );

        CellCoordinate cellCoord(LOCATION_ROW, NUM_FIXED_COLUMNS + locationIndex);

		// failed mode is drawn as dark blue and therefore text is made white to be visible
        
		if (true == isStationLocked)
		{	
			// Set the Lock Station Colour.
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


    //
    // setCurrentProposedStationMode
    //
    void StationModesOverviewGridCtrl::setCurrentProposedStationMode( int locationIndex, EProposedStationModeType proposedStationModeType,
                                                                     const std::string& proposedStationMode, EModeProposalState proposalState )
    {
        FUNCTION_ENTRY( "setCurrentProposedStationMode" );

        CellCoordinate cellCoord(CURRENT_PROPOSED_NORMAL_ROW + proposedStationModeType, 
								 NUM_FIXED_COLUMNS + locationIndex);

        bool isInCellsRequiringFlashList = std::binary_search( m_cellsRequiringFlash.begin(), 
                m_cellsRequiringFlash.end(), cellCoord, compareCellCoordinates );

		//zn++ CL-20840
		//unsigned int proposedStationModeNumber = atoi(proposedStationMode.c_str());
		int proposedStationModeNumber = atoi(proposedStationMode.c_str());
		//++zn
    
        if (proposalState != NOT_CURRENTLY_PROPOSED) 
        {
			/*if (proposedStationModeType == PROPOSED_FOR_EMERGENCY)
			{
				SetItemBkColour( cellCoord.first, cellCoord.second, 
					TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(proposedStationModeNumber));  // solid red
			}

            if (false == isInCellsRequiringFlashList)
            {
                m_cellsRequiringFlash.push_back(cellCoord);
                std::sort(m_cellsRequiringFlash.begin(), m_cellsRequiringFlash.end(), compareCellCoordinates);

                if (false == m_isFlashing)
                {
                    SetTimer( FLASH_TIMER_ID, FLASH_INTERVAL, NULL );
                    m_isFlashing = true;
                }
            }

            if (true == m_flashToggle)
            {
				SetItemBkColour(cellCoord.first, cellCoord.second,
                        TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(proposedStationModeNumber));
            }
            else 
            {
				SetItemBkColour(cellCoord.first, cellCoord.second,
                        TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(proposedStationModeNumber));
            }*/

			// CL17444++ - Do not blink for any proposed mode
			SetItemBkColour( cellCoord.first, cellCoord.second,
                        TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode( proposedStationModeNumber ) );
        }
        else 
        {
            /*if (isInCellsRequiringFlashList)
            {
                m_cellsRequiringFlash.erase(std::lower_bound(m_cellsRequiringFlash.begin(), 
                            m_cellsRequiringFlash.end(), cellCoord, compareCellCoordinates ) );

                if (0 == m_cellsRequiringFlash.size() && true == m_isFlashing)
                {
                    KillTimer( FLASH_TIMER_ID );
                    m_isFlashing = false;
                    m_flashToggle = false;
                }
            }*/

            SetItemBkColour(cellCoord.first, cellCoord.second, GetTextBkColor());
        }

        SetItemText(cellCoord.first, cellCoord.second, _T(proposedStationMode.c_str()));
        SetItemData(cellCoord.first, cellCoord.second, static_cast<LPARAM>(proposalState));

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StationModesOverviewGridCtrl::setCurrentProposedStationMode - row=%d col=%d proposed station mode=%s!!!",
						cellCoord.first, cellCoord.second, proposedStationMode.c_str() );

		RedrawCell(cellCoord.first, cellCoord.second);
        checkCurrentModeFlashing();		
		
		Invalidate(); //zn++ CL-17444

        FUNCTION_EXIT;
    }



    BEGIN_MESSAGE_MAP(StationModesOverviewGridCtrl, CGridCtrl)
    //{{AFX_MSG_MAP(StationModesOverviewGridCtrl)
        ON_WM_KILLFOCUS()
        ON_WM_KEYDOWN()
        ON_WM_SETFOCUS()
        ON_WM_MOUSEMOVE()
        ON_WM_TIMER()
		ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    //
    // PreSubclassWindow
    //
    void StationModesOverviewGridCtrl::PreSubclassWindow() 
    {
        FUNCTION_ENTRY( "PreSubclassWindow" );

        SetEditable( FALSE );
        EnableDragAndDrop( FALSE );
        EnableSelection( FALSE );
        SetRowResize( FALSE );
        SetColumnResize( FALSE );

        // Set the background colour of the control so we cannot see where the background if the columns and rows are
        // smaller than the control.
        SetBkColor( GetSysColor(COLOR_3DFACE) );

        CGridCtrl::PreSubclassWindow();

        FUNCTION_EXIT;
    }


    //
    // OnTimer
    //
    void StationModesOverviewGridCtrl::OnTimer(UINT nIDEvent) 
    {
		//zn++ CL-20840
        //unsigned int mode = 0;
		int mode = 0;
		//++zn
		std::string strMode = "0";

        switch ( nIDEvent )
        {
        case FLASH_TIMER_ID:
            {
                m_flashToggle = !m_flashToggle;

                CellCoordinateVector::iterator cellsReqFlashIt; 
                for (cellsReqFlashIt = m_cellsRequiringFlash.begin();
					 cellsReqFlashIt != m_cellsRequiringFlash.end();
					 ++cellsReqFlashIt)
                {
                    if ( true == m_flashToggle )
                    {
						if ((*cellsReqFlashIt).first != CURRENT_STATION_MODE_ROW)
						{
							strMode = GetItemText(( *cellsReqFlashIt ).first, ( *cellsReqFlashIt ).second);
							mode = atoi(strMode.c_str());

							SetItemBkColour((*cellsReqFlashIt).first, (*cellsReqFlashIt).second, 
								TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(mode));
						}
						else
						{
							//zn++ CL-20840
// 							ECurrentStationModeType stationModeType = static_cast<ECurrentStationModeType>(GetItemData((*cellsReqFlashIt).first, (*cellsReqFlashIt).second));
// 
// 							SetItemBkColour((*cellsReqFlashIt).first, (*cellsReqFlashIt).second,
// 								TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(stationModeType));
							
							std::string modeString = GetItemText((*cellsReqFlashIt).first, (*cellsReqFlashIt).second);
							int modeValue = atoi(modeString.c_str());
							SetItemBkColour((*cellsReqFlashIt).first, (*cellsReqFlashIt).second, TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(modeValue));
							//++zn
						}
					} 
                    else 
                    {
						SetItemBkColour((*cellsReqFlashIt).first, (*cellsReqFlashIt).second, 
							GetTextBkColor());
                    }

                    RedrawCell((*cellsReqFlashIt).first, (*cellsReqFlashIt).second);      
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
    BOOL StationModesOverviewGridCtrl::DrawCell( CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk )
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

		COLORREF TextClr;
		if ( nRow != CURRENT_PROPOSED_EMERGENCY_ROW )
		{
			 TextClr   = (Item.crFgClr == CLR_DEFAULT)? GetTextColor()   : Item.crFgClr;
		}
		else
		{
			TextClr = TextBkClr;
		}

        // Force redraw of background if custom colour
        //if (Item.crBkClr != CLR_DEFAULT)
        bEraseBk = TRUE;

        int nSavedDC = pDC->SaveDC();

        pDC->SetBkMode(TRANSPARENT);

        if (Item.state & GVIS_FOCUSED 
    #if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
            && !pDC->IsPrinting()
    #endif
            ) 
        {
            std::string text = GetItemText(nRow, nCol);
			if(!m_selectedRow->isRowSelected && text.empty() == false )
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
                    pen.CreatePen(PS_SOLID, 3, RGB(0, 0, 255));
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
        else if (Item.state & GVIS_SELECTED 
    #if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
            && !pDC->IsPrinting()
    #endif
            ) 
        {
			if(nCol <= m_numLoadedLocations )
			{
				// Don't delete rowCell because the GridControl does this!
				CGridCell* rowCell = GetCell(nRow, nCol);
				rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
				if(rowCell->crBkClr != CLR_DEFAULT)
				{
					//pDC->FillSolidRect(rect, rowCell->crBkClr);
                    pDC->FillSolidRect(rect, RGB(255,0,0));
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

        //For AutoMode
		if(nRow == CURRENT_STATION_AUTO_MODE_ROW && nCol <=m_numLoadedLocations)
		{
			ECurrentStationAutoModeType stationAutoModeType = static_cast<ECurrentStationAutoModeType>(GetItemData(nRow,nCol));			
			GetCellRect(nRow,nCol,rect);	
			if(stationAutoModeType == TA_IRS_App::CURRENT_STATION_AUTO_MODE_OFF)
			{
				pDC->DrawFrameControl(rect,DFC_BUTTON,DFCS_BUTTONPUSH);			
				SetItemText(nRow,nCol,"");
			}
			else if(stationAutoModeType == TA_IRS_App::CURRENT_STATION_AUTO_MODE_ON)					
			{
				pDC->DrawFrameControl(rect,DFC_BUTTON,DFCS_BUTTONPUSH|DFCS_PUSHED);
				rect.DeflateRect(1,1);
				pDC->FillSolidRect(rect,RGB(255,255,255));
				SetItemText(nRow,nCol,"On");
			}
			
		}

        // to higlight the entire row
        if(m_ProposedRowSelected)
		{
            if((m_proposedRow == nRow) && (m_numLoadedLocations >= nCol ))
            {
                std::string text = GetItemText(nRow, nCol);
                if( text.empty() == false )
                {
                    CBrush brush(RGB(0,0,255));
                    pDC->FrameRect(rect, &brush);
                }
			}
		}

		//zn++ CL-20791
		if (m_isMultiSelected)
		{
			CCellID cell;
			if (m_PrevSelectedCellMap.Lookup(MAKELONG(nRow, nCol), (CCellID&)cell))
			{
				std::string text = GetItemText(nRow, nCol);
				if( text.empty() == false )
				{
					CBrush brush(RGB(0,0,255));
					pDC->FrameRect(rect, &brush);
				}
			}
		}
		//++zn

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
    // OnKillFocus
    //
    void StationModesOverviewGridCtrl::OnKillFocus(CWnd* pNewWnd) 
    {
        CGridCtrl::OnKillFocus(pNewWnd);    
        RedrawCell( GetFocusCell() );
    }


    //
    // OnKeyDown
    //
    void StationModesOverviewGridCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
    {
        if ( nChar == VK_TAB )
        {
            GetParent()->PostMessage( WM_NEXTDLGCTL);
            return;
        }

        CGridCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
    }


    //
    // OnSetFocus
    //
    void StationModesOverviewGridCtrl::OnSetFocus(CWnd* pOldWnd) 
    {
		CCellID focusCell = GetFocusCell();

		if(focusCell.row != CURRENT_PROPOSED_EMERGENCY_ROW)
		{
			RedrawCell(focusCell);
		}

        CGridCtrl::OnSetFocus(pOldWnd);
    }

    //
    // OnMouseMove
    //
    void StationModesOverviewGridCtrl::OnMouseMove(UINT nFlags, CPoint point) 
    {
        CRect rect;
        GetClientRect(rect);

		Invalidate();

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
			//CCellRange myCell(1, 1, 1, 1);
			//DrawCell(myCell);
			//RedrawCell(1, 1);

            switch(m_MouseMode)
            {
                case MOUSE_SELECT_ALL:
                case MOUSE_SELECT_COL:
                case MOUSE_SELECT_ROW:  
                case MOUSE_SELECT_CELLS:    
                    {
                        CCellID idCurrentCell = GetCellFromPt(point);
                        if (!IsValid(idCurrentCell)) 
                            return;

                        if (idCurrentCell != GetFocusCell())
                        {
                            OnSelecting(idCurrentCell);
                            //SetFocusCell(max(idCurrentCell.row, m_nFixedRows),
                            //             max(idCurrentCell.col, m_nFixedCols));
                            if (idCurrentCell.row >= m_nFixedRows &&
                                idCurrentCell.col >= m_nFixedCols)
                            {
                                SetFocusCell(idCurrentCell);
                            }
                        }
                        break;
                    }

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

	void StationModesOverviewGridCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
	{
	
		CCellID clickDownCell = GetCellFromPt(point);		

        if (m_numLoadedLocations <= 0 || clickDownCell.col > m_numLoadedLocations)
        {
            return;
        }

		//zn++ - CL-20790
		if (clickDownCell.col == -1)
		{
			//deselect everything in this control
			clearSelection();
			ResetSelectedRow();
			ResetAutoSelectedRow();

// 			m_isAutoModeRowSelected = false;
// 			for( int iCt =1; iCt <= m_numLoadedLocations; iCt++ )
// 			{
// 				m_selectedrowAutoMode.clear();
// 				SetItemData( CURRENT_STATION_AUTO_MODE_ROW, iCt, static_cast<ECurrentStationAutoModeType>(TA_IRS_App::CURRENT_STATION_AUTO_MODE_OFF ) );	
// 			}

			//zn++ - CL-20791
			m_isMultiSelected = false;
			m_PrevSelectedCellMap.RemoveAll();
			m_ProposedRowSelected = false;
			m_selectedrowProposedTypes.clear();
			//++zn

			Invalidate();

			SendMessageToParent(-1, -1, GVN_SELCHANGED);
		}
		//++zn

		//If it's emergency row/empty row we dont need to select it.just return;
		if ( clickDownCell.row == CURRENT_PROPOSED_EMERGENCY_ROW || 
			 clickDownCell.row == EMPTY_MODE_ROW ||
			 clickDownCell.row == LOCATION_ROW || //)	//CL17276
			 clickDownCell.row == CURRENT_STATION_MODE_ROW ) // zn++ CL-20755: click current station mode and apply, UI will crash
		{
			return;
		}

		//If a master mode is proposed, the congestion mode cannot be selected
		if (clickDownCell.row == CURRENT_PROPOSED_CONGESTION_ROW 
			&& clickDownCell.col != CONTENT_COLUMN) // zn++ CL-17276 just check this condition when the selection is not the whole row
		{
			//zn++ 2nd go thru defects list: Prob2
			CString congestionState = GetItemText(CURRENT_PROPOSED_CONGESTION_ROW, clickDownCell.col);
			
			if (TRUE == congestionState.IsEmpty())
			{
				return;
			}
			//++zn

			CString emergencyState = GetItemText(CURRENT_PROPOSED_EMERGENCY_ROW, clickDownCell.col);

			if (FALSE == emergencyState.IsEmpty())
			{
				return;
			}
		}

		//zn++ 1st go thru defects list: Prob1
		if (clickDownCell.row == CURRENT_PROPOSED_NORMAL_ROW 
			&& clickDownCell.col > CONTENT_COLUMN
			&& clickDownCell.col < NUM_COLUMNS)
		{
			CString normalState = GetItemText(CURRENT_PROPOSED_NORMAL_ROW, clickDownCell.col);

			if (TRUE == normalState.IsEmpty())
			{
				return;
			}
		}
		//++zn

        //If there are no proposed modes in the row, dont need to select it, just return
        if (clickDownCell.row >= CURRENT_STATION_MODE_ROW && 
			clickDownCell.row <= CURRENT_PROPOSED_CONGESTION_ROW)
        {
            // check if all the rows does not have any mode type set
            bool fFoundMode = false;
		    for(int iCt =1; iCt<=m_numLoadedLocations; ++iCt)
		    {
                std::string modeString = GetItemText(clickDownCell.row, iCt);
                if (modeString.empty() == false)
                {
                    fFoundMode = true;
                    break;
                }
		    }

            if (fFoundMode == false)
            {
                return;
            }
        }

        //If it's automode reset row,then we need to know the status of the automode
		if( clickDownCell.row == CURRENT_STATION_AUTO_MODE_ROW && clickDownCell.col != CONTENT_COLUMN )	//CL17276
		{
			ECurrentStationAutoModeType stationAutoModeType = static_cast<ECurrentStationAutoModeType>(GetItemData(clickDownCell.row,clickDownCell.col));			

			if(stationAutoModeType == TA_IRS_App::CURRENT_STATION_AUTO_MODE_OFF)
			{
				stationAutoModeType = TA_IRS_App::CURRENT_STATION_AUTO_MODE_ON;
				m_selectedrowAutoMode.insert( std::make_pair( ( clickDownCell.col - NUM_FIXED_COLUMNS ), stationAutoModeType ) );
				//zn++ when deselect auto mode, should remove the focus
				SetItemData(clickDownCell.row,clickDownCell.col,static_cast<ECurrentStationAutoModeType>(stationAutoModeType));
				Invalidate();
			}
			else if(stationAutoModeType == TA_IRS_App::CURRENT_STATION_AUTO_MODE_ON)
			{
				stationAutoModeType = TA_IRS_App::CURRENT_STATION_AUTO_MODE_OFF;
				if ( m_selectedrowAutoMode.find( clickDownCell.col - NUM_FIXED_COLUMNS ) != m_selectedrowAutoMode.end() )
				{
					m_selectedrowAutoMode.erase( clickDownCell.col - NUM_FIXED_COLUMNS );
					m_isAutoModeRowSelected = false;

					//zn++ - CL-20790
					if (m_selectedrowAutoMode.empty())
					{
						SendMessageToParent(-1, -1, GVN_SELCHANGED);
					}
				//zn++ when deselect auto mode, should remove the focus
				}

				SetItemData(clickDownCell.row,clickDownCell.col,static_cast<ECurrentStationAutoModeType>(stationAutoModeType));
				clearSelection();
				return;
			}
		}

		//Apply base class function
		CGridCtrl::OnLButtonDown(nFlags, point);	
		
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
		else if (m_MouseMode != MOUSE_OVER_COL_DIVIDE && m_MouseMode != MOUSE_OVER_ROW_DIVIDE)        
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

			SetFocusCell(-1,-1);
		
            if (m_LeftClickDownCell.row >= m_nFixedRows &&
                m_LeftClickDownCell.col >= m_nFixedCols)
            {
				SetFocusCell(m_LeftClickDownCell);
            }
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
// 			m_PrevSelectedCellMap.RemoveAll();
// 			if (nFlags & MK_CONTROL) {
// 				for (POSITION pos = m_SelectedCellMap.GetStartPosition(); pos != NULL; )
// 				{
// 					DWORD key;
// 					CCellID cell;
// 					m_SelectedCellMap.GetNextAssoc(pos, key, (CCellID&)cell);
// 					m_PrevSelectedCellMap.SetAt(key, cell);
// 				}
// 			}

        
			if (m_LeftClickDownCell.row < GetFixedRowCount())
				OnFixedRowClick(m_LeftClickDownCell);
			else if (m_LeftClickDownCell.col < GetFixedColumnCount())
				OnFixedColumnClick(m_LeftClickDownCell);
			//zn++ - CL-20791
			else if (nFlags & MK_CONTROL)
			{
				if (clickDownCell.row != CURRENT_STATION_AUTO_MODE_ROW)
				{
					ResetAutoSelectedRow();
				}

				m_ProposedRowSelected = false;
				m_selectedrowProposedTypes.clear();

				CCellID cell;
				if (m_PrevSelectedCellMap.Lookup(MAKELONG(CURRENT_PROPOSED_NORMAL_ROW, m_LeftClickDownCell.col), cell))
				{
					m_PrevSelectedCellMap.RemoveKey(MAKELONG(CURRENT_PROPOSED_NORMAL_ROW, m_LeftClickDownCell.col));
				}
				if (m_PrevSelectedCellMap.Lookup(MAKELONG(CURRENT_PROPOSED_CONGESTION_ROW, m_LeftClickDownCell.col), cell))
				{
					m_PrevSelectedCellMap.RemoveKey(MAKELONG(CURRENT_PROPOSED_CONGESTION_ROW, m_LeftClickDownCell.col));
				}
				Invalidate();

				m_PrevSelectedCellMap.SetAt(MAKELONG(m_LeftClickDownCell.row, m_LeftClickDownCell.col), m_LeftClickDownCell);

				for (POSITION pos = m_PrevSelectedCellMap.GetStartPosition(); pos != NULL; )
				{
					DWORD key;
					CCellID cell;
					m_PrevSelectedCellMap.GetNextAssoc(pos, key, (CCellID&)cell);

					if (cell.row == CURRENT_PROPOSED_NORMAL_ROW)
					{
						CString normalState = GetItemText(CURRENT_PROPOSED_NORMAL_ROW, cell.col);
						if (FALSE == normalState.IsEmpty())
						{
							m_selectedrowProposedTypes.push_back(std::make_pair((cell.col - NUM_FIXED_COLUMNS), PROPOSED_FOR_NORMAL));
						}
					}
					else if (cell.row == CURRENT_PROPOSED_CONGESTION_ROW)
					{
						CString congestionState = GetItemText(CURRENT_PROPOSED_CONGESTION_ROW, cell.col);
						if (FALSE == congestionState.IsEmpty())
						{
							m_selectedrowProposedTypes.push_back(std::make_pair((cell.col - NUM_FIXED_COLUMNS), PROPOSED_FOR_CONGESTION));
						}
					}
				}

				m_isMultiSelected = true;
				RedrawCell(m_LeftClickDownCell);
			}
			//++zn
			else
			{
				if (clickDownCell.row != CURRENT_STATION_AUTO_MODE_ROW)
				{
					ResetAutoSelectedRow();
				}
				m_MouseMode = m_bListMode? MOUSE_SELECT_ROW : MOUSE_SELECT_CELLS;
				//OnSelecting(m_LeftClickDownCell);
				SelectRows(m_LeftClickDownCell);

				//zn++ - CL-20791
				m_isMultiSelected = false;
				m_PrevSelectedCellMap.RemoveAll();
				m_PrevSelectedCellMap.SetAt(MAKELONG(m_LeftClickDownCell.row, m_LeftClickDownCell.col), m_LeftClickDownCell);
				//++zn
			}
			

			m_nTimerID = SetTimer(WM_LBUTTONDOWN, m_nTimerInterval, 0);			

		}   
		m_LastMousePoint = point;
	}	 

	void StationModesOverviewGridCtrl::OnFixedColumnClick(CCellID& cell)
	{
		//CL17276++ - Select the whole auto mode for this grid or not
		if ( cell.col == 0 && cell.row == CURRENT_STATION_AUTO_MODE_ROW )
		{
			clearSelection();
			ResetSelectedRow();

			if ( m_isAutoModeRowSelected )
			{
				m_isAutoModeRowSelected = false;
				for( int iCt =1; iCt <= m_numLoadedLocations; iCt++ )
				{
					m_selectedrowAutoMode.clear();
					SetItemData( CURRENT_STATION_AUTO_MODE_ROW, iCt, static_cast<ECurrentStationAutoModeType>(TA_IRS_App::CURRENT_STATION_AUTO_MODE_OFF ) );	
				}
				Invalidate();

				//zn++ - CL-20790
				SendMessageToParent(-1, -1, GVN_SELCHANGED);
				//++zn
			}
			else
			{
				m_isAutoModeRowSelected = true;
				for( int iCt =1; iCt <= m_numLoadedLocations; iCt++ )
				{
					m_selectedrowAutoMode.insert( std::make_pair( ( iCt - NUM_FIXED_COLUMNS ),  TA_IRS_App::CURRENT_STATION_AUTO_MODE_ON ) );	
					SetItemData( CURRENT_STATION_AUTO_MODE_ROW, iCt, static_cast<ECurrentStationAutoModeType>(TA_IRS_App::CURRENT_STATION_AUTO_MODE_ON ) );	
				}
				Invalidate();

				//zn++ - CL-20790
				SendMessageToParent(-1, -1, GVN_SELCHANGED);
				//++zn
			}		
		}
		//++CL17276

        // set the proposed flags only if the column header is selected
        // and if the row is either 3 or 4.
        // Row 5 should not be allowed to select by the user
        else if ( cell.col == 0 && 
				(cell.row == CURRENT_PROPOSED_NORMAL_ROW || 
				 cell.row == CURRENT_PROPOSED_CONGESTION_ROW) )
        {
			ResetAutoSelectedRow();	
			SendMessageToParent(-1, -1, GVN_SELCHANGED);
		    m_ProposedRowSelected = TRUE;
		    m_proposedRow = cell.row;
			m_selectedrowProposedTypes.clear();

			//zn++ - CL-20791
			m_isMultiSelected = false;
			m_PrevSelectedCellMap.RemoveAll();
			//++zn

		    for(int iCt =1;iCt<=m_numLoadedLocations;iCt++)
		    {
				//zn++ 2nd go thru defect lists Problem 4
// 			    LocationIndexAndProposedType locationIndexAndProposedType =
// 							getProposedStationModeFromCellCoordinates(cell.row,iCt);
// 			    if (locationIndexAndProposedType.second != PROPOSED_IS_UNKNOWN && 
// 				    locationIndexAndProposedType.second != PROPOSED_FOR_EMERGENCY)
// 				{
// 					m_selectedrowProposedTypes.push_back(locationIndexAndProposedType);
// 				}
				if (cell.row == CURRENT_PROPOSED_NORMAL_ROW)
				{
					CString normalState = GetItemText(CURRENT_PROPOSED_NORMAL_ROW, iCt);
					if (FALSE == normalState.IsEmpty())
					{
						m_selectedrowProposedTypes.push_back(std::make_pair((iCt - NUM_FIXED_COLUMNS), PROPOSED_FOR_NORMAL));
					}

					//zn++ - CL-20790
					if (!m_selectedrowProposedTypes.empty())
					{
						SendMessageToParent(3, 0, GVN_SELCHANGED);
					}
					//++zn
				}
				else if (cell.row == CURRENT_PROPOSED_CONGESTION_ROW)
				{
					CString congestionState = GetItemText(CURRENT_PROPOSED_CONGESTION_ROW, iCt);
					if (FALSE == congestionState.IsEmpty())
					{
						m_selectedrowProposedTypes.push_back(std::make_pair((iCt - NUM_FIXED_COLUMNS), PROPOSED_FOR_CONGESTION));
					}

					//zn++ - CL-20790
					if (!m_selectedrowProposedTypes.empty())
					{
						SendMessageToParent(4, 0, GVN_SELCHANGED);
					}
					//++zn
				}
				//++zn
		    }
        }
        else
        {
			ResetSelectedRow();
			ResetAutoSelectedRow();
        }

		CGridCtrl::OnFixedColumnClick(cell);
	}

	bool StationModesOverviewGridCtrl::GetRowSelectedFlag()
	{
		return m_ProposedRowSelected;
	}

	void StationModesOverviewGridCtrl::SetRowSelectedFlag(bool bSet)
	{
		m_ProposedRowSelected = bSet;
	}

	void StationModesOverviewGridCtrl::ResetSelectedRow()
	{
		m_ProposedRowSelected  = false;
		m_proposedRow = -1;
		m_selectedrowProposedTypes.clear();
	}

	// CL17276++
	bool StationModesOverviewGridCtrl::GetAutoRowSelectedFlag()
	{
		return m_isAutoModeRowSelected;
	}

	void StationModesOverviewGridCtrl::ResetAutoSelectedRow()
	{
		m_isAutoModeRowSelected = false;
		m_selectedrowAutoMode.clear();
		for( int i = 1; i <= m_numLoadedLocations; i++ )
		{
			SetItemData( CURRENT_STATION_AUTO_MODE_ROW, i, static_cast<ECurrentStationAutoModeType>(TA_IRS_App::CURRENT_STATION_AUTO_MODE_OFF ) );	
		}
		
		Invalidate();
	}
	//++CL17276


	// TD16704: Current station mode cell has to blink when the current mode does not equal to any of the 
	//          proposed mode
	void StationModesOverviewGridCtrl::checkCurrentModeFlashing()
	{
		CString currentModeString;
		CString proposedModeString;

		for (int col=1; col<NUM_LOCATIONS_PER_GRID+1; col++)
		{
			currentModeString = GetItemText(CURRENT_STATION_MODE_ROW, col);
			CellCoordinate cellCoord(CURRENT_STATION_MODE_ROW, col);

			if (TRUE == currentModeString.IsEmpty() ||
				0 == currentModeString.Compare("#"))
			{
				if ( true == std::binary_search(m_cellsRequiringFlash.begin(), 
							m_cellsRequiringFlash.end(), cellCoord, compareCellCoordinates) )
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StationModesOverviewGridCtrl::checkCurrentModeFlashing - No current mode. Erase col=%d",col);
					m_cellsRequiringFlash.erase( std::lower_bound(m_cellsRequiringFlash.begin(), 
						m_cellsRequiringFlash.end(), cellCoord, compareCellCoordinates) );
					RedrawCell(cellCoord.first, cellCoord.second);
				}

				continue;
			}
	
			int count=0;
			bool isFoundSameMode = false;
			for (int row = CURRENT_PROPOSED_NORMAL_ROW; row <= CURRENT_PROPOSED_EMERGENCY_ROW; ++row)
			{
				proposedModeString = GetItemText(row, col);	

				if (TRUE == proposedModeString.IsEmpty())
				{
					count++;
				}
				else
				{
					// Found difference, quit the loop
					if (0 != currentModeString.Compare(proposedModeString))
					{
						if ( false == std::binary_search(m_cellsRequiringFlash.begin(), 
							m_cellsRequiringFlash.end(), cellCoord, compareCellCoordinates) )
						{
							LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StationModesOverviewGridCtrl::checkCurrentModeFlashing - Add flashing for col=%d, curMode=%s, proMode=%s",
								col,currentModeString,proposedModeString);
							m_cellsRequiringFlash.push_back(cellCoord);
							std::sort( m_cellsRequiringFlash.begin(), m_cellsRequiringFlash.end(), compareCellCoordinates );
							RedrawCell(cellCoord.first, cellCoord.second);
							break;
						}

					}
					else
					{
						isFoundSameMode = true;
					}
				}
			}

			if (true == isFoundSameMode && count==2)
			{
				if ( true == std::binary_search(m_cellsRequiringFlash.begin(), 
							m_cellsRequiringFlash.end(), cellCoord, compareCellCoordinates) )
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "StationModesOverviewGridCtrl::checkCurrentModeFlashing - Erase flashing for col=%d",
								col);
					m_cellsRequiringFlash.erase( std::lower_bound(m_cellsRequiringFlash.begin(), 
						m_cellsRequiringFlash.end(), cellCoord, compareCellCoordinates) );

					//zn++ CL-20840
					//zn++ CL-20829: not only make it steady, but also make it to the correct color, otherwise sometimes white background color
// 					ECurrentStationModeType currentStationModeType = static_cast<ECurrentStationModeType>( GetItemData( cellCoord.first, cellCoord.second ) );
// 					SetItemBkColour( cellCoord.first, cellCoord.second, 
// 					TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationModeType(currentStationModeType) );
					
					std::string modeString = GetItemText(cellCoord.first, cellCoord.second);
					int modeValue = atoi(modeString.c_str());
					SetItemBkColour(cellCoord.first, cellCoord.second, TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(modeValue));
					//++zn

					RedrawCell(cellCoord.first, cellCoord.second);
				}
			}
		}

		// CL17444++
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%d Cells will be flashed!!!", m_cellsRequiringFlash.size() );

		if (false == m_isFlashing)
		{
			SetTimer( FLASH_TIMER_ID, FLASH_INTERVAL, NULL );
			m_isFlashing = true;
        }
		// ++CL17444
	}

	//zn++ - CL-20791
	bool StationModesOverviewGridCtrl::GetMultiSelectedFlag()
	{
		return m_isMultiSelected;
	}
	//++zn
	
} // namespace TA_IRS_App

