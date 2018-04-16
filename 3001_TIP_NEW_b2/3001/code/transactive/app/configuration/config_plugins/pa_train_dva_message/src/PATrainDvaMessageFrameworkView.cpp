/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/configuration/config_plugins/pa_train_dva_message/src/PATrainDvaMessageFrameworkView.cpp $
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
    const int PaTrainDvaMessageFrameworkView::TOP_PANE      = 0;
    const int PaTrainDvaMessageFrameworkView::BOTTOM_PANE   = 1;
    const int PaTrainDvaMessageFrameworkView::ZERO_COLUMN   = 0;
    const int PaTrainDvaMessageFrameworkView::ONE_COLUMN    = 1;

    const int PaTrainDvaMessageFrameworkView::TOP_PANE_SIZE = 200;

    IMPLEMENT_DYNCREATE(PaTrainDvaMessageFrameworkView, CView)


    PaTrainDvaMessageFrameworkView::PaTrainDvaMessageFrameworkView()
    {
    }


    PaTrainDvaMessageFrameworkView::~PaTrainDvaMessageFrameworkView()
    {
    }


BEGIN_MESSAGE_MAP(PaTrainDvaMessageFrameworkView, AbstractFrameworkView)
	//{{AFX_MSG_MAP(PaTrainDvaMessageFrameworkView)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    BOOL PaTrainDvaMessageFrameworkView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
    {
        FUNCTION_ENTRY("Create");

        if (CView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext))
        {
            // Create a splitter with 1 row and 2 columns
            m_splitter.CreateStatic(this,2,1,WS_CHILD|WS_VISIBLE);
            
            // Top pane shows splitter (don't create)
            /*m_splitter.CreateView(TOP_PANE,
                                    ZERO_COLUMN,
                                    RUNTIME_CLASS(splitter),
                                    CSize(0,0),
                                    NULL);*/
            // bottom pane showing detail view
            m_splitter.CreateView(BOTTOM_PANE,ZERO_COLUMN,
                                    getRuntimeClassForDetailView(),
                                    CSize(0,0),
                                    NULL);

            const int TOP_LEFT_PANEL_WIDTH = 760;

            // Create nested splitter at top position
            DWORD childSplitterNid = m_splitter.IdFromRowCol( 0, 0);
            m_topSplitter.CreateStatic( &m_splitter, 1, 2, WS_CHILD|WS_VISIBLE, childSplitterNid);

            // Create the views inside the splitter panes
            m_topSplitter.CreateView(0, 0, getRuntimeClassForListView(), CSize(0,0), NULL);
            m_topSplitter.CreateView(0, 1, RUNTIME_CLASS(PaTrainDvaMessageGlobalView), CSize(0,0), NULL);        

            // Resize the panes so they are visible
            m_topSplitter.SetColumnInfo(0, TOP_LEFT_PANEL_WIDTH, 0);
            m_topSplitter.RecalcLayout();            

            m_splitter.SetRowInfo( TOP_PANE, TOP_PANE_SIZE, 0 );
	        m_splitter.RecalcLayout();

            // Set this to be the splitter's parent so we get all it's messages
            m_splitter.SetParent(this);

            // Now retrieve the views that have been created                 
            TA_Base_App::IAllView* listView = dynamic_cast<TA_Base_App::IAllView*>(m_topSplitter.GetPane(0, 0));
            PaTrainDvaMessageGlobalView* globalVersionView = dynamic_cast<PaTrainDvaMessageGlobalView*>(m_topSplitter.GetPane(0, 1));
            TA_Base_App::IDetailView* detailView = dynamic_cast<TA_Base_App::IDetailView*>(m_splitter.GetPane(BOTTOM_PANE,ZERO_COLUMN));

            globalVersionView->setDetailView(detailView);
            
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


    void PaTrainDvaMessageFrameworkView::OnSize(UINT nType, int cx, int cy) 
    {
        CView::OnSize(nType, cx, cy);
        if (m_splitter.GetSafeHwnd())
        {
            m_splitter.MoveWindow(0,0,cx,cy);
        }
    }


    CRuntimeClass* PaTrainDvaMessageFrameworkView::getRuntimeClassForDetailView()
    {
        return RUNTIME_CLASS(PaTrainDvaMessageDetailView);
    }


    CRuntimeClass* PaTrainDvaMessageFrameworkView::getRuntimeClassForListView()
    {
        return RUNTIME_CLASS(PaTrainDvaMessageListView);
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // PaTrainDvaMessageFrameworkView diagnostics

    #ifdef _DEBUG
    void PaTrainDvaMessageFrameworkView::AssertValid() const
    {
	    AbstractFrameworkView::AssertValid();
    }

    void PaTrainDvaMessageFrameworkView::Dump(CDumpContext& dc) const
    {
	    AbstractFrameworkView::Dump(dc);
    }
    #endif //_DEBUG

}



