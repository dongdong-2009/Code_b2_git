/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/Exceptions.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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

