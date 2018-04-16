#if !defined(TCPSOCKETEXCEPTION)
#define TCPSOCKETEXCEPTION

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/TcpSocketException.h $
  * @author:  J Welton
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * Exception class thrown by the TcpSocket subsystem
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
		class TcpSocketException : public TA_Base_Core::TransactiveException  
	{
    public:
        TcpSocketException() throw();
        TcpSocketException(const std::string& msg) throw();
        virtual ~TcpSocketException() throw();
    };    
}

#endif // !defined(TCPSOCKETEXCEPTION)
