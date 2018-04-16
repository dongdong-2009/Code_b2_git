/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/AlarmAgentException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#include "AlarmAgentException.h"

namespace TA_Base_Core
{

    AlarmAgentException::AlarmAgentException() throw()
    {}

    AlarmAgentException::AlarmAgentException(const std::string& msg) 
        throw(): TransactiveException(msg)
    {}

    AlarmAgentException::~AlarmAgentException() throw()
    {}
}
