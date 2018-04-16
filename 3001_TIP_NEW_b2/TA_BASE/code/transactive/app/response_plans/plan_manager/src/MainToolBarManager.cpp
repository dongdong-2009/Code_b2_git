//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/MainToolBarManager.cpp $
// @author:  Bart Golab
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2012/12/28 19:17:57 $
// Last modified by:  $Author: huangjian $
//
// <description>

#include "stdafx.h"
#include "Resource.h"
#include "MainToolBarManager.h"
#include "TreeNode.h"
#include "RootNode.h"
#include "CategoryNode.h"
#include "PlanNode.h"
#include "TreeNodeFactory.h"
#include "Utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Toolbar types
using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// MainToolBarManager

MainToolBarManager::MainToolBarManager()
{
    FUNCTION_ENTRY( "MainToolBarManager" );
    FUNCTION_EXIT;
}


MainToolBarManager::~MainToolBarManager()
{
    FUNCTION_ENTRY( "~MainToolBarManager" );
    FUNCTION_EXIT;
}


void MainToolBarManager::selectToolBar(EToolBarType toolBarType)
{
    FUNCTION_ENTRY( "selectToolBar" );

    AbstractToolBarManager::selectToolBar((int) toolBarType);

    FUNCTION_EXIT;
}


void MainToolBarManager::hideToolBar()
{
    FUNCTION_ENTRY( "hideToolBar" );

    AbstractToolBarManager::selectToolBar(TBT_NONE);

    FUNCTION_EXIT;
}


void MainToolBarManager::initialiseToolBarList()
{
    FUNCTION_ENTRY( "initialiseToolBarList" );

    addToolBarToList(IDR_CATEGORY_NODE_TOOLBAR);  // TBT_CATEGORY
    if (Utilities::isPlanManagerConfigurationMode())
    {
        addToolBarToList(IDR_PLAN_NODE_ADMIN_TOOLBAR);
    }
    else
    {
        addToolBarToList(IDR_PLAN_NODE_TOOLBAR);      // TBT_PLAN
    }
    FUNCTION_EXIT;
}


