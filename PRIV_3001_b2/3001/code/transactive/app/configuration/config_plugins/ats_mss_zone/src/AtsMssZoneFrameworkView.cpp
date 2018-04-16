/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/configuration/config_plugins/ats_mss_zone/src/AtsMssZoneFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This view contains a splitter. It then loads the top of the splitter as a list of Ats MSS Zones
  * and the bottom as a view for the user to configure a single Ats MSS Zone details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/ats_mss_zone/src/StdAfx.h"
#include "app/configuration/config_plugins/ats_mss_zone/src/AtsMssZoneFrameworkView.h"

#include "app/configuration/config_plugins/ats_mss_zone/src/AtsMssZoneListView.h"
#include "app/configuration/config_plugins/ats_mss_zone/src/AtsMssZoneDetailView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    // These constants are used to pass to the splitter methods so the code is more readable
    const int AtsMssZoneFrameworkView::TOP_PANE      = 0;
    const int AtsMssZoneFrameworkView::BOTTOM_PANE   = 1;
    const int AtsMssZoneFrameworkView::ZERO_COLUMN   = 0;

    const int AtsMssZoneFrameworkView::TOP_PANE_SIZE = 200;

    IMPLEMENT_DYNCREATE(AtsMssZoneFrameworkView, CView)


    AtsMssZoneFrameworkView::AtsMssZoneFrameworkView()
    {
    }


    AtsMssZoneFrameworkView::~AtsMssZoneFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(AtsMssZoneFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(AtsMssZoneFrameworkView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    BOOL AtsMssZoneFrameworkView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
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


    void AtsMssZoneFrameworkView::OnSize(UINT nType, int cx, int cy) 
    {
        CView::OnSize(nType, cx, cy);
        if (m_splitter.GetSafeHwnd())
        {
            m_splitter.MoveWindow(0,0,cx,cy);
        }
    }


    CRuntimeClass* AtsMssZoneFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(AtsMssZoneDetailView);
    }


    CRuntimeClass* AtsMssZoneFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(AtsMssZoneListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // AtsMssZoneFrameworkView diagnostics

    #ifdef _DEBUG
    void AtsMssZoneFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void AtsMssZoneFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}



