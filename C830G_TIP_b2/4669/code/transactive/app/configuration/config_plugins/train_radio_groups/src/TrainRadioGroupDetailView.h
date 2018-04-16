/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_4669_TRAIN_ECP/CODE_4669/transactive/app/configuration/config_plugins/train_radio_groups/src/TrainRadioGroupDetailView.h $
  * @author:  oliverk
  * @version: $Revision: #1$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  * This view allows the user to configure a train radio group's details.
  */
#if !defined(TrainRadioGroupDetailView_H)
#define TrainRadioGroupDetailView_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "app/configuration/config_plugins/train_radio_groups/src/resource.h"
#include "app/configuration/config_plugins/train_radio_groups/src/LocationListCtrl.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"


// forward declarations
namespace TA_IRS_App
{
    class TrainRadioGroupListView;
    class TrainRadioGroupDatabaseAccessor;
}


namespace TA_IRS_Core
{
    class TrainRadioGroup;
}


namespace TA_IRS_App
{

    class TrainRadioGroupDetailView : public TA_Base_App::AbstractDetailView
    {

    protected:


        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
        TrainRadioGroupDetailView();


        /**
         * Destructor
         */
        virtual ~TrainRadioGroupDetailView();


        DECLARE_DYNCREATE( TrainRadioGroupDetailView )


    public:
        

        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the item we want to display. We will use this to
         *                       set new values and apply. If this is NULL then the details should all be hidden
         */
        void populateItemDetails( TA_Base_Core::IConfigItem* currentItem );


        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         */
        virtual void refreshButtonAvailability();


        // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(TrainRadioGroupDetailView)

    public:
        virtual void OnInitialUpdate();

    protected:
        virtual void DoDataExchange( CDataExchange* pDX );  // DDX/DDV support
        //}}AFX_VIRTUAL

        // Implementation

    protected:

#ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump( CDumpContext& dc ) const;
#endif

        // Generated message map functions
        //{{AFX_MSG(TrainRadioGroupDetailView)
        afx_msg void OnButtonApply();
        afx_msg void OnButtonReset();
        afx_msg void OnButtonAddlocation();
        afx_msg void OnKillfocusEditTSI();
        afx_msg void OnButtonRemoveLocation();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()


        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual TA_Base_App::MessageSender::ConfigUpdateType getConfigMessageType() const;


    private:


        /**
         * Hidden as a private method so it is not used.
         */
        TrainRadioGroupDetailView( const TrainRadioGroupDetailView& prml );
        TrainRadioGroupDetailView& operator=( const TrainRadioGroupDetailView& puml );


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the train Radio Group details. These controls will be
         * hidden until the user selects a system controller from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails( bool isShown );


        // Dialog Data
        //{{AFX_DATA(TrainRadioGroupDetailView)
        enum { IDD = IDD_TRAINRADIOGROUP_VIEW };
        LocationListCtrl m_locationList;
        CEdit m_name;
        CEdit m_tsi;
        CTransparentImage m_nameBitmap;
        CTransparentImage m_tsiBitmap;
        //}}AFX_DATA


        // The current train Radio the user is configuring.
        // This is the database object for it.
        TA_IRS_Core::ITrainRadioGroup* m_currentTrainRadioGroup;


        // The object that we can use to retrieve information from the database.
        TrainRadioGroupDatabaseAccessor* m_databaseAccessor;


        // This is the view in the left pane of the splitter
        // showing the list of all train Radio Groups. We need to use this to
        // tell it when system controller details have changed.
        TrainRadioGroupListView* m_listView;

        static const int TSI_FIELD_LENGTH;
    };
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(TrainRadioGroupDetailView_H)
