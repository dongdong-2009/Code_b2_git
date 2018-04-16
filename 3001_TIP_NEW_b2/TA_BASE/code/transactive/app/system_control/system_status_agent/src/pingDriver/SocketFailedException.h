/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/system_status_agent/src/pingDriver/SocketFailedException.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last mofified by:  $Author: haijun.li $
  *
  * This exception is thrown if we can not open the socket for pinging
  */

#if !defined(AFX_SOCKETFAILEDEXCEPTION_H__9F17E9D6_1704_4ACE_BB8C_039D7C3A9F64__INCLUDED_)
#define AFX_SOCKETFAILEDEXCEPTION_H__9F17E9D6_1704_4ACE_BB8C_039D7C3A9F64__INCLUDED_

#include "core/exceptions/src/TransactiveException.h"
#include <string>

namespace TA_Base_App
{
	class SocketFailedException : public TA_Base_Core::TransactiveException   
	{
	public:
		SocketFailedException() throw();
		SocketFailedException(const std::string& errorMessage) throw();
		virtual ~SocketFailedException() throw();

	};
}
#endif // !defined(AFX_SOCKETFAILEDEXCEPTION_H__9F17E9D6_1704_4ACE_BB8C_039D7C3A9F64__INCLUDED_)
