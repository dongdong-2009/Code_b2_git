/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/exceptions/src/AlarmCacheCapacityExceededException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  */

#if !defined(ALARM_CACHE_CAPACITY_EXCEEDED_EXCEPTION)
#define ALARM_CACHE_CAPACITY_EXCEEDED_EXCEPTION

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
		class AlarmCacheCapacityExceededException : public TA_Base_Core::TransactiveException  
	{
    public:
        AlarmCacheCapacityExceededException() throw();
        AlarmCacheCapacityExceededException(const std::string& msg) throw();
        virtual ~AlarmCacheCapacityExceededException() throw();
    };    
}

#endif // !defined(ALARM_CACHE_CAPACITY_EXCEEDED_EXCEPTION)
