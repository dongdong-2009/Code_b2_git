#if defined ( STEPTYPE_LAUNCH_SCHEMATIC )

#ifndef ACTIVELAUNCHSCHEMATICSTEP_H_INCLUDED
#define ACTIVELAUNCHSCHEMATICSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveLaunchSchematicStep.h $
  *  @author:  Bart Golab
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2012/12/27 15:59:31 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Represents a Launch Schematic step in an executing plan.
  */

#include "app/response_plans/plan_agent/src/ActiveStep.h"

namespace TA_Base_App
{
	// Class declaration
	class ActiveLaunchSchematicStep : public ActiveStep
	{
	// Public methods
	public:
		ActiveLaunchSchematicStep( const TA_Base_Core::StepDetail& stepDetail, 
            const TA_Base_Core::LaunchSchematicParameters& params, IActivePlanFlow& flowControl );
		
	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveLaunchSchematicStep& operator=( const ActiveLaunchSchematicStep& );
		ActiveLaunchSchematicStep( const ActiveLaunchSchematicStep& );

		const TA_Base_Core::LaunchSchematicParameters m_params;		
	}; // ActiveLaunchSchematicStep

}; // TA_Base_App

#endif // ACTIVELAUNCHSCHEMATICSTEP_H_INCLUDED

#endif //  STEPTYPE_LAUNCH_SCHEMATIC
