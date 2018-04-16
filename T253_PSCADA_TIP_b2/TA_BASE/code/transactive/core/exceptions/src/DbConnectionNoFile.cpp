/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/DbConnectionNoFile.cpp $
  *  @author:  Robert Stagg
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2017/10/31 16:21:54 $
  *  Last modified by:  $Author: CM $
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

