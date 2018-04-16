//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/FloatingDocTemplate.cpp $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#include "stdafx.h"
#include "FloatingDocTemplate.h"
#include "PlanNode.h"

using namespace TA_Base_App;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFloatingDocTemplate::CFloatingDocTemplate(UINT nIDResource, CRuntimeClass* pDocClass, CRuntimeClass* pFrameClass, CRuntimeClass* pViewClass,PlanNode*  planNode)
    : CSingleDocTemplate (nIDResource,pDocClass,pFrameClass,pViewClass), m_planNode(planNode)
{
    FUNCTION_ENTRY( "CFloatingDocTemplate" );
    FUNCTION_EXIT;
}


CFloatingDocTemplate::~CFloatingDocTemplate()
{
    FUNCTION_ENTRY( "~CFloatingDocTemplate" );
    FUNCTION_EXIT;
}


PlanNode* CFloatingDocTemplate::getNode()
{
    FUNCTION_ENTRY( "getNode" );
    FUNCTION_EXIT;
    return m_planNode;
}


