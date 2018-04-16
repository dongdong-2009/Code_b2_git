/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/bus/trains/TrainSelectionComponent/src/TrainListCtrl.cpp $
  * @author Adam Radics
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2009/11/20 15:34:51 $
  * Last modified by: $Author: builder $
  * 
  * TrainListCtrl defines a list control that displays train (incoming/outgoing)
  * for use in transactive GUIs requiring selectable trains.
  */

#include "bus/trains/TrainSelectionComponent/src/TrainListCtrl.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/mfc_extensions/src/Resource.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"

#include <algorithm>

#define WM_HIDE_SCROLL_BARS ( WM_APP + 1 )


namespace TA_IRS_Bus
{

    using TA_Base_Core::DebugUtil;
    using TA_Base_Core::TransactiveException;
    
    TrainListCtrl::TrainListCtrl()
     : m_currentSortInformation(),
       m_customColumns(),
       m_customDataProvider( NULL ),
	   m_eventListener( NULL ),
       m_paddedList(false)
    {
		FUNCTION_ENTRY( "TrainListCtrl" );

        setNonSelectableGray(true);

		FUNCTION_EXIT;
    }

    TrainListCtrl::~TrainListCtrl()
    {
		FUNCTION_ENTRY( "~TrainListCtrl" );
		FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(TrainListCtrl, ListCtrlSelNoFocus)
	//{{AFX_MSG_MAP(TrainListCtrl)
    ON_NOTIFY_REFLECT_EX(LVN_COLUMNCLICK, onColumnclick)
    ON_NOTIFY_REFLECT_EX(NM_CLICK, onClick)
	ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, onItemchanged)
    ON_WM_VSCROLL()
	ON_WM_KEYDOWN()
    ON_WM_MOUSEWHEEL()
    ON_MESSAGE(WM_HIDE_SCROLL_BARS, onHideScrollBars)
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void TrainListCtrl::PreSubclassWindow() 
    {	
		FUNCTION_ENTRY( "PreSubclassWindow" );

	    CListCtrl::PreSubclassWindow();

        SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
        m_fixedHeaderCtrl.subclassHeader(GetHeaderCtrl());

	    // Set up the standard columns
	    InsertColumn(LOCATION, "Loc");
	    InsertColumn(PV_SERVICE, "PV/Service");
	    InsertColumn(TETRA_ID, "TSI");

        // size visible columns - default sizing
        setStandardColumnSizes(55, 130, 0);

        // add sort indicator
        addSortIndicator( m_currentSortInformation.currentSortColumn,
                          m_currentSortInformation.currentSortOrder );

        // hide the horizontal scrollbar - post a message as delaying mechanism
        // (because window needs to be properly initialized
        // before applying scroll bar visibility settings)
        PostMessage(WM_HIDE_SCROLL_BARS);

		FUNCTION_EXIT;
    }


    //void TrainListCtrl::onHideScrollBars(UINT wParam, LONG lParam)
	LRESULT TrainListCtrl::onHideScrollBars(WPARAM wParam, LPARAM lParam)
    {
		FUNCTION_ENTRY( "onHideScrollBars" );

        setScrollBarVisibility(true, false);

		FUNCTION_EXIT;
		return S_OK;
    }


    void TrainListCtrl::setCustomDataProvider(ICustomTrainDataProvider* provider)
    {
		FUNCTION_ENTRY( "setCustomDataProvider" );

        m_customDataProvider = provider;

		FUNCTION_EXIT;
    }


    int TrainListCtrl::addCustomColumn(const std::string& columnName, int columnSize)
    {
		FUNCTION_ENTRY( "addCustomColumn" );

        // check if it exists
        std::map<std::string, int>::iterator findIter = m_customColumns.find(columnName);

        TA_ASSERT( findIter == m_customColumns.end(),
                   "Attempted to add duplicate custom columns");

        // the index of the column is TETRA_ID + the new size of the map
        int columnIndex = TETRA_ID + m_customColumns.size() + 1;
        m_customColumns[columnName] = columnIndex;

        InsertColumn(columnIndex, columnName.c_str());
        SetColumnWidth(columnIndex, columnSize);

        TA_ASSERT( m_customDataProvider != NULL,
                   "Added a custom column, with the custom data provider still set to NULL" );

        // now force a refresh on all existing rows for this new column
        for (int i = 0; i < GetItemCount(); i++)
        {
            TrainDetailsItemData* itemDetails = reinterpret_cast< TrainDetailsItemData* >( GetItemData( i ) );

            // if there are blank items in the list
            // and this is one of them - skip it
            if ( m_paddedList && (itemDetails == NULL) )
            {
                continue;
            }

            // if this point is reached, the item data mustnt be null
            TA_ASSERT(itemDetails != NULL, "Item details are NULL, when there are not empty list items");

            // set the item data in the structure
            itemDetails->customDataValues[columnIndex] = 
                m_customDataProvider->getCustomData(itemDetails->trainId, columnName);

            SetItemText(i, columnIndex,
                        itemDetails->customDataValues[columnIndex].c_str());
        }
		
		FUNCTION_EXIT;
        return columnIndex;
    }


    void TrainListCtrl::refreshCustomColumn(CommonTypes::TrainIdType trainId, const std::string& columnName)
    {
		FUNCTION_ENTRY( "refreshCustomColumn" );

        // find the index of the column name
        std::map<std::string, int>::iterator findIter = m_customColumns.find(columnName);

        TA_ASSERT( findIter != m_customColumns.end(),
                   "Attempted to update non-existent custom column");

        int columnIndex = findIter->second;

        // if the train is in this list, update the data
        int trainIndex = findTrainIndex(trainId);
        if (trainIndex > -1)
        {
            TA_ASSERT( m_customDataProvider != NULL,
                   "Custom data provider is NULL when there are custom columns in the list" );

            TrainDetailsItemData* itemDetails = reinterpret_cast<TrainDetailsItemData*>(GetItemData(trainIndex));

            // set the item data in the structure
            itemDetails->customDataValues[columnIndex] = 
                m_customDataProvider->getCustomData(itemDetails->trainId, columnName);

            SetItemText(trainIndex, columnIndex,
                        itemDetails->customDataValues[columnIndex].c_str());

            // now if the current sort was by this column, re-sort the list
            if (m_currentSortInformation.currentSortColumn == columnIndex)
            {
                SortItems(trainListSort, reinterpret_cast<LPARAM>( this ) );
            }
        }

		FUNCTION_EXIT;
        
    }


    void TrainListCtrl::setStandardColumnSizes(int locationColumnSize,
                                               int pvServiceColumnSize,
                                               int tetraIdColumnSize /* = 0 */)
    {
		FUNCTION_ENTRY( "setStandardColumnSizes" );

        SetColumnWidth(LOCATION, locationColumnSize);
	    SetColumnWidth(PV_SERVICE, pvServiceColumnSize);
	    SetColumnWidth(TETRA_ID, tetraIdColumnSize);

		FUNCTION_EXIT;
    }


    void TrainListCtrl::updateTrainList( const TA_IRS_Bus::ITrainSelectorGUI::EAction& action, 
										 const TrainInformationTypes::TrainDetails& trainDetails )
    {
		FUNCTION_ENTRY( "updateTrainList" );
        switch (action)
        {
        case TA_IRS_Bus::ITrainSelectorGUI::Add:
            addToList(trainDetails);
            break;
        case TA_IRS_Bus::ITrainSelectorGUI::Update:
            updateList(trainDetails);
            break;
        case TA_IRS_Bus::ITrainSelectorGUI::Delete:
            deleteFromList(trainDetails);
            break;
        case TA_IRS_Bus::ITrainSelectorGUI::ClearAll:
            deleteAllTrains();
            break;
        };

		FUNCTION_EXIT;
    }


    void TrainListCtrl::addToList(const TrainInformationTypes::TrainDetails& td)
    {
		FUNCTION_ENTRY( "addToList" );

        TrainDetailsItemData* itemDetails = new TrainDetailsItemData(td);

        // if there are custom columns and the provider is null
        TA_ASSERT( (m_customColumns.size() == 0) || (m_customDataProvider != NULL),
                   "Custom data provider is NULL when there are custom columns in the list" );

        // for each custom column - retrieve the data
        for(std::map<std::string, int>::iterator iter = m_customColumns.begin();
            iter != m_customColumns.end(); 
			iter++)
        {
            itemDetails->customDataValues[iter->second] = 
                m_customDataProvider->getCustomData(td.trainId, iter->first);
        }

        int nItem = findSortedInsertionPoint(*itemDetails);

		std::string location = getLocationString(*itemDetails);
        InsertItem(nItem, getLocationString(*itemDetails) );
        SetItemText(nItem, PV_SERVICE, getPvServiceString(*itemDetails) );
	    SetItemText(nItem, TETRA_ID, getTetraIdString(*itemDetails) );

        // add the pointer to the structure as the item details
        SetItemData(nItem, (DWORD) itemDetails);

        // for each custom column - populate the data
        for(std::map<int, std::string>::iterator insertIter = itemDetails->customDataValues.begin();
            insertIter != itemDetails->customDataValues.end(); insertIter++)
        {
            SetItemText( nItem,
                         insertIter->first,
                         insertIter->second.c_str() );
        }

		FUNCTION_EXIT;
    }


    void TrainListCtrl::deleteFromList(const TrainInformationTypes::TrainDetails& td)
    {
		FUNCTION_ENTRY( "deleteFromList" );

        int nItem = findTrainIndex(td.trainId);
		if( nItem >= 0)
		{
			TrainDetailsItemData* itemDetails = reinterpret_cast< TrainDetailsItemData* > ( GetItemData( nItem ) );
			if ( itemDetails != NULL)
			{
				delete itemDetails;
				itemDetails = NULL;
			}
			DeleteItem( nItem );
		}

		FUNCTION_EXIT;
    }


    void TrainListCtrl::updateList(const TrainInformationTypes::TrainDetails& td)
    {
		FUNCTION_ENTRY( "updateList" );

        int nItem = findTrainIndex(td.trainId);
		
        if (nItem < 0)
        {
            addToList(td);
        }
        else
        {
            // Update item data to allow searching/sorting to be carried out
            TrainDetailsItemData* itemDetails = reinterpret_cast<TrainDetailsItemData*>(GetItemData(nItem));
            *itemDetails = td;

            // Now update the item at this index
            SetItemText(nItem, LOCATION, getLocationString(*itemDetails) );
            SetItemText(nItem, PV_SERVICE, getPvServiceString(*itemDetails) );
	        SetItemText(nItem, TETRA_ID, getTetraIdString(*itemDetails) );

            // re-sort the list, because it has changed
            SortItems(trainListSort, reinterpret_cast<LPARAM>(this));
        }

		FUNCTION_EXIT;
    }


    void TrainListCtrl::deleteAllTrains()
    {
       FUNCTION_ENTRY( "deleteAllTrains" );

        while ( GetItemCount() > 0)
        {
            // get the item data
            TrainDetailsItemData* itemData = reinterpret_cast<TrainDetailsItemData*>(GetItemData(0));

            // if there are blank items in the list
            // and this is one of them - then this is the end of the trains
            if ( m_paddedList && (itemData == NULL) )
            {
                return;
            }
            
            delete itemData;
			itemData = NULL;
            DeleteItem(0);
        }

		FUNCTION_EXIT;
    }


    int TrainListCtrl::findSortedInsertionPoint(TrainDetailsItemData& td) const
    {
		FUNCTION_ENTRY( "findSortedInsertionPoint" );
        // there are 3 things to sort by
        // plus any custom columns are a string sort

        // sort by location key - this is the proper ordering of locations
        // rather than sorting by text

        // sort by train id (PV/Service) - PV (ie train id) will always be unique
        // so there is no need to include service in the sorting

        // sort by tetra id (if visible). Just a string sort.

        TrainDetailsItemData itemData;
        TrainDetailsItemData* data1 = NULL;
        TrainDetailsItemData* data2 = NULL;

        // set sort comparison order
        if (m_currentSortInformation.currentSortOrder == SortInformation::ASCENDING)
        {
            data1 = &td;
            data2 = &itemData;
        }
        else
        {
            data2 = &td;
            data1 = &itemData;
        }
		int i = 0;
        for (; i < GetItemCount(); i ++)
        {
            // item data at index i
            TrainDetailsItemData* itemDataPtr = reinterpret_cast<TrainDetailsItemData*>(GetItemData(i));

            // if there are blank items in the list
            // and this is one of them - this is the end
            if ( m_paddedList && (itemDataPtr == NULL) )
            {
				FUNCTION_EXIT;
                return i;
            }

            // if this point is reached, the item data mustnt be null
            TA_ASSERT(itemDataPtr != NULL, "Item details are NULL, when there are not empty list items");

            // set the item data structure
            itemData = *itemDataPtr;

            switch (m_currentSortInformation.currentSortColumn)
            {
            case LOCATION:
                // the secondary sort will be the train ID
                if (data1->currentLocation < data2->currentLocation)
                {
                    // we can insert here..
					FUNCTION_EXIT;
                    return i;
                }
                // the two are at the same location
                // use the train id as the secondary sort (Ascending)
                else if (data1->currentLocation == data2->currentLocation)
                {
                    if (td.trainId < itemData.trainId)
                    {
                        // we can insert here..
						FUNCTION_EXIT;
                        return i;
                    }
                }
                break;
            case PV_SERVICE:
                // train number will always be unique - so no secondary sort needed
                if (data1->trainId < data2->trainId)
                {
                    // we can insert here..
					FUNCTION_EXIT;
                    return i;
                }
                break;
            case TETRA_ID:
				{
					int result = data1->serviceNumber.compare(data2->serviceNumber);
					if ( result < 0 )
					{
						// we can insert here..
						FUNCTION_EXIT;
						return i;
					}
				}
                break;
            default: // basic string sort of a custom column
                if (data1->customDataValues[m_currentSortInformation.currentSortColumn] <
                    data2->customDataValues[m_currentSortInformation.currentSortColumn])
                {
                    // we can insert here..
					FUNCTION_EXIT;
                    return i;
                }
                // the two are the same
                // use the train id as the secondary sort (Ascending)
                else if (data1->customDataValues[m_currentSortInformation.currentSortColumn] ==
                         data2->customDataValues[m_currentSortInformation.currentSortColumn])
                {
                    if (td.trainId < itemData.trainId)
                    {
                        // we can insert here..
						FUNCTION_EXIT;
                        return i;
                    }
                }
                break;
            }
        }

        // Insert right at end of list..
		FUNCTION_EXIT;
        return i;
    }


    int TrainListCtrl::findTrainIndex(unsigned char trainId) const
    {
		FUNCTION_ENTRY( "findTrainIndex" );

        // find the first matching train Id
        for ( int i = 0; i < GetItemCount(); i++ )
        {
            // get the item data
            TrainDetailsItemData* itemData = reinterpret_cast<TrainDetailsItemData*>(GetItemData(i));

            // if there are blank items in the list
            // and this is one of them - this is the end
            if ( m_paddedList && (itemData == NULL) )
            {
				FUNCTION_EXIT;
                return -1;
            }

            // if this point is reached, the item data mustnt be null
            TA_ASSERT(itemData != NULL, "Item details are NULL, when there are not empty list items");

            // get the train Id from the structure in the item data
            unsigned int itemTrainId = itemData->trainId;

            if ( itemTrainId == trainId )
            {
                // matching train
				FUNCTION_EXIT;
                return i;
            }
        }

        // no match
		FUNCTION_EXIT;
        return -1;
    }


    int CALLBACK TrainListCtrl::trainListSort(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
    {
		FUNCTION_ENTRY( "trainListSort" );

        TrainListCtrl* currentList = reinterpret_cast<TrainListCtrl*>(lParamSort);
        SortInformation currentSortInformation = currentList->getCurrentSortInformation();

        // lParam1 and lParam2 are TrainDetails*
        TrainDetailsItemData* param1;
        TrainDetailsItemData* param2;

        // if the list has padding items in it
        // the padded items should always be at the position specified
        if ( currentList->m_paddedList )
        {
            if ( (lParam1 == NULL) && (lParam2 == NULL) )
            {
                // 2 padding items are equal
				FUNCTION_EXIT;
                return 0;
            }
            else if (lParam1 == NULL)
            {
                // the first is padding
                // its greater than the second return +ve
				FUNCTION_EXIT;
                return 1;

            }
            else if (lParam2 == NULL)
            {
                // the second is padding
                // its greater than the first, return -ve
				FUNCTION_EXIT;
                return -1;
            }
        }

        // change comparison order based on sort order
        if (currentSortInformation.currentSortOrder == SortInformation::ASCENDING)
        {
            param1 = reinterpret_cast<TrainDetailsItemData*>(lParam1);
            param2 = reinterpret_cast<TrainDetailsItemData*>(lParam2);
        }
        else
        {
            param1 = reinterpret_cast<TrainDetailsItemData*>(lParam2);
            param2 = reinterpret_cast<TrainDetailsItemData*>(lParam1);
        }

        int diff = 0;

        switch (currentSortInformation.currentSortColumn)
        {
        case PV_SERVICE:
            diff = param1->trainId - param2->trainId;
            break;
        case TETRA_ID:
            diff = param1->serviceNumber.compare(param2->serviceNumber);
            break;
        case LOCATION:
            diff = param1->currentLocation - param2->currentLocation;
            break;
        default: // string sort using column values
            diff = param1->customDataValues[currentSortInformation.currentSortColumn]
                    .compare(param2->customDataValues[currentSortInformation.currentSortColumn]);
            break;
        }

        // if the two items are equal - the secondary sort is based on
        // the train id (ASCENDING) - this is always unique
        if (diff == 0)
        {
            diff = reinterpret_cast<TrainDetailsItemData*>(lParam1)->trainId -
                   reinterpret_cast<TrainDetailsItemData*>(lParam2)->trainId;
        }
        
		FUNCTION_EXIT;
        return diff;
    }


    CString TrainListCtrl::getLocationString(const TrainDetailsItemData& td) const
    {
		FUNCTION_ENTRY( "getLocationString" );

        // Interpret the location ID as a string
        CString location;

        try
        {
			if((td.atsValid == false ) || (td.isLocalisationValid == false ))
			{
				location = "(";
			}

            // fix for unallocated, we will make it '???' to make it clearer
            if (td.currentLocation == 0)
            {
                location += "???";
            }
            else
            {
                // look up the number
                location += LocationCache::getInstance().getLocationByKey(td.currentLocation).c_str();
            }

			if((td.atsValid == false ) || (td.isLocalisationValid == false))
			{
				location += ")";
			}
        }
        catch (const TA_Base_Core::TransactiveException& te)
        {
            // if it fails, just use the number (invalid/new location, database error)
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", te.what());
            location.Format("%d", td.currentLocation);
        }
		
		FUNCTION_EXIT;
        return location;
    }


    CString TrainListCtrl::getPvServiceString(const TrainDetailsItemData& td) const
    {
		FUNCTION_ENTRY( "getPvServiceString" );

        CString result(td.serviceNumber.c_str());

        if ( result == "" )
        {
            result.Format("%d/_", td.trainId);
        }

        if (td.isMute)
        {
            result += " (M)";
        }

		FUNCTION_EXIT;
        return result;
    }


    CString TrainListCtrl::getTetraIdString(const TrainDetailsItemData& td) const
    {
		FUNCTION_ENTRY( "getTetraIdString" );
		FUNCTION_EXIT;
        return CString( td.primaryTsi.c_str() );
    }


    void TrainListCtrl::deselectAll()
    {
		FUNCTION_ENTRY( "deselectAll" );

        UINT uSelectedCount = GetSelectedCount();
        UINT i;
        int  nItem = -1;
    
        if (uSelectedCount > 0)
        {
            // Remove all selection masks
            for (i = 0; i < uSelectedCount;i++)
            {
                nItem = GetNextItem(nItem, LVNI_SELECTED);
            
                TA_ASSERT(nItem != -1, "Operation failed");
                        
                SetItemState(nItem, ~LVIS_SELECTED, LVIS_SELECTED);
            }
        }

		FUNCTION_EXIT;
    }


    CommonTypes::TrainIdList TrainListCtrl::getSelectedTrainIds() const
    {
		FUNCTION_ENTRY( "getSelectedTrainIds" );
        std::set< CommonTypes::TrainIdType > result;

        UINT uSelectedCount = GetSelectedCount();
        UINT i;
        int  nItem = -1;
    
        if (uSelectedCount > 0)
        {
            // Remove all selection masks
            for (i = 0; i < uSelectedCount;i++)
            {
                nItem = GetNextItem(nItem, LVNI_SELECTED);
            
                TA_ASSERT(nItem != -1, "Operation failed");

                // get the train number for the item
                unsigned int number = reinterpret_cast<TrainDetailsItemData*>(GetItemData(nItem))->trainId;

                result.insert(number);
            }
        }
		
		FUNCTION_EXIT;
        return result;
    }


	TrainInformationTypes::TrainDetailsList TrainListCtrl::getSelectedTrainData() const
    {
		FUNCTION_ENTRY( "getSelectedTrainData" );

        TrainInformationTypes::TrainDetailsList result;
		int  nItem = -1;

        UINT uSelectedCount = GetSelectedCount();
    
        if (uSelectedCount > 0)
        {
            // Remove all selection masks
            for (int i = 0; i < uSelectedCount;i++)
            {
                nItem = GetNextItem(nItem, LVNI_SELECTED);
            
                TA_ASSERT(nItem != -1, "Operation failed");

                // get the train details for the item
                result.push_back( reinterpret_cast<TrainDetailsItemData*>( GetItemData( nItem ) )->toBasicType() );
            }
        }

		FUNCTION_EXIT;
        return result;
    }


    void TrainListCtrl::setSelectedTrains(const CommonTypes::TrainIdList& trainIds)
    {
		FUNCTION_ENTRY( "setSelectedTrains" );

        for (int i = 0; i < GetItemCount(); i ++)
        {
            // get the item data
            TrainDetailsItemData* itemData = reinterpret_cast<TrainDetailsItemData*>(GetItemData(i));

            // if there are blank items in the list
            // and this is one of them - this is the end
            if ( m_paddedList && (itemData == NULL) )
            {
				FUNCTION_EXIT;
                return;
            }

            TA_ASSERT(itemData != NULL, "Item details are NULL, when there are not empty list items");

            if ( std::find(trainIds.begin(), trainIds.end(), itemData->trainId)
                != trainIds.end())
            {
                // Found this train ID in the selection list
                SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
            }
            else
            {
                SetItemState(i, ~LVIS_SELECTED, LVIS_SELECTED);
            }
        }

		FUNCTION_EXIT;
    }


    void TrainListCtrl::selectAll()
    {
		FUNCTION_ENTRY( "selectAll" );

        for (int i = 0; i < GetItemCount(); i ++)
        {
            // get the item data
            TrainDetailsItemData* itemData = reinterpret_cast<TrainDetailsItemData*>(GetItemData(i));

            // if there are blank items in the list
            // and this is one of them - this is the end
            if ( m_paddedList && (itemData == NULL) )
            {
				FUNCTION_EXIT;
                return;
            }

            SetItemState(i, LVIS_SELECTED, LVIS_SELECTED);
        }

		FUNCTION_EXIT;
    }


    void TrainListCtrl::padList()
    {
		FUNCTION_ENTRY( "padList" );

        if (!m_paddedList)
        {
            // the list is being padded
            m_paddedList = true;

            int itemsToAdd = GetCountPerPage();
            // if the padding is at the top, offset is 0, otherwise its the item count
            int offset = GetItemCount();

            for (int i = offset; i < (itemsToAdd + offset); i++)
            {
                // insert blank items
                InsertItem(i, "");
                SetItemData(i, NULL);
            }
        }

		FUNCTION_EXIT;
    }


    void TrainListCtrl::removePadding()
    {
		FUNCTION_ENTRY( "removePadding" );
        if (m_paddedList)
        {
            // remove all items with NULL item data
            int i = 0;
            while ( i < GetItemCount() )
            {
                if (GetItemData(i) == NULL)
                {
                    DeleteItem(i);
                }
                else
                {
                    i++;
                }
            }

            // all padding items have been removed
            m_paddedList = false;
        }
		
		FUNCTION_EXIT;
    }


    bool TrainListCtrl::isSelectable(int rowNumber)
    {
		FUNCTION_ENTRY( "isSelectable" );
        // get the item data
        TrainDetailsItemData* itemData = reinterpret_cast<TrainDetailsItemData*>(GetItemData(rowNumber));

        // if there are blank items in the list
        // and this is one of them then this is not selectable
        if ( m_paddedList && (itemData == NULL) )
        {
			FUNCTION_EXIT;
            return false;
        }

        if(m_eventListener != NULL)
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CL-21050 [train id=%d][current location=%d][communicating location=%d][row number=%d]",
                         itemData->trainId, itemData->currentLocation, itemData->communicatingLocation, rowNumber );

			FUNCTION_EXIT;
            return m_eventListener->onIsSelectable( this, 
													itemData->trainId, 
													itemData->currentLocation,
													rowNumber,
                                                    itemData->communicatingLocation );
        }

		FUNCTION_EXIT;
        return false;
    }


    void TrainListCtrl::setEventListener(ITrainListCtrlEventListener* eventListener)
    {
		FUNCTION_ENTRY( "setEventListener" );

        m_eventListener = eventListener;

		FUNCTION_EXIT;
    }


    void TrainListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
    {
        // if there is something managing events
	    if (m_eventListener != NULL)
	    {
		    m_eventListener->listScrolledV(this, nSBCode, nPos, pScrollBar);
	    }
	    else
        {
            // pass it on directly
		    redirectedVScroll(nSBCode, nPos, pScrollBar);
        }
    }


    void TrainListCtrl::redirectedVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
    {
        CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
    }


    void TrainListCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
    {
        // if there is something managing events
	    if (m_eventListener != NULL)
	    {
		    
		    m_eventListener->onKeyDown(this, nChar, nRepCnt, nFlags);
	    }
	    else
        {
            // pass it on directly
		    redirectedKeyDown(nChar, nRepCnt, nFlags);
        }
    }


    void TrainListCtrl::redirectedKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
    {
        CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
    }


    BOOL TrainListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
    {
        // if there is something managing events
	    if (m_eventListener != NULL)
	    {
		    return m_eventListener->onMouseWheel(this, nFlags, zDelta, pt);
	    }
        else
        {
            return redirectedOnMouseWheel(nFlags, zDelta, pt);
        }
    }


    BOOL TrainListCtrl::redirectedOnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
    {
        return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
    }


    BOOL TrainListCtrl::onColumnclick(NMHDR* pNMHDR, LRESULT* pResult)
    {
        *pResult = 0;

        // if there is something managing events
	    if (m_eventListener != NULL)
	    {
		    
		    m_eventListener->onColumnclick(this, pNMHDR, pResult);
	    }
	    else
        {
            // pass it on directly
		    redirectedColumnclick(pNMHDR, pResult);
        }

        // this allows the owning window to handle this as well
        return FALSE;
    }

    
    void TrainListCtrl::redirectedColumnclick(NMHDR* pNMHDR, LRESULT* pResult)
    {
        // a column has been clicked
        // get the column, and
        // update the sort order

        NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
        int column = pNMListView->iSubItem;

        // remove the old sort indicator
        removeSortIndicator(m_currentSortInformation.currentSortColumn);

        // if the same column was clicked, reverse the sort order
        if ( m_currentSortInformation.currentSortColumn == column )
        {
            // reverse the sort order
            m_currentSortInformation.currentSortOrder =
                static_cast<SortInformation::SortOrder>(~m_currentSortInformation.currentSortOrder);
        }
        // otherwise set the new sort order ascending
        else
        {
            m_currentSortInformation.currentSortColumn = column;
            m_currentSortInformation.currentSortOrder = SortInformation::ASCENDING;
        }

        // add sort indicator
        addSortIndicator(m_currentSortInformation.currentSortColumn,
                         m_currentSortInformation.currentSortOrder);

        // sort the list
        SortItems(trainListSort, reinterpret_cast<LPARAM>(this));
    }


    BOOL TrainListCtrl::onClick(NMHDR* pNMHDR, LRESULT* pResult)
    {
        *pResult = 0;

        // if there is something managing events
	    if (m_eventListener != NULL)
	    {
		    m_eventListener->onClick(this, pNMHDR, pResult);
	    }

        // this allows the owning window to handle this as well
        return FALSE;
    }


    BOOL TrainListCtrl::onItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
    {
        // call the parent class handler first
        // if it returns true - this has been blocked.
        if (ListCtrlSelNoFocus::OnItemchanged(pNMHDR, pResult) == TRUE)
        {
            return TRUE;
        }

        *pResult = 0;

        // if there is something managing events
	    if (m_eventListener != NULL)
	    {
		    m_eventListener->onItemchanged(this, pNMHDR, pResult);
	    }

        // this allows the owning window to handle this as well
        return FALSE;
    }


    void TrainListCtrl::removeSortIndicator(int column)
    {
		FUNCTION_ENTRY( "removeSortIndicator" );

        // get the header control
        // and remove the image
   	    HDITEM headerItem; 
	    headerItem.mask = HDI_FORMAT | HDI_BITMAP; 
	    CHeaderCtrl* headerCtrl = GetHeaderCtrl(); 
	    headerCtrl->GetItem( column, &headerItem ); 
	    headerItem.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT);

        // if there was a bitmap, delete it (set it to null, LR_SHARED auto cleans)
	    if (headerItem.hbm != NULL) 
	    { 
		    headerItem.hbm = NULL;
	    }

	    headerCtrl->SetItem( column, &headerItem );

		FUNCTION_EXIT;
    }


    void TrainListCtrl::addSortIndicator(int column, SortInformation::SortOrder indicatorType)
    {
		FUNCTION_ENTRY( "addSortIndicator" );

        // Add sort arrows to list columns
	    HDITEM headerItem; 
	    headerItem.mask = HDI_FORMAT | HDI_BITMAP; 
	    CHeaderCtrl* headerCtrl = GetHeaderCtrl(); 
	    headerCtrl->GetItem( column, &headerItem ); 

        // if there was a bitmap, delete it (set it to null, LR_SHARED auto cleans)
	    if (headerItem.hbm != NULL) 
	    { 
		    headerItem.hbm = NULL;
	    }
	    
	    headerItem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT; 

	    // Load the required image (from MFC extensions)
#ifdef _DEBUG
	    headerItem.hbm = (HBITMAP)LoadImage(GetModuleHandle("TA_MFCExtensions_d.dll"),
            MAKEINTRESOURCE((indicatorType == SortInformation::SortOrder::ASCENDING) ? IDB_SORT_ASC : IDB_SORT_DESC),
            IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_SHARED); 
#else
	    headerItem.hbm = (HBITMAP)LoadImage(GetModuleHandle("TA_MFCExtensions.dll"),
            MAKEINTRESOURCE((indicatorType == SortInformation::SortOrder::ASCENDING) ? IDB_SORT_ASC : IDB_SORT_DESC),
            IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_SHARED); 
#endif

	    // Setup the header with the image
	    headerCtrl->SetItem(column, &headerItem); 

		FUNCTION_EXIT;
    }


	TrainListCtrl::SortInformation::SortInformation()
        : currentSortColumn(TrainListCtrl::LOCATION),currentSortOrder(ASCENDING)
    {
		FUNCTION_ENTRY( "SortInformation" );
		FUNCTION_EXIT;
    }
	
	
    TrainListCtrl::TrainDetailsItemData::TrainDetailsItemData()
    {
		FUNCTION_ENTRY( "TrainDetailsItemData" );

		// trainId dont know how to initialized ???
		trainId = 0;
		communicationValid = false;
		primaryTsi = "";
		secondaryTsi = "";
		atsValid = false;
		isLocalisationValid = false;
		currentLocation = 0;
        currentCctvZone = 0;
		isMute = false;
		serviceNumber ="";
		track = TrainInformationTypes::AtsTrackUnknown;

		FUNCTION_EXIT;
    }
	
	
    TrainListCtrl::TrainDetailsItemData::TrainDetailsItemData(const TrainInformationTypes::TrainDetails& in)
      : trainId( in.trainId ),
        communicationValid( in.communicationValid ),
        communicatingLocation( in.communicatingLocation ),
        primaryTsi( in.primaryTsi ),
        secondaryTsi( in.secondaryTsi ),
        atsValid( in.atsValid ),
        isLocalisationValid( in.isLocalisationValid ),
        currentLocation( in.currentLocation ),
        currentCctvZone( in.currentCctvZone ),
        isMute( in.isMute ),
        serviceNumber( in.serviceNumber ),
        track( in.track )
    {
		FUNCTION_ENTRY( "TrainDetailsItemData" );

		FUNCTION_EXIT;
    }
	
	
	void TrainListCtrl::TrainDetailsItemData::operator=(const TrainInformationTypes::TrainDetails& rhs)
    {
		FUNCTION_ENTRY( "operator=" );
        
        trainId = rhs.trainId;
        communicationValid = rhs.communicationValid;
        communicatingLocation = rhs.communicatingLocation;
        primaryTsi = rhs.primaryTsi;
        secondaryTsi = rhs.secondaryTsi;
        atsValid = rhs.atsValid;
        isLocalisationValid = rhs.isLocalisationValid;
        currentLocation = rhs.currentLocation;
        currentCctvZone = rhs.currentCctvZone;
        isMute = rhs.isMute;
        serviceNumber = rhs.serviceNumber;
        track = rhs.track;

        FUNCTION_EXIT;
    }
	
	
    TrainInformationTypes::TrainDetails TrainListCtrl::TrainDetailsItemData::toBasicType() const 
    {
		FUNCTION_ENTRY( "toBasicType" );

		TrainInformationTypes::TrainDetails trainDetail;
		trainDetail.communicatingLocation = communicatingLocation;
		trainDetail.trainId = trainId;
		trainDetail.communicationValid = communicationValid;
		trainDetail.primaryTsi = primaryTsi;
		trainDetail.secondaryTsi = secondaryTsi;
		trainDetail.atsValid = atsValid;
		trainDetail.isLocalisationValid = isLocalisationValid;
		trainDetail.currentLocation = currentLocation;
        trainDetail.currentCctvZone = currentCctvZone;
		trainDetail.isMute = isMute;
		trainDetail.serviceNumber = serviceNumber;
		trainDetail.track = track;

		FUNCTION_EXIT;
		return  trainDetail;
    }


} // TA_IRS_Bus
