/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/ActivePlanDoc.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/09/15 15:58:53 $
  * Last modified by:  $Author: huang.wenbo $
  *
  * <description>
  *
  **/

#include "StdAfx.h"
#include "PlanManager.h"
#include "ActivePlanDoc.h"
#include "PlanAgentUpdateProcessor.h"
#include "OperatorRights.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CActivePlanDoc

IMPLEMENT_DYNCREATE(CActivePlanDoc, CPlanBrowserDoc)

CActivePlanDoc::CActivePlanDoc()
{
    FUNCTION_ENTRY( "CActivePlanDoc" );
    FUNCTION_EXIT;
}


BOOL CActivePlanDoc::OnNewDocument()
{
    FUNCTION_ENTRY( "OnNewDocument" );

    if (!CPlanBrowserDoc::OnNewDocument())
    {
        FUNCTION_EXIT;
        return FALSE;
    }


/*for jianghp
        // Register for active plan updates.
        PlanAgentUpdateProcessor::getInstance().subscribeForActivePlanUpdates(this);

        // Register for LockCategory and SessionId runtime parameter updates.
        TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_LOCKCATEGORY);
        TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);*/

    FUNCTION_EXIT;
    return TRUE;
}


void CActivePlanDoc::OnGenericGuiInitCompleted()
{
    FUNCTION_ENTRY( "OnGenericGuiInitCompleted" );

    // Register for active plan updates.
    PlanAgentUpdateProcessor::getInstance().subscribeForActivePlanUpdates(this);

    // Register for LockCategory and SessionId runtime parameter updates.
    TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_LOCKCATEGORY);
    TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

    FUNCTION_EXIT;
}


void CActivePlanDoc::OnCloseDocument()
{
    FUNCTION_ENTRY( "OnCloseDocument" );

    // TODO: Add your specialized code here and/or call the base class
    PlanAgentUpdateProcessor::getInstance().unsubscribeFromActivePlanUpdates(this);
    TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);

    CPlanBrowserDoc::OnCloseDocument();

    FUNCTION_EXIT;
}


CActivePlanDoc::~CActivePlanDoc()
{
    FUNCTION_ENTRY( "~CActivePlanDoc" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CActivePlanDoc, CPlanBrowserDoc)
    // {{AFX_MSG_MAP(CActivePlanDoc)
        // NOTE - the ClassWizard will add and remove mapping macros here.
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()