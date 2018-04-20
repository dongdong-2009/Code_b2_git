/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/ArchiveException.cpp $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
