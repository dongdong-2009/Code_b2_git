/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/core/exceptions/src/ScadaProxyException.cpp $
  *  @author:  Darren Sampson
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
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
