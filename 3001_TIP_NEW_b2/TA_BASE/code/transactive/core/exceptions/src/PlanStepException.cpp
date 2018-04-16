/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/exceptions/src/PlanStepException.cpp $
  *  @author:  San Teo
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  Implementation of .
  */

// namespace alias for Corba structures/types
 
#include "PlanStepException.h"

namespace TA_Base_Core
{
	PlanStepException::PlanStepException() throw() :
        TA_Base_Core::TransactiveException("Unknown exception"),
        m_stepType(TA_Base_Core::UNDEFINED_STEP)
	{
	}

	PlanStepException::PlanStepException(const std::string& msg, 
                                const TA_Base_Core::EStep stepType) throw() :
		TA_Base_Core::TransactiveException(msg),
        m_stepType(stepType)
	{
	}
	
	PlanStepException::~PlanStepException() throw()
	{
	}

} // TA_Base_Core

