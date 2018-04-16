/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/app/configuration/config_plugins/train_radio_groups/src/AddLocationDialog.h $
  * @author:  oliverk
  * @version: $Revision: #1 $
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  * This dialog is used to add one or more location to trian radio group.
  */
#if !defined(AddLocationDialog_H)
#define AddLocationDialog_H


#include "app/configuration/config_plugins/train_radio_groups/src/resource.h"

#include <string>
#include <vector>


namespace TA_IRS_Core
{
    class ITrainRadioGroup;
}


namespace TA_IRS_App
{
    /**
     * @author oliverk
     * @version 1.0
     * @created 15-Jan-2008 12:51:21 PM
     */

    class AddLocationDialog : public CDialog
    {

    public:

        /**
         * Dialog Data {{AFX_DATA(AddProcessDialog)
         * @author oliverk
         * @version 1.0
         * @created 15-Jan-2008 12:51:21 PM
         */
        enum
        {
            IDD = IDD_ADD_LOCATION_DIALOG
        };

        /**
         * @author oliverk
         * @version 1.0
         * @created 15-Jan-2008 12:51:22 PM
         */

        struct Location
        {
            /**
             * A Process entity key from the database
             */
            unsigned long key;

            /**
             * A Process entity name from the database
             */
            std::string name;
        };

        /**
         * Creates a new dialog with the current group
         *
         * @param currentTrainRadioGroup
         */
        AddLocationDialog( TA_IRS_Core::ITrainRadioGroup& currentTrainRadioGroup );

        
        /**
         * Destructor
         */
        virtual ~AddLocationDialog();


        /**
         * Gets the locations selected using this dialog
         *
         * @return the selected locations
         */
        std::vector<Location> getLocationsToAdd();


        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(PECDialog)
        public:
        virtual BOOL OnInitDialog();
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL


    protected:


        // Generated message map functions
        //{{AFX_MSG(PECDialog)
        afx_msg void onDblclkLocationList( NMHDR* pNMHDR, LRESULT* pResult );
        afx_msg void onItemchangedLocationList( NMHDR* pNMHDR, LRESULT* pResult );
        afx_msg void onOK();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:

        /**
         * Private default constructor
         */
        AddLocationDialog();

        // Dialog Data
        //{{AFX_DATA(PECDialog)
		CListCtrl m_locationList; // the list control holding the locations to select
        //}}AFX_DATA
        

        /**
         * The current radio group so we can filter out already added locations
         */
        TA_IRS_Core::ITrainRadioGroup& m_currentTrainRadioGroup;

        /**
         * The locations selected in the list control
         */
        std::vector<Location> m_locations;
    };

}

#endif // !defined(AddLocationDialog_H)
