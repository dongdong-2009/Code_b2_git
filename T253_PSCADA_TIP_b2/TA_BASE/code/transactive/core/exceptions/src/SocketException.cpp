/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/SocketException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#include "SocketException.h"

namespace TA_Base_Core
{
    SocketException::SocketException() throw()
    {}

    SocketException::SocketException(const std::string& msg) 
        throw(): TransactiveException(msg)
    {}

    SocketException::~SocketException() throw()
    {}
}
