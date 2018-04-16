#if defined ( STEPTYPE_ACTIVE_PLAN_CHECK )

#ifndef ACTIVEACTIVEPLANCHECKSTEP_H_INCLUDED
#define ACTIVEACTIVEPLANCHECKSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveActivePlanCheckStep.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2018/03/14 13:51:20 $
  *  Last modified by:  $Author: lim.cy $
  *
  *  <description>
  */

#include "app/response_plans/plan_agent/src/ActiveStep.h"

namespace TA_Base_App
{
	//Class declaration
	class ActiveActivePlanCheckStep : public ActiveStep
	{
	// Public methods
	public:
		ActiveActivePlanCheckStep( const TA_Base_Core::StepDetail& stepDetail,
            const TA_Base_Core::ActivePlanCheckParameters& params, IActivePlanFlow& flowControl );
		
	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveActivePlanCheckStep& operator=( const ActiveActivePlanCheckStep& );
		ActiveActivePlanCheckStep( const ActiveActivePlanCheckStep& );

        const TA_Base_Core::ActivePlanCheckParameters m_params;
	}; // ActiveActivePlanCheckStep
}; // TA_Base_App

#endif // ACTIVEACTIVEPLANCHECKSTEP_H_INCLUDED

#endif //  STEPTYPE_ACTIVE_PLAN_CHECK
