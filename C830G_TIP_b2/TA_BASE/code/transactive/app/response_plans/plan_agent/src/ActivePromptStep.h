#if defined ( STEPTYPE_PROMPT )

#ifndef ACTIVEPROMPTSTEP_H_INCLUDED
#define ACTIVEPROMPTSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePromptStep.h $
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
	class ActivePromptStep : public ActiveStep
	{
	// Public methods
	public:

			ActivePromptStep( const TA_Base_Core::StepDetail& stepDetail,
								const TA_Base_Core::PromptParameters& params,
							IActivePlanFlow& flowControl );
		
	protected:
			virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:

		ActivePromptStep& operator=( const ActivePromptStep& );
		ActivePromptStep( const ActivePromptStep& );

			const TA_Base_Core::PromptParameters m_params;
		IActivePlanFlow& m_flowControl;
		
		TA_Base_Core::NamedObject<TA_Base_Bus::IPlanManagerCorbaDef,
							 TA_Base_Bus::IPlanManagerCorbaDef_ptr,
							 TA_Base_Bus::IPlanManagerCorbaDef_var> m_planManager;

	}; // ActivePromptStep
}; // TA_Base_App

#endif // ACTIVEPROMPTSTEP_H_INCLUDED

#endif // STEPTYPE_PROMPT

