/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/generic_gui/src/ITransActiveFrame.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Since we have multiple Frame classes we must have an interface to declare common
  * methods.
  */

#if !defined(AFX_ITRANSACTIVE_FRAME_H__8FF8DE95_4565_4123_860A_1F8475DC763D__INCLUDED_)
#define AFX_ITRANSACTIVE_FRAME_H__8FF8DE95_4565_4123_860A_1F8475DC763D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{
    class IGUIAccess;

    class ITransActiveFrame
    {
    public:

        /**
        * Destructor
        */
        virtual ~ITransActiveFrame(){};

       /**
        * setCallbackObject
        *
        * This not only sets the callback IGUIAccess object it also performs
        * the generic initialisation required by all windows. This means this
        * must be called after the Window has been created.
        *
        * @param IGuiAccess& - The IGUIAccess object to perform calls on. 
        */
       virtual void setCallbackObject( IGUIAccess& gui) = 0;
    };
}

#endif // !defined(AFX_ITRANSACTIVE_FRAME_H__8FF8DE95_4565_4123_860A_1F8475DC763D__INCLUDED_)
