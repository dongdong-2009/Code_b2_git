/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingGridControl.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is a wrapper around the CGridCtrl (from mfc_extensions) to modify it to display and allow
  * configuration of MmsScheduling.
  */


#pragma warning(disable:4786 4284 4250)

#include <vector>

#include "app/configuration/config_plugins/mms_scheduling/src/stdafx.h"
#include "app/configuration/config_plugins/mms_scheduling/src/CachedConfig.h"
#include "app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingGridControl.h"
#include "app/configuration/config_plugins/mms_scheduling/src/RightsChangedWindowNotificationHandler.h"
#include "app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingDatabaseAccessor.h"
#include "app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingComponent.h"
#include "app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingView.h"
#include "app/configuration/config_plugins/mms_scheduling/src/resource.h"
#include "app/configuration/config_plugins/mms_scheduling/src/RightsChangedWindowNotificationHandler.h"
#include "core/data_access_interface/mms_dai/src/MmSchedulingAccessFactory.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/resources/resource.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/configuration_updates/src/EObjectType.h"
#include "core/configuration_updates/src/ConfigUpdateDetails.h"
#include "core/synchronisation/src/ThreadGuard.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/data_access_interface/mms_dai/src/IConfigMmScheduling.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Core; using namespace TA_IRS_Core;
using  namespace TA_Base_App::TA_Configuration;
using TA_Base_Core::RunParams;

namespace TA_IRS_App
{
    // 01:15 PM - we don't want seconds
    static const char dtc_timeOfDayFormat[] = _T("hh:mm tt");  
    // Provide the ctime equivalent formatting as used for the time of day.. 
    static const char ctime_timeOfDayFormat[] = _T("%I:%M %p");

    // return in the form: DD/MM/YYYY 11:30:00 AM
    // Define datetime format for last reported column - this date format
    // taken from the PA Manager, except displays in 12 hour format
    // (changes H -> I, where I is 12 hour format, and addition of %p)
    static const char ctime_dateTimeFormat[] = _T("%d/%m/%Y %I:%M:%S %p");

    MmsSchedulingGridControl::MmsSchedulingGridControl() 
    : 
    m_databaseAccessor(NULL), 
    m_inPlaceDt(NULL),
    m_rightsChangeHandler(NULL)
    {
    }


    MmsSchedulingGridControl::~MmsSchedulingGridControl()
    {
    }


BEGIN_MESSAGE_MAP(MmsSchedulingGridControl, CGridCtrl)
	//{{AFX_MSG_MAP(MmsSchedulingGridControl)
    ON_MESSAGE(WM_MMS_RIGHTS_CHANGE, OnRightsChanged)
    ON_MESSAGE(WM_MMS_SCHEDULE_CHANGE, OnScheduleDataChanged)
    ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void MmsSchedulingGridControl::PreSubclassWindow() 
    {
	    SetEditable(TRUE);
        SetDoubleBuffering(TRUE);
	    EnableDragAndDrop(FALSE);

        SetFixedRowCount(1);    // This is the column detail for each record 
        SetFixedColumnCount(1); // This is the row name for schedule record		

        SetColumnCount(COL_MAX);
        SetColumnWidth(COL_SUBSYSTEM_NAME, 200);
        SetColumnWidth(COL_SCHEDULED_TIME, 200);
        SetColumnWidth(COL_LAST_REPORTED, 200);

        // Set the background colour of the control so we cannot see where the background if the columns and rows are
        // smaller than the control
        SetBkColor( GetSysColor(COLOR_3DFACE) );

        /*
        m_profileIcons.Create(SMALL_ICON_SIZE,SMALL_ICON_SIZE,ILC_COLOR32,2,2);
        m_profileIcons.Add( LoadIcon(GetModuleHandle(MmsSchedulingComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_CENTRAL_PROFILE)) );
        m_profileIcons.Add( LoadIcon(GetModuleHandle(MmsSchedulingComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_LOCAL_PROFILE)) );

        SetImageList( &m_profileIcons );
        */

        CGridCtrl::PreSubclassWindow();

        try
        {
            // Begin listening out for changes in rights, in which case need to adjust
            // the read only states of the relevant cells
            if (0 == m_rightsChangeHandler)
            {
                m_rightsChangeHandler =  new RightsChangedWindowNotificationHandler(m_hWnd);
            }
    
            // Listen out for changes in the MMS Scheduling data - must go via
			// the MmsScheduleAccessFactory - because online updateable interface does
			// not allow multiple listeners to attach to the same object of interest
            //OnlineUpdateListener::getInstance().registerAllInterests(TA_Core::MMS_SCHEDULE ,*this);
			TA_IRS_Core::MmSchedulingAccessFactory::getInstance().registerInterest(*this);
        }
        catch (...)
        {
        }
    }


    void MmsSchedulingGridControl::loadItems(
            MmsSchedulingDatabaseAccessor* dbAccessor,
            CProgressCtrl& progress)
    {
        FUNCTION_ENTRY("loadItems");

        TA_ASSERT(dbAccessor != NULL, "Database accessor passed was NULL")
        m_databaseAccessor = dbAccessor;

        setUpHeaders();       

        progress.OffsetPos(10);

        populateWithData();
        
        progress.OffsetPos(40);

        // Now that we've set up we have to set the font on the control
        CFont arialBold;
        arialBold.CreatePointFont(FONTSIZE_FOR_CONTROLS, TYPEFACE_FOR_CONTROLS);
		SetFont(&arialBold);

        // Now all the data is inserted we should auto-size the control to the right size for the text
	    AutoSize();

        FUNCTION_EXIT;
    }


    void MmsSchedulingGridControl::setUpHeaders()
    {
        FUNCTION_ENTRY("setUpHeaders");

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor stored is NULL");

        // Set up the standard item fields so we don't have to do it each time we insert an item
		GV_ITEM item;
        item.mask = GVIF_TEXT|GVIF_FORMAT;
		item.col = 0;
		item.nFormat = DT_LEFT|DT_WORDBREAK;

        //
        // Set up the headings across top row
        //
        item.row = 0;
        
        // Set up the headings, subsystem on far left column, then..
        item.col = 0;
        item.szText = "Subsystem";
        SetItem(&item);

        item.col = 1;
        item.szText = "Scheduled Time";
        SetItem(&item);

        item.col = 2;
        item.szText = "Last Reported";
        SetItem(&item);

        FUNCTION_EXIT;             
    }


    void MmsSchedulingGridControl::populateWithData()
    {
        FUNCTION_ENTRY("populateWithData");

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor stored is NULL");
         
        try
        {
            m_mmsSchedules = m_databaseAccessor->getAllSchedules();
    
            //
            // Set up the headings down the left-most column
            //
            SetRowCount(m_mmsSchedules.size() + 1);
    
     		GV_ITEM item;
            item.mask   = GVIF_TEXT|GVIF_FORMAT|GVIF_STATE;
            item.state  = 0;
		    item.nFormat = DT_LEFT|DT_WORDBREAK;
            // We ignore the first row (0) because it is the corner cell
            int row = 1;
    
            for (ScheduleMap::iterator it = m_mmsSchedules.begin();
                 it != m_mmsSchedules.end(); ++it )
            {
    			item.row = row;
    
                // For each column, fill out the details
                for (int col = 0; col < COL_MAX; ++col )
                {
                    CString text = getCellText(col, it->second);

                    switch (col)
                    {
                    case COL_SUBSYSTEM_NAME:
                        item.state |= GVIS_READONLY;
                        break;
                    case COL_SCHEDULED_TIME:                        
                        // This is the only editable column - provided we have the necessary rights!
                        if (CachedConfig::getInstance().isFunctionPermitted(TA_Base_Bus::aca_CE_CONFIGURE_MMS_SCHEDULING))
                        {
                            // Remove read only flag, has right to edit this field
                            item.state &= ~GVIS_READONLY;
                        }
                        else
                        {
                            item.state |= GVIS_READONLY;
                        }                        
                        break;
                    case COL_LAST_REPORTED:    
                        item.state |= GVIS_READONLY;
                        break;                    
                    }
                    
                    item.szText = text.GetBuffer(0);
                    item.col = col;
        			SetItem(&item);
        
                    // Apply the key value as item data for this cell
                    SetItemData(item.row, item.col, it->first);    
                }
    
                // No images currently        
                // SetItemImage(row, 0, profileIter->second.typeKey);
    
                ++row;
            }
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
			// TD14164 ++
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030072);
			// ++ TD14164 

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            //UE-030001
			// TD14164 ++
            /*MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_030001);
			// TD14164 ++

            FUNCTION_EXIT;
            return;
        }

        // Sort the first column so that all the subsystems are in alphabetical order.
        SortTextItems(0,TRUE);
          
        FUNCTION_EXIT;
    }


    bool MmsSchedulingGridControl::isConfigurationValid(std::vector<unsigned long>& subsystems)
    {
        FUNCTION_ENTRY("isConfigurationValid");
        
        // No constraints applied to grid view
        return true;
        FUNCTION_EXIT;
    }


    void MmsSchedulingGridControl::OnEndEditCell(int nRow, int nCol, CString str)
    {
        FUNCTION_ENTRY("OnEndEditCell");
        
        if (0 == m_inPlaceDt)
        {
            // The in place edit control should be valid, as it should
            // be the only object posting notifications to the grid class
            // which in turn calls this function..
            return;
        }
        
        try
        {
            CTime newTime;

            if (GDT_VALID != m_inPlaceDt->GetTime(newTime))
            {
                return;
            }
        
            unsigned long uniqueId = GetItemData(nRow,nCol);

            TA_ASSERT(m_databaseAccessor != NULL, "Database accessor stored is NULL");

            TA_IRS_Core::IConfigMmScheduling* item = m_databaseAccessor->getItem( uniqueId );

            if (item == NULL)
            {
                // Not sure how this happens..
                return;
            }
                       
            // Now set the new scheduled time
            item->setScheduledTime( newTime.GetTime() );
            
            // Check if item has changed    
            MmsSchedulingView* mainView = dynamic_cast<MmsSchedulingView*>(GetParent());

            if (mainView != NULL)
            {
                // We must now update the reset and apply button since the user has made a change
                mainView->refreshButtonAvailability();
            }                     
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            //UE-010001
            CString errorMsg("Database cannot be accessed by the ");
            errorMsg += RunParams::getInstance().get(RPARAM_APPNAME).c_str();
            errorMsg += ".";
            /*MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << errorMsg;
			userMsg.showMsgBox(IDS_UE_030072);
			// ++ TD14164 

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            //UE-030001
			// TD14164 ++
            /*MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_030001);
			// TD14164 ++

            FUNCTION_EXIT;
            return;
        }

        CGridCtrl::OnEndEditCell(nRow, nCol, str);

        FUNCTION_EXIT;
    }


    void MmsSchedulingGridControl::CreateInPlaceEditControl(CRect& rect, DWORD dwStyle, UINT nID,
                                                     int nRow, int nCol,
                                                     LPCTSTR szText, int nChar)
    {
        // The text defines a date/time setting in our prescribed format
        // But we ignore that and use the actual time value
        // Convert to a suitable time_t value (reverse engineer the time field)
        time_t utcTime = 0;

        if (0 != m_databaseAccessor)
        {
            unsigned long uniqueId = GetItemData(nRow,nCol);
            TA_IRS_Core::IConfigMmScheduling* item = m_databaseAccessor->getItem(uniqueId);
    
            if (0 != item)
            {
                // Use the actual time contained in the object, rather than converting back and forth
                utcTime = item->getScheduledTime();
            }
        }

	    // CInPlaceDateTime, it auto-deletes itself (store a reference to query details from it)
	    m_inPlaceDt = new CInPlaceDateTime(this, 
                                rect,
                                DTS_TIMEFORMAT | DTS_UPDOWN,
                                dtc_timeOfDayFormat,
                                nID,                   // ID of control being created
                                nRow, 
                                nCol, 
                                utcTime, 
                                nChar);        
    }


    LRESULT MmsSchedulingGridControl::OnRightsChanged(UINT wParam, LONG lParam)
    {        
        // Need to refresh the read only state of the time of day column
        bool mayConfigure = CachedConfig::getInstance().isFunctionPermitted(TA_Base_Bus::aca_CE_CONFIGURE_MMS_SCHEDULING);                                 
        
        // Only need to inspect the single editable column
        const DWORD editableColumn = COL_SCHEDULED_TIME;

        for (int i = 0; i < GetRowCount(); ++i )
        {
            DWORD state = GetItemState(i, editableColumn);

            if (mayConfigure)
            {
                // Strip the read only flag, allowed to configure this now
                state &= ~GVIS_READONLY;
            }
            else
            {
                // No longer able to edit this field
                state |= GVIS_READONLY;
            }

            SetItemState(i, editableColumn, state);
        }
        
        return 0;
    }


    void MmsSchedulingGridControl::processUpdate(const ConfigUpdateDetails& updateEvent)
    {
        // We don't really care what data changed, just refresh entire last reported column
        // (this is the only update notifications expect the MMS agent to be sending out)
        if (IsWindow(m_hWnd))
        {
            PostMessage(WM_MMS_SCHEDULE_CHANGE, 0, 0);
        }
    }


    LRESULT MmsSchedulingGridControl::OnScheduleDataChanged(UINT wParam, LONG lParam)
    {
        unsigned int oldScheduleCount = m_mmsSchedules.size();
        m_mmsSchedules = m_databaseAccessor->getAllSchedules();

        if (m_mmsSchedules.size() != oldScheduleCount)
        {
            // Perform a full refresh
            populateWithData();
        }
        else
        {
            // We ignore the first row (0) because it is the corner cell            
            // other than this only need to refresh the read only fields
			for (int row = 1; row < GetRowCount(); ++row )
			{
				// All elements in a given row should have the same item data value
				DWORD scheduleKey = GetItemData(row, 0);

				// Find which row this schedule belongs to
				ScheduleMap::iterator it = m_mmsSchedules.find(scheduleKey);

				if (it == m_mmsSchedules.end())
				{
					// Couldn't find the schedule associated with this row - full refresh required
					populateWithData();
					// Full refresh means we don't need to inspect any more rows..
					break;
				}

                // For each column, fill out the details
                for (int col = 0; col < COL_MAX; ++col )
                {
                    if (GVIS_READONLY != (GetItemState(row, col) & GVIS_READONLY))
                    {
                        // This is an editable field, don't want to adjust it - 
						// in case user has changed it to something..
                        continue;
                    }

                    CString text = getCellText(col, it->second);
        
                    // Apply the key value as item data for this cell
                    SetItemText(row, col, text.GetBuffer(0));
                }
   
            }
        }

		// Force the window to redraw (not automatic when changing text?)
		InvalidateRect(0);

        return 0;
    }

    
    CString MmsSchedulingGridControl::getCellText(int col, TA_IRS_Core::IConfigMmScheduling* schedule)
    {
        CString text = "";

        if (0 != schedule)
        {
            switch (col)
            {
            case COL_SUBSYSTEM_NAME:
                text = schedule->getSubsystemName().c_str();
                break;
            case COL_SCHEDULED_TIME:
                {
                    CTime time(schedule->getScheduledTime());
                    text = time.Format(ctime_timeOfDayFormat);
                }
                break;
            case COL_LAST_REPORTED:    
                {
                    CTime time(schedule->getLastReported());
                    text = time.Format(ctime_dateTimeFormat);
                }
                break;
            default:            
                break;
            }
        }

        return text;
    }
                    

    void MmsSchedulingGridControl::OnDestroy()
    {
        try
        {
            // OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Core::MMS_SCHEDULE, *this);
			TA_IRS_Core::MmSchedulingAccessFactory::getInstance().deregisterInterest(*this);
    
            // Stop listening out for changes in rights
            if (0 != m_rightsChangeHandler)
            {
                delete m_rightsChangeHandler;
                m_rightsChangeHandler = 0;
            }
        }
        catch (...)
        {
        }
    }

}

/*

	std::string MmsGridControl::getTimeString(const time_t* t)
	{
		//return in the form: 11:30:00 AM
		tm* timeStruct = localtime( t );

		int hours = timeStruct->tm_hour;

		std::string dayOrNite = "AM";
		if(hours > 12)
		{
			 dayOrNite = "PM";
			hours = hours - 12;
		}

		int mins = timeStruct->tm_min;

		std::ostringstream strm;
		strm << hours << ":" ;
		strm << std::right << std::setw(2) << std::setfill('0') << mins;
		strm << ":00 " << dayOrNite;

		std::string s;
		s = strm.str();
		return s;
	}

	std::string MmsGridControl::getTimeDateString(const time_t* t)
	{
		//return in the form: D/MM/YYYY 11:30:00 AM
		tm* timeStruct = localtime( t );



		int days = timeStruct->tm_mday;
		int month = timeStruct->tm_mon + 1;
		int year = timeStruct->tm_year + 1900;

		std::ostringstream strm;
		strm << days << "/";
		strm << std::right << std::setw(2) << std::setfill('0') << month;
		strm << "/" << year << " " << getTimeString(t);

		std::string s;
		s = strm.str();
		return s;
	}

	time_t MmsGridControl::stringToTime(const CString& str)
	{
		//todo
		time_t t;
		time(&t);
		return t;

	}
*/
