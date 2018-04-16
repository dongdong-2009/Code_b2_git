#if defined ( STEPTYPE_VMS_RESTORE_SCHEDULE )

#ifndef ACTIVE_VMS_RESTORE_SCHEDULE_STEP_HEADER_INCLUDED
#define ACTIVE_VMS_RESTORE_SCHEDULE_STEP_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveVmsRestoreScheduleStep.h $
  *  @author:  Mahaveer Pareek
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
	class ActiveVmsRestoreScheduleStep : public ActiveStep
	{
	// Public methods
	public:
		ActiveVmsRestoreScheduleStep( const TA_Base_Core::StepDetail& stepDetail,
            const TA_Base_Core::VmsRestoreScheduleParameters& params, IActivePlanFlow& flowControl );
		
	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveVmsRestoreScheduleStep& operator=( const ActiveVmsRestoreScheduleStep& );
		ActiveVmsRestoreScheduleStep( const ActiveVmsRestoreScheduleStep& );

		const TA_Base_Core::VmsRestoreScheduleParameters m_params;
	}; // ActiveVmsRestoreScheduleStep
}; // TA_Base_App

#endif // ACTIVE_VMS_RESTORE_SCHEDULE_STEP_HEADER_INCLUDED

#endif //  STEPTYPE_VMS_RESTORE_SCHEDULE
