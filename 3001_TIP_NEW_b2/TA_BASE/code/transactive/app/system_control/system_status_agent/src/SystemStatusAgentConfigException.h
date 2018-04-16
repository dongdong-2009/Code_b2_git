/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/system_status_agent/src/SystemStatusAgentConfigException.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
  *
  * This exception is thrown if the System Status Agent Configuration is invalid
  */
#if !defined(AFX_SYSTEMSTATUSAGENTCONFIGEXCEPTION_H__58C7AAFE_3316_4456_B99F_1F7E27018301__INCLUDED_)
#define AFX_SYSTEMSTATUSAGENTCONFIGEXCEPTION_H__58C7AAFE_3316_4456_B99F_1F7E27018301__INCLUDED_


#include "core/exceptions/src/TransactiveException.h"
#include <string>

namespace TA_Base_App
{
	class SystemStatusAgentConfigException : public TA_Base_Core::TransactiveException  
	{
	public:
		SystemStatusAgentConfigException(const std::string& errorMessage) throw();
		virtual ~SystemStatusAgentConfigException() throw();

	};
}

#endif // !defined(AFX_SYSTEMSTATUSAGENTCONFIGEXCEPTION_H__58C7AAFE_3316_4456_B99F_1F7E27018301__INCLUDED_)
