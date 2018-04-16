/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/tr_train/src/TrTrainDetailView.h $
  * @author:  Lucky Cuizon
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/01/15 8:15:14 $
  * Last modified by:  $Author: Lucky Cuizon $
  * 
  * This view allows the user to configure an Video Station's details.
  */

#if !defined(AFX_TrTrainDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_TrTrainDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/tr_train/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/mfc_extensions/src/coloured_controls/HighlightItemListCtrl.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_IRS_Core
{
    class ITrain;
}

namespace TA_IRS_App
{
    class TrTrainDatabaseAccessor;
    class TrTrainListView;


    class TrTrainDetailView : public TA_Base_App::AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    TrTrainDetailView();


        /**
         * Destructor
         */
	    virtual ~TrTrainDetailView();

        
        DECLARE_DYNCREATE(TrTrainDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(TrTrainDetailView)
    	enum { IDD = IDD_TR_TRAIN_RECORD_VIEW };
    	CButton	            m_applyBtn;
    	
		TA_Base_Bus::TextColourEdit m_trainId;
		TA_Base_Bus::TextColourEdit m_primaryTsi;
		TA_Base_Bus::TextColourEdit m_secondaryTsi;
		
        CButton	            m_type;

        CTransparentImage   m_transparentImageTrainId;
        CTransparentImage   m_transparentImagePrimaryTsi;
    	CTransparentImage   m_transparentImageSecondaryTsi;

		CStatic m_modifyDate;
		CStatic m_createDate;
    	
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
	    //{{AFX_VIRTUAL(TrTrainDetailView)
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
	    //{{AFX_MSG(TrTrainDetailView)
	    afx_msg void onButtonReset();
        afx_msg void onButtonApply();


		afx_msg void OnKillfocusEditPrimaryTsi();
		afx_msg void OnKillfocusEditSecondaryTsi();
		afx_msg void OnKillfocusEditTrainId();
        
        //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    protected:

        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
		 * 
         */
        virtual TA_Base_App::MessageSender::ConfigUpdateType getConfigMessageType() const
        {
			//this function is no use. but it is needed to implement AbstractView.
            return TA_Base_App::MessageSender::PA_ATS_TRIGGERED_BROADCAST;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        TrTrainDetailView( const TrTrainDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        TrTrainDetailView& operator=(const TrTrainDetailView&);

        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the si_train details. These controls will be
         * hidden until the user selects a si_train from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);   
        
        /**
         * displayLocationDialog - called whenever the user clicks on a 
         *      location selection button, pass in the associated control
         *      to fill it in with the location the user selected
         *
         * @param locationEdit the edit control which should receive the
         *          updated parameters of the selection dialog
         *
         * @return true if there were changes made (that is, a selection was made)
         */
        bool displayLocationDialog(TA_Base_Bus::TextColourEdit& locationEdit);

        /**
         * doSelectionDialog
         *
         * Displays a selection dialog for a specific set of data, and updates
         *  the relevant control
         *
         * @param dataSet the type of selection dialog we want
         *
         * @param filterBySourceLocation set to true if we want to filter
         *          out all data for locations other than the current
         *          location in our config item
         *
         */
        void doSelectionDialog(bool isCAR1);        

    private:


        /**
         * refreshTrainIdEdit - refreshes the train id edit control with
         *  the up to date value present in internally contained config item
         */
        void refreshTrainIdEdit();



		void refreshPrimaryTsi();


		void refreshSecondaryTsi();
        // The titles for the change dialog box
		
        static const int RADIO_NUM_FIELD_LENGTH;
                
		
        TA_IRS_Core::ITrain* m_configItem; // The current PA ATS Triggered Broadcast the user
                                                              // is configuring. This is the database object
                                                              // for it.

        TrTrainListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all PA ATS Triggered Broadcasts. We need to
                                            // use this to tell it when PA ATS Triggered Broadcast details have
                                            // changed.

        TrTrainDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.

    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_TrTrainDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
