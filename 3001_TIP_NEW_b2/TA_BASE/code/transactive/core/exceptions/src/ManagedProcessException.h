/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/ManagedProcessException.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */

#if !defined(MANAGEDPROCESSEXCEPTION)
#define MANAGEDPROCESSEXCEPTION

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
		class ManagedProcessException : public TA_Base_Core::TransactiveException  
	{
    public:
        ManagedProcessException() throw();
        ManagedProcessException(const std::string& msg) throw();
        virtual ~ManagedProcessException() throw();
    };    
}

#endif // !defined(MANAGEDPROCESSEXCEPTION)
