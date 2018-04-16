/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/TalkGroupRadioResourceButtonStrategy.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/IResourceButtonStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/TalkGroupRadioResourceButtonStrategy.h"
#include "app/radio/radio_manager_gui_icdv4/src/resource.h"

#include "core/utilities/src/DebugUtil.h"


TalkGroupRadioResourceButtonStrategy::TalkGroupRadioResourceButtonStrategy()
{

}


TalkGroupRadioResourceButtonStrategy::~TalkGroupRadioResourceButtonStrategy()
{
}

void TalkGroupRadioResourceButtonStrategy::selectionBehaviour()
{
	FUNCTION_ENTRY("selectionBehaviour");

	RadioResourceButtonStrategy::selectionBehaviour();

	if(!The_RadioOperations::instance()->selectSpeaker(m_pResourceButton->m_RadioResource)) 
	{
		//The_RadioOperations::instance()->notifyFailOp(m_pResourceButton->m_RadioResource, m_pResourceButton->getButtonReference());
	}

	FUNCTION_EXIT;
}

void TalkGroupRadioResourceButtonStrategy::unselectionBehaviour()
{
	FUNCTION_ENTRY("unselectionBehaviour");

	RadioResourceButtonStrategy::unselectionBehaviour();

	if(!The_RadioOperations::instance()->unselectSpeaker(m_pResourceButton->m_RadioResource))
	{
		//The_RadioOperations::instance()->notifyFailOp(m_pResourceButton->m_RadioResource, m_pResourceButton->getButtonReference());
	}

	FUNCTION_EXIT;
}

int TalkGroupRadioResourceButtonStrategy::assignmentBehaviour(bool pOnInit)
{
	FUNCTION_ENTRY("assignmentBehaviour");

	unsigned int iButtonRef = m_pResourceButton->m_buttonRef;

	bool isResourceAssigned = The_RadioSystemStatus::instance()->getResource_isAssigned(m_pResourceButton->m_RadioResource); 

	if (!isResourceAssigned)
	{
		isResourceAssigned = The_RadioOperations::instance()->assignResource(m_pResourceButton->m_RadioResource,iButtonRef);
	}

	if (isResourceAssigned)
	{
		RadioResourceButtonStrategy::assignmentBehaviour(pOnInit);
		FUNCTION_EXIT;
		return 1;
	} else 
	{
		The_RadioSystemStatus::instance()->removeResourceButtonConfig(
        m_pResourceButton->m_RadioResource, 
        m_pResourceButton->m_isProfileButton,m_pResourceButton->m_buttonRef);
		FUNCTION_EXIT;
		return 0;
	}
}

bool TalkGroupRadioResourceButtonStrategy::removeAssignedResource()
{
	FUNCTION_ENTRY("removeAssignedResource");

	bool status = The_RadioOperations::instance()->deAssignResource(m_pResourceButton->m_RadioResource);

	if (status) {
	The_RadioSystemStatus::instance()->removeResourceButtonConfig(
        m_pResourceButton->m_RadioResource, 
        m_pResourceButton->m_isProfileButton,m_pResourceButton->m_buttonRef);
	status = RadioResourceButtonStrategy::removeAssignedResource() && status; 
	}

	FUNCTION_EXIT;
    
	return status;
}

void TalkGroupRadioResourceButtonStrategy::rightClickBehaviour()
{
	FUNCTION_ENTRY("rightClickBehaviour");

	CRect rect;
	m_pResourceButton->GetWindowRect(&rect);

    RadioEmergencyModeType lEmergency = The_RadioSystemStatus::instance()->getResource_isEmergencyMode(
        m_pResourceButton->m_RadioResource);

	CMenu menu;
	menu.CreatePopupMenu();
	//menu.AppendMenu(0,ID_STOP_MONITOR_MENU, "Stop Monitoring");
	//menu.AppendMenuA(MF_SEPARATOR,0, "");

    if (lEmergency == RE_NO_EMERGENCY)
    {
		menu.AppendMenu(MF_ENABLED,ID_STOP_MONITOR_MENU, "Stop Monitoring");
		menu.AppendMenuA(MF_SEPARATOR,0, "");
    	menu.AppendMenu(MF_GRAYED,ID_ACKNOWLEDGE_EMERGENCY_MENU, "Ack emergency");
		menu.AppendMenu(MF_ENABLED, ID_START_EMERGENCY_MENU, "Start Emergency");
    	menu.AppendMenu(MF_GRAYED,ID_KNOCKDOWN_EMERGENCY_MENU, "Stop emergency");
    }
    else
    {
		menu.AppendMenu(MF_GRAYED,ID_STOP_MONITOR_MENU, "Stop Monitoring");
		menu.AppendMenuA(MF_SEPARATOR,0, "");
    	menu.AppendMenu(MF_ENABLED,ID_ACKNOWLEDGE_EMERGENCY_MENU, "Ack emergency");
		menu.AppendMenu(MF_GRAYED, ID_START_EMERGENCY_MENU, "Start Emergency");
    	menu.AppendMenu(MF_ENABLED,ID_KNOCKDOWN_EMERGENCY_MENU, "Stop emergency");
    }

   // display the popup menu

    CPoint mousePoint;
    ::GetCursorPos(&mousePoint);

	menu.TrackPopupMenu(TPM_LEFTALIGN, mousePoint.x, mousePoint.y, m_pResourceButton, rect);

	// When "Stop Monitoring" is selected from the popup menu,
	// implementation is done in CResourceButton::OnStopMonitorMenu() 

	menu.DestroyMenu();

	FUNCTION_EXIT;

}
