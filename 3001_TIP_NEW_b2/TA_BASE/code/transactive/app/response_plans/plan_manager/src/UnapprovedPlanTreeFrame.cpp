//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/UnapprovedPlanTreeFrame.cpp $
// @author:  Bart Golab
// @version: $Revision: #4 $
//
// Last modification: $DateTime: 2013/03/20 14:16:56 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#include "app/response_plans/plan_manager/src/StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_manager/src/resource.h"
#include "app/response_plans/plan_manager/src/MainFrame.h"
#include "app/response_plans/plan_manager/src/UnapprovedPlanTreeFrame.h"
#include "app/response_plans/plan_manager/src/UnapprovedPlanTreeView.h"
#include "app/response_plans/plan_manager/src/LabelMaker.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CUnapprovedPlanTreeFrame

IMPLEMENT_DYNCREATE(CUnapprovedPlanTreeFrame, CPlanTreeFrame)

CUnapprovedPlanTreeFrame::CUnapprovedPlanTreeFrame()
{
    FUNCTION_ENTRY( "CUnapprovedPlanTreeFrame" );
    FUNCTION_EXIT;
}


CUnapprovedPlanTreeFrame::~CUnapprovedPlanTreeFrame()
{
    FUNCTION_ENTRY( "~CUnapprovedPlanTreeFrame" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CUnapprovedPlanTreeFrame, CPlanTreeFrame)
    // {{AFX_MSG_MAP(CUnapprovedPlanTreeFrame)
    ON_WM_CREATE()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUnapprovedPlanTreeFrame message handlers

int CUnapprovedPlanTreeFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    FUNCTION_ENTRY( "OnCreate" );

    if (CPlanTreeFrame::OnCreate(lpCreateStruct) == -1)
    {
        FUNCTION_EXIT;
        return -1;
    }

    SetTitle(LabelMaker::getUnapprovedPlanTreeRootNodeLabel());

    // TODO: Add your specialized creation code here
    GetMainFrame()->AddPlanBrowserFrame(ID_VIEW_UNAPPROVEDPLANS, this);

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

    FUNCTION_EXIT;
    return 0;
}


CRuntimeClass *CUnapprovedPlanTreeFrame::GetLeftPaneRuntimeClass()
{
    FUNCTION_ENTRY( "GetLeftPaneRuntimeClass" );
    FUNCTION_EXIT;
    return RUNTIME_CLASS(CUnapprovedPlanTreeView);
}


