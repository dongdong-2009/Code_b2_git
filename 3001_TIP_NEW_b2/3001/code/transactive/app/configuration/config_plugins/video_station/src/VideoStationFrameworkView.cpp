/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/video_station/src/VideoStationFrameworkView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This view contains a splitter. It then loads the top of the splitter as a list of records
  * and the bottom as a view for the user to configure a single records details.
  */

#pragma warning(disable:4786 4284)

#include "StdAfx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
    // These constants are used to pass to the splitter methods so the code is more readable
    const int VideoStationFrameworkView::TOP_PANE      = 0;
    const int VideoStationFrameworkView::BOTTOM_PANE   = 1;
    const int VideoStationFrameworkView::ZERO_COLUMN   = 0;
    
    const int VideoStationFrameworkView::TOP_PANE_SIZE = 250;

    IMPLEMENT_DYNCREATE(VideoStationFrameworkView, CView)


    VideoStationFrameworkView::VideoStationFrameworkView()
    {
    }


    VideoStationFrameworkView::~VideoStationFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(VideoStationFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(VideoStationFrameworkView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    BOOL VideoStationFrameworkView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
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


    void VideoStationFrameworkView::OnSize(UINT nType, int cx, int cy) 
    {
        CView::OnSize(nType, cx, cy);
        if (m_splitter.GetSafeHwnd())
        {
            m_splitter.MoveWindow(0,0,cx,cy);
        }
    }


    CRuntimeClass* VideoStationFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(VideoStationDetailView);
    }


    CRuntimeClass* VideoStationFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(VideoStationListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // VideoStationFrameworkView diagnostics

    #ifdef _DEBUG
    void VideoStationFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void VideoStationFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}



