/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/ArchiveException.cpp $
  * @author Katherine Thomson
  * @version $Revision: #2 $
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
