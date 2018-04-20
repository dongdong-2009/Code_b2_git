/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/ecs_zone/src/ECSZoneDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an ECS Zone's details.
  */

#if !defined(AFX_ECSZoneDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_ECSZoneDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/ecs_zone/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_IRS_Core
{
    class IConfigECSZone;
}


namespace TA_IRS_App
{
    class ECSZoneDatabaseAccessor;
    class ECSZoneListView;


    class ECSZoneDetailView : public TA_Base_App::AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    ECSZoneDetailView();


        /**
         * Destructor
         */
	    virtual ~ECSZoneDetailView();

        
        DECLARE_DYNCREATE(ECSZoneDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(ECSZoneDetailView)
	enum { IDD = IDD_ECSZONERECORD_VIEW };
	CButton	            m_applyBtn;
	TA_Base_Bus::TextColourEdit	    m_zoneIdEdt;
	TA_Base_Bus::TextColourEdit	    m_locationEdt;
	TA_Base_Bus::TextColourEdit	    m_trackEdt;
	TA_Base_Bus::TextColourEdit	    m_occEquipmentEdt;
	TA_Base_Bus::TextColourEdit	    m_stnEquipmentEdt;
	CButton	            m_occEquipmentBtn;
	CButton	            m_stnEquipmentBtn;
	//TD17826 - marc_bugfix_start
	//Support IS_IGNORING_CONGESTION field
	CButton				m_IgnoringCongestion;
	//TD17826 - marc_bugfix_end
    CStatic	            m_modifyDate;
	CStatic	            m_createDate;
    CTransparentImage   m_bitmap2;
    CTransparentImage   m_bitmap3;
    CTransparentImage   m_bitmap4;
    CTransparentImage   m_bitmap5;
    CTransparentImage   m_bitmap6;
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




    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ECSZoneDetailView)
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
	    //{{AFX_MSG(ECSZoneDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onButtonOccEquipment();
	    afx_msg void onButtonStnEquipment();
		//TD17826 - marc_bugfix_start
		afx_msg void onButtonIgnoreCongestion();
		//TD17826 - marc_bugfix_end
	    afx_msg void onKillfocusEditZoneId();
        afx_msg void onKillfocusEditLocation();
        afx_msg void onKillfocusEditTrack();
        afx_msg void onKillfocusEditOccEquipment();
        afx_msg void onKillfocusEditStnEquipment();

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
            return TA_Base_App::MessageSender::ECS_ZONE;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ECSZoneDetailView( const ECSZoneDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ECSZoneDetailView& operator=(const ECSZoneDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the ECS Zone details. These controls will be
         * hidden until the user selects a ECS Zone from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        /**
         * refreshEntityAndEntityType
         *
         * Only one of these items can be enabled at once based on the radio button checked
         * This retrieves the data from the IConfigECSZone to determine which is enabled.
         * It then sets the correct check and enabled and disables the fields.
         */
        void refreshEntityAndEntityType();


    private:
        // The titles for the change dialog box
        static const CString CHANGE_OCC_EQUIPMENT_DIALOG_TITLE;
        static const CString CHANGE_STN_EQUIPMENT_DIALOG_TITLE;

        static const int ECS_ZONE_ID_FIELD_LENGTH;
        static const int ECS_LOCATION_FIELD_LENGTH;
        static const int TRACK_FIELD_LENGTH;
        static const int OCC_EQUIPMENT_FIELD_LENGTH;
        static const int STN_EQUIPMENT_FIELD_LENGTH;

        TA_IRS_Core::IConfigECSZone* m_configItem; // The current ECS Zone the user
                                                              // is configuring. This is the database object
                                                              // for it.

        ECSZoneListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all ECS Zones. We need to
                                            // use this to tell it when ECS Zone details have
                                            // changed.

        ECSZoneDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_ECSZoneDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
