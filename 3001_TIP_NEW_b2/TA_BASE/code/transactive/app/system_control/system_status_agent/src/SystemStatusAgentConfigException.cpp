/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/system_status_agent/src/SystemStatusAgentConfigException.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
  *
  * SystemStatusAgentConfigException class implimentation. See header file for class descrption
  */

#include "SystemStatusAgentConfigException.h"

namespace TA_Base_App
{
	SystemStatusAgentConfigException::SystemStatusAgentConfigException(const std::string& errorMessage) throw()
		: TA_Base_Core::TransactiveException(errorMessage)
	{

	}

	SystemStatusAgentConfigException::~SystemStatusAgentConfigException() throw()
	{

	}
}
