//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/AllPlanTreeFrame.cpp $
// @author:  Bart Golab
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2015/01/08 17:33:12 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#include "app/response_plans/plan_manager/src/StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_manager/src/resource.h"
#include "app/response_plans/plan_manager/src/MainFrame.h"
#include "app/response_plans/plan_manager/src/AllPlanTreeFrame.h"
#include "app/response_plans/plan_manager/src/AllPlanTreeView.h"
#include "app/response_plans/plan_manager/src/LabelMaker.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/ApplicationFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CAllPlanTreeFrame

IMPLEMENT_DYNCREATE(CAllPlanTreeFrame, CPlanTreeFrame)

CAllPlanTreeFrame::CAllPlanTreeFrame()
{
    FUNCTION_ENTRY( "CAllPlanTreeFrame" );
    FUNCTION_EXIT;
}


CAllPlanTreeFrame::~CAllPlanTreeFrame()
{
    FUNCTION_ENTRY( "~CAllPlanTreeFrame" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CAllPlanTreeFrame, CPlanTreeFrame)
    // {{AFX_MSG_MAP(CAllPlanTreeFrame)
    ON_WM_CREATE()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAllPlanTreeFrame message handlers

int CAllPlanTreeFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    FUNCTION_ENTRY( "OnCreate" );

    if (CPlanTreeFrame::OnCreate(lpCreateStruct) == -1)
    {
        FUNCTION_EXIT;
        return -1;
    }

    SetTitle(LabelMaker::getAllPlanTreeRootNodeLabel());

    // TODO: Add your specialized creation code here
    GetMainFrame()->AddPlanBrowserFrame(ID_VIEW_ALLPLANS, this);

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

    FUNCTION_EXIT;
    return 0;
}

CRuntimeClass *CAllPlanTreeFrame::GetLeftPaneRuntimeClass()
{
    FUNCTION_ENTRY( "GetLeftPaneRuntimeClass" );
    FUNCTION_EXIT;
    return RUNTIME_CLASS(CAllPlanTreeView);
}


