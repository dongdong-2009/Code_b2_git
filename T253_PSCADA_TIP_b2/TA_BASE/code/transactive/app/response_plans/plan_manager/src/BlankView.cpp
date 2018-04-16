//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/BlankView.cpp $
// @author:  Bart Golab
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2015/10/15 18:32:45 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "BlankView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CBlankView

IMPLEMENT_DYNCREATE(CBlankView, CView)

CBlankView::CBlankView()
{
    FUNCTION_ENTRY( "CBlankView" );

    // {{AFX_DATA_INIT(CBlankView)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CBlankView::~CBlankView()
{
    FUNCTION_ENTRY( "~CBlankView" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CBlankView, CView)
    // {{AFX_MSG_MAP(CBlankView)
    ON_WM_ERASEBKGND()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBlankView message handlers

void CBlankView::OnDraw(CDC* pDC)
{
    FUNCTION_ENTRY( "OnDraw" );
    FUNCTION_EXIT;
}


BOOL CBlankView::OnEraseBkgnd(CDC* pDC)
{
    FUNCTION_ENTRY( "OnEraseBkgnd" );

    // TODO: Add your message handler code here and/or call default

    CBrush backBrush;
    backBrush.CreateSysColorBrush(COLOR_BTNFACE);

    CBrush* oldBrush = pDC->SelectObject(&backBrush);

    CRect clipBox;
    pDC->GetClipBox(clipBox);
    pDC->PatBlt(clipBox.left, clipBox.top, clipBox.Width(), clipBox.Height(), PATCOPY);
    pDC->SelectObject(oldBrush);

    backBrush.DeleteObject();

    FUNCTION_EXIT;
    return TRUE;
}


