/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/ats_branch_track/src/AtsBranchTrackDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view allows the user to configure an Ats Branch Track's details.
  */

#if !defined(AFX_AtsBranchTrackDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_AtsBranchTrackDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/ats_branch_track/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_IRS_Core
{
    class IConfigAtsBranchTrack;
}


namespace TA_IRS_App
{
    class AtsBranchTrackDatabaseAccessor;
    class AtsBranchTrackListView;


    class AtsBranchTrackDetailView : public TA_Base_App::AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    AtsBranchTrackDetailView();


        /**
         * Destructor
         */
	    virtual ~AtsBranchTrackDetailView();

        
        DECLARE_DYNCREATE(AtsBranchTrackDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(AtsBranchTrackDetailView)
	enum { IDD = IDD_ATSBRANCHTRACKRECORD_VIEW };
	TA_Base_Bus::TextColourEdit		m_branchIdEdt;
	CButton	            m_applyBtn;
	CComboBox			m_trackCombo;
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
	    //{{AFX_VIRTUAL(AtsBranchTrackDetailView)
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
	    //{{AFX_MSG(AtsBranchTrackDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onKillfocusEditBranchId();
	afx_msg void onKillfocusComboTrack();
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
            return TA_Base_App::MessageSender::ATS_BRANCH_TRACK;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        AtsBranchTrackDetailView( const AtsBranchTrackDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        AtsBranchTrackDetailView& operator=(const AtsBranchTrackDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the Ats Branch Track details. These controls will be
         * hidden until the user selects a Ats Branch Track from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        /**
         * refreshEntityAndEntityType
         *
         * Only one of these items can be enabled at once based on the radio button checked
         * This retrieves the data from the IConfigAtsBranchTrack to determine which is enabled.
         * It then sets the correct check and enabled and disables the fields.
         */
        void refreshEntityAndEntityType();


    private:
        static const int ATS_BRANCH_ID_FIELD_LENGTH;
        static const int TRACK_FIELD_LENGTH;

        TA_IRS_Core::IConfigAtsBranchTrack* m_configItem; // The current Ats Branch Track the user
                                                              // is configuring. This is the database object
                                                              // for it.

        AtsBranchTrackListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all Ats Branch Tracks. We need to
                                            // use this to tell it when Ats Branch Track details have
                                            // changed.

        AtsBranchTrackDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_AtsBranchTrackDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
