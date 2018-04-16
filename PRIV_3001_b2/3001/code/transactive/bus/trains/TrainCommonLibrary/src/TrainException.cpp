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

#include "bus/trains/TrainCommonLibrary/src/TrainException.h"


namespace TA_IRS_Bus
{

    TrainException::TrainException() throw ()
    {
    }



    TrainException::~TrainException() throw ()
    {
    }


    TrainException::TrainException(const std::string& msg) throw ()
        : TransactiveException(msg)
    {
    }
}
