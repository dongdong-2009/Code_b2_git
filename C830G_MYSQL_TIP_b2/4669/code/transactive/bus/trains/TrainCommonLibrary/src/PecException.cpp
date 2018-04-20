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

#include "bus/trains/TrainCommonLibrary/src/PecException.h"


namespace TA_IRS_Bus
{

    PecException::PecException() throw ()
    {
    }

    
    PecException::~PecException() throw ()
    {
    }

    
    PecException::PecException(const std::string& msg, ProtocolPecTypes::EPecCommandResult errorType) throw ()
        : TrainException(msg),
          m_errorType(errorType)
    {
    }

    
    ProtocolPecTypes::EPecCommandResult PecException::getErrorType() const throw ()
    {
        return m_errorType;
    }
}
