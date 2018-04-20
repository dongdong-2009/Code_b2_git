#if defined ( STEPTYPE_TERMINATE_PLAN )

#ifndef ACTIVETERMINATEPLANSTEP_H_INCLUDED
#define ACTIVETERMINATEPLANSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveTerminatePlanStep.h $
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
	class ActiveTerminatePlanStep : public ActiveStep
	{
	// Public methods
	public:

			ActiveTerminatePlanStep( const TA_Base_Core::StepDetail& stepDetail,
									 const TA_Base_Core::TerminatePlanParameters& params,
								 IActivePlanFlow& flowControl );
		
	protected:
			virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:

		ActiveTerminatePlanStep& operator=( const ActiveTerminatePlanStep& );
		ActiveTerminatePlanStep( const ActiveTerminatePlanStep& );

			const TA_Base_Core::TerminatePlanParameters m_params;
		IActivePlanFlow& m_flowControl;

	}; // ActiveTerminatePlanStep
}; // TA_Base_App

#endif // ACTIVETERMINATEPLANSTEP_H_INCLUDED

#endif //  STEPTYPE_TERMINATE_PLAN
