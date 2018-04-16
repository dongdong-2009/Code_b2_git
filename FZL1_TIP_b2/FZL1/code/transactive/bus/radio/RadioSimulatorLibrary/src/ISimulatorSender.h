/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/bus/radio/RadioSimulatorLibrary/src/ISimulatorSender.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
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