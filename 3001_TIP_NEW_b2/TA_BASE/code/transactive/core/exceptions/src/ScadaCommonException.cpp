/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/ScadaCommonException.cpp $
  *  @author:  Darren Sampson
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  Implementation of .
  */

#include "core/exceptions/src/ScadaCommonException.h"

namespace TA_Base_Core
{
		ScadaCommonException::ScadaCommonException() throw()
		{
		}

		ScadaCommonException::ScadaCommonException(const std::string& msg) throw() :
			m_msg(msg)
		{
		}
		
		const char* ScadaCommonException::what() const throw()
		{
			return( m_msg.c_str() );
		}
		
		ScadaCommonException::~ScadaCommonException() throw()
		{
		}

} // TA_Base_Core
