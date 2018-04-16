/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/PlanStepException.cpp $
  *  @author:  San Teo
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2017/12/12 14:18:48 $
  *  Last modified by:  $Author: hoa.le $
  *
  *  Implementation of .
  */

// namespace alias for Corba structures/types
 
#include "PlanStepException.h"

namespace TA_Base_Core
{
	PlanStepException::PlanStepException() throw() :
        TA_Base_Core::TransactiveException("Unknown exception"),
    m_stepType( 0 )
	{
	}

	PlanStepException::PlanStepException(const std::string& msg, const int stepType) throw() :
		TA_Base_Core::TransactiveException(msg),
        m_stepType(stepType)
	{
	}
	
	PlanStepException::~PlanStepException() throw()
	{
	}

} // TA_Base_Core

