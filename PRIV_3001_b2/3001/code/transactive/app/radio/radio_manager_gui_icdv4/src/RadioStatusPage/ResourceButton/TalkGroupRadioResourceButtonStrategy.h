/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/TalkGroupRadioResourceButtonStrategy.h $
 * @author:  zou chunzhong
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#ifndef TALK_GROUP_RADIO_RESOURCE_BUTTON_STRATEGY_H_INCLUDED
#define TALK_GROUP_RADIO_RESOURCE_BUTTON_STRATEGY_H_INCLUDED

#include "app/radio/radio_manager_gui_icdv4/src/RadioStatusPage/ResourceButton/RadioResourceButtonStrategy.h"

class TalkGroupRadioResourceButtonStrategy : public RadioResourceButtonStrategy
{

public:
	TalkGroupRadioResourceButtonStrategy();

	virtual ~TalkGroupRadioResourceButtonStrategy();

public:
	/**
	 * This overridable method is from the public method: RadioResourceButtonStrategy;
	 */
	virtual void selectionBehaviour();
	/**
	 * This overridable method is from the public method: RadioResourceButtonStrategy;
	 */
	virtual void unselectionBehaviour();
	/**
	 * This method is different from the base version: int assignmentBehaviour();
	 */
	virtual int assignmentBehaviour(bool pOnInit);

	virtual bool removeAssignedResource();

	virtual void rightClickBehaviour();

};
#endif