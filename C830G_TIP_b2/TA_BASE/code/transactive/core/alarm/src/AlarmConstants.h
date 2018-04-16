/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/alarm/src/AlarmConstants.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */
#if !defined(ALARM_CONSTANTS_H)
#define ALARM_CONSTANTS_H

#include <sys/timeb.h>

namespace TA_Base_Core
{
    namespace AlarmConstants
    {        
        const timeb defaultTime = {0,0,-1,-1};
    }
}

#endif // !defined(ALARM_CONSTANTS_H)
