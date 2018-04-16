/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/EntityOperationsAreaStrategyFactory.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Constructs the appropriate IEntityOperationsAreaStrategy type for the
 * RadioResouceType
 */

#if !defined(AFX_ENTITYOPERATIONSAREASTRATEGYFACTORY_H__71DF3B09_4E20_435F_9017_1A98B13F0481__INCLUDED_)
#define AFX_ENTITYOPERATIONSAREASTRATEGYFACTORY_H__71DF3B09_4E20_435F_9017_1A98B13F0481__INCLUDED_

#include "app/radio/radio_manager_gui/src/RadioSelectedEntity/SelectedEntityOperations/IEntityOperationsAreaStrategy.h"

#include "app/radio/radio_manager_gui/src/RadioUserDefinedEvents.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class EntityOperationsAreaStrategyFactory  
{
	public:

		EntityOperationsAreaStrategyFactory();

		virtual ~EntityOperationsAreaStrategyFactory();

		
	public:

		/**
		 * Constructs the IEntityOperationsAreaStrategy type and returns it
		 */
		static IEntityOperationsAreaStrategy* getEntityOperationsAreaStrategy(RadioResourceType type);
};

#endif // !defined(AFX_ENTITYOPERATIONSAREASTRATEGYFACTORY_H__71DF3B09_4E20_435F_9017_1A98B13F0481__INCLUDED_)
