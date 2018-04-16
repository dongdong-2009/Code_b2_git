/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP/3001/transactive/app/signs/ttis_manager/src/CurrentDownloadsListCtrl.cpp $
  * @author Adam Radics
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2011/01/04 18:37:39 $
  * Last modified by: $Author: huang.wenbo $
  *
  * CurrentDownloadsListCtrl defines a list control that displays
  * train downloads.
  */


#include "stdafx.h"

#include "bus/mfc_extensions/src/Resource.h"
#include "resource.h"

#include "CurrentDownloadsListCtrl.h"
#include "TTISCommonConfiguration.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <algorithm>

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{


    CurrentDownloadsListCtrl::CurrentDownloadsListCtrl()
            : m_currentSortInformation()
    {
        FUNCTION_ENTRY( "CurrentDownloadsListCtrl" );
        FUNCTION_EXIT;
    }


    CurrentDownloadsListCtrl::~CurrentDownloadsListCtrl()
    {
        FUNCTION_ENTRY( "~CurrentDownloadsListCtrl" );
        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP( CurrentDownloadsListCtrl, TA_Base_Bus::ListCtrlSelNoFocus )
        // {{AFX_MSG_MAP(CurrentDownloadsListCtrl)
        ON_NOTIFY_REFLECT( LVN_COLUMNCLICK, onColumnclick )
        // }}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void CurrentDownloadsListCtrl::PreSubclassWindow()
    {
        FUNCTION_ENTRY( "PreSubclassWindow" );

        CListCtrl::PreSubclassWindow();

        FUNCTION_EXIT;
    }


    void CurrentDownloadsListCtrl::Init()
    {
        FUNCTION_ENTRY( "Init" );

        SetExtendedStyle( GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP );
        m_fixedHeaderCtrl.subclassHeader( GetHeaderCtrl() );

        InsertColumn( TRAIN,"TrainID/Service" );
        SetColumnWidth( TRAIN,110 );

        InsertColumn( TYPE,"Type" );
        SetColumnWidth( TYPE,100 );

        InsertColumn( STATUS,"Status" );
        SetColumnWidth( STATUS,LVSCW_AUTOSIZE_USEHEADER );

        addSortIndicator( m_currentSortInformation.currentSortColumn,
                          m_currentSortInformation.currentSortOrder );

        setNonSelectableGray( false );
        setScrollBarVisibility( true, false );

        FUNCTION_EXIT;
    }


    bool CurrentDownloadsListCtrl::isSelectable( int rowNumber )
    {
        FUNCTION_ENTRY( "isSelectable" );

        // only those retrying are selectable
        DownloadStatusDisplay* download =
            reinterpret_cast<DownloadStatusDisplay*>( GetItemData( rowNumber ) );

        if ( download->autoRetry )
        {
            FUNCTION_EXIT;
            return true;
        }
        else
        {
            FUNCTION_EXIT;
            return false;
        }
    }

    std::vector<DownloadStatusDisplay> CurrentDownloadsListCtrl::getInDownloads() const
    {		
        FUNCTION_ENTRY( "getInDownloads" );

        std::vector<DownloadStatusDisplay> indownloadsList;
        UINT itemCount = GetItemCount();
        int i = 0;

        while ( i < itemCount )
        {
            DownloadStatusDisplay* downloadStatus = reinterpret_cast<DownloadStatusDisplay*>( GetItemData( i++ ) );
            //if not finished
            if ( downloadStatus->currentState != DownloadStatusDisplay::Finished )
            {
                indownloadsList.push_back( *downloadStatus );
            }
        }

        FUNCTION_EXIT;
        return indownloadsList;
    }

    std::vector<DownloadStatusDisplay> CurrentDownloadsListCtrl::getSelectedDownloads() const
    {
        FUNCTION_ENTRY( "getSelectedDownloads" );

        std::vector<DownloadStatusDisplay> selectedDownloads;

        UINT uSelectedCount = GetSelectedCount();
        UINT i;
        int  nItem = -1;

        if ( uSelectedCount > 0 )
        {
            for ( i = 0; i < uSelectedCount;i++ )
            {
                nItem = GetNextItem( nItem, LVNI_SELECTED );

                TA_ASSERT( nItem != -1, "MFC lies! GetSelectedCount returned the wrong number." );

                // get the download structure for the item
                selectedDownloads.push_back(
                    *reinterpret_cast<DownloadStatusDisplay*>( GetItemData( nItem ) )
                );
            }
        }

        FUNCTION_EXIT;
        return selectedDownloads;
    }


    void CurrentDownloadsListCtrl::deselectAll()
    {
        FUNCTION_ENTRY( "deselectAll" );

        UINT uSelectedCount = GetSelectedCount();
        UINT i;
        int  nItem = -1;

        if ( uSelectedCount > 0 )
        {
            // Remove all selection masks
            for ( i = 0; i < uSelectedCount;i++ )
            {
                nItem = GetNextItem( nItem, LVNI_SELECTED );

                TA_ASSERT( nItem != -1, "MFC lies! GetSelectedCount returned the wrong number." );

                SetItemState( nItem, ~LVIS_SELECTED, LVIS_SELECTED );
            }
        }

        FUNCTION_EXIT;
    }


    void CurrentDownloadsListCtrl::selectAll()
    {
        FUNCTION_ENTRY( "selectAll" );

        for ( int i = 0; i < GetItemCount(); i ++ )
        {
            SetItemState( i, LVIS_SELECTED, LVIS_SELECTED );
        }

        FUNCTION_EXIT;
    }


    void CurrentDownloadsListCtrl::updateDownloadList( const DownloadStatusDisplay& trainDownload,
                                                       const std::string& serviceNo )
    {
        FUNCTION_ENTRY( "updateDownloadList" );

        // find the entry
        int nItem = findTrainIndex( trainDownload );

        // if its there, update the data
        // if its not, add it

        if ( nItem < 0 )
        {
            DownloadStatusDisplay* download = new DownloadStatusDisplay( trainDownload );

            nItem = findSortedInsertionPoint( trainDownload );

            nItem = InsertItem( nItem, getTrainIdString( *download, serviceNo ) );
			if( nItem > -1)
			{
	            SetItemText( nItem, TYPE, getTypeString( *download ) );
	            SetItemText( nItem, STATUS, download->displayMessage.c_str() );

	            // Set item data to allow searches/sorting
	            SetItemData( nItem, reinterpret_cast<DWORD>( download ) );
			}
        }
        else
        {
            // update the structure
            DownloadStatusDisplay* download =
                reinterpret_cast<DownloadStatusDisplay*>( GetItemData( nItem ) );

            // if the new update is pending, but there is already one
            // in progress, they just updated an existing download
            // just update the retry field.
            if ( ( trainDownload.currentState == DownloadStatusDisplay::Pending ) &&
                    ( download->currentState == DownloadStatusDisplay::InProgress ) )
            {
                download->autoRetry = trainDownload.autoRetry;
            }
            // if the new update is pending, and there is an old completed
            // download, then the display can be updated.
            // if the new update is pending, and the old one is pending,
            // this can be overwritten too
            //
            // Now if the new update says Finished or in progress
            // then whatever is currently being displayed can be overwritten
            else
            {
                download->trainId = trainDownload.trainId;
                download->downloadType = trainDownload.downloadType;
                download->autoRetry = trainDownload.autoRetry;
                download->currentState = trainDownload.currentState;
                download->displayMessage = trainDownload.displayMessage;
            }


            // Now update the item at this index
            SetItemText( nItem, TRAIN, getTrainIdString( *download, serviceNo ) );
            SetItemText( nItem, TYPE, getTypeString( *download ) );
            SetItemText( nItem, STATUS, download->displayMessage.c_str() );


            // if it was selected and no longer can be, reset the selection.
            if ( ( GetItemState( nItem, LVIS_SELECTED ) == LVIS_SELECTED ) &&
                    !isSelectable( nItem ) )
            {
                SetItemState( nItem, ~LVIS_SELECTED, LVIS_SELECTED );
            }

            // re-sort the list, because it has changed
            SortItems( trainListSort, reinterpret_cast<LPARAM>( this ) );
        }

        FUNCTION_EXIT;
    }


    void CurrentDownloadsListCtrl::updateTrainDetails( const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails )
    {
        FUNCTION_ENTRY( "updateTrainDetails" );

        // if the train is currently in the list, update the pv/service column

        for ( int i = 0; i < GetItemCount(); i++ )
        {
            DownloadStatusDisplay* item = reinterpret_cast<DownloadStatusDisplay*>( GetItemData( i ) );

            // if the train id and type match, this is the matching entry
            if ( trainDetails.trainId == item->trainId )
            {
                // matching train
                // update the text
                SetItemText( i, TRAIN, getTrainIdString( *item, std::string( trainDetails.serviceNumber ) ) );

                // no need to resort, the train id is still the same,
                // the service doesnt come into the sorting.
            }
        }

        FUNCTION_EXIT;
    }


    void CurrentDownloadsListCtrl::clearCompleted()
    {
        FUNCTION_ENTRY( "clearCompleted" );

        // find all items not in progress
        // and not auto-retrying
        int i = 0;
        while ( i < GetItemCount() )
        {
            DownloadStatusDisplay* download =
                reinterpret_cast<DownloadStatusDisplay*>( GetItemData( i ) );

            // and delete them
            if ( ( download->currentState == DownloadStatusDisplay::Finished ) &&
                    ( download->autoRetry == false ) )
            {
                // clean up the structure
                delete download;
                download = NULL;

                // delete item i, the next item shuffles down,
                // no need to increment
                DeleteItem( i );
            }
            else
            {
                // proceed to next item
                i++;
            }
        }

        FUNCTION_EXIT;
    }


    bool CurrentDownloadsListCtrl::completedDownloadsInList()
    {
        FUNCTION_ENTRY( "completedDownloadsInList" );

        for ( int i = 0; i < GetItemCount(); i++ )
        {
            DownloadStatusDisplay* download =
                reinterpret_cast<DownloadStatusDisplay*>( GetItemData( i ) );

            if ( ( download->currentState == DownloadStatusDisplay::Finished ) &&
                    ( download->autoRetry == false ) )
            {
                FUNCTION_EXIT;
                return true;
            }
        }

        FUNCTION_EXIT;
        return false;
    }

	bool CurrentDownloadsListCtrl::uncompletedDownloadsInList()
	{
		FUNCTION_ENTRY( "uncompletedDownloadsInList" );

		for ( int i = 0; i < GetItemCount(); i++ )
		{
			DownloadStatusDisplay* download =
				reinterpret_cast<DownloadStatusDisplay*>( GetItemData( i ) );

			if ( ( download->currentState != DownloadStatusDisplay::Finished ) &&
				( download->autoRetry == false ) )
			{
				FUNCTION_EXIT;
				return true;
			}
		}

		FUNCTION_EXIT;
		return false;
	}


    int CurrentDownloadsListCtrl::findSortedInsertionPoint( const DownloadStatusDisplay& trainDownload ) const
    {
        FUNCTION_ENTRY( "findSortedInsertionPoint" );

        // special case, integer comparison
        if ( m_currentSortInformation.currentSortColumn == TRAIN )
        {
            unsigned int newTrainId = trainDownload.trainId;
            unsigned int itemTrainId = 0;

            unsigned int* data1;
            unsigned int* data2;
            int i = 0;

            // set sort comparison order
            if ( m_currentSortInformation.currentSortOrder == SortInformation::ASCENDING )
            {
                data1 = &newTrainId;
                data2 = &itemTrainId;
            }
            else
            {
                data2 = &newTrainId;
                data1 = &itemTrainId;
            }

            for ( i = 0; i < GetItemCount(); i ++ )
            {
                // the id of the train at index i
                itemTrainId = reinterpret_cast<DownloadStatusDisplay*>( GetItemData( i ) )->trainId;

                // Compare current id with the new id - we want
                if ( *data1 < *data2 )
                {
                    // we can insert here..
                    FUNCTION_EXIT;
                    return i;
                }
            }

            // Insert right at end of list..
            FUNCTION_EXIT;
            return i;
        }
        else // string comparisons
        {
            CString newItemString;
            CString currentItemString;
            int i = 0;

            switch ( m_currentSortInformation.currentSortColumn )
            {
                case TYPE:
                    newItemString = getTypeString( trainDownload );
                    break;
                case STATUS:
                    newItemString = trainDownload.displayMessage.c_str();
                    break;
                default:
                    TA_ASSERT( false, "Invalid sort column" );
                    // not possible code
					//break;
            }

            CString* data1;
            CString* data2;

            // set sort comparison order
            if ( m_currentSortInformation.currentSortOrder == SortInformation::ASCENDING )
            {
                data1 = &newItemString;
                data2 = &currentItemString;
            }
            else
            {
                data2 = &newItemString;
                data1 = &currentItemString;
            }

            for ( i = 0; i < GetItemCount(); i ++ )
            {
                // the id of the train at index i
                currentItemString = GetItemText( i, m_currentSortInformation.currentSortColumn );

                // Compare current id with the new id - we want
                if ( *data1 < *data2 )
                {
                    // we can insert here..
                    FUNCTION_EXIT;
                    return i;
                }
            }

            // Insert right at end of list..
            FUNCTION_EXIT;
            return i;

        }
    }


    int CurrentDownloadsListCtrl::findTrainIndex( const DownloadStatusDisplay& trainDownload ) const
    {
        FUNCTION_ENTRY( "findTrainIndex" );

        // find the first matching train Id
        for ( int i = 0; i < GetItemCount(); i++ )
        {
            DownloadStatusDisplay* item = reinterpret_cast<DownloadStatusDisplay*>( GetItemData( i ) );

            // if the train id and type match, this is the matching entry
            if ( ( trainDownload.trainId == item->trainId ) &&
                    ( trainDownload.downloadType == item->downloadType ) )
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


    CString CurrentDownloadsListCtrl::getTrainIdString( const DownloadStatusDisplay& trainDownload,
                                                        const std::string& serviceNo ) const
    {
        FUNCTION_ENTRY( "getTrainIdString" );

        CString result;
        result.Format( "%s", serviceNo.c_str() );

        FUNCTION_EXIT;
        return result;
    }


    CString CurrentDownloadsListCtrl::getTypeString( const DownloadStatusDisplay& trainDownload ) const
    {
        FUNCTION_ENTRY( "getTypeString" );

        CString result;

        switch ( trainDownload.downloadType )
        {
            case LibraryDownload:
            case ScheduleDownload:
                result = "Download";
                break;
            case LibraryUpgrade:
            case ScheduleUpgrade:
                result = "Upgrade";
                break;
            default: // to keep the compiler happy
                result = "Unknown";
                break;
        }

        FUNCTION_EXIT;
        return result;
    }


    int CALLBACK CurrentDownloadsListCtrl::trainListSort( LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort )
    {
        FUNCTION_ENTRY( "trainListSort" );

        CurrentDownloadsListCtrl* currentList = reinterpret_cast<CurrentDownloadsListCtrl*>( lParamSort );
        SortInformation currentSortInformation = currentList->getCurrentSortInformation();

        // lParam1 and lParam2 are DownloadStatusDisplay*
        DownloadStatusDisplay* param1;
        DownloadStatusDisplay* param2;

        // change comparison order based on sort order
        if ( currentSortInformation.currentSortOrder == SortInformation::ASCENDING )
        {
            param1 = reinterpret_cast<DownloadStatusDisplay*>( lParam1 );
            param2 = reinterpret_cast<DownloadStatusDisplay*>( lParam2 );
        }
        else
        {
            param1 = reinterpret_cast<DownloadStatusDisplay*>( lParam2 );
            param2 = reinterpret_cast<DownloadStatusDisplay*>( lParam1 );
        }

        switch ( currentSortInformation.currentSortColumn )
        {
            case TRAIN:
                FUNCTION_EXIT;
                return param1->trainId - param2->trainId;
				//break;
            case TYPE:
                FUNCTION_EXIT;
                return strcmp ( currentList->getTypeString( *param1 ),
                                currentList->getTypeString( *param2 ) );
                //break;
            case STATUS:
                FUNCTION_EXIT;
                return strcmp ( param1->displayMessage.c_str(),
                                param2->displayMessage.c_str() );
                //break;
            default:
                TA_ASSERT( false, "Invalid sort column" );
                //break;
        }

        // keep the compiler happy
        FUNCTION_EXIT;
        return 0;
    }


    void CurrentDownloadsListCtrl::onColumnclick( NMHDR* pNMHDR, LRESULT* pResult )
    {
        FUNCTION_ENTRY( "onColumnclick" );

        // a column has been clicked
        // get the column, and
        // update the sort order

        NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>( pNMHDR );
        int column = pNMListView->iSubItem;

        removeSortIndicator( m_currentSortInformation.currentSortColumn );

        // if the same column was clicked, reverse the sort order
        if ( m_currentSortInformation.currentSortColumn == column )
        {
            // reverse the sort order
            m_currentSortInformation.currentSortOrder =
                static_cast<SortInformation::SortOrder>( ~m_currentSortInformation.currentSortOrder );
        }
        // otherwise set the new sort order ascending
        else
        {
            m_currentSortInformation.currentSortColumn = static_cast<DownloadListColumns>( column );
            m_currentSortInformation.currentSortOrder = SortInformation::ASCENDING;
        }

        addSortIndicator( m_currentSortInformation.currentSortColumn,
                          m_currentSortInformation.currentSortOrder );

        // sort the list
        SortItems( trainListSort, reinterpret_cast<LPARAM>( this ) );

        FUNCTION_EXIT;
    }


    void CurrentDownloadsListCtrl::removeSortIndicator( int column )
    {
        FUNCTION_ENTRY( "removeSortIndicator" );

        // get the header control
        // and remove the image
        HDITEM headerItem;
        headerItem.mask = HDI_FORMAT | HDI_BITMAP;
        CHeaderCtrl* headerCtrl = GetHeaderCtrl();
        headerCtrl->GetItem( column, &headerItem );
        headerItem.fmt &= ~( HDF_BITMAP | HDF_BITMAP_ON_RIGHT );

        // if there was a bitmap, delete it (set it to null, LR_SHARED auto cleans)
        if ( headerItem.hbm != NULL )
        {
            headerItem.hbm = NULL;
        }

        headerCtrl->SetItem( column, &headerItem );

        FUNCTION_EXIT;
    }


    void CurrentDownloadsListCtrl::addSortIndicator( int column, SortInformation::SortOrder indicatorType )
    {
        FUNCTION_ENTRY( "addSortIndicator" );

        // Add sort arrows to list columns
        HDITEM headerItem;
        headerItem.mask = HDI_FORMAT | HDI_BITMAP;
        CHeaderCtrl* headerCtrl = GetHeaderCtrl();
        headerCtrl->GetItem( column, &headerItem );

        // if there was a bitmap, delete it (set it to null, LR_SHARED auto cleans)
        if ( headerItem.hbm != NULL )
        {
            headerItem.hbm = NULL;
        }

        headerItem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT;

        // Load the required image (from MFC extensions)
#ifdef _DEBUG
        headerItem.hbm = ( HBITMAP )LoadImage( GetModuleHandle( "TA_MFCExtensions_d.dll" ),
                                               MAKEINTRESOURCE( ( indicatorType == SortInformation::SortOrder::ASCENDING ) ? IDB_SORT_ASC : IDB_SORT_DESC ),
                                               IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_SHARED );
#else
        headerItem.hbm = ( HBITMAP )LoadImage( GetModuleHandle( "TA_MFCExtensions.dll" ),
                                               MAKEINTRESOURCE( ( indicatorType == SortInformation::SortOrder::ASCENDING ) ? IDB_SORT_ASC : IDB_SORT_DESC ),
                                               IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_SHARED );
#endif

        // Setup the header with the image
        headerCtrl->SetItem( column, &headerItem );

        FUNCTION_EXIT;
    }


//xufeng++ 2006/11/10 TDTD17538
    /*
    DownloadStatusDisplay* CurrentDownloadsListCtrl::getDownloadStatus(const DownloadStatusDisplay& trainDownload)
    {
        int nItem = findTrainIndex(trainDownload);
        DownloadStatusDisplay* download = reinterpret_cast<DownloadStatusDisplay*>(GetItemData(nItem));
        return download;
    }
    */
//++xufeng 2006/11/10 TDTD17538

} // TA_IRS_App

