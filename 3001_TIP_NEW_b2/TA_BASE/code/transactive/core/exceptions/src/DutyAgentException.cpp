/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/DutyAgentException.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  Implementation of .
  */

#include "DutyAgentException.h"

namespace TA_Base_Core
{
		DutyAgentException::DutyAgentException() throw()
		{
		}

		DutyAgentException::DutyAgentException(const std::string& msg) throw() :
			m_msg(msg)
		{
		}
		
		const char* DutyAgentException::what() const throw()
		{
			return( m_msg.c_str() );
		}
		
		DutyAgentException::~DutyAgentException() throw()
		{
		}

} // TA_Base_Core
