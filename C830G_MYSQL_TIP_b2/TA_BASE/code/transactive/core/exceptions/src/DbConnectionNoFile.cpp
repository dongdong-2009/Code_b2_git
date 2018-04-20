/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/DbConnectionNoFile.cpp $
  *  @author:  Robert Stagg
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implements DbConnectionNoFile.
  */

#include "DbConnectionNoFile.h"

namespace TA_Base_Core
{

	DbConnectionNoFile::DbConnectionNoFile(const std::string& msg) throw() :
		  DatabaseException(msg)
	{
	}

} // Closes TA_Base_Core

