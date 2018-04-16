/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourFrameworkView.cpp $
  * @author:  Andy Siow
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of alarm severities
  * and the right side as a view for the user to configure alarm severity details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ecs_station_colour/src/StdAfx.h"
#include "app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourFrameworkView.h"

#include "app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourListView.h"
#include "app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourDetailView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{
    IMPLEMENT_DYNCREATE(ECSStationColourFrameworkView, CFormView)


    ECSStationColourFrameworkView::ECSStationColourFrameworkView()
    {
    }


    ECSStationColourFrameworkView::~ECSStationColourFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(ECSStationColourFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(ECSStationColourFrameworkView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    CRuntimeClass* ECSStationColourFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(ECSStationColourDetailView);
    }


    CRuntimeClass* ECSStationColourFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(ECSStationColourListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // ECSStationColourFrameworkView diagnostics

    #ifdef _DEBUG
    void ECSStationColourFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void ECSStationColourFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}


