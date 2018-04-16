/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Robert van Hugten
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "bus/trains/TrainCommonLibrary/src/GroupInUseException.h"


namespace TA_IRS_Bus
{

    GroupInUseException::GroupInUseException() throw ()
    {
    }


    GroupInUseException::~GroupInUseException() throw ()
    {
    }


    GroupInUseException::GroupInUseException(const std::string& msg) throw ()
        : TrainException( msg )
    {
    }
}
