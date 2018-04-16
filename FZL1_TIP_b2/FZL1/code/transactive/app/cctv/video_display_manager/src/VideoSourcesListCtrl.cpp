/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/app/cctv/video_display_manager/src/VideoSourcesListCtrl.cpp $
  * @author Adam Radics
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/11/18 18:38:22 $
  * Last modified by: $Author: huang.wenbo $
  * 
  * This is a modified train list control for the cctv list which had to be different
  */
#include "StdAfx.h"
#include "app/cctv/video_display_manager/src/VideoDisplayManagerDlg.h"
#include "app/cctv/video_display_manager/src/VideoSourcesListCtrl.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include <algorithm>

/////////////////////


/////////////////////
// colours

#define GREY_COLOUR         RGB(140, 140, 140)
#define ACTIVE_COLOUR       RGB(50,200,50)
#define PRECONFLICT_COLOUR  RGB(255, 128, 0)

#define ACTIVE_STRING       "Active"
#define INACTIVE_STRING     "Inactive"
#define PRECONFLICT_STRING  "Pre-Conflict"

namespace TA_IRS_App
{

    using TA_Base_Core::DebugUtil;
    using TA_Base_Core::TransactiveException;
    using TA_IRS_Bus::LocationCache;

    VideoSourcesListCtrl::VideoSourcesListCtrl()
     : m_sortInfo(4),
       m_pDetailsEdit(NULL)//,
   //    m_localStage(NULL)
    {
        FUNCTION_ENTRY( "VideoSourcesListCtrl" );
        FUNCTION_EXIT;
    }


    VideoSourcesListCtrl::~VideoSourcesListCtrl()
    {
        FUNCTION_ENTRY( "~VideoSourcesListCtrl" );
        FUNCTION_EXIT;
    }


    BEGIN_MESSAGE_MAP(VideoSourcesListCtrl, ListCtrlSelNoFocus)
    //{{AFX_MSG_MAP(VideoSourcesListCtrl)
    ON_NOTIFY_REFLECT(LVN_BEGINDRAG, OnBegindrag)
    ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, onItemChanged)
    ON_WM_MOUSEACTIVATE()
    ON_WM_MOUSEWHEEL()
    ON_WM_LBUTTONDOWN()
    ON_WM_RBUTTONDOWN()
    ON_WM_LBUTTONDBLCLK()
    ON_WM_RBUTTONDBLCLK()
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_WM_CONTEXTMENU()
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void VideoSourcesListCtrl::PreSubclassWindow()
    {
        FUNCTION_ENTRY( "PreSubclassWindow" );

        ListCtrlSelNoFocus::PreSubclassWindow();

        FUNCTION_EXIT;
    }


    void VideoSourcesListCtrl::Init()
    {
        FUNCTION_ENTRY( "Init" );

        SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
        m_fixedHeaderCtrl.subclassHeader(GetHeaderCtrl());
        m_imageList.Create(16, 16, ILC_MASK, 2, 0);
        m_imageList.Add((AfxGetApp()->LoadIcon(IDI_TRAIN)));

        InsertColumn(PV_SERVICE, "PV/Service");
        m_sortInfo.defineSortingSemantic(PV_SERVICE, AutoSortListCtrl::BY_CALLBACK);
        InsertColumn(LOCATION, "LOC");
        m_sortInfo.defineSortingSemantic(LOCATION, AutoSortListCtrl::BY_CALLBACK);
        InsertColumn(STAGE, "Stage");
        m_sortInfo.defineSortingSemantic(STAGE, AutoSortListCtrl::BY_STRING);
        InsertColumn(STATUS, "Status");
        m_sortInfo.defineSortingSemantic(STATUS, AutoSortListCtrl::BY_STRING);

        // Create bitmap
        CClientDC    dc (this);
        CDC         memDC;
        memDC.CreateCompatibleDC(&dc);
        CBitmap bitmap;
        bitmap.CreateCompatibleBitmap(&dc, 16,16);
        CBitmap* pOldMemDCBitmap = memDC.SelectObject( &bitmap );
        memDC.FillSolidRect(0,0,16,16, GREY_COLOUR);
        m_imageList.Draw(&memDC, 0, CPoint(0,0), ILD_BLEND50);
        memDC.SelectObject( pOldMemDCBitmap );
        m_imageList.Add(&bitmap,GREY_COLOUR);

        SetImageList(&m_imageList, LVSIL_SMALL);

        // sort callback for item data
        m_sortInfo.setCallback(this);
        // Sort columns based on label
        m_sortInfo.setCurrentSort(PV_SERVICE, AutoSortListCtrl::ASCENDING);
        // Make label column the active sorting column
        m_sortInfo.activateSort(PV_SERVICE);

        SetColumnWidth(PV_SERVICE, 105);
        SetColumnWidth(LOCATION, 60);
        SetColumnWidth(STAGE, 95);
        SetColumnWidth(STATUS, 95);

        // get the stage for this location
       // m_localStage = DisplayItemManager::getInstance().getBvsStageForLocation(
       //                     LocationCache::getInstance().getLocationKey() );

        FUNCTION_EXIT;
    }


    void VideoSourcesListCtrl::updateTrainList(const TA_IRS_Bus::ITrainSelectorGUI::EAction& action,
                                             const TA_IRS_Bus::TrainInformationTypes::TrainDetails& trainDetails)
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


    void VideoSourcesListCtrl::addToList(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& td)
    {
        FUNCTION_ENTRY( "addToList" );

        // check that the train is in this stage
      //  if ( !trainIsInCurrentStage(td) )
      //  {
      //      FUNCTION_EXIT;
      //      return;
     //   }

        // convert the corba type into the local type
        TrainDetails* itemDetails = new TrainDetails(td);

        int nItem = InsertItem(0, getPvServiceString(*itemDetails) );
        // add the pointer to the structure as the item details
        SetItemData(nItem, (DWORD) itemDetails);

        SetItemText(nItem, LOCATION, getLocationString(*itemDetails) );
   //     SetItemText(nItem, STAGE, getStageString(*itemDetails) );

     /*   CString statusString = getStatusString(*itemDetails);
        SetItemText(nItem, STATUS, statusString );

        if (statusString.Compare(ACTIVE_STRING) == 0)
        {
            setItemColour((DWORD) itemDetails,
                           ACTIVE_COLOUR,
                           I_ITEMDATA,
                           CT_FOREGROUND);
        }
        else if (statusString.Compare(PRECONFLICT_STRING) == 0)
        {
            setItemColour((DWORD) itemDetails,
                           PRECONFLICT_COLOUR,
                           I_ITEMDATA,
                           CT_FOREGROUND);
        }
        else
        {
            unsigned long locationKey = LocationCache::getInstance().getLocationKey();
            TA_Base_Core::ILocation::ELocationType locationType =
                LocationCache::getInstance().getLocationTypeByKey(locationKey);

            if(locationKey != td.currentLocation)
            {
                setItemColour((DWORD) itemDetails,
                          GREY_COLOUR,
                           I_ITEMDATA,
                           CT_FOREGROUND);
            }

            if(locationType == TA_Base_Core::ILocation::OCC)
            {
                clearItemColour((DWORD) itemDetails, I_ITEMDATA);
            }
        }*/

        FUNCTION_EXIT;
    }


    void VideoSourcesListCtrl::deleteFromList(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& td)
    {
        FUNCTION_ENTRY( "deleteFromList" );

        int nItem = findTrainIndex(td.trainId);

        if (nItem < 0)
        {
            // Couldn't find item to delete
            // - already deleted?
            // - never received in the first place?
            // just ignore it.
        }
        else
        {
            // delete the structure
            TrainDetails* itemDetails = reinterpret_cast<TrainDetails*>(GetItemData(nItem));
            clearItemColour((DWORD) itemDetails, I_ITEMDATA);
            delete itemDetails;

            // delete the item
            DeleteItem(nItem);
        }

        FUNCTION_EXIT;
    }


    void VideoSourcesListCtrl::updateList(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& td)
    {
        FUNCTION_ENTRY( "updateList" );

        int nItem = findTrainIndex(td.trainId);

        if (nItem < 0)
        {
            // Couldn't find item to update
            // - we missed the add?
            // it's obviously supposed to be there, so add it.
            addToList(td);
        }
        else
        {
            // check that the train is in this stage
            if ( !trainIsInCurrentStage(td) )
            {
                deleteFromList(td);

               FUNCTION_EXIT;
                return;
            }

            // Update item data to allow searching/sorting to be carried out
            TrainDetails* itemDetails = reinterpret_cast<TrainDetails*>(GetItemData(nItem));
            *itemDetails = td;

      /*      CString statusString = getStatusString(*itemDetails);

            // Now update the item at this index
            SetItemText(nItem, PV_SERVICE, getPvServiceString(*itemDetails) );
            SetItemText(nItem, LOCATION, getLocationString(*itemDetails) );
            SetItemText(nItem, STAGE, getStageString(*itemDetails) );
            SetItemText(nItem, STATUS, statusString );

            if (statusString.Compare(ACTIVE_STRING) == 0)
            {
                setItemColour((DWORD) itemDetails,
                               ACTIVE_COLOUR,
                               I_ITEMDATA,
                               CT_FOREGROUND);
            }
            else if (statusString.Compare(PRECONFLICT_STRING) == 0)
            {
                setItemColour((DWORD) itemDetails,
                               PRECONFLICT_COLOUR,
                               I_ITEMDATA,
                               CT_FOREGROUND);
            }
            else
            {
                clearItemColour((DWORD) itemDetails, I_ITEMDATA);
            }
*/
        }

        FUNCTION_EXIT;
    }


    bool VideoSourcesListCtrl::trainIsInCurrentStage(const TA_IRS_Bus::TrainInformationTypes::TrainDetails& td)
    {
        FUNCTION_ENTRY( "trainIsInCurrentStage" );
          FUNCTION_EXIT;
		return false;

        /*
        if ( (m_localStage != NULL) &&
         (( (VideoDisplayManagerApp*)(AfxGetApp()) )->getLocationType() == TA_Base_Core::ILocation::STATION) )
        {
            if ( ( true == td.atsValid ) &&
                 ( true == td.isLocalisationValid ) )
            {
                // return if the train details match the stage
                FUNCTION_EXIT;
                return ( m_localStage->getStageId() == td.currentCctvZone );
            }
        }
        */
      
      //  return true;
    }


    void VideoSourcesListCtrl::deleteAllTrains()
    {
        FUNCTION_ENTRY( "deleteAllTrains" );

        int i = 0;

        while ( GetItemCount() > 0)
        {
            // get the item data
            TrainDetails* itemData = reinterpret_cast<TrainDetails*>(GetItemData(0));
            clearItemColour((DWORD) itemData, I_ITEMDATA);
            delete itemData;

            // delete the item
            DeleteItem(0);
        }

        FUNCTION_EXIT;
    }


    int VideoSourcesListCtrl::findTrainIndex(unsigned short trainId) const
    {
        FUNCTION_ENTRY( "findTrainIndex" );

        // find the first matching train Id
        for ( int i = 0; i < GetItemCount(); i++ )
        {
            // get the item data
            TrainDetails* itemData = reinterpret_cast<TrainDetails*>(GetItemData(i));

            // if this point is reached, the item data mustnt be null
            TA_ASSERT(itemData != NULL, "Item details are NULL");

            // get the train Id from the structure in the item data
            unsigned short itemTrainId = itemData->trainID;

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


    CString VideoSourcesListCtrl::getLocationString(const TrainDetails& td) const
    {
        FUNCTION_ENTRY( "getLocationString" );

        // Interpret the location ID as a string
        CString location;

        try
        {
            if((td.atsValid == Valid) && (td.isLocalisationValid == false))
            {
                location = "(";
            }

            // look up the number
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

            if((td.atsValid == Valid) && (td.isLocalisationValid == false))
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


    CString VideoSourcesListCtrl::getPvServiceString(const TrainDetails& td) const
    {
        FUNCTION_ENTRY( "getPvServiceString" );

        CString result(td.serviceNumber.c_str());

        if ( result == "" )
        {
            result.Format("%d/_", td.trainID);
        }

        if (td.isMute)
        {
            result += " (M)";
        }

        FUNCTION_EXIT;
        return result;
    }


  /*  CString VideoSourcesListCtrl::getStageString(const TrainDetails& td) const
    {
        FUNCTION_ENTRY( "getStageString" );

        BvsStage* stage =
            DisplayItemManager::getInstance().getBvsStageForLocation(td.currentLocation);

        if (stage != NULL)
        {
            FUNCTION_EXIT;
            return stage->getLocationDescription().c_str();
        }

        FUNCTION_EXIT;
        return "";
    }*/


    // TD16902
   /* CString VideoSourcesListCtrl::getStageNumber(const TrainDetails& td) const
    {
        FUNCTION_ENTRY( "getStageNumber" );

        BvsStage* stage =
            DisplayItemManager::getInstance().getBvsStageForLocation(td.currentLocation);

        if (stage != NULL)
        {
            FUNCTION_EXIT;
            return stage->getStageNumber().c_str();
        }

        FUNCTION_EXIT;
        return "";
    }*/


    // TD16902

    /*CString VideoSourcesListCtrl::getStatusString(const TrainDetails& td) const
    {
        FUNCTION_ENTRY( "getStatusString" );

        BvsStage* stage =
            DisplayItemManager::getInstance().getBvsStageForLocation(td.currentLocation);


        if (stage != NULL)
        {
            if (stage->getActiveTrain() == td.trainID)
            {
                if (stage->isInPreConflict())
                {
                    FUNCTION_EXIT;
                    return PRECONFLICT_STRING;
                }
                else
                {
                    FUNCTION_EXIT;
                    return ACTIVE_STRING;
                }
            }
        }

        FUNCTION_EXIT;
        return INACTIVE_STRING;
    }*/


    unsigned short VideoSourcesListCtrl::getSelectedTrainId() const
    {
        FUNCTION_ENTRY( "getSelectedTrainId" );

        int nItem = GetNextItem(-1, LVNI_SELECTED);

        if (nItem < 0)
        {
            FUNCTION_EXIT;
            return 0;
        }

        // get the item data
        TrainDetails* itemData = reinterpret_cast<TrainDetails*>(GetItemData(nItem));

        // if this point is reached, the item data mustnt be null
        TA_ASSERT(itemData != NULL, "Item details are NULL");

        // return the train Id from the structure in the item data
        FUNCTION_EXIT;
        return itemData->trainID;
    }


    VideoSourcesListCtrl::TrainDetails* VideoSourcesListCtrl::getSelectedTrainDetails()
    {
        FUNCTION_ENTRY( "getSelectedTrainDetails" );

        int nItem = GetNextItem(-1, LVNI_SELECTED);

        if (nItem < 0)
        {
            FUNCTION_EXIT;
            return NULL;
        }

        // get the item data
        TrainDetails* itemData = reinterpret_cast<TrainDetails*>(GetItemData(nItem));

        // if this point is reached, the item data mustnt be null
        TA_ASSERT(itemData != NULL, "Item details are NULL");

        FUNCTION_EXIT;
        return itemData;
    }


    VideoSourcesListCtrl::TrainDetails* VideoSourcesListCtrl::getTrainDetails(unsigned short trainId)
    {
        FUNCTION_ENTRY( "getTrainDetails" );

        int index = findTrainIndex(trainId);

        if (index > -1)
        {
            // get the item data
            TrainDetails* itemData = reinterpret_cast<TrainDetails*>(GetItemData(index));

            // if this point is reached, the item data mustnt be null
            TA_ASSERT(itemData != NULL, "Item details are NULL");

            FUNCTION_EXIT;
            return itemData;
        }

        FUNCTION_EXIT;
        return NULL;
    }


    bool VideoSourcesListCtrl::isSelectable(int rowNumber)
    {
        FUNCTION_ENTRY( "isSelectable" );

        // get the item data
        TrainDetails* itemData = reinterpret_cast<TrainDetails*>(GetItemData(rowNumber));

        TA_ASSERT(itemData != NULL, "Item details are NULL");

        // if at a station
        /*if ( (m_localStage != NULL) &&
             (( (VideoDisplayManagerApp*)(AfxGetApp()) )->getLocationType() == TA_Base_Core::ILocation::STATION) )
        {
            // get the stage for the train
            BvsStage* trainStage =
                DisplayItemManager::getInstance().getBvsStageForLocation(itemData->currentLocation);

            // compare the location against the current stage

            // if they are not the same
            if (m_localStage != trainStage)
            {
                FUNCTION_EXIT;
                return false;
            }
        }*/

        FUNCTION_EXIT;
        return LocationCache::getInstance().isInControlOfLocation(itemData->currentLocation);
    }


    // for sorting
    LRESULT VideoSourcesListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY( "WindowProc" );

        AutoSortListCtrl::WindowProc(message, wParam, lParam, *this, m_sortInfo);

        FUNCTION_EXIT;
        return ListCtrlSelNoFocus::WindowProc(message, wParam, lParam);
    }


    int VideoSourcesListCtrl::sortCallback(LPARAM lParam1, LPARAM lParam2, int columnIndex, AutoSortListCtrl::ECurrentSort currentSort)
    {
        FUNCTION_ENTRY( "sortCallback" );

        TrainDetails* first = NULL;
        TrainDetails* second = NULL;

        if (currentSort == AutoSortListCtrl::DESCENDING)
        {
            first = reinterpret_cast<TrainDetails*>(lParam2);
            second = reinterpret_cast<TrainDetails*>(lParam1);
        }
        else
        {
            // ascending or unknown - which will default to ascending
            first = reinterpret_cast<TrainDetails*>(lParam1);
            second = reinterpret_cast<TrainDetails*>(lParam2);
        }


        if (columnIndex == PV_SERVICE)
        {
            FUNCTION_EXIT;
            return (first->trainID - second->trainID);
        }
        else if (columnIndex == LOCATION)
        {
            unsigned long firstCurrentLocationOrderId = 0;
            unsigned long secondCurrentLocationOrderId = 0;
            try
            {
                firstCurrentLocationOrderId = LocationCache::getInstance().getOrderIdByLocation(first->currentLocation);
                secondCurrentLocationOrderId = LocationCache::getInstance().getOrderIdByLocation(second->currentLocation);
            }
            catch (const TA_Base_Core::TransactiveException& te)
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "TA_Base_Core::TransactiveException", te.what());
            }
            int value = ( firstCurrentLocationOrderId - secondCurrentLocationOrderId);

            // secondary sort by id
            if (value == 0)
            {
                FUNCTION_EXIT;
                return (reinterpret_cast<TrainDetails*>(lParam1)->trainID -
                        reinterpret_cast<TrainDetails*>(lParam2)->trainID);
            }

            FUNCTION_EXIT;
            return value;
        }
        else
        {
            TA_ASSERT(false, "Unsupported column given to sort by callback");
        }

        FUNCTION_EXIT;
        return 0;
    }


    void VideoSourcesListCtrl::OnBegindrag(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "OnBegindrag" );

        // get the selected train
        TrainDetails* selectedTrain = getSelectedTrainDetails();

        // if there is a train selected, allow it to be dragged
        // if not, then the train could not be selected so do not allow dragging
        if(selectedTrain != NULL)
        {
            NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
            SetFocus();
            CRect itemRect;
            CImageList* dragImage = CreateDragImageEx(GetSelectionMark(), &CPoint(pNMListView->ptAction));
            CPoint adjust;
            GetItemRect(GetSelectionMark(),&itemRect,LVIR_BOUNDS);
            adjust.y = pNMListView->ptAction.y - itemRect.top;
            adjust.x = pNMListView->ptAction.x - itemRect.left;
            ( (VideoDisplayManagerDlg*)(AfxGetApp()->m_pMainWnd) )->setDragAdjust(adjust);
            ( (VideoDisplayManagerDlg*)(AfxGetApp()->m_pMainWnd) )->setDragImage(dragImage);
            ::SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_LBUTTONDOWN, NULL, NULL);
        }

        *pResult = 0;

        FUNCTION_EXIT;
    }


    CImageList* VideoSourcesListCtrl::CreateDragImageEx(int nItem, LPPOINT lpPoint)
    {
        FUNCTION_ENTRY( "CreateDragImageEx" );

        CImageList* std = CreateDragImage(nItem, lpPoint);
        IMAGEINFO info;
        CRect stdRect;
        std->GetImageInfo(0,&info);
        stdRect = info.rcImage;

        // Create bitmap
        CClientDC    dc (this);
        CDC         memDC;

        if(!memDC.CreateCompatibleDC(&dc))
        {
            FUNCTION_EXIT;
            return NULL;
        }


        CBitmap bitmap;
        if(!bitmap.CreateCompatibleBitmap(&dc, stdRect.Width(), stdRect.Height()))
        {
            FUNCTION_EXIT;
            return NULL;
        }


        CBitmap* pOldMemDCBitmap = memDC.SelectObject( &bitmap );
        CFont* pOldFont = memDC.SelectObject(GetFont());

        memDC.FillSolidRect(&stdRect, GREY_COLOUR); // mask
        CString state;


        // check if the train is active or pre-conflict

        if (GetItemText(nItem, 1).CompareNoCase(ACTIVE_STRING) == 0)
        {
            memDC.SetTextColor(ACTIVE_COLOUR);
        }
        else if (GetItemText(nItem, 1).CompareNoCase(PRECONFLICT_STRING) == 0)
        {
            memDC.SetTextColor(PRECONFLICT_COLOUR);
        }
        else
        {
            memDC.SetTextColor(GetSysColor(COLOR_WINDOWTEXT));
        }

        LVITEM lvItem;
        DWORD cchBuf(1024);
        lvItem.mask = LVIF_IMAGE;
        lvItem.iItem = nItem;
        lvItem.iSubItem = 0;
        lvItem.iImage = cchBuf;
        GetItem(&lvItem);
        ::DrawIconEx(memDC.GetSafeHdc(),0,0,m_imageList.ExtractIcon(lvItem.iImage - 1),
                        16,16,0,NULL,DI_NORMAL);

        CRect rect, iconRect;
        GetItemRect(nItem, iconRect, LVIR_ICON);
        int columnCount = GetHeaderCtrl()->GetItemCount();
        int columnWidth =  GetColumnWidth(0);
        int offset = __min(iconRect.Width()+2, columnWidth);
        rect = stdRect;
        rect.DeflateRect(offset, 0,0,0);
        rect.right = columnWidth;
        CString str = GetItemText(nItem, 0);
        BOOL shrink = GetStringWidth(str) > (rect.Width()-4);
        if ( shrink )
        {
            str += "...";
        }
        while ( shrink && (str.GetLength() > 0) )
        {
            if ( str.GetLength() > 4 )
            {
                str.Delete(str.GetLength() - 4);
            }
            else
            {
                str.Delete(0);
            }
            shrink = GetStringWidth(str) > (rect.Width()-4);
        }
        if ( str.GetLength() < 4)
        {
            str.Insert(0," ");
        }
        memDC.TextOut(rect.left, rect.top, str );
        for (int i = 1; i < columnCount; i++)
        {
            rect.left = columnWidth + 4;
            rect.right = columnWidth + GetColumnWidth(i);
            str = GetItemText(nItem, i);
            shrink = GetStringWidth(str) > (rect.Width()-8);
            if ( shrink )
            {
                str += "...";
            }
            while ( shrink && (str.GetLength() > 0) )
            {
                if ( str.GetLength() > 5 )
                {
                    str.Delete(str.GetLength() - 4);
                    shrink = GetStringWidth(str) > (rect.Width()-8);
                }
                else if ( str.GetLength() == 5 )
                {
                    str.Delete(str.GetLength() - 4);
                    shrink = GetStringWidth(str) > (rect.Width());
                }
                else
                {
                    str.Delete(str.GetLength() - 1);
                    shrink = GetStringWidth(str) > (rect.Width()-1);
                }

            }
            memDC.TextOut(rect.left, rect.top, str );
            columnWidth = rect.right;
        }
        memDC.SelectObject( pOldFont );
        memDC.SelectObject( pOldMemDCBitmap );


        // Create imagelist
        CImageList* pImageList = new CImageList;
        pImageList->Create(stdRect.Width(), stdRect.Height(),
         ILC_COLOR | ILC_MASK, 0, 1);
        pImageList->Add(&bitmap, GREY_COLOUR); // mask

        delete std;

        FUNCTION_EXIT;
        return pImageList;
    }


    void VideoSourcesListCtrl::OnSetFocus(CWnd* pOldWnd)
    {
        FUNCTION_ENTRY( "OnSetFocus" );

        ListCtrlSelNoFocus::OnSetFocus(pOldWnd);
        int mark = GetSelectionMark();
        if (mark != -1)
        {
            ::SendMessage(AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_LBUTTONDOWN, -1, -1);
            SetItemState(mark, LVIS_SELECTED, LVIS_SELECTED);
        }

        FUNCTION_EXIT;
    }


    void VideoSourcesListCtrl::OnKillFocus(CWnd* pNewWnd)
    {
        FUNCTION_ENTRY( "OnKillFocus" );

        ListCtrlSelNoFocus::OnKillFocus(pNewWnd);

        SetItemState(-1, 0, LVIS_SELECTED);

        FUNCTION_EXIT;
    }


    void VideoSourcesListCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
    {
        FUNCTION_ENTRY( "OnContextMenu" );

        // Invoke the context menu functionality on the main dialog.
        ScreenToClient( &point );
        int hit = HitTest( point, NULL );
        if ( hit != -1 )
        {
            ClientToScreen( &point );
            LPARAM lParam = 0;
            lParam = ( ( point.x ) + (point.y << 16 ) );
            ::SendMessage( AfxGetApp()->m_pMainWnd->GetSafeHwnd(), WM_CONTEXTMENU, MK_RBUTTON, lParam );
        }

        FUNCTION_EXIT;
    }


    int VideoSourcesListCtrl::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
    {
        FUNCTION_ENTRY( "OnMouseActivate" );

        SetFocus();

        FUNCTION_EXIT;
        return CListCtrl::OnMouseActivate(pDesktopWnd, nHitTest, message);
    }


    BOOL VideoSourcesListCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
    {
        FUNCTION_ENTRY( "OnMouseWheel" );

        int mark = GetSelectionMark();
        if (mark != -1)
        {
            SetItemState(mark, LVIS_SELECTED, LVIS_SELECTED);
        }

        FUNCTION_EXIT;
        return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
    }


    void VideoSourcesListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY( "OnLButtonDown" );

        unsigned long locationKey = LocationCache::getInstance().getLocationKey();

        TA_Base_Core::ILocation::ELocationType locationType =
            LocationCache::getInstance().getLocationTypeByKey(locationKey);

        int index = HitTest(point);
        if (index == -1)
        {
            SetFocus();

            FUNCTION_EXIT;
            return;
        }
        CListCtrl::OnLButtonDown(nFlags, point);

        FUNCTION_EXIT;
    }


    void VideoSourcesListCtrl::OnRButtonDown(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY( "OnRButtonDown" );

        int index = HitTest(point);

        if (index == -1)
        {
            SetFocus();

            FUNCTION_EXIT;
            return;
        }
        CListCtrl::OnRButtonDown(nFlags, point);

        FUNCTION_EXIT;
    }


    void VideoSourcesListCtrl::OnLButtonDblClk(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY( "OnLButtonDblClk" );

        int index = HitTest(point);
        if (index == -1)
        {
            SetFocus();

            FUNCTION_EXIT;
            return;
        }
        CListCtrl::OnLButtonDblClk(nFlags, point);

        FUNCTION_EXIT;
    }


    void VideoSourcesListCtrl::OnRButtonDblClk(UINT nFlags, CPoint point)
    {
        FUNCTION_ENTRY( "OnRButtonDblClk" );

        int index = HitTest(point);
        if (index == -1)
        {
            SetFocus();

            FUNCTION_EXIT;
            return;
        }
        CListCtrl::OnRButtonDblClk(nFlags, point);

        FUNCTION_EXIT;
    }


    void VideoSourcesListCtrl::onItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
    {
        FUNCTION_ENTRY( "onItemChanged" );

        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        // Only interested in state changes
        if (pNMListView->uChanged == LVIF_STATE)
        {
            handleSelectionChanged();
        }

        *pResult = 0;

        FUNCTION_EXIT;
    }


    void VideoSourcesListCtrl::handleSelectionChanged()
    {
        FUNCTION_ENTRY( "handleSelectionChanged" );

        if (m_pDetailsEdit != NULL)
        {
            int selectedItem = GetNextItem(-1, LVNI_SELECTED);

            if (selectedItem > -1)
            {
                // update the details edit with details

                // get the item data
              /*  TrainDetails* itemData = reinterpret_cast<TrainDetails*>(GetItemData(selectedItem));
                TA_ASSERT (itemData != NULL, "Entry has NULL item data");
                BvsStage* stage =
                    DisplayItemManager::getInstance().getBvsStageForLocation(itemData->currentLocation);

                // now check if this item is active
                if ( (stage != NULL) && (stage->getActiveTrain() == itemData->trainID))
                {
                    m_pDetailsEdit->SetWindowText(stage->getDetails().c_str());
                }
                else
                {
                    // inactive train
                    CString details("Train: ");
                    details += getPvServiceString(*itemData);
                    details += "\r\nStage: ";
                    // TD16902
                    details += getStageNumber(*itemData);
                    details += "- ";
                    // TD16902
                    details += getStageString(*itemData);
                    details += "\r\nStatus: ";
               //     details += getStatusString(*itemData);

                    m_pDetailsEdit->SetWindowText(details);
                }*/
            }
            else
            {
                m_pDetailsEdit->SetWindowText("No item currently selected");
            }
        }

        FUNCTION_EXIT;
    }


 /*   void VideoSourcesListCtrl::stageUpdated(BvsStage* stage)
    {
        FUNCTION_ENTRY( "stageUpdated" );

        for (int i = 0; i < GetItemCount(); i++)
        {
            TrainDetails* itemData = reinterpret_cast<TrainDetails*>(GetItemData(i));

            TA_ASSERT (itemData != NULL, "Entry has NULL item data");

            BvsStage* matchingStage =
                DisplayItemManager::getInstance().getBvsStageForLocation(itemData->currentLocation);

            if (matchingStage == stage)
            {
                // this train is in the updated stage

                SetItemText(i, STAGE, getStageString(*itemData) );
                CString statusString = getStatusString(*itemData);
                SetItemText(i, STATUS, statusString );

                if (statusString.Compare(ACTIVE_STRING) == 0)
                {
                    setItemColour((DWORD) itemData,
                                   ACTIVE_COLOUR,
                                   I_ITEMDATA,
                                   CT_FOREGROUND);
                }
                else if (statusString.Compare(PRECONFLICT_STRING) == 0)
                {
                    setItemColour((DWORD) itemData,
                                   PRECONFLICT_COLOUR,
                                   I_ITEMDATA,
                                   CT_FOREGROUND);
                }
                else
                {
                    clearItemColour((DWORD) itemData, I_ITEMDATA);

                    unsigned long locationKey = LocationCache::getInstance().getLocationKey();
                    TA_Base_Core::ILocation::ELocationType locationType =
                        LocationCache::getInstance().getLocationTypeByKey(locationKey);

                    if ( (locationKey != itemData->currentLocation) &&
                         (locationType != TA_Base_Core::ILocation::OCC) )
                    {
                        setItemColour( (DWORD) itemData,
                                       GREY_COLOUR,
                                       I_ITEMDATA,
                                       CT_FOREGROUND );
                    }
                }
                RECT rect;
                GetClientRect(&rect);
                RedrawWindow(&rect);
            }
        }

        // update selected item if this list has focus
        if (::GetFocus() == GetSafeHwnd())
        {
            handleSelectionChanged();
        }

        FUNCTION_EXIT;
    }
*/

} // TA_IRS_App
