/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/SubjectAttributeSelector.cpp $
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
	SubjectAttributeSelector::SubjectAttributeSelector( const std::string& attribName, const std::string& category /*= ""*/ ) :
		m_attribName(attribName),
		m_category(category)
	{
	}

	ValueSet SubjectAttributeSelector::getValues( const Context& context ) const
	{
		return( context.select( *this ) );
	}

} // TA_Base_Bus
