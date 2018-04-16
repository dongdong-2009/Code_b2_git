#ifndef PIDS_AGENT_EXCEPTIONS_H_INCLUDED
#define PIDS_AGENT_EXCEPTIONS_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL13_TIP/GZL13/transactive/app/signs/pids_agent/src/Exceptions.h $
  * @author:   Robin Ashcroft 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  * Specifies all the exceptions thrown by the PIDS Agent.
  */

#include <string>

#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_Core
{
	// Generic PIDS Agent Exception

    class PIDSAgentException : public TA_Base_Core::TransactiveException
    {
    public:
        PIDSAgentException( const std::string& what ) throw();
        virtual ~PIDSAgentException() throw();
    };

}

#endif // PIDS_AGENT_EXCEPTIONS_H_INCLUDED

