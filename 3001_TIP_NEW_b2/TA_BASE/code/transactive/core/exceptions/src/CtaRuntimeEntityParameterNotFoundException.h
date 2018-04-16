#if !defined(CtaRuntimeEntityParameterNotFoundException_23D152DC_E778_48d3_A702_A2F7FD1AD5D7__INCLUDED_)
#define CtaRuntimeEntityParameterNotFoundException_23D152DC_E778_48d3_A702_A2F7FD1AD5D7__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems Pty Ltd and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/CtaRuntimeEntityParameterNotFoundException.h $
  * @author K. Graham
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Exception to be thrown from CtaRuntime when entity configuration cannot be found
  */

#include "TransactiveException.h"

namespace TA_Base_Core
{
    class CtaRuntimeEntityParameterNotFoundException : public TransactiveException 
    {
    public:

        CtaRuntimeEntityParameterNotFoundException(const std::string& msg) throw();


	    virtual ~CtaRuntimeEntityParameterNotFoundException() throw();

    };
}
#endif // !defined(CtaRuntimeEntityParameterNotFoundException_23D152DC_E778_48d3_A702_A2F7FD1AD5D7__INCLUDED_)
