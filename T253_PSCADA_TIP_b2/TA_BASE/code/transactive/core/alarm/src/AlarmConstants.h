/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/alarm/src/AlarmConstants.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
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
