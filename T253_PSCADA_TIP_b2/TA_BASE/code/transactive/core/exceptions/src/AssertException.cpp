/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/AssertException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
// AssertException.cpp: implementation of the AssertException class.
//
//////////////////////////////////////////////////////////////////////

#include "AssertException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{
	AssertException::AssertException() 
		throw()
	{

	}

	AssertException::AssertException(const std::string& msg) 
		throw(): TransactiveException(msg)
	{

	}

	AssertException::~AssertException() throw()
	{

	}
}
