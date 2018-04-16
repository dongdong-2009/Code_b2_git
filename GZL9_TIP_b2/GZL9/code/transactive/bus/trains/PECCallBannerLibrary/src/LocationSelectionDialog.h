/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(LocationSelectionDialog_H)
#define LocationSelectionDialog_H

#include "bus/banner_pages/call_page_library/src/stdafx.h"
#include "bus/trains/PECCallBannerLibrary/src/resource.h"


namespace TA_IRS_Bus
{
    /**
     * @author adamr
     * @version 1.0
     * @created 21-Dec-2007 3:49:53 PM
     */

    class LocationSelectionDialog : public CDialog
    {

    public:

        /**
         * Constructs a new dialog with the given initial location selection
         *
         * @param initialLocation    The location to initially select in the drop down
         * @param pParent
         */
        LocationSelectionDialog( unsigned long initialLocation, CWnd* pParent = NULL );


        /**
         * Destructor
         */
        virtual ~LocationSelectionDialog();


        /**
         * This will get the location selected by the user.
         *
         * @return the selected location key
         */
        unsigned long getSelectedLocation();


        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(LocationSelectionDialog)
        public:
        virtual BOOL OnInitDialog();
        protected:
        virtual void DoDataExchange(CDataExchange* pDX); // DDX/DDV support
        //}}AFX_VIRTUAL


    protected:
        // Generated message map functions
        //{{AFX_MSG(LocationSelectionDialog)
        /**
         * This is called as the result of the selection changing for m_locationList.
         * It is triggered by the ON_CBN_SELCHANGE event.
         */
        afx_msg void onSelectionChangeLocationList();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()
        
    private:

        /**
         * Private default constructor
         */
        LocationSelectionDialog();

        // Dialog Data
        //{{AFX_DATA(LocationSelectionDialog)
        enum { IDD = IDD_LOCATION_SELECTION };
        CComboBox m_locationList; // The combo box to hold all the locations
        //}}AFX_DATA


        /**
         * The initial location to select in the dialog
         */
        unsigned long m_initialLocation;


        /**
         * The location selected in the dialog
         */
        unsigned long m_selectedLocation;
    };

}

#endif // !defined(LocationSelectionDialog_H)
