/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/exceptions/src/UtilitiesException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  */
// UtilitiesException.cpp: implementation of the UtilitiesException class.
//
//////////////////////////////////////////////////////////////////////

#include "UtilitiesException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{

	UtilitiesException::UtilitiesException() throw()
	{

	}

	UtilitiesException::UtilitiesException(const std::string& msg) 
		throw(): TransactiveException(msg)
	{

	}

	UtilitiesException::~UtilitiesException() throw()
	{

	}
}
