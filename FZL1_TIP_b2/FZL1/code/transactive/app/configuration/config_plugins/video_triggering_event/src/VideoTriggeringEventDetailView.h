/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/video_triggering_event/src/VideoTriggeringEventDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an Video Station's details.
  */

#if !defined(AFX_VideoTriggeringEventDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_VideoTriggeringEventDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/video_triggering_event/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_IRS_Core
{
    class IConfigVideoTriggeringEvent;
}


namespace TA_IRS_App
{
    class VideoTriggeringEventDatabaseAccessor;
    class VideoTriggeringEventListView;


    class VideoTriggeringEventDetailView : public TA_Base_App::AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    VideoTriggeringEventDetailView();


        /**
         * Destructor
         */
	    virtual ~VideoTriggeringEventDetailView();

        
        DECLARE_DYNCREATE(VideoTriggeringEventDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(VideoTriggeringEventDetailView)
	    enum { IDD = IDD_VIDEO_TRIGGERING_EVENT_RECORD_VIEW };
    	CButton	            m_applyBtn;
        CStatic	            m_modifyDate;
    	CStatic	            m_createDate;

        CButton	            m_type;

        TA_Base_Bus::TextColourEdit      m_stationIdEdt;
        TA_Base_Bus::TextColourEdit      m_eventIdEdt;
        TA_Base_Bus::TextColourEdit	    m_locationEdt;
        TA_Base_Bus::TextColourEdit      m_triggeringEdt;
		TA_Base_Bus::TextColourEdit      m_alarmEdt; // TD15848 ajlaredo++

        CButton             m_manuallyClosedChk;

    	CTransparentImage   m_transparentImageId;
        CTransparentImage   m_transparentImageEvent;
    	CTransparentImage   m_transparentImageLocation;
        CTransparentImage   m_transparentImageTriggering;
		CTransparentImage   m_transparentImageAlarm; // TD15848 ajlaredo++
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
	    //{{AFX_VIRTUAL(VideoTriggeringEventDetailView)
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
	    //{{AFX_MSG(VideoTriggeringEventDetailView)
	    afx_msg void onButtonReset();
        afx_msg void onButtonApply();

        afx_msg void onButtonManualClosing();
        afx_msg void onButtonStation();
		afx_msg void onButtonAlarmEntity(); // TD15848 

        afx_msg void onKillFocusEditStationId();
        afx_msg void onKillFocusEditEventId();
        afx_msg void onKillFocusEditLocation();
	    afx_msg void onKillFocusEditTriggering();
		afx_msg void onKillFocusEditAlarm(); // TD15848
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
            return TA_Base_App::MessageSender::VI_TRIGGERING_EVENTS;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        VideoTriggeringEventDetailView( const VideoTriggeringEventDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        VideoTriggeringEventDetailView& operator=(const VideoTriggeringEventDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the Video Station details. These controls will be
         * hidden until the user selects a Video Station from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);    

    private:
        
        static const CString CHANGE_LOCATION_DIALOG_TITLE;
		static const CString CHANGE_ALARMTYPE_DIALOG_TITLE;

        static const int EVENT_ID_FIELD_LENGTH;
        static const int LOCATION_FIELD_LENGTH;
        static const int TRIGGERING_FIELD_LENGTH;
		static const int ALARM_FIELD_LENGTH; // TD15848
        
        TA_IRS_Core::IConfigVideoTriggeringEvent* m_configItem; // The current Video Station the user
                                                      // is configuring. This is the database object
                                                      // for it.

        VideoTriggeringEventListView* m_listView; // This is the view in the top pane of the splitter
                                          // showing the list of all Video Stations. We need to
                                          // use this to tell it when Video Station details have
                                          // changed.

        VideoTriggeringEventDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                           // information from the database.
    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_VideoTriggeringEventDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
