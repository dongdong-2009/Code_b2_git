/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/TalkGroupRadioResourceButtonStrategy.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/IResourceButtonStrategy.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/TalkGroupRadioResourceButtonStrategy.h"
#include "app/radio/radio_manager_gui/src/resource.h"

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

	if (NULL == m_pResourceButton) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	RadioResourceButtonStrategy::selectionBehaviour();

	The_RadioOperations::instance()->selectSpeaker(m_pResourceButton->m_RadioResource);

	FUNCTION_EXIT;
}

void TalkGroupRadioResourceButtonStrategy::unselectionBehaviour()
{
	FUNCTION_ENTRY("unselectionBehaviour");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	RadioResourceButtonStrategy::unselectionBehaviour();

	The_RadioOperations::instance()->unselectSpeaker(m_pResourceButton->m_RadioResource);	

	FUNCTION_EXIT;
}

int TalkGroupRadioResourceButtonStrategy::assignmentBehaviour(bool pOnInit)
{
	FUNCTION_ENTRY("assignmentBehaviour");

	if (NULL == m_pResourceButton) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return -1;
	}

	unsigned int iButtonRef = m_pResourceButton->m_buttonRef;
	
	LOG_GENERIC (SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,
				 "TG Strategy : Assigning Resource on Alias : %s",
				 m_pResourceButton->m_RadioResource.alias.c_str());

	bool isResourceAssigned;
	
	isResourceAssigned = The_RadioOperations::instance()->assignResource(m_pResourceButton->m_RadioResource,iButtonRef);

	if (isResourceAssigned)
	{
		RadioResourceButtonStrategy::assignmentBehaviour(pOnInit);
	}
	else 
	{
		The_RadioSystemStatus::instance()->removeResourceButtonConfig(
        m_pResourceButton->m_RadioResource, 
        m_pResourceButton->m_isProfileButton,m_pResourceButton->m_buttonRef);
	}
	

	FUNCTION_EXIT;
	return 1;
}

bool TalkGroupRadioResourceButtonStrategy::removeAssignedResource()
{
	FUNCTION_ENTRY("removeAssignedResource");

	if (NULL == m_pResourceButton) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return false;
	}

	int count;
	try 
	{
		count = The_RadioSystemStatus::instance()->getResourceCounter(m_pResourceButton->m_RadioResource);
	}
	catch (...)	
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Exception occured while connecting MFT Agent");
		count = 0;
	}

	if (count > 1) 
	{
		The_RadioSystemStatus::instance()->showErrorMessageFromRadio();
		return false;
	}

	bool status = The_RadioOperations::instance()->deAssignResource(m_pResourceButton->m_RadioResource);

	if (status) 
	{
	The_RadioSystemStatus::instance()->removeResourceButtonConfig(
        m_pResourceButton->m_RadioResource, 
        m_pResourceButton->m_isProfileButton,m_pResourceButton->m_buttonRef);
	}

	FUNCTION_EXIT;
    
	return status;
}

void TalkGroupRadioResourceButtonStrategy::rightClickBehaviour()
{
	FUNCTION_ENTRY("rightClickBehaviour");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

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
    else if (lEmergency == RE_UNACK_EMERGENCY)
    {
		menu.AppendMenu(MF_GRAYED,ID_STOP_MONITOR_MENU, "Stop Monitoring");
		menu.AppendMenuA(MF_SEPARATOR,0, "");
    	menu.AppendMenu(MF_ENABLED,ID_ACKNOWLEDGE_EMERGENCY_MENU, "Ack emergency");
		menu.AppendMenu(MF_GRAYED, ID_START_EMERGENCY_MENU, "Start Emergency");
    	menu.AppendMenu(MF_ENABLED,ID_KNOCKDOWN_EMERGENCY_MENU, "Stop emergency");
    } // peter.wong, DTL-365
	else if (lEmergency == RE_EMERGENCY)
	{
		menu.AppendMenu(MF_GRAYED,ID_STOP_MONITOR_MENU, "Stop Monitoring");
		menu.AppendMenuA(MF_SEPARATOR,0, "");
    	menu.AppendMenu(MF_GRAYED,ID_ACKNOWLEDGE_EMERGENCY_MENU, "Ack emergency");
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

void TalkGroupRadioResourceButtonStrategy::cancelCurrentOperation(ApiState state)
{
	FUNCTION_ENTRY("cancelCurrentOperation");

	if (NULL == m_pResourceButton) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	switch (state) 
	{
		case RA_ASSIGN_STATE:
			this->m_pResourceButton->stopMonitor();
			resourceDeAssignment();
			break;
		case RA_DEASSIGN_STATE:
			this->m_pResourceButton->cancelStopMonitor();
			break;
		case RA_SELECT_STATE:
			this->m_pResourceButton->setCheck(0);
			RadioResourceButtonStrategy::unselectionBehaviour();;
			break;
		case RA_DESELECT_STATE:
			this->m_pResourceButton->setCheck(1);
			RadioResourceButtonStrategy::selectionBehaviour();
			break;
	};

	FUNCTION_EXIT;
}

void TalkGroupRadioResourceButtonStrategy::resourceDeAssignment()
{
	FUNCTION_ENTRY("resourceDeAssignment");

	if (NULL == m_pResourceButton) 
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	m_pResourceButton->m_RadioResource.initialise();
	m_pResourceButton->m_PrevRadioResource.initialise();

	IResourceButtonStrategy *pTempResourceButtonStrategy = ResourceButtonStrategyFactory::getResourceButtonStrategy(m_pResourceButton->m_RadioResource.type);
	TA_ASSERT(pTempResourceButtonStrategy != NULL, "RadioResourceButtonStrategy strategy");

	pTempResourceButtonStrategy->initialise(this->m_pResourceButton);
		
	m_pResourceButton->setResourceButtonStrategy(pTempResourceButtonStrategy);

	FUNCTION_EXIT;
}