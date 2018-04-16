/**
 * The source code in this file is the property of 
 * Ripple Systems Pty Ltd and is not for redistribution
 * in any form.
 *
 * Source: $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/AuthenticationSecurityException.cpp $
 * @author N. Jardine
 * @version $Revision: #1 $
 * Last modification: $DateTime: 2017/10/31 16:21:54 $
 * Last modified by:  $Author: CM $
 * 
 * This file outlines a simple exception class for use with
 * authentication code. It is derived from TransactiveException.
 * This exception will be thrown when Authentication is denied,
 * and is used to specifify the reason for the denial.
 */

#include "core/exceptions/src/AuthenticationSecurityException.h"

namespace TA_Base_Core
{


	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////


	AuthenticationSecurityException::AuthenticationSecurityException(const char *msg, const ESecurityReason reason) 
		throw(): TransactiveException(msg), m_reason(reason)
	{
		logException();
	}	

	AuthenticationSecurityException::~AuthenticationSecurityException() throw()
	{

	}

	void AuthenticationSecurityException::logException()
	{
		//TODO: Add DebugUtil logging of the exception here.
	}

} // Closes TA_Base_Core
