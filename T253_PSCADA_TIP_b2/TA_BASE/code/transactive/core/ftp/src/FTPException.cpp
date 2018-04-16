/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/ftp/src/FTPException.cpp $
  * @author:  J. Welton
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * Exception class thrown by the FTP Interface
  *
  */

#include "core/ftp/src/FTPException.h"

namespace TA_Base_Core
{
	FTPException::FTPException() throw() {}

    FTPException::FTPException(const std::string& msg) 
            throw(): TransactiveException(msg) {}

    FTPException::~FTPException() throw() {}
}
