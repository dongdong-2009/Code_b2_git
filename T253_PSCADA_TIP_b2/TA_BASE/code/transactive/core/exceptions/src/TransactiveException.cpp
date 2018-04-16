/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/TransactiveException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
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
