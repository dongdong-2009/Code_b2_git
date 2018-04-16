/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/PRIV_3001/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/EvaluateDutyPolicy.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2018/03/14 13:51:20 $
 * Last modified by:  $Author: lim.cy $
 *
 * Applies the Duty Policy for each of the DutyPrimitives in the range itBegin to itEnd.
 * 
 */

#if !defined(EVALUATEDUTYPOLICY_H_INCLUDED)
#define EVALUATEDUTYPOLICY_H_INCLUDED

namespace TA_Base_Bus
{
	class EvaluateDutyPolicy
	{

	public:
		static IDutyPolicy::POLICY_RESULT evaluate( const IDutyPolicy& dutyPolicy,
			IDutyPolicy::POLICY_RESULT (IDutyPolicy::*policy)(const DutyContext&) const, DutyContext& context,
			const DASType::DutySet& srcDuty, DutyPrimitive& replace, DASType::DutySet& noAdd );
	};
}

#endif // !defined(EVALUATEDUTYPOLICY_H_INCLUDED)
