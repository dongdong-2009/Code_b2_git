/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/mfc_extensions/src/decorator_button/IDecoratorButtonBehaviourStrategy.h $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 */

#if !defined(IDecoratorButtonBehaviourStrategy_3525F666_E11A_4353_A38F_F645CBA7B7D6__INCLUDED_)
#define IDecoratorButtonBehaviourStrategy_3525F666_E11A_4353_A38F_F645CBA7B7D6__INCLUDED_

#include <afxwin.h>         // MFC core and standard components

namespace TA_Base_Bus
{
    class IDecoratorButtonBehaviourStrategy
    {
    public:
        virtual ~IDecoratorButtonBehaviourStrategy() {};

	    /**
	      * leftClickBehaviour
	      * 
	      * Callback when left button is released
	      */
	    virtual void leftClickBehaviour() =0;

	    /**
	      * rightClickBehaviour
	      * 
	      * Callback when right button is released
	      */
	    virtual void rightClickBehaviour() =0;

	    /**
	      * dropBehaviour
	      * 
	      * Callback when a file is dropped on the button.
          * The button must have ACCEPT FILES style, in order to use this
	      */
	    virtual void dropBehaviour(HDROP hDropInfo) =0;

    };
};

#endif // !defined(IDecoratorButtonBehaviourStrategy_3525F666_E11A_4353_A38F_F645CBA7B7D6__INCLUDED_)
