/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui/src/RadioStatusPage/ResourceButton/ButtonDecoratorType.h $
  * @author zou chunzhong
  * @version $Revision: #2 $
  * Last modification: $Modtime:$
  * Last modified by: $Author: grace.koh $
  */
#ifndef BUTTON_DECORATOR_TYPE_H_INCLUDED
#define BUTTON_DECORATOR_TYPE_H_INCLUDED


/** 
  * Defines the set of Button Decorators;
  * The order of the attributs must be maintained as they reflect the order in which the docorators can be applied;
  * e.g: DT_BACKGROUND->DT_BORDER->DT_BITMAP->DT_TEXT;
  */
enum ButtonDecoratorType
{
	DT_BACKGROUND = 0,
	DT_BORDER = 1,
	DT_BITMAP = 2,
	DT_TEXT = 3
};


#endif
