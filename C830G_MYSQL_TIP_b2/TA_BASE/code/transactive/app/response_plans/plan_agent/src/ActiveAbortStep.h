#if defined ( STEPTYPE_ABORT )

#ifndef ACTIVEABORTSTEP_H_INCLUDED
#define ACTIVEABORTSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveAbortStep.h $
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
	class ActiveAbortStep : public ActiveStep
	{
	// Public methods
	public:
			ActiveAbortStep( const TA_Base_Core::StepDetail& stepDetail, 
						 IActivePlanFlow& flowControl );
		
	protected:
			virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:

		ActiveAbortStep& operator=( const ActiveAbortStep& );
		ActiveAbortStep( const ActiveAbortStep& );

		IActivePlanFlow& m_flowControl;

	}; // ActiveAbortStep
}; // TA_Base_App


#endif // ACTIVEABORTSTEP_H_INCLUDED

#endif // STEPTYPE_ABORT
