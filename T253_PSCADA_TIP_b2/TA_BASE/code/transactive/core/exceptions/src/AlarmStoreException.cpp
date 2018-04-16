/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/AlarmStoreException.cpp $
  * @author:  Ross Tucker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  *
  */

#include "AlarmStoreException.h"

namespace TA_Base_Core
{

    AlarmStoreException::AlarmStoreException() throw()
    {}

    AlarmStoreException::AlarmStoreException(const std::string& msg) 
        throw(): TransactiveException(msg)
    {}

    AlarmStoreException::~AlarmStoreException() throw()
    {}
}
