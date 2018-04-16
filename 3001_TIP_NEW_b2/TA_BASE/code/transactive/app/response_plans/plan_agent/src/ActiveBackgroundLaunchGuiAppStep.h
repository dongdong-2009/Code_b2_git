#if defined ( STEPTYPE_BACKGROUND_LAUNCH_GUI_APP )

#ifndef ACTIVEBACKGROUNDLAUNCHGUIAPPSTEP_H_INCLUDED
#define ACTIVEBACKGROUNDLAUNCHGUIAPPSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveBackgroundLaunchGuiAppStep.h $
  *  @author:  Bart Golab
  *  @version: $Revision: #4 $
  *
  *  Last modification: $DateTime: 2014/01/24 21:40:40 $
  *  Last modified by:  $Author: huangjian $
  *
  *  Represents a Launch Schematic step in an executing plan.
  */

#include "app/response_plans/plan_agent/src/ActiveStep.h"

namespace TA_Base_App
{
	// Class declaration
	class ActiveBackgroundLaunchGuiAppStep : public ActiveStep
	{
	// Public methods
	public:
		ActiveBackgroundLaunchGuiAppStep( const TA_Base_Core::StepDetail& stepDetail, 
            const TA_Base_Core::BackgroundLaunchGuiAppParameters& params, IActivePlanFlow& flowControl );
		
	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveBackgroundLaunchGuiAppStep& operator=( const ActiveBackgroundLaunchGuiAppStep& );
		ActiveBackgroundLaunchGuiAppStep( const ActiveBackgroundLaunchGuiAppStep& );

        const TA_Base_Core::BackgroundLaunchGuiAppParameters m_params;
        unsigned short m_sendPopupMessageTryTimes;
	}; // ActiveBackgroundLaunchGuiAppStep

}; // TA_Base_App

#endif // ACTIVEBACKGROUNDLAUNCHGUIAPPSTEP_H_INCLUDED

#endif //  STEPTYPE_BACKGROUND_LAUNCH_GUI_APP
