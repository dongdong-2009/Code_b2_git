/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/AtsAgentException.cpp $
  *  @author:  Darren Sampson
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
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
