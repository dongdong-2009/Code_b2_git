#ifndef TIS_AGENT_EXCEPTIONS_H_INCLUDED
#define TIS_AGENT_EXCEPTIONS_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/Exceptions.h $
  * @author:   Robin Ashcroft 
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Specifies all the exceptions thrown by the TIS Agent.
  */

#include <string>

#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_Core
{
	// Generic TIS Agent Exception

    class TISAgentException : public TA_Base_Core::TransactiveException
    {
    public:
        TISAgentException( const std::string& what ) throw();
        virtual ~TISAgentException() throw();
    };

}

#endif // TIS_AGENT_EXCEPTIONS_H_INCLUDED

