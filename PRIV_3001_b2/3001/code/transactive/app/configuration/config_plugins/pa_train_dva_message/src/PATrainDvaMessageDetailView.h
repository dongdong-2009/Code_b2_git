/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/pa_train_dva_message/src/PATrainDvaMessageDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view allows the user to configure an PA Train DVA Message's details.
  */

#if !defined(AFX_PaTrainDvaMessageDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_PaTrainDvaMessageDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/pa_train_dva_message/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_IRS_Core
{
    class IConfigPaTrainDvaMessage;
}


namespace TA_IRS_App
{
    class PaTrainDvaMessageDatabaseAccessor;
    class PaTrainDvaMessageListView;


    class PaTrainDvaMessageDetailView : public TA_Base_App::AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    PaTrainDvaMessageDetailView();


        /**
         * Destructor
         */
	    virtual ~PaTrainDvaMessageDetailView();

        
        DECLARE_DYNCREATE(PaTrainDvaMessageDetailView)

        /**
         * sendOutGlobalTrainVersionChangeNotification
         *
         * Used to send out special notification indicating that the
         *  global train version has been changed
         *  (via the PaTrainDvaMessageGlobalView window)
         */
        void sendOutGlobalTrainVersionChangeNotification();

    // Form Data
    public:
	    //{{AFX_DATA(PaTrainDvaMessageDetailView)
    	enum { IDD = IDD_PA_TRAIN_DVA_MESSAGE_RECORD_VIEW };
    	CButton	            m_applyBtn;
    	
        TA_Base_Bus::TextColourEdit      m_idEdt;
        TA_Base_Bus::TextColourEdit	    m_labelEdt;
        
        CStatic	            m_modifyDate;
    	CStatic	            m_createDate;
        CButton	            m_type;
    	CTransparentImage   m_transparentImageLabel;
        CTransparentImage   m_transparentImageId;
    	
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
	    //{{AFX_VIRTUAL(PaTrainDvaMessageDetailView)
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
	    //{{AFX_MSG(PaTrainDvaMessageDetailView)
	    afx_msg void onButtonReset();
        afx_msg void onButtonApply();
        	    
        afx_msg void onKillFocusEditId();
        afx_msg void onKillFocusEditLabel();

        afx_msg void OnClickZoneList(NMHDR* pNMHDR, LRESULT* pResult);
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
            return TA_Base_App::MessageSender::PA_TRAIN_DVA_MESSAGE;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        PaTrainDvaMessageDetailView( const PaTrainDvaMessageDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        PaTrainDvaMessageDetailView& operator=(const PaTrainDvaMessageDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the PA Train DVA Message details. These controls will be
         * hidden until the user selects a PA Train DVA Message from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);    

    private:
           
        static const int LABEL_FIELD_LENGTH;
        static const int EQUIPMENT_ENTITY_NAME_FIELD_LENGTH;

        TA_Base_Core::IConfigPaTrainDvaMessage* m_configItem; // The current PA Train DVA Message the user
                                                              // is configuring. This is the database object
                                                              // for it.

        PaTrainDvaMessageListView* m_listView; // This is the view in the top pane of the splitter
                                            // showing the list of all PA Train DVA Messages. We need to
                                            // use this to tell it when PA Train DVA Message details have
                                            // changed.

        PaTrainDvaMessageDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve 
                                                             // information from the database.
    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_PaTrainDvaMessageDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
