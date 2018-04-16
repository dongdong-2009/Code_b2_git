#if defined ( STEPTYPE_SHUTDOWN_SYSTEM )

#ifndef ACTIVESHUTDOWNSYSTEMSTEP_H_INCLUDED
#define ACTIVESHUTDOWNSYSTEMSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveDecisionStep.h $
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
	class ActiveShutdownSystemStep : public ActiveStep
	{
	// Public methods
	public:
        ActiveShutdownSystemStep( const TA_Base_Core::StepDetail& stepDetail, const TA_Base_Core::ShutdownSystemParameters& params, IActivePlanFlow& flowControl );

	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveShutdownSystemStep& operator=( const ActiveShutdownSystemStep& );
		ActiveShutdownSystemStep( const ActiveShutdownSystemStep& );

        std::string getHostname();

		const TA_Base_Core::ShutdownSystemParameters m_params;
	}; // ActiveShutdownSystemStep
}; // TA_Base_App

#endif // ACTIVESHUTDOWNSYSTEMSTEP_H_INCLUDED


#endif //  STEPTYPE_SHUTDOWN_SYSTEM

