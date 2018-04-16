/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/ecs_zone/src/ECSZoneListView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view contains a list of all ECS Zones available to be configured. This view must
  * communicate with the ECSZoneDetailView as this will show the details of the item selected in this view.
  */

#if !defined(AFX_ECSZoneListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
#define AFX_ECSZoneListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_

#include <afxcview.h>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h"
#include "app/configuration/config_plugins/ecs_zone/src/ECSZoneDatabaseAccessor.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_IRS_App
{
    class ECSZoneListView : public TA_Base_App::AbstractListView
    {
    protected:
        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
	    ECSZoneListView();

	    virtual ~ECSZoneListView();

	    DECLARE_DYNCREATE(ECSZoneListView)


    // Operations
    public:

        //
        // Overrides from AbstractListView
        //

        /**
         * loadAllItems
         *
         * This retrieves all the items and populates the display list for the user.
         * PreCondition: setDatabaseAccessor has been called
         */
        virtual void loadAllItems( );

        
        /**
         * itemDetailsChanged
         *
         * This method is called when an items details change. If any of the fields change
         * we must update our list with the new details.
         *
         * @param unsigned long - The id of the object that has changed
         * @param CString - The name of the object. This will be empty and can be ignored
         */
        virtual void itemDetailsChanged(unsigned long id, const CString& name);


        /**
         * copyItem
         *
         * This requires an item to be selected. It will then copy that item and set it to be the
         * newly selected item.
         */
        virtual void copyItem();


        /**
         * newItem
         *
         * This will create a new item.
         */
        virtual void newItem();

        //virtual void deleteItem();


        
        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ECSZoneListView)
	    public:
	    virtual void OnInitialUpdate();
	    protected:
	    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(ECSZoneListView)
    	afx_msg void OnSize(UINT nType, int cx, int cy);
	    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void onColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
    	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    protected:

        /**
         * getComponentIcon
         *
         * This should return the component icon for items to be shown in the list
         *
         * @return HICON - The icon to be shown
         */
        virtual HICON getComponentIcon()
        {
            return m_componentIcon;
        }


        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual TA_Base_App::MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            return TA_Base_App::MessageSender::ECS_ZONE;
        }

        
    private:
	  /**
	   * updateHeaderSort
	   * 
	   * This method updates the column header when clicked on, to indicate 
	   * the current sort status i.e. ascending/descending.
	   *
	   * @param The column index
	   */
	   void updateHeaderSort(const unsigned int column);


       /**
        * compareItems
        *
        * This compares two items at a time to determine whether they should be swapped or not. This
        * function is passed to the CListCtrl, SortItems() method.
        *
        * @param LPARAM - This is the associated data of the first item. This is an index into the
        *                 m_actions vector
        * @param LPARAM - This is the associated data of the second item. This is an index into the
        *                 m_actions vector
        * @param LPARAM - This is a pointer to this object. ie the ECSZoneListView object.
        */
       static int CALLBACK compareItems(LPARAM index1, LPARAM index2, LPARAM actionListCtrl);


        static const int APPLIED_COLUMN;
        static const int ECS_ZONE_ID_COLUMN;
        static const int ECS_LOCATION_COLUMN;
        static const int TRACK_COLUMN;
        static const int OCC_EQUIPMENT_COLUMN;
        static const int STN_EQUIPMENT_COLUMN;
		//TD17826 - marc_bugfix_start
		static const int IGNORE_CONGESTION_COLUMN;
		//TD17826 - marc_bugfix_end

        static const CString APPLIED_COLUMN_NAME;
        static const CString ECS_ZONE_ID_COLUMN_NAME;
        static const CString ECS_LOCATION_COLUMN_NAME;
        static const CString TRACK_COLUMN_NAME;
        static const CString OCC_EQUIPMENT_COLUMN_NAME;
        static const CString STN_EQUIPMENT_COLUMN_NAME;
		//TD17826 - marc_bugfix_start
		static const CString IGNORE_CONGESTION_COLUMN_NAME;
		//TD17826 - marc_bugfix_end

        static const int APPLIED_COLUMN_WIDTH;
        static const int ECS_ZONE_ID_COLUMN_WIDTH;
        static const int ECS_LOCATION_COLUMN_WIDTH;
        static const int TRACK_COLUMN_WIDTH;
        static const int OCC_EQUIPMENT_COLUMN_WIDTH;
        static const int STN_EQUIPMENT_COLUMN_WIDTH;
		//TD17826 - marc_bugfix_start
		static const int IGNORE_CONGESTION_COLUMN_WIDTH;
		//TD17826 - marc_bugfix_end


        static const bool ASCENDING;
        static const bool DESCENDING;

        // These are the possible values for the status of each ECS Zone
        static const CString NEW;
        static const CString MODIFIED;
        static const CString UPTODATE;

        HICON m_componentIcon;

        bool m_sortOrder;   // This indicates which order we are currently sorting in.
        unsigned int m_sortColumn;  // This indicates which column is currently sorted.

        // This holds all the items currently being displayed. We keep this up-to-date so we can use it to
        // sort the columns.
        std::map<unsigned long,ECSZoneDatabaseAccessor::ECSZoneRecord> m_currentDisplayedItems;

    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_ECSZoneListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
