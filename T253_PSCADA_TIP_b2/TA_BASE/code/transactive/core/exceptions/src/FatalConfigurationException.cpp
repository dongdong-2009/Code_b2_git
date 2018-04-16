/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/FatalConfigurationException.cpp $
  *  @author:  Darren Sampson
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2017/10/31 16:21:54 $
  *  Last modified by:  $Author: CM $
  *
  *  Implementation of .
  */

#include "FatalConfigurationException.h"

namespace TA_Base_Core
{
		FatalConfigurationException::FatalConfigurationException() throw()
		{
		}

		FatalConfigurationException::FatalConfigurationException(const std::string& msg) throw() :
			m_msg(msg)
		{
		}
		
		const char* FatalConfigurationException::what() const throw()
		{
			return( m_msg.c_str() );
		}
		
		FatalConfigurationException::~FatalConfigurationException() throw()
		{
		}

} // TA_Base_Core
