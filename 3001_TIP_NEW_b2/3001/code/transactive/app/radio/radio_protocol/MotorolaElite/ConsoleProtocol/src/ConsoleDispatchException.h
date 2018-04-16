/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/ConsoleProtocol/src/ConsoleDispatchException.h $
  * @author:  Wu Min Tao
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2009/12/15 19:31:45 $
  * Last modified by:  $Author: grace.koh $
  *
  */
#ifndef __CONSOLE_DISPATCH_EXCEPTION_H
#define __CONSOLE_DISPATCH_EXCEPTION_H
#include <string>

#include "core/exceptions/src/TransactiveException.h"

namespace TA_IRS_App {
		class ConsoleDispatchException : public TA_Base_Core::TransactiveException
		{
		public:
			ConsoleDispatchException() : TA_Base_Core::TransactiveException() {};
            ConsoleDispatchException(const std::string& what) : TA_Base_Core::TransactiveException(what) {};
			virtual ~ConsoleDispatchException() {};
		};
	
};

#endif
