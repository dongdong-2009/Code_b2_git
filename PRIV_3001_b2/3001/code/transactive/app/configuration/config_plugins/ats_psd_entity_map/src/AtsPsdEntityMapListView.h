/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/ats_psd_entity_map/src/AtsPsdEntityMapListView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view contains a list of all ATS PSD Entity Maps available to be configured. This view must
  * communicate with the AtsPsdEntityMapDetailView as this will show the details of the item selected in this view.
  */

#if !defined(AFX_AtsPsdEntityMapListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
#define AFX_AtsPsdEntityMapListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_

#include <afxcview.h>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h"
#include "app/configuration/config_plugins/ats_psd_entity_map/src/AtsPsdEntityMapDatabaseAccessor.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_IRS_App
{
    class AtsPsdEntityMapListView : public TA_Base_App::AbstractListView
    {
    protected:
        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
	    AtsPsdEntityMapListView();

	    virtual ~AtsPsdEntityMapListView();

	    DECLARE_DYNCREATE(AtsPsdEntityMapListView)


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
	    //{{AFX_VIRTUAL(AtsPsdEntityMapListView)
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
	    //{{AFX_MSG(AtsPsdEntityMapListView)
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
            return TA_Base_App::MessageSender::ATS_PSD_ENTITY_MAP;
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
        * @param LPARAM - This is a pointer to this object. ie the AtsPsdEntityMapListView object.
        */
       static int CALLBACK compareItems(LPARAM index1, LPARAM index2, LPARAM actionListCtrl);


        static const int	   APPLIED_COLUMN;              
        static const int       MSSZONE_COLUMN;			
        static const int       STATION_COLUMN;			
        static const int       PLATFORM_COLUMN;             
        static const int       PSDNUMBER_COLUMN;			
        static const int       DPTYPE_COLUMN;			
        static const int       DPINPUTOUTPUT_COLUMN;		
        static const int       DPNAME_COLUMN;			

        static const CString   APPLIED_COLUMN_NAME;        
        static const CString   MSSZONE_COLUMN_NAME;		
        static const CString   STATION_COLUMN_NAME;		
        static const CString   PLATFORM_COLUMN_NAME;       
        static const CString   PSDNUMBER_COLUMN_NAME;	
        static const CString   DPTYPE_COLUMN_NAME;		
        static const CString   DPINPUTOUTPUT_COLUMN_NAME;
        static const CString   DPNAME_COLUMN_NAME;		

        static const int       APPLIED_COLUMN_WIDTH;        
        static const int       MSSZONE_COLUMN_WIDTH;		
        static const int       STATION_COLUMN_WIDTH;		
        static const int       PLATFORM_COLUMN_WIDTH;       
        static const int       PSDNUMBER_COLUMN_WIDTH;	
        static const int       DPTYPE_COLUMN_WIDTH;		
        static const int       DPINPUTOUTPUT_COLUMN_WIDTH;
        static const int       DPNAME_COLUMN_WIDTH;		

        static const bool ASCENDING;
        static const bool DESCENDING;

        // These are the possible values for the status of each ATS PSD Entity Map
        static const CString NEW;
        static const CString MODIFIED;
        static const CString UPTODATE;

        HICON m_componentIcon;

        bool m_sortOrder;   // This indicates which order we are currently sorting in.
        unsigned int m_sortColumn;  // This indicates which column is currently sorted.

        // This holds all the items currently being displayed. We keep this up-to-date so we can use it to
        // sort the columns.
        std::map<unsigned long,AtsPsdEntityMapDatabaseAccessor::AtsPsdEntityMapRecord> m_currentDisplayedItems;

    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_AtsPsdEntityMapListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
