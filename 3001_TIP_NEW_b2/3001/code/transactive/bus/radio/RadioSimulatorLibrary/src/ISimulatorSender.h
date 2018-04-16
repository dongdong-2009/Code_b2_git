/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/bus/radio/RadioSimulatorLibrary/src/ISimulatorSender.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */


#ifndef SIMULATORSENDER_H
#define SIMULATORSENDER_H

#include "core/defaults/src/DefaultEnvironment.h"

namespace TA_IRS_Bus
{
    
    class ISimulatorSender
    {
    public:
        virtual void simulatorSendEvent(const char * buf, const unsigned int length) = 0;
    };

}

#endif // SIMULATORSENDER_H