/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/TA_BASE/transactive/core/exceptions/src/CtaRuntimeEntityParameterNotFoundException.cpp $
  * @author K. Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Exception to be thrown from CtaRuntime when entity configuration cannot be found
  */

#include "CtaRuntimeEntityParameterNotFoundException.h"


namespace TA_Base_Core
{
    CtaRuntimeEntityParameterNotFoundException::CtaRuntimeEntityParameterNotFoundException(const std::string& msg)
        throw(): TransactiveException(msg)
    {
        // Empty
    }


    CtaRuntimeEntityParameterNotFoundException::~CtaRuntimeEntityParameterNotFoundException() throw()
    {
        // Empty
    }
}