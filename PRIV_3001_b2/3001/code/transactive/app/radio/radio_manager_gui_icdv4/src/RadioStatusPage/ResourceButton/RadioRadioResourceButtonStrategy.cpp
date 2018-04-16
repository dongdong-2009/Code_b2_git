/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/RadioRadioResourceButtonStrategy.cpp $
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
#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/RadioRadioResourceButtonStrategy.h"


RadioRadioResourceButtonStrategy::RadioRadioResourceButtonStrategy()
{
}


RadioRadioResourceButtonStrategy::~RadioRadioResourceButtonStrategy()
{

}

bool RadioRadioResourceButtonStrategy::removeAssignedResource()
{
	FUNCTION_ENTRY("removeAssignedResource");

	bool isInPrivateCall = (PC_NOT_INVOLVED != 
        The_RadioOperations::instance()->getResource_isInPrivateCall( m_pResourceButton->m_RadioResource ));

	bool isInAmbienceListeningCall = The_RadioOperations::instance()->getResource_isInAmbienceListeningCall( m_pResourceButton->m_RadioResource );

	if( isInPrivateCall == true || isInAmbienceListeningCall == true )
	{
		MessageBox( NULL, "Resource in use", "RadioRadioResourceButtonStrategy", MB_OK|MB_ICONERROR );
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
