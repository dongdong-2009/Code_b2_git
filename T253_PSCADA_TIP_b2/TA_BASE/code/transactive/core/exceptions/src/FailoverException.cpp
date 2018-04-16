/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/FailoverException.cpp $
  * @author:  <Rob Ashcroft>
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2017/10/31 16:21:54 $
  * Last modified by:  $Author: CM $
  * 
  * <description>
  */


// FailoverException.cpp: implementation of the FailoverException class.
//
//////////////////////////////////////////////////////////////////////

#include "FailoverException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{
    FailoverException::FailoverException() throw()
    {

    }

    FailoverException::FailoverException(const std::string& msg) throw() 
            : TA_Base_Core::TransactiveException(msg) , m_message(msg)
	        {

	        }

    FailoverException::~FailoverException() throw()
    {

    }

    const char* FailoverException::what() const throw()
    {
        return m_message.c_str();
    }
};
