/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/ecs_master_mode/src/ECSMasterModeDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view allows the user to configure an ECS Master Mode's details.
  */

#if !defined(AFX_ECSMasterModeDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_ECSMasterModeDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/ecs_master_mode/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"


#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include <vector>
#include <string>

namespace TA_IRS_Core
{
    class IConfigECSMasterMode;
}


namespace TA_IRS_App
{
    class ECSMasterModeDatabaseAccessor;
    class ECSMasterModeListView;


    class ECSMasterModeDetailView : public TA_Base_App::AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    ECSMasterModeDetailView();


        /**
         * Destructor
         */
	    virtual ~ECSMasterModeDetailView();

        
        DECLARE_DYNCREATE(ECSMasterModeDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(ECSMasterModeDetailView)
	enum { IDD = IDD_ECS_MASTER_MODE_RECORD_VIEW };
	CButton	m_editStationButton;
	CListCtrl	m_stationListCtrl;
	CButton	            m_applyBtn;
	TA_Base_Bus::TextColourEdit	    m_masterModeEdt;
    TA_Base_Bus::TextColourEdit	    m_conditionEdt;
	TA_Base_Bus::TextColourEdit	    m_ecsZoneEdt;
	TA_Base_Bus::TextColourEdit	    m_evacuationEdt;
	TA_Base_Bus::TextColourEdit	    m_airflowEdt;
    TA_Base_Bus::TextColourEdit	    m_displayEdt;
	TA_Base_Bus::TextColourEdit		m_verificationMsgEdt;
	TA_Base_Bus::TextColourEdit	    m_situationEdt;
	CButton	            m_ecsZoneBtn;
	CStatic	            m_modifyDate;
	CStatic	            m_createDate;
    CTransparentImage   m_bitmap1;
    CTransparentImage   m_bitmap2;
    CTransparentImage   m_bitmap3;
    CTransparentImage   m_bitmap4;
    CTransparentImage   m_bitmap5;
    CButton	            m_type;
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


		void setListItem(unsigned long setStationModeKey);

		void removeListItem();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ECSMasterModeDetailView)
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
	    //{{AFX_MSG(ECSMasterModeDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onButtonECSZone();
	    afx_msg void onKillfocusEditMasterMode();
        afx_msg void onKillfocusEditCondition();
        afx_msg void onKillfocusEditECSZone();
        afx_msg void onKillfocusEditEvacuation();
        afx_msg void onKillfocusEditAirflow();
        afx_msg void onKillfocusEditDisplay();
		afx_msg void onKillfocusEditVerification();
		afx_msg void onKillfocusEditSituation();
	afx_msg void OnEditStationButton();
	afx_msg void OnClickStationList(NMHDR* pNMHDR, LRESULT* pResult);
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
            return TA_Base_App::MessageSender::ECS_MASTER_MODE;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ECSMasterModeDetailView( const ECSMasterModeDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ECSMasterModeDetailView& operator=(const ECSMasterModeDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the ECS Master Mode details. These controls will be
         * hidden until the user selects a ECS Master Mode from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        /**
         * refreshEntityAndEntityType
         *
         * Only one of these items can be enabled at once based on the radio button checked
         * This retrieves the data from the IConfigECSMasterMode to determine which is enabled.
         * It then sets the correct check and enabled and disables the fields.
         */
        void refreshEntityAndEntityType();

        /**
         * populateStationList
         *
         * Helper method that populates the list control with all the associated station modes
		 * given the selected master mode.
		 *
		 * @param The master mode pkey
         */
		void populateStationList(unsigned long masterModeKey);

		// 7196-Start		
		/**
         * sendUpdateMessageForChangeItem
         *
         * This will send the online update message when there is apply button pressed event. 
         *
         *               
         */
		void sendUpdateMessageForChangeItem();
		// 7196-End
    private:
        // The titles for the change dialog box
        static const CString CHANGE_ECS_ZONE_DIALOG_TITLE;
        
        static const int KEY_FIELD_LENGTH;
        static const int MASTER_MODE_FIELD_LENGTH;
        static const int CONDITION_FIELD_LENGTH;
        static const int ECS_ZONE_FIELD_LENGTH;
        static const int EVACUATION_FIELD_LENGTH;
        static const int AIRFLOW_FIELD_LENGTH;
        static const int DISPLAY_FIELD_LENGTH;
		static const int VERIFICATION_FIELD_LENGTH;
		static const int SITUATION_FIELD_LENGTH;

		static const int ECSMasterModeDetailView::STATION_MODE_KEY_COLUMN;
		static const int ECSMasterModeDetailView::LOCATION_COLUMN;
		static const int ECSMasterModeDetailView::STATION_MODE_COLUMN;
		static const int ECSMasterModeDetailView::OPERATION_COLUMN;
		static const int ECSMasterModeDetailView::TRACK_COLUMN;

		static const int ECSMasterModeDetailView::STATION_MODE_KEY_COLUMN_WIDTH;
		static const int ECSMasterModeDetailView::LOCATION_COLUMN_WIDTH;
		static const int ECSMasterModeDetailView::STATION_MODE_COLUMN_WIDTH;
		static const int ECSMasterModeDetailView::OPERATION_COLUMN_WIDTH;
		static const int ECSMasterModeDetailView::TRACK_COLUMN_WIDTH;
		
		static const CString STATION_MODE_KEY_COLUMN_NAME;
		static const CString LOCATION_COLUMN_NAME;
		static const CString STATION_MODE_COLUMN_NAME;
		static const CString OPERATION_COLUMN_NAME;
		static const CString TRACK_COLUMN_NAME;

        TA_IRS_Core::IConfigECSMasterMode* m_configItem; // The current ECS Master Mode the user
                                                              // is configuring. This is the database object
                                                              // for it.

        ECSMasterModeListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all ECS Master Modes. We need to
                                            // use this to tell it when ECS Master Mode details have
                                            // changed.

        ECSMasterModeDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.

		std::map<unsigned long, std::string> m_allLocationKeysVector;
		
		unsigned int m_numberOfLocations;

    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_ECSMasterModeDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
