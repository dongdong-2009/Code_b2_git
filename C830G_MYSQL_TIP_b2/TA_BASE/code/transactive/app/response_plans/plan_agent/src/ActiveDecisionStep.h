#if defined ( STEPTYPE_DECISION )

#ifndef ACTIVEDECISIONSTEP_H_INCLUDED
#define ACTIVEDECISIONSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveDecisionStep.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */

namespace TA_Base_App
{
	//Class declaration
	class ActiveDecisionStep : public ActiveStep
	{
	// Public methods
	public:

			ActiveDecisionStep( const TA_Base_Core::StepDetail& stepDetail,
								const TA_Base_Core::DecisionParameters& params,
							IActivePlanFlow& flowControl );

            TA_Base_Core::StepNumber getNextDecisionStep(TA_Base_Core::EPlanStepCompletionStatus status);
		
	protected:
			virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:

		ActiveDecisionStep& operator=( const ActiveDecisionStep& );
		ActiveDecisionStep( const ActiveDecisionStep& );

			const TA_Base_Core::DecisionParameters m_params;
		IActivePlanFlow& m_flowControl;
		

	}; // ActiveDecisionStep
}; // TA_Base_App

#endif // ACTIVEDECISIONSTEP_H_INCLUDED


#endif //  STEPTYPE_DECISION

