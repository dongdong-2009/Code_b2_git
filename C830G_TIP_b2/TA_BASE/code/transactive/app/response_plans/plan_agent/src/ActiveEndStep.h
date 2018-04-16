#ifndef ACTIVE_END_STEP_H_INCLUDED
#define ACTIVE_END_STEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveEndStep.h $
  *  @author:  Mahaveer Pareek
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
	class ActiveEndStep : public ActiveStep
	{
	// Public methods
	public:
			ActiveEndStep( const TA_Base_Core::StepDetail& stepDetail, 
						 IActivePlanFlow& flowControl );
		
	protected:
			virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:

		ActiveEndStep& operator=( const ActiveEndStep& );
		ActiveEndStep( const ActiveEndStep& );

		IActivePlanFlow& m_flowControl;

	}; // ActiveEndStep
}; // TA_Base_App

#endif // ACTIVE_END_STEP_H_INCLUDED
