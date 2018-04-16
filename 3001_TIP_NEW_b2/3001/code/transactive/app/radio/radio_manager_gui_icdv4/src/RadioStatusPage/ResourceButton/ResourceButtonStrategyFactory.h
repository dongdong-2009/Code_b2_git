/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/ResourceButtonStrategyFactory.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef RESOURCE_BUTTON_STRATEGY_FACTORY_H_INCLUDED
#define RESOURCE_BUTTON_STRATEGY_FACTORY_H_INCLUDED


#include "bus/radio/radio_bus/src/RadioTypes.h"


//Forward declaration;
class IResourceButtonStrategy;

class ResourceButtonStrategyFactory
{
	public:
		ResourceButtonStrategyFactory();

		virtual ~ResourceButtonStrategyFactory();

	public:
		static IResourceButtonStrategy* getResourceButtonStrategy(RadioResourceType type);

};
#endif