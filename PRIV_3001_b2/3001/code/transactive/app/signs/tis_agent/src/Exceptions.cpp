/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/Exceptions.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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

