/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/TimeScheduleEntryListCtrl.cpp $
  * @author Adam Radics
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * TimeScheduleEntryListCtrl defines a list control that displays
  * train time schedule entries.
  */

#include "stdafx.h"
#include "resource.h"
#include "TimeScheduleEntryListCtrl.h"
#include "TTISPredefinedMessages.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include <algorithm>

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{


	TimeScheduleEntryListCtrl::TimeScheduleEntryListCtrl()
	 : m_editEnabled(false),
	   m_sortInfo(5),
	   m_ttisPredefinedMessages( NULL )
	{
	}


	TimeScheduleEntryListCtrl::~TimeScheduleEntryListCtrl()
	{

	    if ( NULL != m_ttisPredefinedMessages )
	    {
	        m_ttisPredefinedMessages = NULL;
	        
	        TTISPredefinedMessages::removeInstance();
	    }
	}


	BEGIN_MESSAGE_MAP(TimeScheduleEntryListCtrl, TA_Base_Bus::ListCtrlSelNoFocus)
		//{{AFX_MSG_MAP(TimeScheduleEntryListCtrl)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	void TimeScheduleEntryListCtrl::PreSubclassWindow() 
	{
		CListCtrl::PreSubclassWindow();

	    SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	    m_fixedHeaderCtrl.subclassHeader(GetHeaderCtrl());
	   
		InsertColumn(START_TIME,"Start Time");
		SetColumnWidth(START_TIME,80);
	    m_sortInfo.defineSortingSemantic(START_TIME, AutoSortListCtrl::BY_STRING);

	    InsertColumn(END_TIME,"End Time");
		SetColumnWidth(END_TIME,75);
	    m_sortInfo.defineSortingSemantic(END_TIME, AutoSortListCtrl::BY_STRING);

	    InsertColumn(MESSAGE,"Message");
		SetColumnWidth(MESSAGE,263);
	    m_sortInfo.defineSortingSemantic(MESSAGE, AutoSortListCtrl::BY_STRING);

	    InsertColumn(REPEAT_INTERVAL,"Repeat Interval");
		SetColumnWidth(REPEAT_INTERVAL,110);
	    m_sortInfo.defineSortingSemantic(REPEAT_INTERVAL, AutoSortListCtrl::BY_STRING);

	    InsertColumn(PRIORITY,"Priority");
		SetColumnWidth(PRIORITY,62);
	    m_sortInfo.defineSortingSemantic(PRIORITY, AutoSortListCtrl::BY_INTEGER);

	    // Sort columns based on label
	    m_sortInfo.setCurrentSort(START_TIME, AutoSortListCtrl::ASCENDING);
	    // Make label column the active sorting column
	    m_sortInfo.activateSort(START_TIME);

	    setScrollBarVisibility(true, false);

	    if ( NULL == m_ttisPredefinedMessages )
	    {
	        m_ttisPredefinedMessages = &( TTISPredefinedMessages::getInstance() );
	    }
	}


	bool TimeScheduleEntryListCtrl::isSelectable(int rowNumber)
	{
	    // items are only selectable if edit is enabled
	    return m_editEnabled;
	}


	void TimeScheduleEntryListCtrl::addTimeScheduleEntry(ITrainTimeScheduleEntry* scheduleEntry,
	                                                     ITrainTimeSchedule* owningSchedule)
	{
	    TA_ASSERT(scheduleEntry != NULL, "Schedule entry to add is null");
	    TA_ASSERT(owningSchedule != NULL, "Schedule entry to add is null");

	    // insert the item
	    int pos = GetItemCount();
	    InsertItem(pos, getTimeString(scheduleEntry->getStartTime()));
	    SetItemText(pos, END_TIME, getTimeString(scheduleEntry->getEndTime()));
	    SetItemText(pos, MESSAGE, getMessageName(scheduleEntry, owningSchedule));
	    SetItemText(pos, REPEAT_INTERVAL, getRepeatIntervalString(scheduleEntry->getRepeatInterval()));
	    SetItemText(pos, PRIORITY, getPriorityString(scheduleEntry->getPriority()));

	    // set this to be used later
	    SetItemData(pos, reinterpret_cast<DWORD>(scheduleEntry));

	    // the auto sort list will keep the list sorted
	}


	ITrainTimeScheduleEntry* TimeScheduleEntryListCtrl::removeSelectedTimeScheduleEntry()
	{
	    POSITION position = GetFirstSelectedItemPosition();

	    TA_ASSERT(position != NULL, "The remove entry button should be disabled if nothing is selected");

	    // get the item data to return
	    int nItem = GetNextSelectedItem(position);
	    ITrainTimeScheduleEntry* itemData = reinterpret_cast<ITrainTimeScheduleEntry*>(GetItemData(nItem));

	    TA_ASSERT(itemData != NULL, "Item data is null");

	    // delete the item
	    DeleteItem(nItem);

	    return itemData;
	}


	void TimeScheduleEntryListCtrl::removeAllEntries()
	{
	    DeleteAllItems();
	}


	void TimeScheduleEntryListCtrl::freeTextDescriptionUpdated(ITrainTimeSchedule::AdhocMessageNumber message,
	                                                           std::string newTitle)
	{
	    // find any entries with this free text message and update them to the new text.
	    for (int i = 0; i < GetItemCount(); i++)
	    {
	        // get the item data
	        ITrainTimeScheduleEntry* itemData = reinterpret_cast<ITrainTimeScheduleEntry*>(GetItemData(i));
	        TA_ASSERT(itemData != NULL, "Item data is null");

	        if ( (itemData->getMessageTable() == ITrainTimeScheduleEntry::FreeText) &&
	             (itemData->getMessageId() == message) )
	        {
	            SetItemText(i, MESSAGE, newTitle.c_str());
	        }
	    }
	}


	void TimeScheduleEntryListCtrl::predefinedMessagesChanged()
	{
	    for (int i = 0; i < GetItemCount(); i++)
	    {
	        // get the item data
	        ITrainTimeScheduleEntry* itemData = reinterpret_cast<ITrainTimeScheduleEntry*>(GetItemData(i));
	        TA_ASSERT(itemData != NULL, "Item data is null");

	        if (itemData->getMessageTable() == ITrainTimeScheduleEntry::NormalPreDefined)
	        {
	            SetItemText(i, MESSAGE, getMessageName(itemData, NULL));
	        }
	    }
	}


	void TimeScheduleEntryListCtrl::setEditMode(bool editMode)
	{
	    m_editEnabled = editMode;

	    // deselect any selected items if not in edit mode
	    if (!m_editEnabled)
	    {
	        UINT uSelectedCount = GetSelectedCount();
	        UINT i;
	        int  nItem = -1;
	    
	        if (uSelectedCount > 0)
	        {
	            // Remove all selection masks
	            for (i = 0; i < uSelectedCount;i++)
	            {
	                nItem = GetNextItem(nItem, LVNI_SELECTED);
	            
	                TA_ASSERT(nItem != -1, "MFC lies! GetSelectedCount returned the wrong number.");
	                        
	                SetItemState(nItem, ~LVIS_SELECTED, LVIS_SELECTED);
	            }
	        }
	    }

	    // force a redraw to grey/ungrey items
	    RedrawItems(0, GetItemCount() - 1);
	}


	// for sorting
	LRESULT TimeScheduleEntryListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
	{     
	    AutoSortListCtrl::WindowProc(message, wParam, lParam, *this, m_sortInfo);

	    return ListCtrlSelNoFocus::WindowProc(message, wParam, lParam);
	}


	CString TimeScheduleEntryListCtrl::getTimeString(const time_t& time)
	{
	    CTime temp(time);

	    return temp.Format("%H:%M");
	}


	CString TimeScheduleEntryListCtrl::getMessageName(ITrainTimeScheduleEntry* scheduleEntry,
	                                                  ITrainTimeSchedule* owningSchedule)
	{
	    switch (scheduleEntry->getMessageTable())
	    {
	    case ITrainTimeScheduleEntry::FreeText:
	        {
	            // this will be null if the pre-defined message library was updated
	            // and the free text messages havent changed.

				TA_ASSERT(owningSchedule!=NULL, "Schedule Entry does not belong to a schedule");

	            ITrainTimeSchedule::AdhocMessageNumber messageId =
	                static_cast<ITrainTimeSchedule::AdhocMessageNumber>(scheduleEntry->getMessageId());

	            if (messageId > ITrainTimeSchedule::Adhoc6)
	            {
	                return CString("Invalid free text message");
	            }
	            else
	            {
	                return CString(
	                    owningSchedule->getAdHocMessageTitle(messageId).c_str() );
	            }
	        }
	        break;
	    case ITrainTimeScheduleEntry::NormalPreDefined:
	        {
	            const TA_Base_Core::PredefinedMessage* message = 
	                m_ttisPredefinedMessages->getNormalMessageById(scheduleEntry->getMessageId());

	            if (message != NULL)
	            {
	                return CString(message->description.c_str());
	            }
	            else
	            {
	                return CString("Invalid pre defined message");
	            }
	        }
	        break;
	    case ITrainTimeScheduleEntry::EmergencyPreDefined:
	    default:
	        TA_ASSERT(false, "Invalid message table");
			return CString("Invalid message table");
	        break;
	    }
	}


	CString TimeScheduleEntryListCtrl::getRepeatIntervalString(int repeatInterval)
	{
	    // convert repeat interval from just minutes into hours and minutes
	    int hours = repeatInterval / 60;
	    int minutes = repeatInterval % 60;

	    CString temp;
	    // HH:MM
	    temp.Format("%02d:%02d", hours, minutes);

	    return temp;
	}


	CString TimeScheduleEntryListCtrl::getPriorityString(unsigned short priority)
	{
	    CString temp;
	    temp.Format("%d", priority);
	    return temp;
	}


} // TA_IRS_App

