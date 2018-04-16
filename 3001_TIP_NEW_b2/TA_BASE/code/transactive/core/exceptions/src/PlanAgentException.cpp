/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/PlanAgentException.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  Implementation of .
  */

#include "PlanAgentException.h"

namespace TA_Base_Core

{

    PlanAgentException::PlanAgentException() throw() :
        m_msg("Unknown error"),
        m_errorCode(TA_Base_Core::PAE_UNKNOWN_ERROR),
        m_itemId(0)
	{
	}

	PlanAgentException::PlanAgentException(const std::string& msg, const TA_Base_Core::EPlanAgentError error, const unsigned long itemId /*=0*/) throw() :
		m_msg(msg),
        m_errorCode(error),
        m_itemId(itemId)
	{

	}

	

	const char* PlanAgentException::what() const throw()

	{

		return( m_msg.c_str() );

	}

	

	PlanAgentException::~PlanAgentException() throw()

	{

	}



} // TA_Base_Core

