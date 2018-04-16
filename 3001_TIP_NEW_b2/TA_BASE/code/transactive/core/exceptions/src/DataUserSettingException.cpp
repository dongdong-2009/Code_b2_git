/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/DataUserSettingException.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  */
#include "core\exceptions\src\DataUserSettingException.h"

namespace TA_Base_Core
{

    DataUserSettingException::DataUserSettingException(const char* msg) 
        : m_failType(GENERIC), TransactiveException(msg)
    {

    }	

    DataUserSettingException::DataUserSettingException(const char* msg, const Type failType)
        : m_failType(failType), TransactiveException(msg)
    {

    }
    DataUserSettingException::~DataUserSettingException()
    {

    }



} // End TA_Base_Core
