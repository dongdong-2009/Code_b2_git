/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/tr_train/src/TrTrainListView.cpp $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/01/15 8:15:14 $
  * Last modified by:  $Author: Lucky Cuizon $
  * 
  * This view contains a list of all Video stations available to be configured. This view must communicate
  * with the VideoStationDetailView as this will show the details of the item selected in this view.
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/IItem.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"

#include "bus/resources/resource.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/IProgressStatusBar.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"
#include "app/configuration/config_plugins/tr_train/src/TrTrainListView.h"
#include "app/configuration/config_plugins/tr_train/src/TrTrainComponent.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_Core;

using  namespace TA_Base_App::TA_Configuration;

namespace TA_IRS_App
{
    // These are the possible values for the status of each PA Zone
    const CString TrTrainListView::NEW = "New";
    const CString TrTrainListView::MODIFIED = "Modified";
    const CString TrTrainListView::UPTODATE = "";

	const CString TrTrainListView::COL_MOD_STATUS = "Status";
	//const CString TrTrainListView::COL_KEY = "Key" ;
	const CString TrTrainListView::COL_TRAIN_ID = "Train ID" ;
	const CString TrTrainListView::COL_PRIMARY_TSI = "RadioID 1";
	const CString TrTrainListView::COL_SECONDARY_TSI = "RadioID 2";

	const int	TrTrainListView::MOD_STATUS_COLUMN =  0;
	//const int	TrTrainListView::KEY_COLUMN = 1;		
	const int	TrTrainListView::TRAINID_COLUMN = 1 ;
	const int   TrTrainListView::PRIMARY_TSI_COLUMN = 2 ;
	const int   TrTrainListView::SECONDARY_TSI_COLUMN = 3;

    const bool TrTrainListView::ASCENDING = true;
    const bool TrTrainListView::DESCENDING = false;

    IMPLEMENT_DYNCREATE(TrTrainListView, CListView)


    TrTrainListView::TrTrainListView()
    :     m_sortOrder(DESCENDING),        // Set to descending so the first time we sort it switches to ASCENDING
          m_sortColumn(TrTrainListColumns::getDefaultSortColumn())
    {
        m_componentIcon = LoadIcon(GetModuleHandle(TrTrainComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_SI_TRAIN) );
    }


    TrTrainListView::~TrTrainListView()
    {
    }


BEGIN_MESSAGE_MAP(TrTrainListView, AbstractListView)
	//{{AFX_MSG_MAP(TrTrainListView)
	ON_WM_CREATE()
    ON_WM_SIZE()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, onColumnclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    int TrTrainListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
    {
        // Set the styles for our list control before it is created
        lpCreateStruct->style = LVS_REPORT | LVS_SHOWSELALWAYS | WS_BORDER;

	    if (CListView::OnCreate(lpCreateStruct) == -1)
		    return -1;
	    
	    return 0;
    }


    void TrTrainListView::OnInitialUpdate() 
    {
	    CListView::OnInitialUpdate();
	    
        GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_GRIDLINES );

        m_columnHelper.setupListControlColumns(GetListCtrl());
    }


    void TrTrainListView::OnSize(UINT nType, int cx, int cy) 
    {
        CListView::OnSize(nType, cx, cy);
        // Call the CListView::ONSize instead of the one in the abstract class above
    }


    void TrTrainListView::loadAllItems()
    {
        FUNCTION_ENTRY("loadAllItems");

        TA_ASSERT(getDatabaseAccessor() != NULL, "setDatabaseAccessor() must be called before this method");
        TA_ASSERT(getDetailView() != NULL, "The detail view has not been set yet");
        TA_ASSERT(getStatusBar() != NULL, "The status bar has not been set yet");

        // Retrieve the progress bar and display it on the status bar
        CProgressCtrl& progress = getStatusBar()->displayProgressBar();
        progress.SetStep(1);
        progress.SetPos(3); // Indicate that we have started loading and use the wait cursor to do the rest.

        TrTrainDatabaseAccessor* dbAccessor = dynamic_cast<TrTrainDatabaseAccessor*>(getDatabaseAccessor());
        TA_ASSERT( dbAccessor != NULL, "DB accessor could not be cast to an PA Zone one");

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
            MessageBox(errorMsg,RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);

            getStatusBar()->hideProgressBar();
            AfxGetApp()->DoWaitCursor(-1);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            //UE-030001
            MessageBox("Unable to retrieve configuration information. Please try again, and if problem persists, contact support.",RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);

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
        for (std::map<unsigned long, TrTrainDatabaseAccessor::CfgTrTrainRecord>::iterator iter 
                                                                    = m_currentDisplayedItems.begin(); 
             iter != m_currentDisplayedItems.end(); 
             ++iter)
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
            
            m_columnHelper.insertRowAtHead(GetListCtrl(), iter->second, iter->first);
            progress.StepIt();
        }

        // Now sort the columns
        updateHeaderSort(m_sortColumn);
        GetListCtrl().SortItems(compareItems, reinterpret_cast<DWORD>(this));

        
        getStatusBar()->hideProgressBar();
        AfxGetApp()->DoWaitCursor(-1);

        FUNCTION_EXIT;
    }

    
    void TrTrainListView::itemDetailsChanged(unsigned long id, const CString& name)
    {
        FUNCTION_ENTRY("itemChanged");

        // We must find the item in our list with it's data set to be the same as the id
        // passed in.

        // Search all lParams stored with each list item for the id we have.
        LVFINDINFO info;
        info.flags = LVFI_PARAM;
        info.lParam = id;

        TrTrainDatabaseAccessor* dbAccessor = dynamic_cast<TrTrainDatabaseAccessor*>(getDatabaseAccessor());
        TA_ASSERT( dbAccessor != NULL, "DB accessor could not be cast to an PA Zone one");

        TrTrainDatabaseAccessor::CfgTrTrainRecord details = dbAccessor->getItemDetails( id );

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

			m_columnHelper.updateRow(GetListCtrl(),foundItemIndex, details);
			//GetListCtrl().SetItemText(foundItemIndex, MOD_STATUS_COLUMN,    details.status);
			//GetListCtrl().SetItemText(foundItemIndex, KEY_COLUMN,			 details.key);

// 			GetListCtrl().SetItemText(foundItemIndex, TRAINID_COLUMN,      details.trainId);
//             GetListCtrl().SetItemText(foundItemIndex, PRIMARY_TSI_COLUMN,		 details.primaryTsi);
// 			GetListCtrl().SetItemText(foundItemIndex, SECONDARY_TSI_COLUMN,		 details.secondaryTsi);
        }
        else
        {
			// Add the new details
            details.status = NEW;
			
			int position = m_columnHelper.insertRowAtHead(GetListCtrl(), details, id);

//             int position = GetListCtrl().InsertItem(0,"",0);
// 			
// 			
// 			GetListCtrl().SetItemText(position, MOD_STATUS_COLUMN,    details.status);
// 			//GetListCtrl().SetItemText(foundItemIndex, KEY_COLUMN,			 details.key);
// 			
// 			GetListCtrl().SetItemText(foundItemIndex, TRAINID_COLUMN,      details.trainId);
//             GetListCtrl().SetItemText(foundItemIndex, PRIMARY_TSI_COLUMN,  details.primaryTsi);
// 			GetListCtrl().SetItemText(foundItemIndex, SECONDARY_TSI_COLUMN,	   details.secondaryTsi);			
// 			
//             GetListCtrl().SetItemData(position, id);

            // Now select this as the current item
            GetListCtrl().SetItemState(position, 0xFFFFFF, LVIS_SELECTED);

        }

        // Keep our stored map up-to-date so we can perform accurate and quick sorting
        std::map<unsigned long, TrTrainDatabaseAccessor::CfgTrTrainRecord>::iterator matching;
        matching = m_currentDisplayedItems.find(id);
        if (matching != m_currentDisplayedItems.end())
        {
            matching->second = details;
        }
        else
        {
            m_currentDisplayedItems.insert( 
                std::map<unsigned long, TrTrainDatabaseAccessor::CfgTrTrainRecord>::value_type(id, details) );
        }

        // Now sort the columns again
        m_sortOrder = !m_sortOrder; // Make sure we sort in the same order. To do this we must change the
                                    // sort order as the sorting method will change it again
        updateHeaderSort(m_sortColumn);
        GetListCtrl().SortItems(compareItems, reinterpret_cast<unsigned long>(this));

        FUNCTION_EXIT;
    }


    void TrTrainListView::copyItem()
    {
        FUNCTION_ENTRY("copyItem");
		
        POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
        if (pos == NULL)
        {
            //UE-030059
            MessageBox("An item must be selected before a copy can be performed", RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONEXCLAMATION);
            FUNCTION_EXIT;
            return;
        }
		
		std::vector<TA_Base_Core::IConfigItem*> newItems;
		int selectedCount = GetListCtrl().GetSelectedCount();
		
		for(int i=0; i<selectedCount; i++)
		{
			// An item is selected
			int selected = GetListCtrl().GetNextSelectedItem(pos);
			unsigned long itemId = GetListCtrl().GetItemData( selected );
			
			try
			{
				TA_ASSERT(getDatabaseAccessor() != NULL, "Database accesssor is NULL");
				
				TA_Base_Core::IConfigItem* newItem = getDatabaseAccessor()->copyItem(itemId);
				
				if (newItem !=  NULL)
				{
					//itemDetailsChanged(newItem->getUniqueIdentifier(), "");
					// Store the item so we can add it to the list later
                    newItems.push_back(newItem);
				}
			}
			catch ( const TA_Base_Core::DataException& )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DataException","Could not copy the item");
				//UE-030060
				MessageBox("The item could not be copied because of an error with the data.",RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);
			}
			catch ( const TA_Base_Core::DatabaseException& )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException","Could not copy the item");
				//UE-030060
				MessageBox("The item could not be copied because of a database error.",RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);
			}
		}
		
        // Now add all the new items to the list now that we've created them.
        for(std::vector<TA_Base_Core::IConfigItem*>::iterator iter = newItems.begin(); iter != newItems.end(); ++iter)
        {
            // Insert this item into the list and set it to be the selected item
			itemDetailsChanged((*iter)->getUniqueIdentifier(), "");
        }
		
        FUNCTION_EXIT;
    }
	
    void TrTrainListView::newItem()
    {
		FUNCTION_ENTRY("newItem");
        
        TA_ASSERT(getDatabaseAccessor() != NULL, "Database accessor is NULL");
		
        try
        {
            TA_Base_Core::IConfigItem* newItem = getDatabaseAccessor()->newItem();
			
            if (newItem !=  NULL)
            {
                itemDetailsChanged(newItem->getUniqueIdentifier(), ""); //TODO
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException","Could not create a new item");
            //UE-030060
            MessageBox("The item could not be created because of an error with the data.",RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException","Could not create a new item");
            //UE-030060
            MessageBox("The item could not be created because of a database error.",RunParams::getInstance().get(RPARAM_APPNAME).c_str(),MB_OK|MB_ICONSTOP);
        }
		
		
        FUNCTION_EXIT;
		
    }

    void TrTrainListView::onColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        updateHeaderSort(pNMListView->iSubItem);

        GetListCtrl().SortItems(compareItems, reinterpret_cast<unsigned long>(this));
	    
	    *pResult = 0;
    }


	void TrTrainListView::updateHeaderSort(const unsigned int column)
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

        headerItem.hbm = (HBITMAP)LoadImage(GetModuleHandle(TrTrainComponent::DLL_NAME), MAKEINTRESOURCE(resourceToLoad), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
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


    int CALLBACK TrTrainListView::compareItems(LPARAM index1, LPARAM index2, LPARAM listCtrl)
    {
        TrTrainListView* list = reinterpret_cast<TrTrainListView*>(listCtrl);
        // Double check it has converted ok
        list = dynamic_cast<TrTrainListView*>(list);

        TA_ASSERT(list != NULL, "The wrong type was passed as the third parameter to this method");

        // Now retrieve the data for these items so we can start comparing it
        std::map<unsigned long, TrTrainDatabaseAccessor::CfgTrTrainRecord>::iterator item1 
                                                        = list->m_currentDisplayedItems.find( index1 );
        std::map<unsigned long, TrTrainDatabaseAccessor::CfgTrTrainRecord>::iterator item2 
                                                        = list->m_currentDisplayedItems.find( index2 );

        if (item1 == list->m_currentDisplayedItems.end() || 
            item2 == list->m_currentDisplayedItems.end())
        {
            // Cannot compare
            return 0;
        }

        int returnVal(0);
        
        returnVal = (list->m_columnHelper).compareItems(list->m_sortColumn, item1->second, item2->second);

        // If we are sorting descending then switch the sign of the returnVal
        if (list->m_sortOrder == DESCENDING)
        {
            returnVal = -returnVal;
        }

        return returnVal;
    }

    
    void TrTrainListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    /////////////////////////////////////////////////////////////////////////////
    // TrTrainListView diagnostics

    #ifdef _DEBUG
    void TrTrainListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void TrTrainListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

