#if defined ( STEPTYPE_JUMP )

#ifndef ACTIVEJUMPSTEP_H_INCLUDED
#define ACTIVEJUMPSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveJumpStep.h $
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
	class ActiveJumpStep : public ActiveStep
	{
	// Public methods
	public:

			ActiveJumpStep( const TA_Base_Core::StepDetail& stepDetail,
				            const TA_Base_Core::JumpParameters& params,
						IActivePlanFlow& flowControl );
		
	protected:
			virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		
		ActiveJumpStep& operator=( const ActiveJumpStep& );
		ActiveJumpStep( const ActiveJumpStep& );

			const TA_Base_Core::JumpParameters m_params;
		IActivePlanFlow& m_flowControl;

	}; // ActiveJumpStep
}; // TA_Base_App

#endif // ACTIVEJUMPSTEP_H_INCLUDED

#endif // STEPTYPE_JUMP 
