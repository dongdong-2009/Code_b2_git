/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/ats_dss_plan_map/src/AtsDssPlanMapListView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all DSS Plan Maps available to be configured. This view must communicate
  * with the AtsDssPlanMapDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ats_dss_plan_map/src/StdAfx.h"
#include "app/configuration/config_plugins/ats_dss_plan_map/src/AtsDssPlanMapListView.h"
#include "app/configuration/config_plugins/ats_dss_plan_map/src/AtsDssPlanMapComponent.h"
#include "app/configuration/config_plugins/ats_dss_plan_map/src/resource.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/IProgressStatusBar.h"

#include "bus/resources/resource.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164

#include "core/data_access_interface/src/IConfigItem.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
typedef TA_IRS_App::AtsDssPlanMapDatabaseAccessor::AtsDssPlanMap AtsDssPlanMap;

using namespace TA_Base_App::TA_Configuration;

namespace TA_IRS_App
{
    const int AtsDssPlanMapListView::APPLIED_COLUMN = 1;
    const int AtsDssPlanMapListView::DSSEVENT_COLUMN = APPLIED_COLUMN + 1;
    const int AtsDssPlanMapListView::PLAN_COLUMN =APPLIED_COLUMN + 2;

    const CString AtsDssPlanMapListView::APPLIED_COLUMN_NAME = "Status";
    const CString AtsDssPlanMapListView::DSSEVENT_COLUMN_NAME = "DSS Event";
    const CString AtsDssPlanMapListView::PLAN_COLUMN_NAME = "Plan";

    const int AtsDssPlanMapListView::APPLIED_COLUMN_WIDTH = 75;
    const int AtsDssPlanMapListView::DSSEVENT_COLUMN_WIDTH = 100;
    const int AtsDssPlanMapListView::PLAN_COLUMN_WIDTH = 300;

    // These are the possible values for the status of each DSS Plan Map
    const CString AtsDssPlanMapListView::NEW = "New";
    const CString AtsDssPlanMapListView::MODIFIED = "Modified";
    const CString AtsDssPlanMapListView::UPTODATE = "";

    const bool AtsDssPlanMapListView::ASCENDING = true;
    const bool AtsDssPlanMapListView::DESCENDING = false;

    IMPLEMENT_DYNCREATE(AtsDssPlanMapListView, CListView)


    AtsDssPlanMapListView::AtsDssPlanMapListView()
    :     m_sortOrder(DESCENDING),        // Set to descending so the first time we sort it switches to ASCENDING
          m_sortColumn(DSSEVENT_COLUMN)  // This is the column to sort on when the list control gets created
    {
        m_componentIcon = LoadIcon(GetModuleHandle(AtsDssPlanMapComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ATS) );
    }


    AtsDssPlanMapListView::~AtsDssPlanMapListView()
    {
    }


BEGIN_MESSAGE_MAP(AtsDssPlanMapListView, AbstractListView)
	//{{AFX_MSG_MAP(AtsDssPlanMapListView)
	ON_WM_CREATE()
    ON_WM_SIZE()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, onColumnclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    int AtsDssPlanMapListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
    {
        // Set the styles for our list control before it is created
        lpCreateStruct->style = LVS_REPORT | LVS_SHOWSELALWAYS | WS_BORDER;

	    if (CListView::OnCreate(lpCreateStruct) == -1)
		    return -1;
	    
	    return 0;
    }


    void AtsDssPlanMapListView::OnInitialUpdate() 
    {
	    CListView::OnInitialUpdate();
	    
        GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_GRIDLINES );

        // Insert the columns to display the data
        GetListCtrl().InsertColumn(0,   "",   LVCFMT_LEFT, 0 );
        GetListCtrl().InsertColumn(APPLIED_COLUMN,   APPLIED_COLUMN_NAME,   LVCFMT_LEFT, APPLIED_COLUMN_WIDTH  );
        GetListCtrl().InsertColumn(DSSEVENT_COLUMN,	 DSSEVENT_COLUMN_NAME,  LVCFMT_LEFT, DSSEVENT_COLUMN_WIDTH );
        //GetListCtrl().InsertColumn(DSSINPUT_COLUMN,  DSSINPUT_COLUMN_NAME,  LVCFMT_LEFT, DSSINPUT_COLUMN_WIDTH );
        GetListCtrl().InsertColumn(PLAN_COLUMN,      PLAN_COLUMN_NAME,      LVCFMT_LEFT, PLAN_COLUMN_WIDTH     );
    }


    void AtsDssPlanMapListView::OnSize(UINT nType, int cx, int cy) 
    {
        CListView::OnSize(nType, cx, cy);
        // Call the CListView::ONSize instead of the one in the abstract class above
    }


    void AtsDssPlanMapListView::loadAllItems()
    {
        FUNCTION_ENTRY("loadAllItems");

        TA_ASSERT(getDatabaseAccessor() != NULL, "setDatabaseAccessor() must be called before this method");
        TA_ASSERT(getDetailView() != NULL, "The detail view has not been set yet");
        TA_ASSERT(getStatusBar() != NULL, "The status bar has not been set yet");

        // Retrieve the progress bar and display it on the status bar
        CProgressCtrl& progress = getStatusBar()->displayProgressBar();
        progress.SetStep(1);
        progress.SetPos(3); // Indicate that we have started loading and use the wait cursor to do the rest.

        typedef AtsDssPlanMapDatabaseAccessor::AtsDssPlanMap AtsDssPlanMap;
        AtsDssPlanMapDatabaseAccessor* dbAccessor = dynamic_cast<AtsDssPlanMapDatabaseAccessor*>(getDatabaseAccessor());
        TA_ASSERT( dbAccessor != NULL, "DB accessor could not be cast to an DSS Plan Map one");

        m_currentDisplayedItems.clear();

        try
        {
            AfxGetApp()->DoWaitCursor(1);

            // Tell the database accessor to load all items from the database
            dbAccessor->loadItems();
            
            // Now get a list of item names and ids so we can populate our list
            // The progress bar will start moving in this method. This method also sets the range
            // on the status bar.
            m_currentDisplayedItems = dbAccessor->getAllItemDetails(progress);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
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

            getStatusBar()->hideProgressBar();
            AfxGetApp()->DoWaitCursor(-1);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            //UE-030001
			// TD14164 ++
            /*MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_030001);
			// TD14164 ++

            getStatusBar()->hideProgressBar();
            AfxGetApp()->DoWaitCursor(-1);

            FUNCTION_EXIT;
            return;
        }

        if (m_currentDisplayedItems.empty())
        {
            getStatusBar()->hideProgressBar();

            FUNCTION_EXIT;
            return;
        }


        // Set the range to be the number of entities we have then we can just step by one each time.

        // Now add all the items into our list and set their data to be their ids
        for (std::map<unsigned long,AtsDssPlanMap>::iterator iter = m_currentDisplayedItems.begin(); 
             iter != m_currentDisplayedItems.end(); ++iter)
        {
            if (dbAccessor->getItem(iter->first)->isNew())
            {
                iter->second.status = NEW;
            }
            else if (dbAccessor->getItem(iter->first)->hasChanged())
            {
                iter->second.status = MODIFIED;
            }
            else
            {
                iter->second.status = UPTODATE;
            }

            int position = GetListCtrl().InsertItem(0,"",0);
            GetListCtrl().SetItemText(position, APPLIED_COLUMN, iter->second.status);
            GetListCtrl().SetItemText(position, DSSEVENT_COLUMN, iter->second.dssEventCString);
           // GetListCtrl().SetItemText(position, DSSINPUT_COLUMN, iter->second.dssInputCString);
            GetListCtrl().SetItemText(position, PLAN_COLUMN, iter->second.plan);
            
            GetListCtrl().SetItemData(position, iter->first);
            progress.StepIt();
        }

        // Now sort the columns
        updateHeaderSort(m_sortColumn);
        GetListCtrl().SortItems(compareItems, reinterpret_cast<unsigned long>(this));

        
        getStatusBar()->hideProgressBar();
        AfxGetApp()->DoWaitCursor(-1);

        FUNCTION_EXIT;
    }

    
    void AtsDssPlanMapListView::itemDetailsChanged(unsigned long id, const CString& name)
    {
        FUNCTION_ENTRY("itemChanged");

        // We must find the item in our list with it's data set to be the same as the id
        // passed in.

        // Search all lParams stored with each list item for the id we have.
        LVFINDINFO info;
        info.flags = LVFI_PARAM;
        info.lParam = id;

        AtsDssPlanMapDatabaseAccessor* dbAccessor = dynamic_cast<AtsDssPlanMapDatabaseAccessor*>(getDatabaseAccessor());
        TA_ASSERT( dbAccessor != NULL, "DB accessor could not be cast to an DSS Plan Map one");

        AtsDssPlanMapDatabaseAccessor::AtsDssPlanMap details = dbAccessor->getItemDetails( id );

        // If the item is found then update it with the new details

        int foundItemIndex = GetListCtrl().FindItem(&info);
        if (foundItemIndex != -1)
        {
            if (dbAccessor->getItem(id)->isNew())
            {
                details.status = NEW;
            }
            else if (dbAccessor->getItem(id)->hasChanged())
            {
                details.status = MODIFIED;
            }
            else
            {
                details.status = UPTODATE;
            }

            GetListCtrl().SetItemText(foundItemIndex, APPLIED_COLUMN, details.status);
            GetListCtrl().SetItemText(foundItemIndex, DSSEVENT_COLUMN, details.dssEventCString);
           // GetListCtrl().SetItemText(foundItemIndex, DSSINPUT_COLUMN, details.dssInputCString);
            GetListCtrl().SetItemText(foundItemIndex, PLAN_COLUMN, details.plan);

        }
        else
        {
            // Add the new details
            details.status = NEW;

            int position = GetListCtrl().InsertItem(0,"",0);
            GetListCtrl().SetItemText(position, APPLIED_COLUMN, details.status);
            GetListCtrl().SetItemText(position, DSSEVENT_COLUMN, details.dssEventCString);
           //GetListCtrl().SetItemText(position, DSSINPUT_COLUMN, details.dssInputCString);
            GetListCtrl().SetItemText(position, PLAN_COLUMN, details.plan);
            GetListCtrl().SetItemData(position, id);

            // Now select this as the current item
            GetListCtrl().SetItemState(position, 0xFFFFFF, LVIS_SELECTED);
        }

        // Keep our stored map up-to-date so we can perform accurate and quick sorting
        std::map<unsigned long,AtsDssPlanMap>::iterator matching;
        matching = m_currentDisplayedItems.find(id);
        if (matching != m_currentDisplayedItems.end())
        {
            matching->second = details;
        }
        else
        {
            m_currentDisplayedItems.insert( std::map<unsigned long,AtsDssPlanMap>::value_type(id,details) );
        }

        // Now sort the columns again
        m_sortOrder = !m_sortOrder; // Make sure we sort in the same order. To do this we must change the
                                    // sort order as the sorting method will change it again
        updateHeaderSort(m_sortColumn);
        GetListCtrl().SortItems(compareItems, reinterpret_cast<unsigned long>(this));

        FUNCTION_EXIT;
    }


    void AtsDssPlanMapListView::copyItem()
    {
        FUNCTION_ENTRY("copyItem");

        POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
        if (pos == NULL)
        {
            //UE-030059
			// TD14164 ++
            /*MessageBox("An item must be selected before a copy can be performed", RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONEXCLAMATION);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString action = "copy";
			userMsg << action;
			userMsg.showMsgBox(IDS_UE_030059);
			// TD14164 ++
            FUNCTION_EXIT;
            return;
        }

        std::vector<TA_Base_Core::IConfigItem*> newItems;

        while(pos != NULL)
        {
            // An item is selected
            int selected = GetListCtrl().GetNextSelectedItem(pos);
            unsigned long itemId = GetListCtrl().GetItemData( selected );

            try
            {
                TA_ASSERT(getDatabaseAccessor() != NULL, "Database accesssor is NULL");

                TA_Base_Core::IConfigItem* newItem = getDatabaseAccessor()->copyItem(itemId);

                if (newItem != NULL)
                {
                    // Add the item so it can be added to the list later
                    newItems.push_back(newItem);
                }
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException","Could not copy the item");
                //UE-030060
				// TD14164 ++
                /*MessageBox("The item could not be copied because of an error with the data.",RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
				TA_Base_Bus::TransActiveMessage userMsg;
				CString reason = "of an error with the data";
				userMsg << reason;
				userMsg.showMsgBox(IDS_UE_030069);
				// ++ TD14164
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException","Could not copy the item");
                //UE-030060
				// TD14164 ++
                /*MessageBox("The item could not be copied because of a database error.",RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
				TA_Base_Bus::TransActiveMessage userMsg;
				CString reason = "of a database error";
				userMsg << reason;
				userMsg.showMsgBox(IDS_UE_030069);
				// ++ TD14164
            }
        }

        // Now finally add the items we have created into the list
        for(std::vector<TA_Base_Core::IConfigItem*>::iterator iter = newItems.begin(); iter != newItems.end(); ++iter)
        {
            itemDetailsChanged((*iter)->getUniqueIdentifier(), "");
        }

        FUNCTION_EXIT;
    }


    void AtsDssPlanMapListView::newItem()
    {
        FUNCTION_ENTRY("newItem");
        
        TA_ASSERT(getDatabaseAccessor() != NULL, "Database accessor is NULL");

        try
        {
            TA_Base_Core::IConfigItem* newItem = getDatabaseAccessor()->newItem();

            // Unselect the current selections
            POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
            while (pos != NULL)
            {
                int selected = GetListCtrl().GetNextSelectedItem(pos);
                GetListCtrl().SetItemState(selected,0,LVIS_SELECTED);
            }

            if (newItem !=  NULL)
            {
                itemDetailsChanged(newItem->getUniqueIdentifier(), "");
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException","Could not create a new item");
            //UE-030060
			// TD14164 ++
            /*MessageBox("The item could not be created because of an error with the data.",RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString reason = "of an error with the data";
			userMsg << reason;
			userMsg.showMsgBox(IDS_UE_030070);
			// ++ TD14164
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException","Could not create a new item");
            //UE-030060
			// TD14164 ++
            /*MessageBox("The item could not be created because of a database error.",RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);*/
			TA_Base_Bus::TransActiveMessage userMsg;
			CString reason = "of a database error";
			userMsg << reason;
			userMsg.showMsgBox(IDS_UE_030070);
			// ++ TD14164
        }


        FUNCTION_EXIT;
    }
    

    void AtsDssPlanMapListView::onColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        updateHeaderSort(pNMListView->iSubItem);

        GetListCtrl().SortItems(compareItems, reinterpret_cast<unsigned long>(this));
	    
	    *pResult = 0;
    }


	void AtsDssPlanMapListView::updateHeaderSort(const unsigned int column)
	{
		FUNCTION_ENTRY("updateHeaderSort");

		TA_ASSERT(column >= 0, "Column was -ve!");

		//First find if the selected column is currently sorted on. If so, toggle 
		//the sorting arrangment. 
		CHeaderCtrl* headerCtrl = GetListCtrl().GetHeaderCtrl(); 

        HDITEM headerItem; 
		headerItem.mask = HDI_FORMAT | HDI_BITMAP; 
		headerCtrl->GetItem(column, &headerItem); 

		if (headerItem.hbm != 0) 
		{ 
			DeleteObject(headerItem.hbm); 
			headerItem.hbm = 0; 
		}
        
        // If this is the same column we sorted on last time then now we want to sort in the opposite order
		if(m_sortColumn == column)
		{
			m_sortOrder = !m_sortOrder;
		}
        else
        {
            // If this is a different column reset the sort order to be ascending first.
            m_sortOrder = ASCENDING;
        }

        // Set up ready to add our bitmap
		headerItem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT; 

        /// Load the bitmap based on whether we are sorting ascending or descending
        int resourceToLoad = IDB_DESCENDING;
        if (m_sortOrder == ASCENDING)
        {
            resourceToLoad = IDB_ASCENDING;
        }

        headerItem.hbm = (HBITMAP)LoadImage(GetModuleHandle(AtsDssPlanMapComponent::DLL_NAME), MAKEINTRESOURCE(resourceToLoad), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		headerCtrl->SetItem(column, &headerItem); 

        // If the column we are sorting on has changed then we must remove the arrow bitmap from the old
        // column header
		if(m_sortColumn != column) 
		{ 
			headerCtrl->GetItem(m_sortColumn, &headerItem); 
			headerItem.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT); 

			if(headerItem.hbm != 0) 
			{ 
				DeleteObject(headerItem.hbm); 
				headerItem.hbm = 0; 
			} 
			headerCtrl->SetItem(m_sortColumn, &headerItem); 

            // Now set our new sort column
            m_sortColumn = column;
		}

		FUNCTION_EXIT;
	}


    int CALLBACK AtsDssPlanMapListView::compareItems(LPARAM index1, LPARAM index2, LPARAM listCtrl)
    {
        AtsDssPlanMapListView* list = reinterpret_cast<AtsDssPlanMapListView*>(listCtrl);
        // Double check it has converted ok
        list = dynamic_cast<AtsDssPlanMapListView*>(list);

        TA_ASSERT(list != NULL, "The wrong type was passed as the third parameter to this method");

        // Now retrieve the data for these items so we can start comparing it
        std::map<unsigned long,AtsDssPlanMap>::iterator item1 = list->m_currentDisplayedItems.find( index1 );
        std::map<unsigned long,AtsDssPlanMap>::iterator item2 = list->m_currentDisplayedItems.find( index2 );

        if (item1 == list->m_currentDisplayedItems.end() || 
            item2 == list->m_currentDisplayedItems.end())
        {
            // Cannot compare
            return 0;
        }

        CString string1("");
        CString string2("");

		int returnVal;

        switch(list->m_sortColumn)
        {
            case(APPLIED_COLUMN):
                    string1 = item1->second.status;
                    string2 = item2->second.status;
					returnVal = string1.CompareNoCase(string2);
                    break;

            case(DSSEVENT_COLUMN):
					returnVal = item1->second.dssEvent - item2->second.dssEvent;
                    break;

            //case(DSSINPUT_COLUMN):
           //        returnVal = item1->second.dssInput - item2->second.dssInput;
            //        break;

            case(PLAN_COLUMN):
                    string1 = item1->second.plan;
                    string2 = item2->second.plan;
					returnVal = string1.CompareNoCase(string2);
                    break;
        }
        
        // If we are sorting descending then switch the sign of the returnVal
        if (list->m_sortOrder == DESCENDING)
        {
            returnVal = -returnVal;
        }

        return returnVal;
    }

    
    void AtsDssPlanMapListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    /////////////////////////////////////////////////////////////////////////////
    // AtsDssPlanMapListView diagnostics

    #ifdef _DEBUG
    void AtsDssPlanMapListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void AtsDssPlanMapListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

