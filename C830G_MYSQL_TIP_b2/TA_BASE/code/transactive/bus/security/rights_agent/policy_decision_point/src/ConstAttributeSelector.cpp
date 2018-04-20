/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/ConstAttributeSelector.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  Implementation of .
  */

#include "PolicyDecisionPoint.h"

namespace TA_Base_Bus
{
	ConstAttributeSelector::ConstAttributeSelector( const std::string& attribValue )
	{
		m_valueSet.insert( attribValue );
	}

	ValueSet ConstAttributeSelector::getValues( const Context& context ) const
	{
		return( m_valueSet );
	}

} // TA_Base_Bus
