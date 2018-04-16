#if defined ( STEPTYPE_EVALUATE_EXPRESSION)

#ifndef ACTIVEEVALUATEEXPRESSIONSTEP_H_INCLUDED
#define ACTIVEEVALUATEEXPRESSIONSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveEvaluateExpressionStep.h $
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
	class ActiveEvaluateExpressionStep : public ActiveStep
	{
	// Public methods
	public:
		ActiveEvaluateExpressionStep( const TA_Base_Core::StepDetail& stepDetail,
            const TA_Base_Core::EvaluateExpressionParameters& params, IActivePlanFlow& flowControl );

	protected:
        virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		const TA_Base_Core::EvaluateExpressionParameters m_params;
	}; // ActiveEvaluateExpressionStep
}; // TA_Base_App

#endif // ACTIVEEVALUATEEXPRESSIONSTEP_H_INCLUDED


#endif //  STEPTYPE_EVALUATE_EXPRESSION

