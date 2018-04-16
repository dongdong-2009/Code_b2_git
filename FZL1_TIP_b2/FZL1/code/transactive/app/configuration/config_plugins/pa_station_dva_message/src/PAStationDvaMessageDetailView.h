/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/pa_station_dva_message/src/PAStationDvaMessageDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an PA Station DVA Message's details.
  */

#if !defined(AFX_PaStationDvaMessageDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_PaStationDvaMessageDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/pa_station_dva_message/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/mfc_extensions/src/coloured_controls/HighlightItemListCtrl.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigPaDvaMessage;
}

namespace TA_IRS_App
{
    class PaStationDvaMessageDatabaseAccessor;
    class PaStationDvaMessageListView;


    class PaStationDvaMessageDetailView : public TA_Base_App::AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    PaStationDvaMessageDetailView();


        /**
         * Destructor
         */
	    virtual ~PaStationDvaMessageDetailView();

        
        DECLARE_DYNCREATE(PaStationDvaMessageDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(PaStationDvaMessageDetailView)
    	enum { IDD = IDD_PA_STATION_DVA_MESSAGE_RECORD_VIEW };
    	CButton	            m_applyBtn;
    	
        TA_Base_Bus::TextColourEdit	    m_locationEdt;
    	TA_Base_Bus::TextColourEdit	    m_idEdt;
        TA_Base_Bus::TextColourEdit	    m_labelEdt;
        //CButton	            m_locationBtn;
        CComboBox           m_msgTypeCombo;

        // TA_Base_Bus::TextColourEdit has issues with read only states of these dialogs
        
        CStatic	            m_modifyDate;
    	CStatic	            m_createDate;
        CButton	            m_type;
    	CTransparentImage   m_transparentImageLocation;
        CTransparentImage   m_transparentImageId;
    	CTransparentImage   m_transparentImageLabel;
    	CTransparentImage   m_transparentImageType;
        
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
	    //{{AFX_VIRTUAL(PaStationDvaMessageDetailView)
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
	    //{{AFX_MSG(PaStationDvaMessageDetailView)
	    afx_msg void onButtonReset();
        afx_msg void onButtonApply();
        
	    afx_msg void onButtonLocation();
        afx_msg void onKillfocusEditLocation();
        afx_msg void onKillFocusEditId();
        afx_msg void onKillFocusEditLabel();
        afx_msg void onSelchangeTypeCombo();

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
            return TA_Base_App::MessageSender::PA_DVA_MESSAGE;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        PaStationDvaMessageDetailView( const PaStationDvaMessageDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        PaStationDvaMessageDetailView& operator=(const PaStationDvaMessageDetailView&);

        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the PA Station DVA Message details. These controls will be
         * hidden until the user selects a PA Station DVA Message from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);   

        /**
         * onLocationChanged
         *
         * Called whenever the operator changes the location of the zone
         *      (certain parameters become invalid in this situation)
         */
        void onLocationChanged();
        
    private:
        // The titles for the change dialog box
        static const CString CHANGE_LOCATION_DIALOG_TITLE;
           
        static const int LOCATION_FIELD_LENGTH;
        static const int ID_FIELD_LENGTH;
        static const int LABEL_FIELD_LENGTH;
        
        TA_Base_Core::IConfigPaDvaMessage* m_configItem; // The current PA Station DVA Message the user
                                                              // is configuring. This is the database object
                                                              // for it.

        PaStationDvaMessageListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all PA Station DVA Messages. We need to
                                            // use this to tell it when PA Station DVA Message details have
                                            // changed.

        PaStationDvaMessageDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.
    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_PaStationDvaMessageDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
