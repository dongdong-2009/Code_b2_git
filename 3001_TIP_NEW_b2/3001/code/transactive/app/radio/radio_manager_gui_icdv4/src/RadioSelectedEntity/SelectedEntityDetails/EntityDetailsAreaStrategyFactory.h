/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/EntityDetailsAreaStrategyFactory.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 * Constructs the appropriate IEntityDetailsAreaStrategy type.
 */

#if !defined(AFX_ENTITYDETAILSAREASTRATEGYFACTORY_H__5FC4FDA5_52F5_47AD_8CAD_0C4F19295CAB__INCLUDED_)
#define AFX_ENTITYDETAILSAREASTRATEGYFACTORY_H__5FC4FDA5_52F5_47AD_8CAD_0C4F19295CAB__INCLUDED_

#include "app/radio/radio_manager_gui_icdv4/src/RadioSelectedEntity/SelectedEntityDetails/IEntityDetailsAreaStrategy.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class EntityDetailsAreaStrategyFactory  
{
	public:

		EntityDetailsAreaStrategyFactory();
		virtual ~EntityDetailsAreaStrategyFactory();

	public:

		/**
		 * Constructs the appropriate strategy and returns it
		 */
		static IEntityDetailsAreaStrategy* getEntityDetailsAreaStrategy(RadioResourceType type);
};

#endif // !defined(AFX_ENTITYDETAILSAREASTRATEGYFACTORY_H__5FC4FDA5_52F5_47AD_8CAD_0C4F19295CAB__INCLUDED_)
