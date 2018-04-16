/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/tis_ratis_destination/src/TisRatisDestinationDetailView.h $
  * @author:  James
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This view allows the user to configure an Tis Ratis Destination's details.
  */

#if !defined(AFX_TisRatisDestinationDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_TisRatisDestinationDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/tis_ratis_destination/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"

#include "bus/mfc_extensions/src/coloured_controls/HighlightItemListCtrl.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_IRS_Core
{
    class IConfigTiRatisDestination;
}


namespace TA_IRS_App
{
    class TisRatisDestinationDatabaseAccessor;
    class TisRatisDestinationListView;


    class TisRatisDestinationDetailView : public TA_Base_App::AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    TisRatisDestinationDetailView();


        /**
         * Destructor
         */
	    virtual ~TisRatisDestinationDetailView();

        
        DECLARE_DYNCREATE(TisRatisDestinationDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(TisRatisDestinationDetailView)
    	enum { IDD = IDD_TIS_RATIS_DESTINATION_RECORD_VIEW };
		//TD18055 marc_bugfix support for destination mandatory marker image
		CTransparentImage	m_destinationBitmap;
    	CButton	            m_applyBtn;
    	CStatic	            m_modifyDate;
    	CStatic	            m_createDate;
        TA_Base_Bus::TextColourEdit	    m_destinationEdt;

    	//}}AFX_DATA
       

        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the item we want to display. We will use this to
         *                       set new values and apply. If this is NULL then the details should all be hidden
         */
        void populateItemDetails(TA_Base_Core::IConfigItem* currentItem);

        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         */
        virtual void refreshButtonAvailability();




    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(TisRatisDestinationDetailView)
	public:
	virtual void OnInitialUpdate();
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

    protected:

    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif


	    // Generated message map functions
	    //{{AFX_MSG(TisRatisDestinationDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();

        afx_msg void onKillfocusDestinationEdt();

        //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    protected:

        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual TA_Base_App::MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            return TA_Base_App::MessageSender::TIS_RATIS_DESTINATION;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        TisRatisDestinationDetailView( const TisRatisDestinationDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        TisRatisDestinationDetailView& operator=(const TisRatisDestinationDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the Tis Ratis Destination details. These controls will be
         * hidden until the user selects a Tis Ratis Destination from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        /**
         * refreshEntityAndEntityType
         *
         * Only one of these items can be enabled at once based on the radio button checked
         * This retrieves the data from the IConfigTisRatisDestination to determine which is enabled.
         * It then sets the correct check and enabled and disables the fields.
         */
        void refreshEntityAndEntityType();
    
        /**
         * populateAssociatedEntityArea
         *
         * Refreshes the list control showing all the associated entities
         */
        void populateAssociatedEntityArea(bool initialisation = false);
        
        enum EAssocUpdate
        {
            ADD_NEW,
            EDIT_EXISTING,
            REMOVE_EXISTING
        };

    private:
        // The titles for the change dialog box
        static const int DESTINATION_FIELD_LENGTH;

        TA_IRS_Core::IConfigTiRatisDestination* m_configItem; // The current Tis Ratis Destination the user
                                                              // is configuring. This is the database object
                                                              // for it.

        TisRatisDestinationListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all Tis Ratis Destinations. We need to
                                            // use this to tell it when Tis Ratis Destination details have
                                            // changed.

        TisRatisDestinationDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.
    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_TisRatisDestinationDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
