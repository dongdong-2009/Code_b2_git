#ifndef ISTATE_VALIDATION_ITEM_H
#define ISTATE_VALIDATION_ITEM_H

/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/IStateValidationItem.h $
 * @author:  Katherine Thomson
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 */

namespace TA_IRS_App
{
    class IStateValidationItem
    {
    public:

        /**
          * ~IAgentRequestCommand
          *
          * Standard destructor.
          */
        virtual ~IStateValidationItem() {};


        /** 
          * validateState
          *
          * Callback for StateValidationCycle to let this item know its time
          * to validate its state.
          */
        virtual void validateState() = 0;

    }; // IStateValidationItem

} // TA_IRS_App

#endif // ISTATE_VALIDATION_ITEM_H