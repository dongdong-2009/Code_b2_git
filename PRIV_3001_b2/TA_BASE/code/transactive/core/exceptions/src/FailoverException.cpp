/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/exceptions/src/FailoverException.cpp $
  * @author:  <Rob Ashcroft>
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
