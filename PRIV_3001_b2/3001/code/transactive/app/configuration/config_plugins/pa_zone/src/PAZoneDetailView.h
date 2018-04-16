/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/pa_zone/src/PAZoneDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view allows the user to configure an PA Zone's details.
  */

#if !defined(AFX_PaZoneDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_PaZoneDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/pa_zone/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"

#include "bus/mfc_extensions/src/coloured_controls/HighlightItemListCtrl.h"
#include "core/data_access_interface/pa_4669/src/IPaTisZoneMap.h"

#include <list>
#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigPaZone;
}


namespace TA_IRS_App
{
    class PaZoneDatabaseAccessor;
    class PaZoneListView;


    class PaZoneDetailView : public TA_Base_App::AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    PaZoneDetailView();


        /**
         * Destructor
         */
	    virtual ~PaZoneDetailView();

        
        DECLARE_DYNCREATE(PaZoneDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(PaZoneDetailView)
    	enum { IDD = IDD_PA_ZONE_RECORD_VIEW };
    	CButton	            m_applyBtn;
    	TA_Base_Bus::TextColourEdit	    m_locationEdt;
    	TA_Base_Bus::TextColourEdit	    m_idEdt;
    	TA_Base_Bus::TextColourEdit	    m_labelEdt;
        TA_Base_Bus::TextColourEdit	    m_stnEquipmentEntityNameEdt;
    	TA_Base_Bus::TextColourEdit      m_occEquipmentEntityNameEdt;
        CButton	            m_locationBtn;
        CButton             m_stnEquipmentEntityNameBtn;
        CButton             m_occEquipmentEntityNameBtn;
    	CStatic	            m_modifyDate;
    	CStatic	            m_createDate;
        CButton	            m_type;
    	CTransparentImage   m_transparentImageLocation;
    	CTransparentImage   m_transparentImageId;
    	CTransparentImage   m_transparentImageLabel;
    	CTransparentImage   m_transparentImageSen;
        TA_Base_Bus::HighlightItemListCtrl           m_associationListCtrl;
        CButton	            m_addAssociationBtn;
    	CButton	            m_editAssociationBtn;
    	CButton	            m_removeAssociationBtn;
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
	    //{{AFX_VIRTUAL(PaZoneDetailView)
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
	    //{{AFX_MSG(PaZoneDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onButtonLocation();
	    afx_msg void onButtonStnEquipmentEntityName();
        afx_msg void onButtonOccEquipmentEntityName();
	    afx_msg void onButtonAddAssociation();
	    afx_msg void onButtonEditAssociation();
        afx_msg void onButtonRemoveAssociation();

        afx_msg void onKillfocusEditLocation();
        afx_msg void onKillfocusEditStnEquipmentEntityName();
        afx_msg void onKillfocusEditOccEquipmentEntityName();
        afx_msg void onKillFocusEditZoneId();
        afx_msg void onKillFocusEditLabel();

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
            return TA_Base_App::MessageSender::PA_ZONE;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        PaZoneDetailView( const PaZoneDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        PaZoneDetailView& operator=(const PaZoneDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the PA Zone details. These controls will be
         * hidden until the user selects a PA Zone from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        /**
         * refreshEntityAndEntityType
         *
         * Only one of these items can be enabled at once based on the radio button checked
         * This retrieves the data from the IConfigPaZone to determine which is enabled.
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
        /**
         * updateAssociations
         *
         * Updates the Equipment Entity Associations list
         * @param update the type of update to perform to the list
         *      uses current selection data within the equipment entity list control
         *      for the edit and remove operations
         */
        void    updateAssociations(EAssocUpdate update);

        /**
         * onLocationChanged
         *
         * Called whenever the operator changes the location of the zone
         *      (certain parameters become invalid in this situation)
         */
        void onLocationChanged();

        /**
         * stripExistingAssociationsFromMap
         *
         * Strips all the currently existing entity associations from the input map
         *
         * @param items[IN/OUT] the map which we are to modify (strip items from)
         * @param forceInclusion the string identifying an item we don't want
         *                  to be removed from 'items' (forced to be left in items map, if it exists there)
         */
        void stripExistingAssociationsFromMap(TA_Base_App::ChangeDialog::ItemsForSelection& items, CString forceInclusion = "");

    private:
        std::list<std::string> getAssociatedEquipmentEntities();
        void setAssociatedEquipmentEntity(const std::string& entityName, bool isAssociated);
        void updateListViewItemStatusForPaTisZoneMaps();

        // The titles for the change dialog box
        static const CString CHANGE_LOCATION_DIALOG_TITLE;
        static const CString CHANGE_ENTITY_NAME;
        static const CString SELECT_ASSOCIATION_ENTITY_NAME;
           
        static const int LOCATION_FIELD_LENGTH;
        static const int ZONE_ID_FIELD_LENGTH;
        static const int LABEL_FIELD_LENGTH;
        static const int EQUIPMENT_ENTITY_NAME_FIELD_LENGTH;

        TA_Base_Core::IConfigPaZone* m_configItem; // The current PA Zone the user
                                                              // is configuring. This is the database object
                                                              // for it.
        PaZoneDatabaseAccessor::PaTisZoneMapsRecord* m_paTisZoneMapsRecord;

        TA_Base_Core::IPaTisZoneMaps* m_configPaTisZoneMaps;
        TA_Base_Core::IPaTisZoneMaps m_paTisZoneMapsToBeRemoved;

        PaZoneListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all PA Zones. We need to
                                            // use this to tell it when PA Zone details have
                                            // changed.

        PaZoneDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.

        bool m_allEntityAssociationsValid;
    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_PaZoneDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
