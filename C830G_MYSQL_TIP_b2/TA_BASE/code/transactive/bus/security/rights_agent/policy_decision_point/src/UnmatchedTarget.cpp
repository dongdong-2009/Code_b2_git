/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/UnmatchedTarget.cpp $
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

	PdpConst::EPolicyResult UnmatchedTarget::evaluate( PdpConst::EMatchResult targetResult )
	{
		switch( targetResult )
		{
			case PdpConst::NO_MATCH_MR:
				return( PdpConst::NOT_APPLICABLE_PR );

			case PdpConst::INDETERMINATE_MR:
				return( PdpConst::INDETERMINATE_PR );

			default:
				std::ostringstream msg;
				msg << "targetResult == " << targetResult;
				TA_ASSERT( false, msg.str().c_str() );
				return( PdpConst::UNDEFINED_PR );
				break;
		}
	}
	
} // TA_Base_Bus
