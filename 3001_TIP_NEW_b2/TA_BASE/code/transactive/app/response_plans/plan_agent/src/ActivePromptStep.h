#if defined ( STEPTYPE_PROMPT )

#ifndef ACTIVEPROMPTSTEP_H_INCLUDED
#define ACTIVEPROMPTSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePromptStep.h $
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
	class ActivePromptStep : public ActiveStep
	{
	// Public methods
	public:
		ActivePromptStep( const TA_Base_Core::StepDetail& stepDetail, const TA_Base_Core::PromptParameters& params, IActivePlanFlow& flowControl );
		
	protected:
        virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActivePromptStep& operator=( const ActivePromptStep& );
		ActivePromptStep( const ActivePromptStep& );

        const TA_Base_Core::PromptParameters m_params;
	}; // ActivePromptStep
}; // TA_Base_App

#endif // ACTIVEPROMPTSTEP_H_INCLUDED

#endif // STEPTYPE_PROMPT

