/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/MultiSelectRadioResourceButtonStrategy.cpp $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/MultiSelectRadioResourceButtonStrategy.h"



MultiSelectRadioResourceButtonStrategy::MultiSelectRadioResourceButtonStrategy()
{

}


MultiSelectRadioResourceButtonStrategy::~MultiSelectRadioResourceButtonStrategy()
{

}

bool MultiSelectRadioResourceButtonStrategy::removeAssignedResource()
{
	FUNCTION_ENTRY("removeAssignedResource");

	GroupConnectionState isGroupConnected = The_RadioOperations::instance()->getResource_isGroupConnected( m_pResourceButton->m_RadioResource );

	if( isGroupConnected == CONNECTED || isGroupConnected == CONNECTION_BUSY )
	{	//UE-xxxx;
		MessageBox(NULL, "Resource in use","MultiSelectRadioResourceButtonStrategy",MB_OK|MB_ICONERROR);
	}
	else
	{	
		The_RadioSystemStatus::instance()->removeResourceButtonConfig(
			m_pResourceButton->m_RadioResource, 
            m_pResourceButton->m_isProfileButton,m_pResourceButton->m_buttonRef);

		return RadioResourceButtonStrategy::removeAssignedResource();
	}

	FUNCTION_EXIT;
    return !isGroupConnected;
}




void MultiSelectRadioResourceButtonStrategy::unselectionBehaviour()
{
	FUNCTION_ENTRY("unselectionBehaviour");

	RadioResourceButtonStrategy::unselectionBehaviour();

	bool status = The_RadioOperations::instance()->releaseGroup(m_pResourceButton->m_RadioResource);

	if (!status) {
		RadioResourceButtonStrategy::selectionBehaviour();
		return;
	}

	FUNCTION_EXIT;
}

void MultiSelectRadioResourceButtonStrategy::selectionBehaviour()
{
	FUNCTION_ENTRY("selectionBehaviour");

	RadioResourceButtonStrategy::selectionBehaviour();

	bool status = The_RadioOperations::instance()->connectGroup(m_pResourceButton->m_RadioResource);

	if (!status) {
		RadioResourceButtonStrategy::unselectionBehaviour();
		return;
	}

	FUNCTION_EXIT;
}