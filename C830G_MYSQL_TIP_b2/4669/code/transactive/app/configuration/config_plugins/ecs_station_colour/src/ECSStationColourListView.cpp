/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourListView.cpp $
  * @author:  Andy Siow
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all alarm severities available to be configured. This view must communicate
  * with the ECSStationColourDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ecs_station_colour/src/StdAfx.h"
#include "app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourListView.h"
#include "app/configuration/config_plugins/ecs_station_colour/src/ECSStationColourComponent.h"
#include "app/configuration/config_plugins/ecs_station_colour/src/resource.h"

#include "bus/resources/resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{
    IMPLEMENT_DYNCREATE(ECSStationColourListView, CListView)


    ECSStationColourListView::ECSStationColourListView()
    {
		// 7197-Start
        //m_componentIcon = LoadIcon(GetModuleHandle(ECSStationColourComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ALARM_SEVERITY) );		
		m_componentIcon = LoadIcon(GetModuleHandle(ECSStationColourComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ECS_STATION_COLOR) );
		// 7197-End
		
    }


    ECSStationColourListView::~ECSStationColourListView()
    {
    }


BEGIN_MESSAGE_MAP(ECSStationColourListView, AbstractListView)
	//{{AFX_MSG_MAP(ECSStationColourListView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ECSStationColourListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    /////////////////////////////////////////////////////////////////////////////
    // ECSStationColourListView diagnostics

    #ifdef _DEBUG
    void ECSStationColourListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void ECSStationColourListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

