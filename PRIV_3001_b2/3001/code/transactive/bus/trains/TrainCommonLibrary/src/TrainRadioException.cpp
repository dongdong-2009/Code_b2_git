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

#include "bus/trains/TrainCommonLibrary/src/TrainRadioException.h"


namespace TA_IRS_Bus
{
    TrainRadioException::TrainRadioException() throw ()
    {
    }

    
    TrainRadioException::~TrainRadioException() throw ()
    {
    }

    
    TrainRadioException::TrainRadioException(const std::string& msg, 
                                             CommonTypes::ERadioErrorType errorType) throw ()
        : TrainException(msg), 
          m_errorType(errorType)
    {
    }

    
    CommonTypes::ERadioErrorType TrainRadioException::getErrorType() const throw ()
    {
        return m_errorType;
    }
}

