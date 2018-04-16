/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/SiteRadioResourceButtonStrategy.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef SITE_RADIO_RESOURCE_BUTTON_STRATEGY_H_INCLUDED
#define SITE_RADIO_RESOURCE_BUTTON_STRATEGY_H_INCLUDED

#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/RadioResourceButtonStrategy.h"

class SiteRadioResourceButtonStrategy : public RadioResourceButtonStrategy
{

public:
	SiteRadioResourceButtonStrategy();

	virtual ~SiteRadioResourceButtonStrategy();

	virtual bool removeAssignedResource();
	
	virtual void cancelCurrentOperation(ApiState state) {};

};
#endif