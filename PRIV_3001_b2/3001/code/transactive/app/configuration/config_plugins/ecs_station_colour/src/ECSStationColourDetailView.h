/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourDetailView.h $
  * @author:  Andy Siow
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view allows the user to configure a ECSStationColour's details.
  */

#if !defined(AFX_ECSStationColourDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_ECSStationColourDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/ecs_station_colour/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigECSStationColourData;
}


namespace TA_IRS_App
{
    class ECSStationColourDatabaseAccessor;
    class ECSStationColourListView;


    class ECSStationColourDetailView : public TA_Base_App::AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    ECSStationColourDetailView();


        /**
         * Destructor
         */
	    virtual ~ECSStationColourDetailView();

        
        DECLARE_DYNCREATE(ECSStationColourDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(ECSStationColourDetailView)
	enum { IDD = IDD_ECS_STATION_COLOUR_VIEW };
	CTransparentImage	m_nameBitmap;
    CTransparentImage	m_colourBitmap;
	CStatic	m_type;
    CStatic	m_modifyDate;
    CStatic	m_createDate;
	CEdit	m_name;
    CEdit   m_colour;
	//}}AFX_DATA


        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the ECSStationColour we want to display. We will use this to
         *                         set new values and apply. If this is NULL then the details should all be hidden
         */
        void populateItemDetails(TA_Base_Core::IConfigItem* currentECSStationColour);


        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         */
        virtual void refreshButtonAvailability();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ECSStationColourDetailView)
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
	    //{{AFX_MSG(ECSStationColourDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
        afx_msg void onButtonColour();
	    afx_msg void onKillfocusEditName();
        afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    protected:

        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, ECS_STATION_COLOUR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual TA_Base_App::MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            return TA_Base_App::MessageSender::ECS_STATION_COLOUR;
        }

        /**
         *  Sets the background colour of the Colour edit field.
         */
        void ECSStationColourDetailView::setColourEditBackground(COLORREF colour);

    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ECSStationColourDetailView( const ECSStationColourDetailView&);


        /**
         * ECSStationColour =
         *
         * Hidden as a private method so it is not used.
         */
        ECSStationColourDetailView& operator=(const ECSStationColourDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the ECSStationColour details. These controls will be
         * hidden until the user selects a ECSStationColour from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);

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

        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;
        static const int SOUND_FIELD_LENGTH;


        // The title of the select sound dialog
        static const char* SELECT_SOUND_DIALOG_TITLE;


        // The brush used to paint the Colour edit control
        CBrush* m_editBackgroundBrush;


        TA_Base_Core::IConfigECSStationColourData* m_currentECSStationColour;   // The current ECSStationColour the user is configuring.
                                                   // This is the database object for it.

        ECSStationColourListView* m_listView; // This is the view in the left pane of the splitter
                                    // showing the list of all alarm severities. We need to use this to
                                    // tell it when ECSStationColour details have changed.

        ECSStationColourDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                     // from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_ECSStationColourDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
