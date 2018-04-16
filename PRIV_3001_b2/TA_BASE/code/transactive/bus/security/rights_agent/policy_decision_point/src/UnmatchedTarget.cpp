/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/rights_agent/policy_decision_point/src/UnmatchedTarget.cpp $
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
