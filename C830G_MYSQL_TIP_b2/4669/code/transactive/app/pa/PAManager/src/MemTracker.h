/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/4669_T01271350/4669/transactive/app/pa/PAManager/src/MemTracker.h $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by: $Author: builder $
  * 
  * Provides memory leak tracking facilities
  *
  */

#ifndef _MEMTRACKER_HEADER_
#define _MEMTRACKER_HEADER_

namespace MemTracker
{
    /**
     * Attaches hook to new handler
     *
     */
    void attachMemoryHook();
}

#endif //_MEMTRACKER_HEADER_