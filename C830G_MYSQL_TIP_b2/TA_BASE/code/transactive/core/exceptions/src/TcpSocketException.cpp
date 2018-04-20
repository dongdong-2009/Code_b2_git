/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/TcpSocketException.cpp $
  * @author:  J. Welton
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Exception class thrown by the TcpSocket subsystem
  *
  */

#include "TcpSocketException.h"

namespace TA_Base_Core
{

    TcpSocketException::TcpSocketException() throw()
    {}

    TcpSocketException::TcpSocketException(const std::string& msg) 
        throw(): TransactiveException(msg)
    {}

    TcpSocketException::~TcpSocketException() throw()
    {}
}
