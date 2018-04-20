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

#include "bus/trains/TrainCommonLibrary/src/TrainCallException.h"

namespace TA_IRS_Bus
{
    TrainCallException::TrainCallException() throw ()
    {
    }

    
    TrainCallException::~TrainCallException() throw ()
    {
    }

    
    TrainCallException::TrainCallException(const std::string& msg, 
                                           ProtocolCallTypes::ECallTrainCommandStatus errorType) throw ()
        : TrainException(msg),
          m_errorType(errorType)
    {
    }

    
    ProtocolCallTypes::ECallTrainCommandStatus TrainCallException::getErrorType() const throw ()
    {
        return m_errorType;
    }
}
