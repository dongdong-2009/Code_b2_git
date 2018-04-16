/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/RightsException.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  Implementation of .
  */

#include "RightsException.h"

namespace TA_Base_Core
{
		RightsException::RightsException() throw() :
            m_type(GENERAL)
		{
		}

		RightsException::RightsException(const std::string& msg, ERightsFailType type /*=GENERAL*/) throw() :
			m_msg(msg), m_type(type)
		{
		}
		
		const char* RightsException::what() const throw()
		{
			return( m_msg.c_str() );
		}
		
		RightsException::~RightsException() throw()
		{
		}

        RightsException::ERightsFailType RightsException::getFailType() const
        {
            return m_type;
        }
       
} // TA_Base_Core
