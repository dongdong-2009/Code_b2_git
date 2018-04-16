//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanEditorToolBarManager.cpp $
// @author:  Bart Golab
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2013/03/20 14:16:56 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#include "stdafx.h"
#include "Resource.h"
#include "core/utilities/src/DebugUtil.h"
#include "PlanEditorToolBarManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanEditorToolBarManager

PlanEditorToolBarManager::PlanEditorToolBarManager()
{
    FUNCTION_ENTRY( "PlanEditorToolBarManager" );
    FUNCTION_EXIT;
}


PlanEditorToolBarManager::~PlanEditorToolBarManager()
{
    FUNCTION_ENTRY( "~PlanEditorToolBarManager" );
    FUNCTION_EXIT;
}


void PlanEditorToolBarManager::selectToolBar(EToolBarType toolBarType)
{
    FUNCTION_ENTRY( "selectToolBar" );

    AbstractToolBarManager::selectToolBar((int) toolBarType);

    FUNCTION_EXIT;
}


void PlanEditorToolBarManager::initialiseToolBarList()
{
    FUNCTION_ENTRY( "initialiseToolBarList" );

    addToolBarToList(IDR_PLAN_EDITOR_GENERAL_TOOLBAR);   // TBT_GENERAL
    addToolBarToList(IDR_PLAN_EDITOR_STEPS_TOOLBAR);     // TBT_STEPS
    addToolBarToList(IDR_PLAN_EDITOR_FLOWCHART_TOOLBAR); // TBT_FLOWCHART

    FUNCTION_EXIT;
}


