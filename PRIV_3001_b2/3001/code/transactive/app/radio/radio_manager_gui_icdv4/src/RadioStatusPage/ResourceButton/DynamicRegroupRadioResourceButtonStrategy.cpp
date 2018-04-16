/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/DynamicRegroupRadioResourceButtonStrategy.cpp $
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
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/DynamicRegroupRadioResourceButtonStrategy.h"


DynamicRegroupRadioResourceButtonStrategy::DynamicRegroupRadioResourceButtonStrategy()
{

}


DynamicRegroupRadioResourceButtonStrategy::~DynamicRegroupRadioResourceButtonStrategy()
{

}

bool DynamicRegroupRadioResourceButtonStrategy::removeAssignedResource()
{
	FUNCTION_ENTRY("removeAssignedResource");

	GroupConnectionState isGroupConnected = The_RadioOperations::instance()->getResource_isGroupConnected( 
                    m_pResourceButton->m_RadioResource );

	if( isGroupConnected == CONNECTED )
	{	//UE-xxxx;
		MessageBox(NULL, "Resource in use","DynamicRegroupRadioResourceButtonStrategy",MB_OK|MB_ICONERROR);
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


void DynamicRegroupRadioResourceButtonStrategy::unselectionBehaviour()
{
	FUNCTION_ENTRY("unselectionBehaviour");

	RadioResourceButtonStrategy::unselectionBehaviour();

	RadioResource res;
	res = m_pResourceButton->m_RadioResource;
	
	std::vector<RadioResource> mems = The_RadioSystemStatus::instance()->getResourceMembers(res);
	{
		res.alias = "";
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Length of members: %d", mems.size());
		std::vector<RadioResource>::iterator iter = mems.begin();
		for ( ; iter != mems.end(); ++iter)
		{
			if (RR_TALKGROUP == (*iter).type) res.alias = (*iter).alias;
		}

		if (res.alias == "") {
			::MessageBox(NULL, "Talkgroup is not assigned to DGNA", "Error connecting", MB_OK|MB_ICONERROR);
			return;
		}
	}
	bool status = The_RadioOperations::instance()->releaseGroup(res);

	if (!status) {
		RadioResourceButtonStrategy::selectionBehaviour();
		return;
	}

	FUNCTION_EXIT;
}

void DynamicRegroupRadioResourceButtonStrategy::selectionBehaviour()
{
	FUNCTION_ENTRY("selectionBehaviour");

	RadioResourceButtonStrategy::selectionBehaviour();

	RadioResource res;
	res = m_pResourceButton->m_RadioResource;
	
	std::vector<RadioResource> mems = The_RadioSystemStatus::instance()->getResourceMembers(res);
	{
		res.alias = "";
		std::vector<RadioResource>::iterator iter = mems.begin();
		for ( ; iter != mems.end(); ++iter)
		{
			if (RR_TALKGROUP == (*iter).type) res.alias = (*iter).alias;
		}

		if (res.alias == "") {
			::MessageBox(NULL, "Talkgroup is not assigned to DGNA", "Error connecting", MB_OK|MB_ICONERROR);
			return;
		}
	}	

	bool status = The_RadioOperations::instance()->connectGroup(res);

	if (!status) {
		RadioResourceButtonStrategy::unselectionBehaviour();
		return;
	}

	FUNCTION_EXIT;
}
