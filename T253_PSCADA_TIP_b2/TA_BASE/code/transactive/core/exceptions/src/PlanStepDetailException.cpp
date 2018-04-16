/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/core/exceptions/src/PlanStepDetailException.cpp $
  *  @author:  San Teo
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2017/12/12 14:18:48 $
  *  Last modified by:  $Author: hoa.le $
  *
  *  Implementation of .
  */

// namespace alias for Corba structures/types
 
#include "PlanStepDetailException.h"

namespace TA_Base_Core
{
    PlanStepDetailException::PlanStepDetailException() : TA_Base_Core::TransactiveException( "Unknown exception" ),
    m_errorCode( TA_Base_Core::PSDE_UNSPECIFIED ),
    m_type( "Unknown" ),
    m_stepPos( 0 )
	{
	}

	PlanStepDetailException::PlanStepDetailException(const std::string& msg, const std::string& type,
        const TA_Base_Core::EPlanStepDetailError error, const unsigned long stepPos ) : TA_Base_Core::TransactiveException( msg ),
    m_errorCode(error),
    m_type(type),
    m_stepPos(stepPos)
	{
	}
	
    PlanStepDetailException::~PlanStepDetailException() throw()
	{
	}

    const TA_Base_Core::EPlanStepDetailError PlanStepDetailException::getErrorType() const
    {
        return m_errorCode;
    }

    const std::string PlanStepDetailException::getDetailType() const
    {
        return m_type;
    }

    const unsigned long PlanStepDetailException::getStepPosition() const
    {
        return m_stepPos;
    }

} // TA_Base_Core

