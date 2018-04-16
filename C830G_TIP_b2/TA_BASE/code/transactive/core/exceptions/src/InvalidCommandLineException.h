/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/InvalidCommandLineException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
