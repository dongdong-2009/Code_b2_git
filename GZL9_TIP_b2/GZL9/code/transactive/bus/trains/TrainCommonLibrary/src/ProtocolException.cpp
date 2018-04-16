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

#include "bus/trains/TrainCommonLibrary/src/ProtocolException.h"


namespace TA_IRS_Bus
{

    ProtocolException::ProtocolException() throw ()
    {
    }

    ProtocolException::~ProtocolException() throw ()
    {
    }

    ProtocolException::ProtocolException(const std::string& msg,
                                         const std::string& fieldName, 
                                         const std::string& fieldValue) throw ()
        : TrainException(msg),
          m_fieldName(fieldName),
          m_fieldValue(fieldValue)
    {
    }

    std::string ProtocolException::getFieldName() const throw ()
    {
        return m_fieldName;
    }

    std::string ProtocolException::getFieldValue() const throw ()
    {
        return m_fieldValue;
    }
}
