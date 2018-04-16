/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/pa_ats_triggered_broadcast/src/PAAtsTriggeredBroadcastDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view allows the user to configure an PA ATS Triggered Broadcast's details.
  */

#if !defined(AFX_PaAtsTriggeredBroadcastDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_PaAtsTriggeredBroadcastDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/pa_ats_triggered_broadcast/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/mfc_extensions/src/coloured_controls/HighlightItemListCtrl.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigPaAtsTriggeredBroadcast;
}

namespace TA_IRS_App
{
    class PaAtsTriggeredBroadcastDatabaseAccessor;
    class PaAtsTriggeredBroadcastListView;


    class PaAtsTriggeredBroadcastDetailView : public TA_Base_App::AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    PaAtsTriggeredBroadcastDetailView();


        /**
         * Destructor
         */
	    virtual ~PaAtsTriggeredBroadcastDetailView();

        
        DECLARE_DYNCREATE(PaAtsTriggeredBroadcastDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(PaAtsTriggeredBroadcastDetailView)
    	enum { IDD = IDD_PA_ATS_TRIGGERED_BROADCAST_RECORD_VIEW };
    	CButton	            m_applyBtn;
    	
        TA_Base_Bus::TextColourEdit	    m_locationEdt;
    	TA_Base_Bus::TextColourEdit	    m_destLocationEdt;
        TA_Base_Bus::TextColourEdit      m_zoneGroupEdt;      // Dependent on location
        TA_Base_Bus::TextColourEdit      m_platformIdEdt;    // Dependent on location

        TA_Base_Bus::TextColourEdit      m_dvaMessageEdt;    // Dependent on location
        CEdit               m_dvaVersionEdt;
        TA_Base_Bus::TextColourEdit      m_eventTypeEdt;
               
        CButton             m_isLastTrainCheck;
        CButton             m_isEnabledCheck;
        
        CStatic	            m_modifyDate;
    	CStatic	            m_createDate;
        CButton	            m_type;

        CTransparentImage   m_transparentImageLocation;
        CTransparentImage   m_transparentImageDestLocation;
    	CTransparentImage   m_transparentImageZoneGroup;
    	CTransparentImage   m_transparentImagePlatformId;
    	CTransparentImage   m_transparentImageDvaMessage;
    	CTransparentImage   m_transparentImageDvaVersion;
    	CTransparentImage   m_transparentImageEventType;
    	
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
	    //{{AFX_VIRTUAL(PaAtsTriggeredBroadcastDetailView)
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
	    //{{AFX_MSG(PaAtsTriggeredBroadcastDetailView)
	    afx_msg void onButtonReset();
        afx_msg void onButtonApply();
        
	    afx_msg void onButtonLocation();
	    afx_msg void onButtonDestinationLocation();
        afx_msg void onButtonZoneGroup();
        afx_msg void onButtonPlatformId();
        afx_msg void onButtonStationDvaMessage();
        afx_msg void onButtonEventType();

        afx_msg void onButtonIsLastTrain();
        afx_msg void onButtonIsEnabled();

        afx_msg void onKillfocusEditLocation();
        afx_msg void onKillfocusEditDestLocation();
        
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
            return TA_Base_App::MessageSender::PA_ATS_TRIGGERED_BROADCAST;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        PaAtsTriggeredBroadcastDetailView( const PaAtsTriggeredBroadcastDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        PaAtsTriggeredBroadcastDetailView& operator=(const PaAtsTriggeredBroadcastDetailView&);

        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the PA ATS Triggered Broadcast details. These controls will be
         * hidden until the user selects a PA ATS Triggered Broadcast from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);   
        
        /**
         * checkDvaMessageVersionOnApply
         *
         * Checks that the DVA Message version information is up to date
         *      with data in database, displays warning if it is not
         */
        void checkDvaMessageVersionOnApply();

        /**
         * onLocationChanged
         *
         * Called whenever the operator changes the location of the zone
         *      (certain parameters become invalid in this situation)
         */
        void onLocationChanged();
        
        /**
         * Called whenever the focus is lost on the input
         *  'location' edit control
         *
         * @return true if the location was changed, and changes accepted
         */
        bool onKillfocusLocation(TA_Base_Bus::TextColourEdit& editControl);

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
        void doSelectionDialog(PaAtsTriggeredBroadcastDatabaseAccessor::EDataSet dataSet,
                                bool filterBySourceLocation);        

    private:

        /**
         * refreshZoneGroupEdit - refreshes the zone group edit control with
         *  the up to date value present in internally contained config item
         */
        void refreshZoneGroupEdit();

        /**
         * refreshPlatformIdEdit - refreshes the platform ID edit control with
         *  the up to date value present in internally contained config item
         */
        void refreshPlatformIdEdit();
        
        /**
         * refreshDvaMessageEdit - refreshes the DVA Message edit control with
         *  the up to date value present in internally contained config item
         */
        void refreshDvaMessageEdit();
        
        /**
         * refreshEventTypeEdit - refreshes the event type edit control with
         *  the up to date value present in internally contained config item
         */
        void refreshEventTypeEdit();

        // The titles for the change dialog box
        static const CString CHANGE_LOCATION_DIALOG_TITLE;
        static const CString SELECT_ZONE_GROUP_TITLE;
        static const CString SELECT_PLATFORM_ID_TITLE;
        static const CString SELECT_DVA_MESSAGE_TITLE;
        static const CString SELECT_EVENT_TYPE_TITLE;
           
        static const int LOCATION_FIELD_LENGTH;
                
        TA_Base_Core::IConfigPaAtsTriggeredBroadcast* m_configItem; // The current PA ATS Triggered Broadcast the user
                                                              // is configuring. This is the database object
                                                              // for it.

        PaAtsTriggeredBroadcastListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all PA ATS Triggered Broadcasts. We need to
                                            // use this to tell it when PA ATS Triggered Broadcast details have
                                            // changed.

        PaAtsTriggeredBroadcastDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.
    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_PaAtsTriggeredBroadcastDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
