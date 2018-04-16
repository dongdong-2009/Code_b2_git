/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/ats_fixed_block_map/src/AtsFixedBlockMapListView.h $
  * @author:  Karen Graham
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/07/02 16:00:02 $
  * Last modified by:  $Author: peter.wong $
  * 
  * This view contains a list of all Ats Fixed Block Maps available to be configured. This view must
  * communicate with the AtsFixedBlockMapDetailView as this will show the details of the item selected in this view.
  */

#if !defined(AFX_AtsFixedBlockMapListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
#define AFX_AtsFixedBlockMapListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_

#include <afxcview.h>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h"
#include "app/configuration/config_plugins/ats_fixed_block_map/src/AtsFixedBlockMapDatabaseAccessor.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_IRS_App
{
    class AtsFixedBlockMapListView : public TA_Base_App::AbstractListView
    {
    protected:
        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
	    AtsFixedBlockMapListView();

	    virtual ~AtsFixedBlockMapListView();

	    DECLARE_DYNCREATE(AtsFixedBlockMapListView)


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
	    //{{AFX_VIRTUAL(AtsFixedBlockMapListView)
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
	    //{{AFX_MSG(AtsFixedBlockMapListView)
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
            return TA_Base_App::MessageSender::ATS_FIXED_BLOCK_MAP;
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
        * @param LPARAM - This is a pointer to this object. ie the AtsFixedBlockMapListView object.
        */
		static int CALLBACK compareItems(LPARAM index1, LPARAM index2, LPARAM actionListCtrl);

		static const int	   APPLIED_COLUMN;
		static const int	   ZONE_TYPE_COLUMN;
		static const int	   ZONE_COLUMN;
		static const int	   TRACK_COLUMN;
		static const int       BRANCH_ID_COLUMN;;
		static const int       MIN_ABSCISSA_COLUMN;
		static const int       MAX_ABSCISSA_COLUMN;
		static const int       ARM_ID_COLUMN;

		static const CString   APPLIED_COLUMN_NAME;
		static const CString   ZONE_TYPE_COLUMN_NAME;
		static const CString   ZONE_COLUMN_NAME;
		static const CString   TRACK_COLUMN_NAME;
		static const CString   BRANCH_ID_COLUMN_NAME;
		static const CString   MIN_ABSCISSA_COLUMN_NAME;
		static const CString   MAX_ABSCISSA_COLUMN_NAME;
		static const CString   ARM_ID_COLUMN_NAME;

		static const int       APPLIED_COLUMN_WIDTH;
		static const int       ZONE_TYPE_COLUMN_WIDTH;
		static const int       ZONE_COLUMN_WIDTH;
		static const int       TRACK_COLUMN_WIDTH;
		static const int       BRANCH_ID_COLUMN_WIDTH;
		static const int       MIN_ABSCISSA_COLUMN_WIDTH;
		static const int       MAX_ABSCISSA_COLUMN_WIDTH;
		static const int	   ARM_ID_COLUMN_WIDTH;

        static const bool ASCENDING;
        static const bool DESCENDING;

        // These are the possible values for the status of each Ats Fixed Block Map
        static const CString NEW;
        static const CString MODIFIED;
        static const CString UPTODATE;

        HICON m_componentIcon;

        bool m_sortOrder;   // This indicates which order we are currently sorting in.
        unsigned int m_sortColumn;  // This indicates which column is currently sorted.

        // This holds all the items currently being displayed. We keep this up-to-date so we can use it to
        // sort the columns.
        std::map<unsigned long,AtsFixedBlockMapDatabaseAccessor::AtsFixedBlockMapRecord> m_currentDisplayedItems;

    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_AtsFixedBlockMapListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
