/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsListView.h $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all ITSIs available to be configured. This view must communicate
  * with the ITSIDetailView as this will show the details of the item selected in this view.
  */

// ITSIGroupsListView.h: interface for the ITSIGroupsListView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITSIGROUPSLISTVIEW_H__F016B451_DCC0_4ADB_B922_7718A9371E26__INCLUDED_)
#define AFX_ITSIGROUPSLISTVIEW_H__F016B451_DCC0_4ADB_B922_7718A9371E26__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxcview.h>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h"

namespace TA_IRS_App
{

	class ITSIGroupsListView : public TA_Base_App::AbstractListView  
	{

	public:

        /**
         * deleteItem
         *
         * This requires an item to be selected. This warns the user about what they are deleting and then
         * calls the base class version of this method. It also checks that the current ITSI is not a system
         * ITSI as these cannot be deleted.
         */
        virtual void deleteItem();


        /**
         * copyItem
         *
         * This requires an item to be selected. This method also checks to see if the current ITSI
         * is a system only ITSI before copying. It then calls the base class version of this method.
         */
        virtual void copyItem();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ITSIGroupsListView)
	    public:
	    protected:
	    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	    //}}AFX_VIRTUAL

	protected:

        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
		ITSIGroupsListView();
		
		virtual ~ITSIGroupsListView();

	    DECLARE_DYNCREATE(ITSIGroupsListView)

    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

	    //{{AFX_MSG(ITSIGroupsListView)
    	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

        /**
         * getComponentIcon
         *
         * This should return the component icon for items to be shown in the list
         *
         * @return HICON - The icon to be shown
         */
        virtual HICON getComponentIcon()
        {
            return m_componentIcon;
        }

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
        HICON m_componentIcon;
	};

} // END OF NAMESPACE

#endif // !defined(AFX_ITSIGROUPSLISTVIEW_H__F016B451_DCC0_4ADB_B922_7718A9371E26__INCLUDED_)
