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

#include "bus/trains/TrainCommonLibrary/src/InvalidTrainException.h"


namespace TA_IRS_Bus
{

    InvalidTrainException::InvalidTrainException() throw ()
    {
    }



    InvalidTrainException::~InvalidTrainException() throw ()
    {
    }


    InvalidTrainException::InvalidTrainException(const std::string& msg, 
                                                 CommonTypes::TrainIdType trainId) throw ()
        : TrainException(msg),
          m_trainId(trainId)
    {
    }


    CommonTypes::TrainIdType InvalidTrainException::getTrainId() const throw ()
    {
        return m_trainId;
    }
}
