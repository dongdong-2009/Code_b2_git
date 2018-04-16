/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/ProcessAlreadyRunningException.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#include "ProcessAlreadyRunningException.h"

namespace TA_Base_Core
{

    ProcessAlreadyRunningException::ProcessAlreadyRunningException() throw()
    {}

    ProcessAlreadyRunningException::ProcessAlreadyRunningException(const std::string& msg) 
        throw(): TransactiveException(msg)
    {}

    ProcessAlreadyRunningException::~ProcessAlreadyRunningException() throw()
    {}
}
