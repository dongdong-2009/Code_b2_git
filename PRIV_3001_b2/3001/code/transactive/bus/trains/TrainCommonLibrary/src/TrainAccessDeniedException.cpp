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

#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"


namespace TA_IRS_Bus
{

    TrainAccessDeniedException::TrainAccessDeniedException() throw ()
    {
    }

    TrainAccessDeniedException::~TrainAccessDeniedException() throw ()
    {
    }


    TrainAccessDeniedException::TrainAccessDeniedException(const std::string& msg) throw ()
        : TrainException( msg )
    {
    }
}
