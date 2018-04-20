/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/configuration/config_plugins/ecs_zone/src/ECSZoneFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the top of the splitter as a list of ECS Zones
  * and the bottom as a view for the user to configure a single ECS Zone details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ecs_zone/src/StdAfx.h"
#include "app/configuration/config_plugins/ecs_zone/src/ECSZoneFrameworkView.h"

#include "app/configuration/config_plugins/ecs_zone/src/ECSZoneListView.h"
#include "app/configuration/config_plugins/ecs_zone/src/ECSZoneDetailView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    // These constants are used to pass to the splitter methods so the code is more readable
    const int ECSZoneFrameworkView::TOP_PANE      = 0;
    const int ECSZoneFrameworkView::BOTTOM_PANE   = 1;
    const int ECSZoneFrameworkView::ZERO_COLUMN   = 0;

    const int ECSZoneFrameworkView::TOP_PANE_SIZE = 200;

    IMPLEMENT_DYNCREATE(ECSZoneFrameworkView, CView)


    ECSZoneFrameworkView::ECSZoneFrameworkView()
    {
    }


    ECSZoneFrameworkView::~ECSZoneFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(ECSZoneFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(ECSZoneFrameworkView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    BOOL ECSZoneFrameworkView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
    {
        FUNCTION_ENTRY("Create");

        if (CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
        {
            // Create a splitter with 1 row and 2 columns
            m_splitter.CreateStatic(this,2,1,WS_CHILD|WS_VISIBLE);

            // Create the views inside the splitter panes
            m_splitter.CreateView(TOP_PANE,ZERO_COLUMN,getRuntimeClassForListView(),CSize(0,0),NULL);
            m_splitter.CreateView(BOTTOM_PANE,ZERO_COLUMN,getRuntimeClassForDetailView(),CSize(0,0),NULL);

            // Resize the panes so they are visible
            m_splitter.SetRowInfo( TOP_PANE, TOP_PANE_SIZE, 0 );
	        m_splitter.RecalcLayout();

            // Set this to be the splitter's parent so we get all it's messages
            m_splitter.SetParent(this);

            // Now retrieve the views that have been created
            TA_Base_App::IAllView* listView = dynamic_cast<TA_Base_App::IAllView*>(m_splitter.GetPane(TOP_PANE,ZERO_COLUMN));
            TA_Base_App::IDetailView* detailView = dynamic_cast<TA_Base_App::IDetailView*>(m_splitter.GetPane(BOTTOM_PANE,ZERO_COLUMN));

            setListView(*listView);
            setDetailView(*detailView);
    
            // Let the views know about each other so they can 'talk' to each other.
            if (listView != NULL)
            {
                listView->setDetailView( detailView );
            }
            if (detailView != NULL)
            {
                detailView->setListView( listView );
            }

        }

        FUNCTION_EXIT;
        return TRUE;
    }


    void ECSZoneFrameworkView::OnSize(UINT nType, int cx, int cy) 
    {
        CView::OnSize(nType, cx, cy);
        if (m_splitter.GetSafeHwnd())
        {
            m_splitter.MoveWindow(0,0,cx,cy);
        }
    }


    CRuntimeClass* ECSZoneFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(ECSZoneDetailView);
    }


    CRuntimeClass* ECSZoneFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(ECSZoneListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // ECSZoneFrameworkView diagnostics

    #ifdef _DEBUG
    void ECSZoneFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void ECSZoneFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}



