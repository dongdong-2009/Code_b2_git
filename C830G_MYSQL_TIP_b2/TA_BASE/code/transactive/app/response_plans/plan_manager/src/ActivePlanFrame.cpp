//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/ActivePlanFrame.cpp $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2008/11/28 16:26:01 $
// Last modified by:  $Author: builder $
//
// <description>

#include "app/response_plans/plan_manager/src/StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_manager/src/resource.h"
#include "app/response_plans/plan_manager/src/MainFrame.h"
#include "app/response_plans/plan_manager/src/ActivePlanFrame.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CActivePlanFrame

IMPLEMENT_DYNCREATE(CActivePlanFrame, CPlanBrowserFrame)

CActivePlanFrame::CActivePlanFrame()
{
    FUNCTION_ENTRY( "CActivePlanFrame" );
    FUNCTION_EXIT;
}


CActivePlanFrame::~CActivePlanFrame()
{
    FUNCTION_ENTRY( "~CActivePlanFrame" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CActivePlanFrame, CPlanBrowserFrame)
    // {{AFX_MSG_MAP(CActivePlanFrame)
    ON_WM_CREATE()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActivePlanFrame message handlers

int CActivePlanFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    FUNCTION_ENTRY( "OnCreate" );

    if (CPlanBrowserFrame::OnCreate(lpCreateStruct) == -1)
    {
        FUNCTION_EXIT;
        return -1;
    }

    SetTitle("Active Plans");

    // TODO: Add your specialized creation code here
    GetMainFrame()->AddPlanBrowserFrame(ID_VIEW_ACTIVEPLANS, this);

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

    FUNCTION_EXIT;
    return 0;
}


