/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/Condition.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2012/02/06 16:15:14 $
  *  Last modified by:  $Author: haijun.li $
  *
  *  Implementation of .
  */

#include "PolicyDecisionPoint.h"

// namespace alias for Corba structures/types

namespace TA_Base_Bus
{
	Condition::Condition( const Function* function ) :
		m_function(function)
	{
	}

	PdpConst::EConditionResult Condition::evaluate( const Context& context ) const
	{
		TA_ASSERT( NULL != m_function.get(), "m_function == NULL" );
		return( m_function->evaluate( context ) );
	}

} // TA_Base_Bus
