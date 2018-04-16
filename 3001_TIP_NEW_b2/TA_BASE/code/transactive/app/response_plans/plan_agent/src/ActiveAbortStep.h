#if defined ( STEPTYPE_ABORT )

#ifndef ACTIVEABORTSTEP_H_INCLUDED
#define ACTIVEABORTSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveAbortStep.h $
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
	class ActiveAbortStep : public ActiveStep
	{
	// Public methods
	public:
		ActiveAbortStep( const TA_Base_Core::StepDetail& stepDetail, IActivePlanFlow& flowControl );
		
	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveAbortStep& operator=( const ActiveAbortStep& );
		ActiveAbortStep( const ActiveAbortStep& );
	}; // ActiveAbortStep
}; // TA_Base_App


#endif // ACTIVEABORTSTEP_H_INCLUDED

#endif // STEPTYPE_ABORT
