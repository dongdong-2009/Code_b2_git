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

#include "bus/trains/TrainCommonLibrary/src/PaException.h"


namespace TA_IRS_Bus
{

    PaException::PaException() throw ()
    {
    }

    
    PaException::~PaException() throw ()
    {
    }

    
    PaException::PaException(const std::string& msg, ProtocolPaTypes::EPaCommandResult errorType) throw ()
        : TrainException(msg),
          m_errorType(errorType)
    {
    }

    
    ProtocolPaTypes::EPaCommandResult PaException::getErrorType() const throw ()
    {
        return m_errorType;
    }

}
