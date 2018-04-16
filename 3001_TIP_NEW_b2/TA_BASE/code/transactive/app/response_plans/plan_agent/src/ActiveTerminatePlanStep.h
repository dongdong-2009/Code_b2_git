#if defined ( STEPTYPE_TERMINATE_PLAN )

#ifndef ACTIVETERMINATEPLANSTEP_H_INCLUDED
#define ACTIVETERMINATEPLANSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveTerminatePlanStep.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  <description>
  */

#include "app/response_plans/plan_agent/src/ActiveStep.h"

namespace TA_Base_App
{
	//Class declaration
	class ActiveTerminatePlanStep : public ActiveStep
	{
	// Public methods
	public:
		ActiveTerminatePlanStep( const TA_Base_Core::StepDetail& stepDetail,
            const TA_Base_Core::TerminatePlanParameters& params, IActivePlanFlow& flowControl );
		
	protected:
			virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveTerminatePlanStep& operator=( const ActiveTerminatePlanStep& );
		ActiveTerminatePlanStep( const ActiveTerminatePlanStep& );

		const TA_Base_Core::TerminatePlanParameters m_params;
	}; // ActiveTerminatePlanStep
}; // TA_Base_App

#endif // ACTIVETERMINATEPLANSTEP_H_INCLUDED

#endif //  STEPTYPE_TERMINATE_PLAN
