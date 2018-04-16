/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/exceptions/src/DbConnectionNoFile.cpp $
  *  @author:  Robert Stagg
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
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

