/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/CtaRuntimeEntityParameterNotFoundException.cpp $
  * @author K. Graham
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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