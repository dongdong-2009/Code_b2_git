/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/ReportManagerException.cpp $
  *  @author:  A. Lee
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  */

#include "ReportManagerException.h"

namespace TA_Base_Core
{
		ReportManagerException::ReportManagerException() throw()
		{
		}

		ReportManagerException::ReportManagerException(const std::string& msg) throw() :
			m_msg(msg)
		{
		}
		
		const char* ReportManagerException::what() const throw()
		{
			return( m_msg.c_str() );
		}
		
		ReportManagerException::~ReportManagerException() throw()
		{
		}

} // TA_Base_Core
