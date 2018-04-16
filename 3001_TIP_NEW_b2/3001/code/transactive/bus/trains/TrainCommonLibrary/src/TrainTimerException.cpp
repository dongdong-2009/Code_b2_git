/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "bus/trains/TrainCommonLibrary/src/TrainTimerException.h"


namespace TA_IRS_Bus
{

    TrainTimerException::TrainTimerException() throw ()
    {
    }

    
    TrainTimerException::~TrainTimerException() throw ()
    {
    }

    
    TrainTimerException::TrainTimerException(const std::string& msg) throw ()
        : TrainException(msg)
    {

    }
}
