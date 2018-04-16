#if defined ( STEPTYPE_LAUNCH_GUI_APP )

#ifndef ACTIVELAUCHGUIAPPSTEP_H_INCLUDED
#define ACTIVELAUCHGUIAPPSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveLaunchGuiAppStep.h $
  *  @author:  Mahaveer Pareek
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
	class ActiveLaunchGuiAppStep : public ActiveStep
	{
	// Public methods
	public:
		ActiveLaunchGuiAppStep( const TA_Base_Core::StepDetail& stepDetail,
            const TA_Base_Core::LaunchGuiAppParameters& params, IActivePlanFlow& flowControl );

	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveLaunchGuiAppStep& operator=( const ActiveLaunchGuiAppStep& );
		ActiveLaunchGuiAppStep( const ActiveLaunchGuiAppStep& );

		const TA_Base_Core::LaunchGuiAppParameters m_params;
	}; // ActiveLaunchGuiAppStep
}; // TA_Base_App

#endif // ACTIVELAUCHGUIAPPSTEP_H_INCLUDED


#endif //  STEPTYPE_LAUNCH_GUI_APP
