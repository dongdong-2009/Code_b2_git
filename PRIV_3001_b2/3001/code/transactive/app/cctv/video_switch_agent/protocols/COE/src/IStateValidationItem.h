#ifndef ISTATE_VALIDATION_ITEM_H
#define ISTATE_VALIDATION_ITEM_H

/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/PRIV_3001/3001/transactive/app/cctv/video_switch_agent/protocols/COE/src/IStateValidationItem.h $
 * @author:  Katherine Thomson
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
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