/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/InvalidCommandLineException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  */

#if !defined(INVALIDCOMMANDLINEEXCEPTION)
#define INVALIDCOMMANDLINEEXCEPTION

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
		class InvalidCommandLineException : public TA_Base_Core::TransactiveException  
	{
    public:
        InvalidCommandLineException() throw();
        InvalidCommandLineException(const std::string& msg) throw();
        virtual ~InvalidCommandLineException() throw();
    };    
}

#endif // !defined(INVALIDCOMMANDLINEEXCEPTION)
