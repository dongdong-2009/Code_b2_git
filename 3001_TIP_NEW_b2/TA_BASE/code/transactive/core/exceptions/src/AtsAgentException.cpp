/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/AtsAgentException.cpp $
  *  @author:  Darren Sampson
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  Implementation of .
  */

#include "AtsAgentException.h"

namespace TA_Base_Core
{
		AtsAgentException::AtsAgentException() throw()
		{
		}

		AtsAgentException::AtsAgentException(const std::string& msg) throw() :
			m_msg(msg)
		{
		}
		
		const char* AtsAgentException::what() const throw()
		{
			return( m_msg.c_str() );
		}
		
		AtsAgentException::~AtsAgentException() throw()
		{
		}

} // TA_Base_Core
