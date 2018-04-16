/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/SubjectAttributeSelector.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
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
