/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/Exceptions.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Specifies all the exceptions thrown by the TIS Agent.
  */

#include "app/signs/tis_agent/src/Exceptions.h"

namespace TA_IRS_Core
{
        TISAgentException::TISAgentException( const std::string& what ) throw()
            : TransactiveException( what )
        {
            // Do nothing.
        }
        TISAgentException::~TISAgentException() throw() {};
}

