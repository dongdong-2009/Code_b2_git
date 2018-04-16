#if !defined(FTPEXCEPTION)
#define FTPEXCEPTION

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/ftp/src/FTPException.h $
  * @author:  J Welton
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Exception class thrown by the FTP Interface
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/exceptions/src/TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
	class FTPException : public TA_Base_Core::TransactiveException  
	{
    public:
   		FTPException() throw();
        FTPException(const std::string& msg) throw();
        virtual ~FTPException() throw();
	};    
}

#endif // !defined(FTPEXCEPTION)
