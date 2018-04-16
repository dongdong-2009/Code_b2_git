/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/InterlockingException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#include "InterlockingException.h"


namespace TA_Base_Core
{
    InterlockingException::InterlockingException(const std::string& message) throw()
        : TransactiveException(message)
    {
    }

    InterlockingException::~InterlockingException() throw()
    {
    }
} // Closes TA_Base_Core
