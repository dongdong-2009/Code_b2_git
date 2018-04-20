/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/ReportManagerException.cpp $
  *  @author:  A. Lee
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
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
