/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsDetailView.h $
  * @author:  Derrick Liew
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This view allows the user to configure a itsi-group's details.
  */

#if !defined(AFX_ITSIGROUPSDETAILVIEW_H__CFD6F0CD_1A13_4FEA_871B_ADCA9FC8C713__INCLUDED_)
#define AFX_ITSIGROUPSDETAILVIEW_H__CFD6F0CD_1A13_4FEA_871B_ADCA9FC8C713__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/radio_itsi_groups/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

// ITSIGroupsDetailView.h : header file
//

namespace TA_Base_Core
{
    class IConfigITSIGroup;
}

namespace TA_IRS_App
{
    class ITSIGroupsDatabaseAccessor;
    class ITSIGroupsListView;

	/////////////////////////////////////////////////////////////////////////////
	// ITSIGroupsDetailView dialog

	class ITSIGroupsDetailView : public TA_Base_App::AbstractDetailView
	{
	// Construction
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    ITSIGroupsDetailView();


        /**
         * Destructor
         */
	    virtual ~ITSIGroupsDetailView();

        
        DECLARE_DYNCREATE(ITSIGroupsDetailView)

	public:

	// Dialog Data
		//{{AFX_DATA(ITSIGroupsDetailView)
		enum { IDD = IDD_ITSI_GROUPS };
		CStatic	m_staticUsed;
	    CStatic	m_modifyDate;
	    CStatic	m_createDate;
	    CEdit	m_name;
	    CTransparentImage	m_nameBitmap;
		CString	m_usedText;
			// NOTE: the ClassWizard will add data members here
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
		//{{AFX_VIRTUAL(ITSIGroupsDetailView)
		public:
		virtual void OnInitialUpdate();

		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:

    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

	//}}AFX_MSG
		// Generated message map functions
		//{{AFX_MSG(ITSIGroupsDetailView)
			// NOTE: the ClassWizard will add member functions here
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onKillfocusEditName();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual TA_Base_App::MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            return TA_Base_App::MessageSender::RADIO_ITSI_GROUP;
        }

    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ITSIGroupsDetailView( const ITSIGroupsDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ITSIGroupsDetailView& operator=(const ITSIGroupsDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the ITSI group details. These controls will be
         * hidden until the user selects a ITSI group from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        /**
         * updateReadOnlyFields
         *
         * This will look at the type of ITSI group and enable or disable the fields as appropriate.
         * 
         */
        void updateReadOnlyFields();

    private:

        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;

		// The current ITSI group viewed / open from the database
		TA_Base_Core::IConfigITSIGroup* m_curITSIGp;

        ITSIGroupsListView* m_listView; // This is the view in the left pane of the splitter
                                     // showing the list of all ITSI groups. We need to use this to
                                     // tell it when ITSI group details have changed.

        ITSIGroupsDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                      // from the database.

	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
} // END OF NAMESPACE

#endif // !defined(AFX_ITSIGROUPSDETAILVIEW_H__CFD6F0CD_1A13_4FEA_871B_ADCA9FC8C713__INCLUDED_)
