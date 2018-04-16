/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/bus/radio/RadioSimulatorLibrary/src/ISimulatorSender.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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