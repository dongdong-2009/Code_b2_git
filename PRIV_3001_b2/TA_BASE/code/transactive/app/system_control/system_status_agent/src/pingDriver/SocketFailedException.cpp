/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/app/system_control/system_status_agent/src/pingDriver/SocketFailedException.cpp $
  * @author:   Chris DeWolfe
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last mofified by:  $Author: lim.cy $
  *
  * SocketFailedException class implimentation. See header file for class descrption
  */

#include "app/system_control/system_status_agent/src/pingDriver/SocketFailedException.h"

namespace TA_Base_App
{
	SocketFailedException::SocketFailedException() throw()
	{

	}

	SocketFailedException::SocketFailedException(const std::string& errorMessage)
		throw() : TA_Base_Core::TransactiveException(errorMessage)
	{


	}

	SocketFailedException::~SocketFailedException() throw()
	{

	}
}
