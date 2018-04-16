/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/AlarmStoreException.h $
  * @author:  Ross Tucker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */

#if !defined(ALARMSTOREEXCEPTION)
#define ALARMSTOREEXCEPTION

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "TransactiveException.h"
#include <string>

namespace TA_Base_Core
{
		class AlarmStoreException : public TA_Base_Core::TransactiveException  
	{
    public:
        AlarmStoreException() throw();
        AlarmStoreException(const std::string& msg) throw();
        virtual ~AlarmStoreException() throw();
    };    
}

#endif // !defined(ALARMSTOREEXCEPTION)
