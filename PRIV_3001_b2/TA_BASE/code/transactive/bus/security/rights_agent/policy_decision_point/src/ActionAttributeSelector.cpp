/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/ActionAttributeSelector.cpp $
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
	ActionAttributeSelector::ActionAttributeSelector( const std::string& attribName ) :
		m_attribName(attribName)
	{
	}

	ValueSet ActionAttributeSelector::getValues( const Context& context ) const
	{
		return( context.select( *this ) );
	}

} // TA_Base_Bus
