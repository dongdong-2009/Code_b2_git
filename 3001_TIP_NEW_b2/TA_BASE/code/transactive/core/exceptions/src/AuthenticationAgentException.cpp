/**
 * The source code in this file is the property of 
 * Ripple Systems Pty Ltd and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/AuthenticationAgentException.cpp $
 * @author N. Jardine
 * @version $Revision: #2 $
 * Last modification: $DateTime: 2012/02/06 16:15:14 $
 * Last modified by:  $Author: haijun.li $
 * 
 * This file outlines a simple exception class for use with
 * authentication code. It is derived from TransactiveException.
 * This exception is thrown when an unexpected error occurrs in 
 * in the Authentication Agent.
 */

#include "core/exceptions/src/AuthenticationAgentException.h"

namespace TA_Base_Core
{


	AuthenticationAgentException::AuthenticationAgentException(const char *msg) throw(): 
		  TransactiveException(msg)
	{
	}	

	AuthenticationAgentException::~AuthenticationAgentException() throw()
	{

	}

} // Closes TA_Base_Core
