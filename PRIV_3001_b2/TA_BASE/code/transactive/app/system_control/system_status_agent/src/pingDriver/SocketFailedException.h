/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/app/system_control/system_status_agent/src/pingDriver/SocketFailedException.h $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
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
