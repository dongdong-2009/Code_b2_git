/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/exceptions/src/AlarmAgentException.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * <description>
  *
  */

#if !defined(ALARMAGENTEXCEPTION)
#define ALARMAGENTEXCEPTION

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/exceptions/src/TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
		class AlarmAgentException : public TA_Base_Core::TransactiveException  
	{
    public:
        AlarmAgentException() throw();
        AlarmAgentException(const std::string& msg) throw();
        virtual ~AlarmAgentException() throw();
    };    
}

#endif // !defined(ALARMAGENTEXCEPTION)
