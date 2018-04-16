//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditAssignCameraToMonitorPage.cpp $
// @author:  Katherine Thomson
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
//

#include "stdafx.h"

#include "resource.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/editor/src/PlanStepEditAssignCameraToMonitorPage.h"
#include "app/response_plans/plan_step_plugin/assign_camera_to_monitor/editor/src/AssignCameraToMonitorStep.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditAssignCameraToMonitorPage dialog

IMPLEMENT_DYNAMIC(CPlanStepEditAssignCameraToMonitorPage, CPlanStepEditParameterPage)

CPlanStepEditAssignCameraToMonitorPage::CPlanStepEditAssignCameraToMonitorPage(AssignCameraToMonitorStep* step)
  : CPlanStepEditParameterPage(IDD_PLAN_STEP_EDIT_ASSIGN_CAMERA_TO_MONITOR_DLG, step)
{
    FUNCTION_ENTRY( "CPlanStepEditAssignCameraToMonitorPage" );

    FUNCTION_EXIT;
}


CPlanStepEditAssignCameraToMonitorPage::~CPlanStepEditAssignCameraToMonitorPage()
{
    FUNCTION_ENTRY( "~CPlanStepEditAssignCameraToMonitorPage" );

    // Do nothing.

    FUNCTION_EXIT;
}


void CPlanStepEditAssignCameraToMonitorPage::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CPlanStepEditParameterPage::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CPlanStepEditAssignCameraToMonitorPage)
    DDX_Control(pDX, IDC_CAMERA, m_camera);
    DDX_Control(pDX, IDC_MONITOR, m_monitor);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepEditAssignCameraToMonitorPage, CPlanStepEditParameterPage)
    // {{AFX_MSG_MAP(CPlanStepEditAssignCameraToMonitorPage)
    ON_BN_CLICKED(IDC_PICK_CAMERA_BUTTON, OnPickCameraButton)
    ON_BN_CLICKED(IDC_PICK_MONITOR_BUTTON, OnPickMonitorButton)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditAssignCameraToMonitorPage message handlers


void CPlanStepEditAssignCameraToMonitorPage::OnPickCameraButton()
{
    FUNCTION_ENTRY( "OnPickCameraButton" );

    popupTree( ParamCamera, ParamCamera, m_camera);

    FUNCTION_EXIT;
}


void CPlanStepEditAssignCameraToMonitorPage::OnPickMonitorButton()
{
    FUNCTION_ENTRY( "OnPickMonitorButton" );

    popupTree( ParamMonitor, ParamMonitor, m_monitor);

    FUNCTION_EXIT;
}


void CPlanStepEditAssignCameraToMonitorPage::EnableParameterControls(bool enable)  // TD#614/1170/1409
{
    FUNCTION_ENTRY( "EnableParameterControls" );

    CWnd* cameraButton = GetDlgItem(IDC_PICK_CAMERA_BUTTON);
    if (cameraButton)
    {
        cameraButton->EnableWindow(enable);
    }

    CWnd* monitorButton = GetDlgItem(IDC_PICK_MONITOR_BUTTON);
    if (monitorButton)
    {
        monitorButton->EnableWindow(enable);
    }

    FUNCTION_EXIT;
}


void CPlanStepEditAssignCameraToMonitorPage::DisplayParameters()
{
    FUNCTION_ENTRY( "DisplayParameters" );

	std::vector<ItemPair> vecItems;
	GetPlanStep()->getActivityParameter().getNameValuePairs( vecItems );
	for ( std::vector<ItemPair>::iterator itLoop = vecItems.begin(); vecItems.end() != itLoop; ++itLoop )
	{
		if ( itLoop->Name.compare( ParamCamera ) == 0 )
		{
			m_camera.SetWindowText( itLoop->Item.c_str() );
		}
		else if ( itLoop->Name.compare( ParamMonitor ) == 0 )
		{
			m_monitor.SetWindowText( itLoop->Item.c_str() );
		}
	}

    FUNCTION_EXIT;
}


void CPlanStepEditAssignCameraToMonitorPage::UpdateParameters()
{
    FUNCTION_ENTRY( "UpdateParameters" );
    FUNCTION_EXIT;
}

}
