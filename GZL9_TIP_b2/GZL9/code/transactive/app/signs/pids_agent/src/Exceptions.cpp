/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL13_TIP/GZL13/transactive/app/signs/pids_agent/src/Exceptions.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  * Specifies all the exceptions thrown by the PIDS Agent.
  */

#include "app/signs/pids_agent/src/Exceptions.h"

namespace TA_IRS_Core
{
        PIDSAgentException::PIDSAgentException( const std::string& what ) throw()
            : TransactiveException( what )
        {
            // Do nothing.
        }
        PIDSAgentException::~PIDSAgentException() throw() {};
}

