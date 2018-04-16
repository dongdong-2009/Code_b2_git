/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsFrameworkView.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of ITSI groups
  * and the right side as a view for the user to configure ITSI group details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/radio_itsi_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsFrameworkView.h"
#include "app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsListView.h"
#include "app/configuration/config_plugins/radio_itsi_groups/src/ITSIGroupsDetailView.h"

// ITSIGroupsFrameworkView.cpp: implementation of the ITSIGroupsFrameworkView class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    IMPLEMENT_DYNCREATE(ITSIGroupsFrameworkView, CView)


	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	ITSIGroupsFrameworkView::ITSIGroupsFrameworkView()
	{

	}

	ITSIGroupsFrameworkView::~ITSIGroupsFrameworkView()
	{

	}

	BEGIN_MESSAGE_MAP(ITSIGroupsFrameworkView, AbstractFrameworkView)
		//{{AFX_MSG_MAP(ITSIGroupsFrameworkView)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

    
    CRuntimeClass* ITSIGroupsFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(ITSIGroupsDetailView);
    }


    CRuntimeClass* ITSIGroupsFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(ITSIGroupsListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // ITSIFrameworkGroupsView diagnostics

    #ifdef _DEBUG
    void ITSIGroupsFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void ITSIGroupsFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

} // end of namespace
