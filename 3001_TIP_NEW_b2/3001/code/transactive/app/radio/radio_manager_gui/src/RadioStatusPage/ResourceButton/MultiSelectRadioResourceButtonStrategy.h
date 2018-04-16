/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/MultiSelectRadioResourceButtonStrategy.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef MULTI_SELECT_RADIO_RESOURCE_BUTTON_STRATEGY_H_INCLUDED
#define MULTI_SELECT_RADIO_RESOURCE_BUTTON_STRATEGY_H_INCLUDED


#include "app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/RadioResourceButtonStrategy.h"

class MultiSelectRadioResourceButtonStrategy : public RadioResourceButtonStrategy
{

public:
	MultiSelectRadioResourceButtonStrategy();

	virtual ~MultiSelectRadioResourceButtonStrategy();

	virtual int assignmentBehaviour(bool pOnInit);

	virtual bool removeAssignedResource();

	virtual void selectionBehaviour();

	virtual void unselectionBehaviour();

	virtual void cancelCurrentOperation(ApiState state);

};
#endif