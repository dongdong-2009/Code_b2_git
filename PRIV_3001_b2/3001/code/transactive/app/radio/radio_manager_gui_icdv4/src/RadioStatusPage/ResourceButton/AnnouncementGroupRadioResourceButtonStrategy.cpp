/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/AnnouncementGroupRadioResourceButtonStrategy.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/resource.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"

#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/AnnouncementGroupRadioResourceButtonStrategy.h"


AnnouncementGroupRadioResourceButtonStrategy::AnnouncementGroupRadioResourceButtonStrategy()
{

}


AnnouncementGroupRadioResourceButtonStrategy::~AnnouncementGroupRadioResourceButtonStrategy()
{

}

void AnnouncementGroupRadioResourceButtonStrategy::selectionBehaviour()
{
	FUNCTION_ENTRY("selectionBehaviour");

	RadioResourceButtonStrategy::selectionBehaviour();

	The_RadioOperations::instance()->selectSpeaker(m_pResourceButton->m_RadioResource );

	FUNCTION_EXIT;
}

void AnnouncementGroupRadioResourceButtonStrategy::unselectionBehaviour()
{
	FUNCTION_ENTRY("unselectionBehaviour");

	RadioResourceButtonStrategy::unselectionBehaviour();

	The_RadioOperations::instance()->unselectSpeaker(m_pResourceButton->m_RadioResource);

	FUNCTION_EXIT;
}

bool AnnouncementGroupRadioResourceButtonStrategy::removeAssignedResource()
{
	FUNCTION_ENTRY("removeAssignedResource");

	bool isResourceActive = The_RadioOperations::instance()->getResource_isActive( m_pResourceButton->m_RadioResource );

	if( isResourceActive == true )
	{	//UE-xxxx;
		MessageBox(NULL, "Resource in use","AnnouncementGroupRadioResourceButtonStrategy",MB_OK|MB_ICONERROR);
        return false;
	}
	else
	{	
		The_RadioSystemStatus::instance()->removeResourceButtonConfig(
			m_pResourceButton->m_RadioResource, 
            m_pResourceButton->m_isProfileButton,m_pResourceButton->m_buttonRef);

		return RadioResourceButtonStrategy::removeAssignedResource();
	}

	FUNCTION_EXIT;
}

//TD18141
void AnnouncementGroupRadioResourceButtonStrategy::rightClickBehaviour()
{
	FUNCTION_ENTRY("rightClickBehaviour");

	CRect rect;
	m_pResourceButton->GetWindowRect(&rect);

    RadioEmergencyModeType lEmergency = The_RadioSystemStatus::instance()->getResource_isEmergencyMode(
        m_pResourceButton->m_RadioResource);

	CMenu menu;
	menu.CreatePopupMenu();
	menu.AppendMenu(0,ID_STOP_MONITOR_MENU, "Stop Monitoring");

    if (lEmergency == RE_NO_EMERGENCY)
    {
    	menu.AppendMenu(MF_GRAYED,ID_ACKNOWLEDGE_EMERGENCY_MENU, "Acknowledge emergency");
    	menu.AppendMenu(MF_GRAYED,ID_KNOCKDOWN_EMERGENCY_MENU, "Knockdown emergency");
    }
    else
    {
    	menu.AppendMenu(MF_ENABLED,ID_ACKNOWLEDGE_EMERGENCY_MENU, "Acknowledge emergency");
    	menu.AppendMenu(MF_ENABLED,ID_KNOCKDOWN_EMERGENCY_MENU, "Knockdown emergency");
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
//TD18141
