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

#include "bus/trains/TrainCommonLibrary/src/TtisException.h"

namespace TA_IRS_Bus
{

    TtisException::TtisException() throw ()
    {
    }


    TtisException::~TtisException() throw ()
    {
    }

    
    TtisException::TtisException(const std::string& msg,
                                 ProtocolTtisTypes::ETtisResult errorType) throw ()
        : TrainException(msg),
          m_errorType(errorType)
    {
    }

    
    ProtocolTtisTypes::ETtisResult TtisException::getErrorType() const throw ()
    {
        return m_errorType;
    }
}
