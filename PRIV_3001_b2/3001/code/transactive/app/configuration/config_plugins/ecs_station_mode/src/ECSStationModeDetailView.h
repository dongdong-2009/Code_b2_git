/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/ecs_station_mode/src/ECSStationModeDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view allows the user to configure an ECS Station Mode's details.
  */

#if !defined(AFX_ECSStationModeDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_ECSStationModeDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/ecs_station_mode/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_IRS_Core
{
    class IConfigECSStationMode;
}


namespace TA_IRS_App
{
    class ECSStationModeDatabaseAccessor;
    class ECSStationModeListView;


    class ECSStationModeDetailView : public TA_Base_App::AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    ECSStationModeDetailView();


        /**
         * Destructor
         */
	    virtual ~ECSStationModeDetailView();

        
        DECLARE_DYNCREATE(ECSStationModeDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(ECSStationModeDetailView)
	enum { IDD = IDD_ECS_STATION_MODE_RECORD_VIEW };
	CButton	            m_applyBtn;
	TA_Base_Bus::TextColourEdit	    m_stationModeEdt;
	TA_Base_Bus::TextColourEdit	    m_locationEdt;
	TA_Base_Bus::TextColourEdit	    m_operationEdt;
	TA_Base_Bus::TextColourEdit	    m_trackEdt;
    TA_Base_Bus::TextColourEdit	    m_conditionEdt;
    CEdit	            m_logicEdt;
	CButton	            m_locationBtn;
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
	    //{{AFX_VIRTUAL(ECSStationModeDetailView)
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
	    //{{AFX_MSG(ECSStationModeDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onButtonLocation();
	    afx_msg void onKillfocusEditStationMode();
        afx_msg void onKillfocusEditLocation();
        afx_msg void onKillfocusEditOperation();
        afx_msg void onKillfocusEditTrack();
        afx_msg void onKillfocusEditCondition();
        afx_msg void onKillfocusEditLogic();
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
            return TA_Base_App::MessageSender::ECS_STATION_MODE;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ECSStationModeDetailView( const ECSStationModeDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ECSStationModeDetailView& operator=(const ECSStationModeDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the ECS Station Mode details. These controls will be
         * hidden until the user selects a ECS Station Mode from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        /**
         * refreshEntityAndEntityType
         *
         * Only one of these items can be enabled at once based on the radio button checked
         * This retrieves the data from the IConfigECSStationMode to determine which is enabled.
         * It then sets the correct check and enabled and disables the fields.
         */
        void refreshEntityAndEntityType();


    private:
        // The titles for the change dialog box
        static const CString CHANGE_LOCATION_DIALOG_TITLE;
        
        static const int KEY_FIELD_LENGTH;
        static const int STATION_MODE_FIELD_LENGTH;
        static const int LOCATION_FIELD_LENGTH;
        static const int OPERATION_FIELD_LENGTH;
        static const int TRACK_FIELD_LENGTH;
        static const int CONDITION_FIELD_LENGTH;
        static const int LOGIC_FIELD_LENGTH;

        TA_IRS_Core::IConfigECSStationMode* m_configItem; // The current ECS Station Mode the user
                                                              // is configuring. This is the database object
                                                              // for it.

        ECSStationModeListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all ECS Station Modes. We need to
                                            // use this to tell it when ECS Station Mode details have
                                            // changed.

        ECSStationModeDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_ECSStationModeDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
