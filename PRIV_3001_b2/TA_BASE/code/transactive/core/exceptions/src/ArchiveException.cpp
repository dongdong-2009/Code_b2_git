/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/TA_BASE/transactive/core/exceptions/src/ArchiveException.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * Exception used by the ArchiveManager and the archive_manager_dai.
  */

#include "core/exceptions/src/ArchiveException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{
	ArchiveException::ArchiveException( const std::string& msg ) 
		throw()
        : TransactiveException(msg)
	{
        // Do nothing
	}
}
