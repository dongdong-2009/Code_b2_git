/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/ats_dss_plan_map/src/AtsDssPlanMapDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This view allows the user to configure an DSS Plan Map's details.
  */

#if !defined(AFX_AtsDssPlanMapDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_AtsDssPlanMapDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/ats_dss_plan_map/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_IRS_Core
{
    class IConfigAtsDssPlanMap;
}


namespace TA_IRS_App
{
    class AtsDssPlanMapDatabaseAccessor;
    class AtsDssPlanMapListView;


    class AtsDssPlanMapDetailView : public TA_Base_App::AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    AtsDssPlanMapDetailView();


        /**
         * Destructor
         */
	    virtual ~AtsDssPlanMapDetailView();

        
        DECLARE_DYNCREATE(AtsDssPlanMapDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(AtsDssPlanMapDetailView)
	enum { IDD = IDD_DSSPLANMAP_VIEW };
        TA_Base_Bus::TextColourEdit	m_plan;
        TA_Base_Bus::TextColourEdit	m_dssEvent;
        CTransparentImage   m_bitmapPlan;
        CTransparentImage	m_bitmapDssEvent;
	    CButton	m_type;
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
	    //{{AFX_VIRTUAL(AtsDssPlanMapDetailView)
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
	    //{{AFX_MSG(AtsDssPlanMapDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
		afx_msg void onButtonPlan();
		afx_msg void onKillfocusEditDssEvent();
		afx_msg void onKillfocusEditPlan();
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
            return TA_Base_App::MessageSender::ALARM_PLAN_ASSOCIATION;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        AtsDssPlanMapDetailView( const AtsDssPlanMapDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        AtsDssPlanMapDetailView& operator=(const AtsDssPlanMapDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the DSS Plan Map details. These controls will be
         * hidden until the user selects a DSS Plan Map from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);

    private:
        static const int PLAN_FIELD_LENGTH;
        static const int DSSEVENT_FIELD_LENGTH;

        TA_IRS_Core::IConfigAtsDssPlanMap* m_currentAtsDssPlanMap; // The current DSS Plan Map the user
                                                              // is configuring. This is the database object
                                                              // for it.

        AtsDssPlanMapListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all DSS Plan Maps. We need to
                                            // use this to tell it when DSS Plan Map details have
                                            // changed.

        AtsDssPlanMapDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_AtsDssPlanMapDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
