/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/DbConnectionFailed.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implements DbConnectionFailed.
  */

#include "DbConnectionFailed.h"

namespace TA_Base_Core
{

	DbConnectionFailed::DbConnectionFailed(const std::string& msg) throw() :
		  DatabaseException(msg)
	{
	}

} // Closes TA_Base_Core

