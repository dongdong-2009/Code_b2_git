/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/exceptions/src/PlanStepDetailException.cpp $
  *  @author:  San Teo
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

// namespace alias for Corba structures/types
 
#include "PlanStepDetailException.h"

namespace TA_Base_Core
{
	PlanStepDetailException::PlanStepDetailException() throw() :
        m_errorCode(TA_Base_Core::PSDE_UNSPECIFIED),
        m_type(TA_Base_Core::PSDT_NAME),
        m_stepNum(0),
        TA_Base_Core::TransactiveException("Unknown exception")
	{
	}

	PlanStepDetailException::PlanStepDetailException(const std::string& msg, 
                                                    const TA_Base_Core::EPlanStepDetailType type,
                                                    const TA_Base_Core::EPlanStepDetailError error, 
                                                    const unsigned long stepId) throw() :
        m_errorCode(error),
        m_type(type),
        m_stepNum(stepId),
		TA_Base_Core::TransactiveException(msg)
	{
	}
	
	PlanStepDetailException::~PlanStepDetailException() throw()
	{
	}

} // TA_Base_Core

