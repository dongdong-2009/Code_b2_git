/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingGridControl.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is a wrapper around the CGridCtrl (from mfc_extensions) to modify it to display and allow
  * configuration of rights.
  */

#if !defined(AFX_MMSSCHEDULINGGRIDCONTROL_H__715F497C_7179_4956_90F9_6313A0F35620__INCLUDED_)
#define AFX_MMSSCHEDULINGGRIDCONTROL_H__715F497C_7179_4956_90F9_6313A0F35620__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>

#define IMPORT_EXTENSIONS
#include "bus/mfc_extensions/src/mfc_extensions.h"
#include "app/configuration/config_plugins/mms_scheduling/src/MmsSchedulingDatabaseAccessor.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"

class CInPlaceDateTime;

namespace TA_IRS_App
{
    class MmsSchedulingDatabaseAccessor;
    class RightsChangedWindowNotificationHandler;

    class MmsSchedulingGridControl : public CGridCtrl, public virtual TA_Base_Core::IOnlineUpdatable
    {
    public:
        enum EColumns
        {
            COL_SUBSYSTEM_NAME,
            COL_SCHEDULED_TIME,
            COL_LAST_REPORTED,
            COL_MAX
        };
        
        /**
         * Constructor
         */
	    MmsSchedulingGridControl();


        /**
         * Destructor
         */
	    virtual ~MmsSchedulingGridControl();


        /**
         * loadItems
         *
         * This is called when the rest of the view is ready for the grid to be loaded with details
         *
         * @param MmsSchedulingDatabaseAccessor* - This is used to retrieve the rights from the database
         * @param CProgressCtrl& - This is used to indicate how we are going with the loading
         */
        void loadItems(MmsSchedulingDatabaseAccessor* dbAccessor, CProgressCtrl& progress);


        /**
         * isConfigurationValid
         *
         * This looks at the current configuration and determines if it is valid or not.
         *
         * @param vector<unsigned long> - If the configuration is invalid this will be populated with all the
         *                                subsystem pkeys with invalid configuration.
         *
         * @return bool - Indicates whether the configuration is valid. Returns true if valid, false otherwise
         */
        bool isConfigurationValid(std::vector<unsigned long>& subsystems);

        /**
         * processUpdate
         *
         * When there is a configuration update of an mms schedule then this method
         * will be invoked to process the update accordingly.
         *
         * @param ConfigUpdateDetails& - The update details so we know what to modify
         */
        virtual void processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(MmsSchedulingGridControl)
	    protected:
	    virtual void PreSubclassWindow();

	    //}}AFX_VIRTUAL

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(MmsSchedulingGridControl)
		    // NOTE - the ClassWizard will add and remove member functions here.
            afx_msg LRESULT OnRightsChanged(UINT wParam, LONG lParam);       
            afx_msg LRESULT OnScheduleDataChanged(UINT wParam, LONG lParam);
            afx_msg void OnDestroy();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


        /**
         * OnEndEditCell
         *
         * This is an override that is called when the user has finished editing a cell. In here
         * we will update our database objects and store the changes ready for applying later.
         *
         * @param int - The row the user has modified
         * @param int - The column the user has modified
         * @param CString - This is the new value the user has entered for the cell
         */
        virtual void OnEndEditCell(int nRow, int nCol, CString str);


        /**
         * CreateInPlaceEditControl
         *
         * This is an override that is called when the user begins editing a cell. We must override
         * it because we want to create a date time control rather than just let the user type
         *  in text
         *
         * @param Parameters just need to be passed onto the base implementation of this method
         */
        virtual void CreateInPlaceEditControl(CRect& rect, DWORD dwStyle, UINT nID,
                                                     int nRow, int nCol,
                                                     LPCTSTR szText, int nChar);


    private:

        /**
         * @return string representation of the text that should be displayed at
         *      the specified cell
         * @param col defines the column index of the cell
         * @param schedule defines the data for the row (row data for this cell)
         */
        CString getCellText(int col, TA_IRS_Core::IConfigMmScheduling* schedule);

        /**
         * setUpHeaders
         * 
         * This sets up the row and column headers after retrieving all the required information
         * from the database
         */
        void setUpHeaders();

        /**
         * populateWithData
         *
         * This loads all the information and adds it into the grid
         */
        void populateWithData();

        // This is used to retrieve rights etc from the database
        MmsSchedulingDatabaseAccessor* m_databaseAccessor;

        // This is a key to hold the MMS Schedule keys and related objects. THe names will be displayed to the user
        // and the keys will be set in the MmsScheduling database objects.
        ScheduleMap m_mmsSchedules;

        // This contains the icons to indicate particulars of this rows
        // MMS Schedule
        CImageList m_profileIcons;

        CInPlaceDateTime*   m_inPlaceDt;
        RightsChangedWindowNotificationHandler* m_rightsChangeHandler;
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_MMSSCHEDULINGGRIDCONTROL_H__715F497C_7179_4956_90F9_6313A0F35620__INCLUDED_)
