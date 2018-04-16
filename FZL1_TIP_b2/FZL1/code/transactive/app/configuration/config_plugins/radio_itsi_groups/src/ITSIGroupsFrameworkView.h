/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsFrameworkView.h $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of ITSI groups
  * and the right side as a view for the user to configure ITSI group details.
  */

// ITSIGroupsFrameworkView.h: interface for the ITSIGroupsFrameworkView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITSIGROUPSFRAMEWORKVIEW_H__6225159E_273A_456D_BAA7_A6469A126257__INCLUDED_)
#define AFX_ITSIGROUPSFRAMEWORKVIEW_H__6225159E_273A_456D_BAA7_A6469A126257__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractFrameworkView.h"

namespace TA_IRS_App
{

	class ITSIGroupsFrameworkView : public TA_Base_App::AbstractFrameworkView  
	{

	public:

	protected:
        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
		ITSIGroupsFrameworkView();

	    DECLARE_DYNCREATE(ITSIGroupsFrameworkView)
		
		virtual ~ITSIGroupsFrameworkView();

    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

	    //{{AFX_MSG(ITSIGroupsFrameworkView)
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

        /**
         * getRuntimeClassForListView
         *
         * This returns the runtime class for the list view.
         *
         * @return CRuntimeClass* - The result of a RUNTIME_CLASS call on the
         *                          class name required.
         */
        virtual CRuntimeClass* getRuntimeClassForListView();


        /**
         * getRuntimeClassForDetailView
         *
         * This returns the runtime class for the detail view.
         *
         * @return CRuntimeClass* - The result of a RUNTIME_CLASS call on the
         *                          class name required.
         */
         virtual CRuntimeClass* getRuntimeClassForDetailView();

	};

} //end of namespace

#endif // !defined(AFX_ITSIGROUPSFRAMEWORKVIEW_H__6225159E_273A_456D_BAA7_A6469A126257__INCLUDED_)
