/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_log_viewer/src/DisplayLogListCtrl.cpp $
  * @author Adam Radics
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  *
  * This is used to display Tis display request logs.
  * The request data is stored as part of this list.
  */


#include "stdafx.h"
#include "DisplayLogListCtrl.h"
#include "PredefinedMessageHelper.h"
#include "UserMessages.h"
#include "RightsManager.h"  // TES891

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/tis_agent/src/IDisplayRequest.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestData.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

#include <ctime>
#include "boost/date_time/gregorian/gregorian.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"

using TA_Base_Core::DebugUtil;
using TA_IRS_Core::IDisplayRequest;

namespace TA_IRS_App
{

DisplayLogListCtrl::DisplayLogListCtrl()
 : m_selectionListener(NULL),
   m_loadingThread(NULL),
   m_loadProgressDisplay(NULL),
   // TES891 <<
   m_canDisplayTTISFreeText(false),
   m_canDisplayTTISPredefined(false),
   m_canDisplaySTISFreeText(false),
   m_canDisplaySTISPredefined(false),
    // >> TES891
   m_loading(false),
   m_sortInfo(10/*TD17565*/),
   m_latestIndex(-1)
{
    FUNCTION_ENTRY( "DisplayLogListCtrl" );

    m_loadingThread = new DisplayRequestLoadingThread( this );

    FUNCTION_EXIT;
}


DisplayLogListCtrl::~DisplayLogListCtrl()
{
    FUNCTION_ENTRY( "~DisplayLogListCtrl" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(DisplayLogListCtrl, ListCtrlSelNoFocus)
    // {{AFX_MSG_MAP(DisplayLogListCtrl)
    ON_WM_DESTROY()
    ON_NOTIFY_REFLECT_EX(NM_CLICK, onClick)
    ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, onItemchanged)
    ON_MESSAGE(WM_DISPLAY_REQUEST_LOAD_START, onDisplayRequestLoadStart)
    ON_MESSAGE(WM_ADD_DISPLAY_REQUEST, onAddDisplayRequest)
    ON_MESSAGE(WM_DISPLAY_REQUEST_LOAD_END, onDisplayRequestLoadEnd)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


void DisplayLogListCtrl::PreSubclassWindow()
{
    FUNCTION_ENTRY( "PreSubclassWindow" );

    CListCtrl::PreSubclassWindow();

    // format the list box
    InsertColumn(LogColumns::TIMESTAMP, "Date/Time");
    m_sortInfo.defineSortingSemantic(LogColumns::TIMESTAMP, AutoSortListCtrl::BY_CALLBACK);
    InsertColumn(LogColumns::SYSTEM, "SYS");
    m_sortInfo.defineSortingSemantic(LogColumns::SYSTEM, AutoSortListCtrl::BY_STRING);
    InsertColumn(LogColumns::SUBMITTED_BY, "Submitted By");
    m_sortInfo.defineSortingSemantic(LogColumns::SUBMITTED_BY, AutoSortListCtrl::BY_STRING);
    InsertColumn(LogColumns::TYPE, "Type");
    m_sortInfo.defineSortingSemantic(LogColumns::TYPE, AutoSortListCtrl::BY_STRING);
    InsertColumn(LogColumns::MESSAGE_CONTENT, "Message Content");
    m_sortInfo.defineSortingSemantic(LogColumns::MESSAGE_CONTENT, AutoSortListCtrl::BY_STRING);
    // hongran++ TD17502
    InsertColumn(LogColumns::STATE, "State");
    m_sortInfo.defineSortingSemantic(LogColumns::STATE, AutoSortListCtrl::BY_STRING);
    // ++hongran TD17502
    InsertColumn(LogColumns::START_TIME, "Start Date/Time");
    m_sortInfo.defineSortingSemantic(LogColumns::START_TIME, AutoSortListCtrl::BY_CALLBACK);
    InsertColumn(LogColumns::END_TIME, "End Date/Time");
    m_sortInfo.defineSortingSemantic(LogColumns::END_TIME, AutoSortListCtrl::BY_CALLBACK);
    InsertColumn(LogColumns::PRIORITY, "PRI");
    m_sortInfo.defineSortingSemantic(LogColumns::PRIORITY, AutoSortListCtrl::BY_CALLBACK);
    InsertColumn(LogColumns::RATIS_TAG, "RATIS Tag");
    m_sortInfo.defineSortingSemantic(LogColumns::RATIS_TAG, AutoSortListCtrl::BY_STRING);


    SetColumnWidth(LogColumns::TIMESTAMP, 135);
    SetColumnWidth(LogColumns::SYSTEM, 60);
    SetColumnWidth(LogColumns::SUBMITTED_BY, 120);
    SetColumnWidth(LogColumns::TYPE, 100);
    SetColumnWidth(LogColumns::MESSAGE_CONTENT, 325);
    // hongran++ TD17502
    SetColumnWidth(LogColumns::STATE, 80);
    // ++hongran TD17502
    SetColumnWidth(LogColumns::START_TIME, 140);
    SetColumnWidth(LogColumns::END_TIME, 140);
    SetColumnWidth(LogColumns::PRIORITY, 57);
    SetColumnWidth(LogColumns::RATIS_TAG, 100);

    SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT |LVS_EX_INFOTIP);//TD17907 liqiang	
	//EnableToolTips();//TD17907 liqiang++
    m_fixedHeaderCtrl.subclassHeader(GetHeaderCtrl());

    // TES891 ~ get user's rights prior to populating list for the first time <<
    RightsManager& rightsManager = RightsManager::getInstance();
    m_canDisplayTTISFreeText   = rightsManager.canDisplayTTISFreeText();
    m_canDisplayTTISPredefined = rightsManager.canDisplayTTISPredefined();
    m_canDisplaySTISFreeText   = rightsManager.canDisplaySTISFreeText();
    m_canDisplaySTISPredefined = rightsManager.canDisplaySTISPredefined();
    // >> TES891

    // sort callback for item data
    m_sortInfo.setCallback(this);
    // Sort columns based on label
    m_sortInfo.setCurrentSort(LogColumns::TIMESTAMP, AutoSortListCtrl::DESCENDING);
    // Make label column the active sorting column
    m_sortInfo.activateSort(LogColumns::TIMESTAMP);

    // load display requests
    loadAllDisplayRequests();

    FUNCTION_EXIT;
}


void DisplayLogListCtrl::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );

    CListCtrl::OnDestroy();

    // clean up objects
    cleanupDisplayRequests();

    if (m_loadingThread != NULL)
    {
        m_loadingThread->terminateAndWait();

        delete m_loadingThread;
        m_loadingThread = NULL;
    }

    FUNCTION_EXIT;
}


void DisplayLogListCtrl::setSelectionListener(IRequestSelectionListener* listener)
{
    FUNCTION_ENTRY( "setSelectionListener" );

    m_selectionListener = listener;

    FUNCTION_EXIT;
}


void DisplayLogListCtrl::setProgressDisplay(ILoadProgressDisplay* progressDisplay)
{
    FUNCTION_ENTRY( "setProgressDisplay" );

    m_loadProgressDisplay = progressDisplay;

    FUNCTION_EXIT;
}


IDisplayRequest* DisplayLogListCtrl::getSelectedRequest()
{
    FUNCTION_ENTRY( "getSelectedRequest" );

    // get the selected item
    int selected = -1;
    POSITION currSel = GetFirstSelectedItemPosition();

    // if something is selected
    if(currSel != NULL)
    {
        // get the selected index
        selected = GetNextSelectedItem(currSel);
    }

    if (selected > -1)
    {
        // get the item data for the selected item
        DWORD data = GetItemData(selected);
        IDisplayRequest* request = reinterpret_cast<IDisplayRequest*>(data);

        // it should never be NULL
        TA_ASSERT( request != NULL, "Item data for selected item is NULL");

        // return it
        FUNCTION_EXIT;
        return request;
    }

    // nothing found
    FUNCTION_EXIT;
    return NULL;
}


void DisplayLogListCtrl::loadAllDisplayRequests()
{
    FUNCTION_ENTRY( "loadAllDisplayRequests" );

    // make sure it is empty first
    // also prevents any memory leaks
    cleanupDisplayRequests();

    // start the thread to background load
    m_loadingThread->start();

    FUNCTION_EXIT;
}


void DisplayLogListCtrl::cleanupDisplayRequests()
{
    FUNCTION_ENTRY( "cleanupDisplayRequests" );

    // get the number of items in the list
    int itemCount = GetItemCount();

    for (int i = 0; i < itemCount; i++)
    {
        // get the item data for the next item
        DWORD data = GetItemData(0);

        // clean it up
        IDisplayRequest* displayRequest = reinterpret_cast<IDisplayRequest*>(data);

        TA_ASSERT(displayRequest != NULL, "Display request associated with item is null");

        delete displayRequest;
        displayRequest = NULL;

        // delete the item
        DeleteItem(0);
    }

    FUNCTION_EXIT;
}


LRESULT DisplayLogListCtrl::onDisplayRequestLoadStart(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "onDisplayRequestLoadStart" );

    // suppress any messages while loading
    UserMessages::getInstance().setMessageSuppression(true);

    m_loading = true;

    // preallocate space
    SetItemCount(wParam);

    if (m_loadProgressDisplay != NULL)
    {
        // show the progress bar
        m_loadProgressDisplay->showProgressBar(true, 0, wParam, 1, 0);
    }

    // stop the list redrawing
    // SetRedraw(FALSE);
    FUNCTION_EXIT;
    return 0;
}


LRESULT DisplayLogListCtrl::onAddDisplayRequest(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "onAddDisplayRequest" );

    // wParam is a pointer to an IDisplayRequest object
    IDisplayRequest* newRequest = reinterpret_cast<IDisplayRequest*>(wParam);

    TA_ASSERT(newRequest != NULL, "Update sent with a null request");

    // add it to the list
    addDisplayRequest(newRequest);

    if (m_loading && (m_loadProgressDisplay != NULL) )
    {
        // show the progress bar
        m_loadProgressDisplay->stepProgressBar();
    }

    FUNCTION_EXIT;
    return 0;
}


LRESULT DisplayLogListCtrl::onDisplayRequestLoadEnd(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "onDisplayRequestLoadEnd" );

    m_loading = false;

    if (m_loadProgressDisplay != NULL)
    {
        // show the progress bar
        m_loadProgressDisplay->showProgressBar(false);
    }

    // start the list redrawing
    SetRedraw(TRUE);
    Invalidate();
    UpdateWindow();

    // ok to display messages again
    UserMessages::getInstance().setMessageSuppression(false);

    FUNCTION_EXIT;
    return 0;
}


// TES891 ~ added method
void DisplayLogListCtrl::reload(const bool displayTTISFreeText,
                                const bool displayTTISPredefined,
                                const bool displaySTISFreeText,
                                const bool displaySTISPredefined)
{
    FUNCTION_ENTRY( "reload" );

    m_canDisplayTTISFreeText = displayTTISFreeText;
    m_canDisplayTTISPredefined = displayTTISPredefined;
    m_canDisplaySTISFreeText = displaySTISFreeText;
    m_canDisplaySTISPredefined = displaySTISPredefined;

    // clean up objects
    cleanupDisplayRequests();

    if (m_loadingThread != NULL)
    {
        m_loadingThread->terminateAndWait();

        delete m_loadingThread;
        m_loadingThread = NULL;
    }

    // load display requests
    m_loadingThread = new DisplayRequestLoadingThread(this);
    loadAllDisplayRequests();

    FUNCTION_EXIT;
}


bool DisplayLogListCtrl::isSelectable(int rowNumber)
{
    FUNCTION_ENTRY( "isSelectable" );
    FUNCTION_EXIT;
    return !m_loading;
}


void DisplayLogListCtrl::addDisplayRequest(IDisplayRequest* request)
{
    FUNCTION_ENTRY( "addDisplayRequest" );

    TA_ASSERT(request != NULL, "Request is null");

    int position = GetItemCount();

    position = InsertItem(position, TA_IRS_Core::DisplayRequestData::timestampToString( request->getTimestamp() ).c_str());

    // the item data is a pointer to the database object
    SetItemData( position, reinterpret_cast<DWORD>(request) );

    SetItemText(position, LogColumns::SYSTEM, request->getDestinationSystem().c_str() );

    SetItemText(position, LogColumns::SUBMITTED_BY, request->getSubmittedBy().c_str() );

    SetItemText(position, LogColumns::TYPE, TA_IRS_Core::DisplayRequestData::messageTypeToString( request->getMessageType(), request->getLibrarySection() ).c_str() );

    SetItemText(position, LogColumns::MESSAGE_CONTENT, PredefinedMessageHelper::getMessageString( request ).c_str() );

    // hongran++ TD17502
    TA_Base_Core::IfSendSuccess ifSuccess = request->getIfSendSuccess();
    if (ifSuccess == TA_Base_Core::Pass)
    {
        SetItemText(position, LogColumns::STATE, "Pass" );
    }
    else if (ifSuccess == TA_Base_Core::Fail)
    {
        SetItemText(position, LogColumns::STATE, "Fail" );
    }
    else
    {
        SetItemText(position, LogColumns::STATE, "N/A" );
    }
    // ++hongran TD17502

    // Only display the start and end time if the message is cyclic
    if (request->getMessageType() == TA_IRS_Core::IDisplayRequest::ADHOC
        ||request->getMessageType() == TA_IRS_Core::IDisplayRequest::PREDEFINED)
    {
        if (request->getPriority() > 3)
        {
            SetItemText(position, LogColumns::START_TIME, TA_IRS_Core::DisplayRequestData::startEndTimeToString( request->getStartTime() ).c_str() );

            SetItemText(position, LogColumns::END_TIME, TA_IRS_Core::DisplayRequestData::startEndTimeToString( request->getEndTime() ).c_str() );
        }
    }
    else if(request->getMessageType() == TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW
        || request->getMessageType() == TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE
        ||request->getMessageType() == TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW
        ||request->getMessageType() == TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE)
    {
        SetItemText(position, LogColumns::START_TIME, TA_IRS_Core::DisplayRequestData::startEndTimeToString( request->getStartTime() ).c_str() );

        SetItemText(position, LogColumns::END_TIME, TA_IRS_Core::DisplayRequestData::startEndTimeToString( request->getEndTime() ).c_str() );
    }

    if( request->getMessageType() != TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR &&
        request->getMessageType() != TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR)
    {
        SetItemText(position, LogColumns::PRIORITY, TA_IRS_Core::DisplayRequestData::priorityToString( request->getPriority() ).c_str() );
    }

    SetItemText(position, LogColumns::RATIS_TAG, TA_IRS_Core::DisplayRequestData::ratisTagToString( request->getRatisTag() ).c_str());

    FUNCTION_EXIT;
}


BOOL DisplayLogListCtrl::onClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "onClick" );

    LPNMITEMACTIVATE lpnmitem = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

    *pResult = 0;

    // only use on click for deselection
    POSITION currSel = GetFirstSelectedItemPosition();

    if ( (currSel == NULL) && (m_selectionListener != NULL) )
    {
        m_selectionListener->requestSelectionChanged(IRequestSelectionListener::None, false);
    }

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
    // TODO: Add your control notification handler code here
    CPoint pt(pNMListView->ptAction);

    // Select the item the user clicked on.
    UINT uFlags;
    m_latestIndex = this->HitTest(pt, &uFlags);

    if(m_latestIndex != -1)
    {
        bool isReinstatable = true;
        IRequestSelectionListener::RequestType selectedType = IRequestSelectionListener::None;
        std::vector<IDisplayRequest*> requests = getAllSelectedItems();
        if (requests.size() == 0)
        {
            FUNCTION_EXIT;
            return FALSE;
        }
        TA_ASSERT(requests.size() != 0, "Selected request has null item data");

        IDisplayRequest* lastSelected = getLastSelectItem();
        if (lastSelected == NULL)
        {
            FUNCTION_EXIT;
            return FALSE;
        }

        std::string messageLibrary = lastSelected->getDestinationSystem();
        TA_IRS_Core::IDisplayRequest::EMessageType messageType =
            static_cast<TA_IRS_Core::IDisplayRequest::EMessageType>( lastSelected->getMessageType());

        if (messageLibrary == "TTIS")
        {
            switch(messageType)
            {
            case TA_IRS_Core::IDisplayRequest::PREDEFINED:
                selectedType = IRequestSelectionListener::TtisPredefined;
                break;
            case TA_IRS_Core::IDisplayRequest::ADHOC:
                selectedType = IRequestSelectionListener::TtisFreeText;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW:
                selectedType = IRequestSelectionListener::Ratis_In_New;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW:
                selectedType = IRequestSelectionListener::Ratis_Out_New;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE:
                selectedType = IRequestSelectionListener::Ratis_In_Update;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE:
                selectedType = IRequestSelectionListener::Ratis_Out_Update;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR:
                selectedType = IRequestSelectionListener::Ratis_In_Clear;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR:
                selectedType = IRequestSelectionListener::Ratis_Out_Clear;
                break;
            default:
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Invalid message type for TTIS %d", messageType );
            }
        }
        else if (messageLibrary == "STIS")
        {
            switch(messageType)
            {
            case TA_IRS_Core::IDisplayRequest::PREDEFINED:
                selectedType = IRequestSelectionListener::StisPredefined;
                break;
            case TA_IRS_Core::IDisplayRequest::ADHOC:
                selectedType = IRequestSelectionListener::StisFreeText;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW:
                selectedType = IRequestSelectionListener::Ratis_In_New;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW:
                selectedType = IRequestSelectionListener::Ratis_Out_New;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE:
                selectedType = IRequestSelectionListener::Ratis_In_Update;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE:
                selectedType = IRequestSelectionListener::Ratis_Out_Update;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR:
                selectedType = IRequestSelectionListener::Ratis_In_Clear;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR:
                selectedType = IRequestSelectionListener::Ratis_Out_Clear;
                break;
            default:
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Invalid message type for STIS %d", messageType );
            }
        }
        else
        {
            switch(messageType)
            {
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW:
                selectedType = IRequestSelectionListener::Ratis_In_New;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW:
                selectedType = IRequestSelectionListener::Ratis_Out_New;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE:
                selectedType = IRequestSelectionListener::Ratis_In_Update;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE:
                selectedType = IRequestSelectionListener::Ratis_Out_Update;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR:
                selectedType = IRequestSelectionListener::Ratis_In_Clear;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR:
                selectedType = IRequestSelectionListener::Ratis_Out_Clear;
                break;
            default:
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Invalid message type for STIS %d", messageType );
            }
        }

        if (requests.size() == 1)
        {
            std::vector<IDisplayRequest*>::iterator firstIter = requests.begin();
            int priority = (*firstIter)->getPriority();
            std::string endTimeString = (*firstIter)->getEndTime();
            TA_IRS_Core::IDisplayRequest::EMessageType msgType =
                static_cast<TA_IRS_Core::IDisplayRequest::EMessageType>( (*firstIter)->getMessageType());
            if(msgType == TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW
                || msgType == TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE
                || msgType == TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR )
            {
                isReinstatable = false;
            }
            else if( msgType == TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR )
            {
                isReinstatable = true;
            }
            else if( msgType == TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE
                || msgType == TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW)
            {
                isReinstatable = isStisMessageReinstatable(endTimeString);
            }
            else
            {
                if((*firstIter)->getContinuous())
                {
                    isReinstatable = true;
                }
                else if (priority < 4)
                {
                    isReinstatable = true;
                }
                else
                {
                    if ((*firstIter)->getDestinationSystem() ==  "TTIS") {
                        isReinstatable = isTtisMessageReinstatable(endTimeString);
                    }
                    else if ((*firstIter)->getDestinationSystem() ==  "STIS") {
                        isReinstatable = isStisMessageReinstatable(endTimeString);
                    }
                    else
                        isReinstatable = true;
                }
            }
            m_selectionListener->requestSelectionChanged(selectedType, isReinstatable);
        }
        else
        {
            std::vector<IDisplayRequest*>::iterator iter = requests.begin();
            for (; iter != requests.end(); ++iter)
            {
                std::string endTimeString = (*iter)->getEndTime();
                int priority = (*iter)->getPriority();
                TA_IRS_Core::IDisplayRequest::EMessageType msgType =
                    static_cast<TA_IRS_Core::IDisplayRequest::EMessageType>( (*iter)->getMessageType());

                if(msgType == TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW
                    || msgType == TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE
                    || msgType == TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR )
                {
                    isReinstatable = false;
                    break;
                }
                else if( msgType == TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR )
                {
                    isReinstatable = true;
                }
                else if( msgType == TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE
                    || msgType == TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW)
                {
                    isReinstatable = isStisMessageReinstatable(endTimeString);
                    if(!isReinstatable)
                    {
                        break;
                    }

                }
                else
                {
                    if((*iter)->getContinuous())
                    {
                        isReinstatable = true;
                    }
                    else if (priority < 4)
                    {
                        isReinstatable = true;
                    }
                    else
                    {
                        if ((*iter)->getDestinationSystem() ==  "TTIS") {
                            isReinstatable = isTtisMessageReinstatable(endTimeString);
                        }
                        else if ((*iter)->getDestinationSystem() ==  "STIS") {
                            isReinstatable = isStisMessageReinstatable(endTimeString);
                        }
                        else
                            isReinstatable = true;

                        if(!isReinstatable)
                        {
                            break;
                        }

                    }
                }
            }
            m_selectionListener->requestSelectionChanged(selectedType, isReinstatable);
        }
    }

    FUNCTION_EXIT;
    return FALSE;
}


BOOL DisplayLogListCtrl::onItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "onItemchanged" );

    // TODO: Add your control notification handler code here

    // if the parent list blocked the message
    if (ListCtrlSelNoFocus::OnItemchanged(pNMHDR, pResult) == TRUE)
    {
        // dont handle it
        FUNCTION_EXIT;
        return TRUE;
    }

    NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

    *pResult = 0;

    m_latestIndex = pNMListView->iItem;

    // Only interested in state changes (ie selection)
    if (pNMListView->uChanged != LVIF_STATE)
    {
        FUNCTION_EXIT;
        return FALSE;
    }

    // if it is being selected
    if ( (m_selectionListener != NULL) &&
         ((pNMListView->uNewState & LVIS_SELECTED ) == LVIS_SELECTED) &&
         ((pNMListView->uOldState & LVIS_SELECTED ) == 0) )
    {
        // get the item
//        DWORD data = GetItemData(pNMListView->iItem);
//        IDisplayRequest* request = reinterpret_cast<IDisplayRequest*>(data);
        bool isReinstatable = true;
        IRequestSelectionListener::RequestType selectedType = IRequestSelectionListener::None;
        std::vector<IDisplayRequest*> requests = getAllSelectedItems();
        if (requests.size() == 0)
        {
            FUNCTION_EXIT;
            return FALSE;
        }
        TA_ASSERT(requests.size() != 0, "Selected request has null item data");

        IDisplayRequest* lastSelected = getLastSelectItem();
        if (lastSelected == NULL)
        {
            FUNCTION_EXIT;
            return FALSE;
        }

        std::string messageLibrary = lastSelected->getDestinationSystem();
        TA_IRS_Core::IDisplayRequest::EMessageType messageType =
            static_cast<TA_IRS_Core::IDisplayRequest::EMessageType>( lastSelected->getMessageType());

        if (messageLibrary == "TTIS")
        {
            switch(messageType)
            {
            case TA_IRS_Core::IDisplayRequest::PREDEFINED:
                selectedType = IRequestSelectionListener::TtisPredefined;
                break;
            case TA_IRS_Core::IDisplayRequest::ADHOC:
                selectedType = IRequestSelectionListener::TtisFreeText;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW:
                selectedType = IRequestSelectionListener::Ratis_In_New;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW:
                selectedType = IRequestSelectionListener::Ratis_Out_New;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE:
                selectedType = IRequestSelectionListener::Ratis_In_Update;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE:
                selectedType = IRequestSelectionListener::Ratis_Out_Update;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR:
                selectedType = IRequestSelectionListener::Ratis_In_Clear;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR:
                selectedType = IRequestSelectionListener::Ratis_Out_Clear;
                break;
            default:
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Invalid message type for TTIS %d", messageType );
            }
        }
        else if (messageLibrary == "STIS")
        {
            switch(messageType)
            {
            case TA_IRS_Core::IDisplayRequest::PREDEFINED:
                selectedType = IRequestSelectionListener::StisPredefined;
                break;
            case TA_IRS_Core::IDisplayRequest::ADHOC:
                selectedType = IRequestSelectionListener::StisFreeText;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW:
                selectedType = IRequestSelectionListener::Ratis_In_New;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW:
                selectedType = IRequestSelectionListener::Ratis_Out_New;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE:
                selectedType = IRequestSelectionListener::Ratis_In_Update;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE:
                selectedType = IRequestSelectionListener::Ratis_Out_Update;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR:
                selectedType = IRequestSelectionListener::Ratis_In_Clear;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR:
                selectedType = IRequestSelectionListener::Ratis_Out_Clear;
                break;
            default:
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Invalid message type for STIS %d", messageType );
            }
        }
        else
        {
            switch(messageType)
            {
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW:
                selectedType = IRequestSelectionListener::Ratis_In_New;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW:
                selectedType = IRequestSelectionListener::Ratis_Out_New;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE:
                selectedType = IRequestSelectionListener::Ratis_In_Update;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE:
                selectedType = IRequestSelectionListener::Ratis_Out_Update;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR:
                selectedType = IRequestSelectionListener::Ratis_In_Clear;
                break;
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR:
                selectedType = IRequestSelectionListener::Ratis_Out_Clear;
                break;
            default:
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Invalid message type for STIS %d", messageType );
            }
        }

        if (requests.size() == 1)
        {
            std::vector<IDisplayRequest*>::iterator firstIter = requests.begin();
            int priority = (*firstIter)->getPriority();
            std::string endTimeString = (*firstIter)->getEndTime();
            TA_IRS_Core::IDisplayRequest::EMessageType msgType =
                static_cast<TA_IRS_Core::IDisplayRequest::EMessageType>( (*firstIter)->getMessageType());
            if(msgType == TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW
                || msgType == TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE
                || msgType == TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR )
            {
                isReinstatable = false;
            }
            else if( msgType == TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR )
            {
                isReinstatable = true;
            }
            else if( msgType == TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE
                || msgType == TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW)
            {
                isReinstatable = isStisMessageReinstatable(endTimeString);
            }
            else
            {
                if((*firstIter)->getContinuous())
                {
                    isReinstatable = true;
                }
                else if (priority < 4)
                {
                    isReinstatable = true;
                }
                else
                {
                    if ((*firstIter)->getDestinationSystem() ==  "TTIS") {
                        isReinstatable = isTtisMessageReinstatable(endTimeString);
                    }
                    else if ((*firstIter)->getDestinationSystem() ==  "STIS") {
                        isReinstatable = isStisMessageReinstatable(endTimeString);
                    }
                    else
                        isReinstatable = true;
                }
            }
            m_selectionListener->requestSelectionChanged(selectedType, isReinstatable);
        }
        else
        {
            std::vector<IDisplayRequest*>::iterator iter = requests.begin();
            for (; iter != requests.end(); ++iter)
            {
                std::string endTimeString = (*iter)->getEndTime();
                int priority = (*iter)->getPriority();
                TA_IRS_Core::IDisplayRequest::EMessageType msgType =
                    static_cast<TA_IRS_Core::IDisplayRequest::EMessageType>( (*iter)->getMessageType());

                if(msgType == TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW
                    || msgType == TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE
                    || msgType == TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR )
                {
                    isReinstatable = false;
                }
                else if( msgType == TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR )
                {
                    isReinstatable = true;
                }
                else if( msgType == TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE
                    || msgType == TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW)
                {
                    isReinstatable = isStisMessageReinstatable(endTimeString);
                    if(!isReinstatable)
                    {
                        break;
                    }

                }
                else
                {
                    if((*iter)->getContinuous())
                    {
                        isReinstatable = true;
                    }
                    else if (priority < 4)
                    {
                        isReinstatable = true;
                    }
                    else
                    {
                        if ((*iter)->getDestinationSystem() ==  "TTIS") {
                            isReinstatable = isTtisMessageReinstatable(endTimeString);
                        }
                        else if ((*iter)->getDestinationSystem() ==  "STIS") {
                            isReinstatable = isStisMessageReinstatable(endTimeString);
                        }
                        else
                            isReinstatable = true;

                        if(!isReinstatable)
                        {
                            break;
                        }

                    }
                }
            }
            m_selectionListener->requestSelectionChanged(selectedType, isReinstatable);
        }
    }

    FUNCTION_EXIT;
    return FALSE;
}


LRESULT DisplayLogListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "WindowProc" );

    AutoSortListCtrl::WindowProc(message, wParam, lParam, *this, m_sortInfo);

    FUNCTION_EXIT;
    return ListCtrlSelNoFocus::WindowProc(message, wParam, lParam);
}


int DisplayLogListCtrl::sortCallback(LPARAM lParam1, LPARAM lParam2, int columnIndex, AutoSortListCtrl::ECurrentSort currentSort)
{
    FUNCTION_ENTRY( "sortCallback" );

    IDisplayRequest* first = NULL;
    IDisplayRequest* second = NULL;

    if (currentSort == AutoSortListCtrl::DESCENDING)
    {
        first = reinterpret_cast<IDisplayRequest*>(lParam2);
        second = reinterpret_cast<IDisplayRequest*>(lParam1);
    }
    else
    {
        // ascending or unknown - which will default to ascending
        first = reinterpret_cast<IDisplayRequest*>(lParam1);
        second = reinterpret_cast<IDisplayRequest*>(lParam2);
    }

    TA_ASSERT(first != NULL, "Request in list is null");
    TA_ASSERT(second != NULL, "Request in list is null");

    int result = 0;

    if (columnIndex == LogColumns::TIMESTAMP)
    {
        result = first->getTimestamp().compare(second->getTimestamp());
    }
    else if (columnIndex == LogColumns::START_TIME)
    {
        int firstPriority = first->getPriority();
        int secondPriority = second->getPriority();

        if ( firstPriority > 3 && secondPriority > 3)
        {
            result = first->getStartTime().compare(second->getStartTime());
        }
        else if (firstPriority > 3 && secondPriority <= 3)
        {
            result = -1;
        }
        else if (firstPriority <= 3 && secondPriority >= 3)
        {
            result = 1;
        }
        else if (firstPriority <= 3 && secondPriority <= 3)
        {
            result = 0;
        }
    }
    else if (columnIndex == LogColumns::END_TIME)
    {
        int firstPriority = first->getPriority();
        int secondPriority = second->getPriority();

        if ( firstPriority > 3 && secondPriority > 3)
        {
            result = first->getEndTime().compare(second->getEndTime());
        }
        else if (firstPriority > 3 && secondPriority <= 3)
        {
            result = -1;
        }
        else if (firstPriority <= 3 && secondPriority >= 3)
        {
            result = 1;
        }
        else if (firstPriority <= 3 && secondPriority <= 3)
        {
            result = 0;
        }
    }
    else if (columnIndex == LogColumns::PRIORITY)
    {
        result = first->getPriority() - second->getPriority();
    }
    else
    {
        TA_ASSERT(false, "Unsupported column given to sort by callback");
    }


    // secondary sort by timestamp (descending)
    if (result == 0)
    {
        first = reinterpret_cast<IDisplayRequest*>(lParam2);
        second = reinterpret_cast<IDisplayRequest*>(lParam1);

        result = second->getTimestamp().compare(first->getTimestamp());
    }

    FUNCTION_EXIT;
    return result;
}


bool DisplayLogListCtrl::isTtisMessageReinstatable(const std::string& endTime)
{
    FUNCTION_ENTRY( "isTtisMessageReinstatable" );

    // Convert end time into hours and minutes.
    std::istringstream timeStream(endTime);
    int timeInt;
    timeStream >> timeInt;
    int endTimeHours = timeInt / 100;
    int endTimeMins = timeInt % 100;

    // Get current time
    time_t t = time(NULL);
    struct tm* currentTime = localtime(&t);
    int currentTimeHours = currentTime->tm_hour;
    int currentTimeMins = currentTime->tm_min;

    FUNCTION_EXIT;
    return (currentTimeHours < endTimeHours
        || (currentTimeHours == endTimeHours && currentTimeMins < endTimeMins));
}


bool DisplayLogListCtrl::isStisMessageReinstatable(const std::string& endDateTime)
{
    FUNCTION_ENTRY( "isStisMessageReinstatable" );

    // Convert end date/time into boost ptimes.

    std::string delimitedEndDateTime(endDateTime);
    delimitedEndDateTime.insert(8, "T"); // Delimiter between date and time required by from_iso_string().

    try
    {
        boost::posix_time::ptime endTime = boost::posix_time::from_iso_string(delimitedEndDateTime);
        boost::posix_time::ptime currentTime(boost::posix_time::second_clock::local_time());

        FUNCTION_EXIT;
        return (currentTime < endTime);
    }
    catch(...)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error converting %s into a date/time", endDateTime.c_str());

        FUNCTION_EXIT;
        return false;
    }
}


IDisplayRequest* DisplayLogListCtrl::getLastSelectItem()
{
    FUNCTION_ENTRY( "getLastSelectItem" );

    // get the item data for the selected item
    if (m_latestIndex != -1)
    {
        DWORD data = GetItemData(m_latestIndex);

        FUNCTION_EXIT;
        return reinterpret_cast<IDisplayRequest*>(data);
    }
    else
    {
        FUNCTION_EXIT;
        return NULL;
    }
}


std::vector<IDisplayRequest*> DisplayLogListCtrl::getAllSelectedItems()
{
    FUNCTION_ENTRY( "getAllSelectedItems" );

    std::vector<IDisplayRequest*> displays;
    // get the selected item
    int selected = -1;
    POSITION currSel = GetFirstSelectedItemPosition();

    while(currSel != NULL)
    {
        selected = GetNextSelectedItem(currSel);
        if (selected > -1)
        {
            DWORD data = GetItemData(selected);
            IDisplayRequest* request = reinterpret_cast<IDisplayRequest*>(data);

            // it should never be NULL
            TA_ASSERT( request != NULL, "Item data for selected item is NULL");
            displays.push_back(request);
        }
    }

    FUNCTION_EXIT;
    return displays;
}


} // TA_App

