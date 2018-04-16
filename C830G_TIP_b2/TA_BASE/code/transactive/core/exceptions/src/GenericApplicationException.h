/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/GenericApplicationException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#if !defined(GENERICAPPLICATIONEXCEPTION)
#define GENERICAPPLICATIONEXCEPTION

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
		class GenericApplicationException : public TA_Base_Core::TransactiveException  
	{
    public:
        GenericApplicationException() throw();
        GenericApplicationException(const std::string& msg) throw();
        virtual ~GenericApplicationException() throw();
    };    
}

#endif // !defined(GENERICAPPLICATIONEXCEPTION)
