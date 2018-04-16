/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/4669_T01271350/TA_BASE/transactive/bus/security/duty_agent/duty_agent_state/src/DutyPolicyCombAlg.h $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/11/28 16:26:01 $
 * Last modified by:  $Author: builder $
 *
 * Knows how to sum results from multiple policy queries resulting from multiple source
 * sessions being present for a givent duty change/query.
 */

#if !defined(DUTYPOLICYCOMBALG_H_INCLUDED)
#define DUTYPOLICYCOMBALG_H_INCLUDED

namespace TA_Base_Bus
{
	class DutyPolicyCombAlg
	{

	public:
		DutyPolicyCombAlg();

		void setResult( IDutyPolicy::POLICY_RESULT result);
		
		// allow for loop optimisation
		bool isResultKnown() const;

		IDutyPolicy::POLICY_RESULT getCombinedResult() const;
		IDutyPolicy::POLICY_RESULT getLastResult() const;

	private:
		IDutyPolicy::POLICY_RESULT m_combinedResult;
		IDutyPolicy::POLICY_RESULT m_lastResult;
		bool m_resultKnown;
	};
}







#endif // !defined(DUTYPOLICYCOMBALG_H_INCLUDED)
