/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/PatchGroupRadioResourceButtonStrategy.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/PatchGroupRadioResourceButtonStrategy.h"


PatchGroupRadioResourceButtonStrategy::PatchGroupRadioResourceButtonStrategy()
{

}


PatchGroupRadioResourceButtonStrategy::~PatchGroupRadioResourceButtonStrategy()
{

}

int PatchGroupRadioResourceButtonStrategy::assignmentBehaviour(bool pOnInit)
{
	FUNCTION_ENTRY("assignmentBehaviour");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return -1;
	}

	unsigned int iButtonRef = m_pResourceButton->m_buttonRef;

	bool isResourceAssigned;

	//bool isResourceAssigned = The_RadioSystemStatus::instance()->getResource_isAssigned(m_pResourceButton->m_RadioResource); 

	//if (!isResourceAssigned)
	//{
		isResourceAssigned = The_RadioOperations::instance()->assignResource(m_pResourceButton->m_RadioResource,iButtonRef);
	//}

	if (isResourceAssigned)
	{
		RadioResourceButtonStrategy::assignmentBehaviour(pOnInit);
	} else 
	{
		The_RadioSystemStatus::instance()->removeResourceButtonConfig(
        m_pResourceButton->m_RadioResource, 
        m_pResourceButton->m_isProfileButton,m_pResourceButton->m_buttonRef);
	}
	

	FUNCTION_EXIT;
	return 1;
}

bool PatchGroupRadioResourceButtonStrategy::removeAssignedResource()
{
	FUNCTION_ENTRY("removeAssignedResource");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return false;
	}

	GroupConnectionState isGroupConnected = The_RadioOperations::instance()->getResource_isGroupConnected( m_pResourceButton->m_RadioResource );

	if( isGroupConnected == CONNECTED || isGroupConnected == CONNECTION_BUSY)
	{	//UE-xxxx;
		MessageBox(NULL, "Resource in use","PatchGroupRadioResourceButtonStrategy",MB_OK|MB_ICONERROR);
	}
	else
	{			
		The_RadioOperations::instance()->deAssignResource(m_pResourceButton->m_RadioResource);
		The_RadioSystemStatus::instance()->removeResourceButtonConfig(
			m_pResourceButton->m_RadioResource, 
            m_pResourceButton->m_isProfileButton,m_pResourceButton->m_buttonRef);

		return RadioResourceButtonStrategy::removeAssignedResource();
	}

	FUNCTION_EXIT;
    return !isGroupConnected;

}


void PatchGroupRadioResourceButtonStrategy::unselectionBehaviour()
{
	FUNCTION_ENTRY("unselectionBehaviour");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	RadioResourceButtonStrategy::unselectionBehaviour();

	The_RadioOperations::instance()->releaseGroup(m_pResourceButton->m_RadioResource);

	FUNCTION_EXIT;
}

void PatchGroupRadioResourceButtonStrategy::selectionBehaviour()
{
	FUNCTION_ENTRY("selectionBehaviour");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	RadioResourceButtonStrategy::selectionBehaviour();

	The_RadioOperations::instance()->connectGroup(m_pResourceButton->m_RadioResource);

	FUNCTION_EXIT;
}

void PatchGroupRadioResourceButtonStrategy::cancelCurrentOperation(ApiState state)
{
	FUNCTION_ENTRY("cancelCurrentOperation");

	if (NULL == m_pResourceButton) {
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "resourceButton component is NULL");
		return;
	}

	switch (state) {
		case RA_ASSIGN_STATE:
			this->m_pResourceButton->stopMonitor();
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