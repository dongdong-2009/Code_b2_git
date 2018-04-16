/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/ScadaProxyException.cpp $
  *  @author:  Darren Sampson
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2017/10/31 16:21:54 $
  *  Last modified by:  $Author: CM $
  *
  *  Implementation of .
  */

#include "ScadaProxyException.h"

namespace TA_Base_Core
{
		ScadaProxyException::ScadaProxyException() throw()
		{
		}

		ScadaProxyException::ScadaProxyException(const std::string& msg) throw() :
			m_msg(msg)
		{
		}
		
		const char* ScadaProxyException::what() const throw()
		{
			return( m_msg.c_str() );
		}
		
		ScadaProxyException::~ScadaProxyException() throw()
		{
		}

} // TA_Base_Core
