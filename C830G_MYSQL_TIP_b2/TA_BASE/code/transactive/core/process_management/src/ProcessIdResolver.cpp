/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/process_management/src/ProcessIdResolver.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#include "core/process_management/src/ProcessIdResolver.h"
#include "core/exceptions/src/ManagedProcessException.h"
#include "core/utilities/src/DebugUtil.h"

#if !defined( WIN32 )
    #include <unistd.h>
#else
    #include <process.h>
#endif

using TA_Base_Core::ManagedProcessException;

namespace TA_Base_Core
{

    //
    // getProcessId
    //
    unsigned long ProcessIdResolver::getProcessId()
    {
        #if !defined( WIN32 )
            pid_t pid = getpid();
            if( pid <= 0 )
            {
                TA_THROW( ManagedProcessException("Failed to resolve the process id") );
            }
            return static_cast<unsigned long>(pid);
        #else
            return static_cast<unsigned long>(_getpid());
        #endif
    }
};
