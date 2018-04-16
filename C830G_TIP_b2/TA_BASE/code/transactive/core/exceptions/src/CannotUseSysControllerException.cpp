/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/CannotUseSysControllerException.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * This is used by the Configuration Editor. It is thrown when it cannot connect to the system using
  * the System Controller. This basically means it will have to connect using a direct database connection
  */

#include "core/exceptions/src/CannotUseSysControllerException.h"

namespace TA_Base_Core
{

    CannotUseSysControllerException::CannotUseSysControllerException(const std::string& msg) throw() 
            : TA_Base_Core::TransactiveException(msg)
    {

	}
 
    CannotUseSysControllerException::~CannotUseSysControllerException() throw()
    {

    }
};
