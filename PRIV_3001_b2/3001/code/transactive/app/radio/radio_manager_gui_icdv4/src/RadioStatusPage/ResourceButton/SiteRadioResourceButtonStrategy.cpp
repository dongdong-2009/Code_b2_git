/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioSystemStatus.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/SiteRadioResourceButtonStrategy.h"


SiteRadioResourceButtonStrategy::SiteRadioResourceButtonStrategy()
{

}


SiteRadioResourceButtonStrategy::~SiteRadioResourceButtonStrategy()
{

}

bool SiteRadioResourceButtonStrategy::removeAssignedResource()
{
	FUNCTION_ENTRY("removeAssignedResource");


	The_RadioSystemStatus::instance()->removeResourceButtonConfig(
		m_pResourceButton->m_RadioResource, 
        m_pResourceButton->m_isProfileButton,m_pResourceButton->m_buttonRef);

	FUNCTION_EXIT;
	return RadioResourceButtonStrategy::removeAssignedResource();
}