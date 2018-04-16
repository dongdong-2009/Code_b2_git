/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/AlarmActionException.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is thrown if an alarm action cannot be performed. The msg field (retrieved by what())
  * holds an appropriate error message to display to the user.
  */

#include "core/exceptions/src/AlarmActionException.h"

namespace TA_Base_Core
{
    AlarmActionException::AlarmActionException(const EAlarmActionFail failType) throw()
    : m_failType(failType),
      TransactiveException()
    {
    }


    AlarmActionException::AlarmActionException(const EAlarmActionFail failType, const std::string& what) throw()
    : m_failType(failType),
      TransactiveException(what)
    {
    }


    AlarmActionException::~AlarmActionException() throw()
    {
    }
}
