/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/PlanAgentException.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2017/10/31 16:21:54 $
  *  Last modified by:  $Author: CM $
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

