/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/alarm/src/AlarmConstants.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
