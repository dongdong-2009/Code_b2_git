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

#include "bus/trains/TrainCommonLibrary/src/CctvException.h"


namespace TA_IRS_Bus
{

    CctvException::CctvException() throw ()
    {
    }


    CctvException::~CctvException() throw ()
    {
    }


    CctvException::CctvException( const std::string& msg,
                                  ProtocolCctvTypes::ECctvCommandStatus errorType ) throw ()
        : TrainException( msg ),
          m_errorType( errorType )
    {
    }


    ProtocolCctvTypes::ECctvCommandStatus CctvException::getErrorType() const throw ()
    {
        return m_errorType;
    }
}
