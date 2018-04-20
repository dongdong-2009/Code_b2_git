/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/TransactiveException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */
// TransactiveException.cpp: implementation of the TransactiveException class.
//
//////////////////////////////////////////////////////////////////////

#include "TransactiveException.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{
	TransactiveException::TransactiveException()
		throw(): std::exception()
	{
	}

	TransactiveException::TransactiveException(const std::string& msg) 
		throw(): std::exception(), m_msg(msg)
	{
	}

	const char* TransactiveException::what() const throw()
	{
		return m_msg.c_str();
	}

	TransactiveException::~TransactiveException() throw()
	{
	}
}
