/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ecs/ecs_manager/src/StationModesListCtrl.cpp $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  */

#include "app/ecs/ecs_manager/src/stdafx.h"
#include "app/ecs/ecs_manager/src/StationModesListCtrl.h"
#include "app\ecs\ecs_manager\src\ECSColourAccessor.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{
    //
    // Constructor
    //
    StationModesListCtrl::StationModesListCtrl(): 
	m_flashToggle( false ), 
	m_isFlashing( false ),
	m_currentAppliedIndex(-1),
	m_currentSelectionIndex(-1)		// should be initilised
    {
        FUNCTION_ENTRY( "Constructor" );

        FUNCTION_EXIT;
    }


    //
    // Destructor
    //
    StationModesListCtrl::~StationModesListCtrl()
    {
        FUNCTION_ENTRY( "Destructor" );

        //
        // Nothing to clean up.
        //

        FUNCTION_EXIT;
    }


    //
    // populateStationModeDetails
    //
    void StationModesListCtrl::populateStationModeDetails( const StationModeDetails& stationModeDetails )
    {
        FUNCTION_ENTRY( "populateStationModeDetails" );

        DeleteAllItems();

        KillTimer( FLASH_TIMER_ID );
        m_isFlashing = false;   
        m_blinkingItemIndexes.clear();

        StationModeDetails::const_iterator stationModeDetailsIt = stationModeDetails.begin();
        int insertionIndex = 0;

        while ( stationModeDetailsIt != stationModeDetails.end() )
        {
            insertionIndex = InsertItem( insertionIndex, _T( ( *stationModeDetailsIt ).stationModeAsString.c_str() ) );
            setStationModeProposalState( insertionIndex, NOT_CURRENTLY_PROPOSED ); 
            SetItemText( insertionIndex, 1, _T( ( *stationModeDetailsIt ).operation.c_str() ) );
            SetItemText( insertionIndex, 2, _T( ( *stationModeDetailsIt ).track.c_str() ) );
            SetItemText( insertionIndex, 3, _T( ( *stationModeDetailsIt ).condition.c_str() ) );
            insertionIndex++;
            stationModeDetailsIt++;
        }

        // Force redraw so as to show changes immediately (even if parent dialog still initialising
        // other stuff.
        RedrawWindow();

        FUNCTION_EXIT;
    }


    //
    // setStationModeProposalState
    //
    void StationModesListCtrl::setStationModeProposalState( int stationModeIndex, EModeProposalState stationModeProposalState )
    {
        FUNCTION_ENTRY( "setStationModeProposalState" );

        EModeProposalState prevStationModeProposalState = getStationModeProposalState( stationModeIndex );
        SetItemData( stationModeIndex, stationModeProposalState );
        if ( prevStationModeProposalState != NOT_CURRENTLY_PROPOSED )
        {
            std::vector<int>::iterator itFound = std::find( m_blinkingItemIndexes.begin(), m_blinkingItemIndexes.end(), stationModeIndex );
            if ( m_blinkingItemIndexes.end() != itFound )
            {
                m_blinkingItemIndexes.erase( itFound );
            }
        }
        if ( stationModeProposalState != NOT_CURRENTLY_PROPOSED )
        {
            m_blinkingItemIndexes.push_back( stationModeIndex );
            std::sort( m_blinkingItemIndexes.begin(), m_blinkingItemIndexes.end() );
			//std::unique( m_blinkingItemIndexes.begin(), m_blinkingItemIndexes.end() );
			m_blinkingItemIndexes.erase(std::unique( m_blinkingItemIndexes.begin(), m_blinkingItemIndexes.end() ),m_blinkingItemIndexes.end());
        }

        if ( ( 1 == m_blinkingItemIndexes.size() ) && ( false == m_isFlashing ) )
        {
            SetTimer( FLASH_TIMER_ID, FLASH_INTERVAL, NULL );
            m_isFlashing = true;
        }
        else if ( ( 0 == m_blinkingItemIndexes.size() ) && ( m_isFlashing ) )
        {
            KillTimer( FLASH_TIMER_ID );
            m_isFlashing = false;
        }


        RedrawItems( stationModeIndex, stationModeIndex );

        FUNCTION_EXIT;
    }


    //
    // getStationModeProposalState
    //
    EModeProposalState StationModesListCtrl::getStationModeProposalState( int stationModeIndex )
    {
        FUNCTION_ENTRY( "getStationModeProposalState" );

        EModeProposalState returnVal = static_cast< EModeProposalState >( GetItemData( stationModeIndex ) );

        FUNCTION_EXIT;
        return returnVal;
    }


    BEGIN_MESSAGE_MAP(StationModesListCtrl, CListCtrl)
        //{{AFX_MSG_MAP(StationModesListCtrl)
        ON_NOTIFY_REFLECT( NM_CUSTOMDRAW, onCustomDraw )
        ON_WM_TIMER()
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // StationModesListCtrl message handlers


    //
    // onCustomDraw
    //
    void StationModesListCtrl::onCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
    {
        NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>( pNMHDR );

        // Take the default processing unless we set this to something else below.
        *pResult = CDRF_DODEFAULT;

		// Used to remember if an item is selected across function calls
        static bool isSelected = false;
		

        // First thing - check the draw stage. If it's the control's prepaint
        // stage, then tell Windows we want messages for every item.
        if ( CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage )
        {
           *pResult = CDRF_NOTIFYITEMDRAW;
        }
        else if ( CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage )
        {
            // This is the prepaint stage for an item. Here's where we set the
            // item's text color. Our return value will tell Windows to draw the
            // item itself, but it will use the new color we set here.
			isSelected = false;

            // Get the index for the item
            int index = pLVCD->nmcd.dwItemSpec;

			if(GetItemState(index, LVIS_SELECTED) == LVIS_SELECTED)
			{
				// If the item is selected then we turn the Windows selection state off so that
				// our drawing colours are used instead of the default. This will be turned back on
				// in the custom draw method in the post paint stage.
				isSelected = true;
				setCurrentSelection(index);
				SetItemState(index, 0, LVIS_SELECTED);
			}

            switch ( getStationModeProposalState( index ) )
            {
            case CURRENTLY_PROPOSED:
				//fall through
            case CURRENTLY_OVERRIDING_PROPOSED:
                {
                    if ( m_flashToggle )
                    {
                        // Do nothing so it draws as normal.
						if ( index == m_currentAppliedIndex )
						{
							std::string strMode = GetItemText(index,0);
							//zn++ CL-20840
							//unsigned int modeIndex = atoi(strMode.c_str());
							int modeIndex = atoi(strMode.c_str());
							//++zn
							pLVCD->clrTextBk = TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(modeIndex);
						}
                    }
                    else
                    {
						// marvin++
						std::string strMode = GetItemText(index,0);
						//zn++ CL-20840
						//unsigned int modeIndex = atoi(strMode.c_str());
						int modeIndex = atoi(strMode.c_str());
						//++zn
                        pLVCD->clrTextBk = TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(modeIndex);
						// ++marvin
                    }
                }
                break;
			case NOT_CURRENTLY_PROPOSED:
				{
					if ( index == m_currentAppliedIndex )
					{
						std::string strMode = GetItemText(index,0);
						//zn++ CL-20840
						//unsigned int modeIndex = atoi(strMode.c_str());
						int modeIndex = atoi(strMode.c_str());
						//++zn
						pLVCD->clrTextBk = TA_IRS_App::ECSColourAccessor::getInstance().returnColourGivenStationMode(modeIndex);
					}
				}
				break;
            default:
                // liuyu++ 10745c Blue Border selection highlight
                //if(isSelected)
                //{
                //    pLVCD->clrTextBk = RGB(0, 255, 255);
                //}
                // ++liuyu 10745c Blue Border selection highlight
                break; // Draw as normal.
            }

			*pResult = CDRF_DODEFAULT | CDRF_NOTIFYPOSTPAINT;

        }
		else if ( CDDS_ITEMPOSTPAINT == pLVCD->nmcd.dwDrawStage )
        {
            int index = pLVCD->nmcd.dwItemSpec;

			CRect rect;
            GetItemRect(index, &rect,LVIR_BOUNDS);

			if(isSelected)
			{
                CDC* newDC = CDC::FromHandle(pLVCD->nmcd.hdc);

                // liuyu++ 10745c Blue Border selection highlight
                // CPen   newPen  (PS_SOLID, 3, RGB(256,256,256));
                CPen   newPen  (PS_SOLID, 3, RGB(0,0,255));
                // ++liuyu 10745c Blue Border selection highlight
                CPen* oldPen = newDC->SelectObject(&newPen);

				newDC->SelectStockObject(NULL_BRUSH);
				newDC->Rectangle(&rect);

                newDC->SelectObject(oldPen);

				// Turn selecting back on if necessary
                SetItemState(index, LVIS_SELECTED, LVIS_SELECTED);
				//setCurrentSelection(index);

                // Validate this item so it is not redrawn again
                ValidateRect(&rect);
			}

			*pResult = CDRF_SKIPDEFAULT;//CDRF_DODEFAULT;
		}

    }


    //
    // OnTimer
    //
    void StationModesListCtrl::OnTimer(UINT nIDEvent) 
    {
            switch ( nIDEvent )
            {
            case FLASH_TIMER_ID:
                {
                    m_flashToggle = !m_flashToggle;
                    std::vector< int >::iterator blinkingItemIndexesIt = m_blinkingItemIndexes.begin();
                    int top = GetTopIndex();
                    int bottom = top + GetCountPerPage();
                    while ( blinkingItemIndexesIt != m_blinkingItemIndexes.end() )
                    {
                        if ( ( ( *blinkingItemIndexesIt ) >= top ) && ( ( *blinkingItemIndexesIt ) <= bottom ) )
                        {
                            RedrawItems( ( *blinkingItemIndexesIt ), ( *blinkingItemIndexesIt ) );
                        }
                        blinkingItemIndexesIt++;
                    }
                }
                return; // else CListCtrl::OnTimer kills the timer
            default:
                break;
            }
        
        CListCtrl::OnTimer(nIDEvent);
    }


	void StationModesListCtrl::setCurrentSelection(int index)
	{
		m_currentSelectionIndex = index;
	}


	int StationModesListCtrl::getCurrentSelection()
	{
		return m_currentSelectionIndex;
	}


	void StationModesListCtrl::setCurrentAppliedMode(int index, bool isOverriding)
	{
		if (isOverriding)
		{
			m_currentAppliedIndex = index;
		}
		else
		{
			m_currentAppliedIndex = -1;
		}

		RedrawItems( m_currentAppliedIndex, m_currentAppliedIndex );
	}

} // namespace TA_IRS_App
