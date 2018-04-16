/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/pa_zone_group/src/PAZoneGroupDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an PA Zone Group's details.
  */

#if !defined(AFX_PaZoneGroupDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_PaZoneGroupDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/mfc_extensions/src/coloured_controls/HighlightItemListCtrl.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigPaZoneGroup;
}


namespace TA_IRS_App
{
    class PaZoneGroupDatabaseAccessor;
    class PaZoneGroupListView;


    class PaZoneGroupDetailView : public TA_Base_App::AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    PaZoneGroupDetailView();


        /**
         * Destructor
         */
	    virtual ~PaZoneGroupDetailView();

        
        DECLARE_DYNCREATE(PaZoneGroupDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(PaZoneGroupDetailView)
    	enum { IDD = IDD_PA_ZONE_GROUP_RECORD_VIEW };
    	CButton	            m_applyBtn;
    	TA_Base_Bus::TextColourEdit	    m_locationEdt;
    	TA_Base_Bus::TextColourEdit	    m_labelEdt;
        CButton	            m_locationBtn;
        CButton             m_isEventGroupBtn;
        CStatic	            m_modifyDate;
    	CStatic	            m_createDate;
        CButton	            m_type;
    	CTransparentImage   m_transparentImageLocation;
    	CTransparentImage   m_transparentImageLabel;
    	TA_Base_Bus::HighlightItemListCtrl           m_zoneListCtrl;
        CButton	            m_addZoneBtn;
    	CButton	            m_removeZoneBtn;
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
	    //{{AFX_VIRTUAL(PaZoneGroupDetailView)
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
	    //{{AFX_MSG(PaZoneGroupDetailView)
	    afx_msg void onButtonReset();
        afx_msg void onButtonApply();
        afx_msg void onButtonIsEventGroup();
	    afx_msg void onButtonLocation();
	    afx_msg void onButtonAddZone();
	    afx_msg void onButtonRemoveZone();
        afx_msg void onKillFocusEditLabel();
        afx_msg void OnClickZoneList(NMHDR* pNMHDR, LRESULT* pResult);
        //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    protected:

        /**
         * sendMessagesForTheChange (base class extension)
         *
         * Do some re-processing of update notifications, PA Zone Groups have some special case
         *  situations that require handling
         */
        virtual void sendMessagesForTheChange(TA_Base_App::MessageSender::UpdateDetails updateDetails);

        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual TA_Base_App::MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            return TA_Base_App::MessageSender::PA_ZONE_GROUP;
        }

    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        PaZoneGroupDetailView( const PaZoneGroupDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        PaZoneGroupDetailView& operator=(const PaZoneGroupDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the PA Zone Group details. These controls will be
         * hidden until the user selects a PA Zone Group from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);
    
        /**
         * populateZoneListArea
         *
         * Refreshes the list control showing all the associated entities
         */
        void populateZoneListArea(bool initialisation = false);
        
        enum EZoneUpdate
        {
            ADD_NEW,            
            REMOVE_EXISTING
        };

        /**
         * updateZoneList
         *
         * Updates the Equipment Entity Associations list
         * @param update the type of update to perform to the list
         *      uses current selection data within the equipment entity list control
         *      for the edit and remove operations
         */
        void    updateZoneList(EZoneUpdate update);

        /**
         * onLocationChanged
         *
         * Called whenever the operator changes the location of the zone
         *      (certain parameters become invalid in this situation)
         */
        void onLocationChanged();

        /**
         * getCurrentZones
         *
         * @return a set of zones corresponding to the zones current associated with the
         *      m_configItem object
         * @pre m_configItem is not null
         */
        std::set<unsigned long> getCurrentZones();

        /**
         * popupZoneSelectionDialog
         *
         * Pops up a multiple selection dialog allowing the operator to select a 
         *  range of zones
         * 
         * @return a list of the primary keys of the zones selected in the popup
         *
         */
        std::list<unsigned long> popupZoneSelectionDialog();

    private:
        // The titles for the change dialog box
        static const CString CHANGE_LOCATION_DIALOG_TITLE;
        static const CString SELECT_ZONES_TITLE;
           
        static const int LOCATION_FIELD_LENGTH;
        static const int LABEL_FIELD_LENGTH;
        static const int EQUIPMENT_ENTITY_NAME_FIELD_LENGTH;

        TA_Base_Core::IConfigPaZoneGroup* m_configItem; // The current PA Zone Group the user
                                                              // is configuring. This is the database object
                                                              // for it.

        PaZoneGroupListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all PA Zone Groups. We need to
                                            // use this to tell it when PA Zone Group details have
                                            // changed.

        PaZoneGroupDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.

        bool m_allZonesValid;

        /// Holds the creation time of the contained m_configItem object, at the time
        /// when apply changes button was pressed
        time_t  m_applyCreationTime;

        CFont m_fntZoneList;
    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_PaZoneGroupDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
