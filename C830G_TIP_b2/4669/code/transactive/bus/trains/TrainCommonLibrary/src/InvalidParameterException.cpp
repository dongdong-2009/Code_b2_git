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

#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"

namespace TA_IRS_Bus
{

    InvalidParameterException::InvalidParameterException() throw ()
    {
    }



    InvalidParameterException::~InvalidParameterException() throw ()
    {
    }


    InvalidParameterException::InvalidParameterException(const std::string& msg, 
                                                         const std::string& parameterName, 
                                                         const std::string& parameterValue) throw ()
        : TrainException(msg),
          m_parameterName(parameterName),
          m_parameterValue(parameterValue)
    {
    }


    std::string InvalidParameterException::getParameterName() const throw ()
    {
        return m_parameterName;
    }


    std::string InvalidParameterException::getParameterValue() const throw ()
    {
        return m_parameterValue;
    }

}
