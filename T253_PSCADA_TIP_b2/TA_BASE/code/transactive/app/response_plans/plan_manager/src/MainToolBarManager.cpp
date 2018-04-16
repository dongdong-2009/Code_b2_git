//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/MainToolBarManager.cpp $
// @author:  Bart Golab
// @version: $Revision: #4 $
//
// Last modification: $DateTime: 2015/08/31 17:46:20 $
// Last modified by:  $Author: huang.wenbo $
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

	extendToolBar(addToolBarToList(IDR_CATEGORY_NODE_TOOLBAR));

    if (Utilities::isPlanManagerConfigurationMode())
    {
		extendToolBar(addToolBarToList(IDR_PLAN_NODE_ADMIN_TOOLBAR));
    }
    else
    {
		extendToolBar(addToolBarToList(IDR_PLAN_NODE_TOOLBAR));
    }

    FUNCTION_EXIT;
}


void MainToolBarManager::extendToolBar( CToolBar *toolBar )
{
    FUNCTION_ENTRY( "extendToolBar" );

    if ( NULL == toolBar )
    {
        return;
    }

    int idPlanNew = toolBar->CommandToIndex(ID_PLAN_NEW);
    if (idPlanNew != -1)
    {
        toolBar->GetToolBarCtrl().SetExtendedStyle(toolBar->GetToolBarCtrl().GetExtendedStyle() | TBSTYLE_EX_DRAWDDARROWS);
        toolBar->SetButtonStyle(idPlanNew, toolBar->GetButtonStyle(idPlanNew) | TBSTYLE_DROPDOWN );
        toolBar->SetSizes(CSize(40, 55), CSize(24,24));
    }

    FUNCTION_EXIT;
}
