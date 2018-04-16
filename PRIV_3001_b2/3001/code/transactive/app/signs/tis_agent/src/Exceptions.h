#ifndef TIS_AGENT_EXCEPTIONS_H_INCLUDED
#define TIS_AGENT_EXCEPTIONS_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/Exceptions.h $
  * @author:   Robin Ashcroft 
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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

