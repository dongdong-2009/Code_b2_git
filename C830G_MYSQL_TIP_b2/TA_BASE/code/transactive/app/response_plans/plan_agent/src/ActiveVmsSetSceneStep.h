#if defined ( STEPTYPE_VMS_SET_SCENE )

#ifndef ACTIVE_VMS_SET_SCENE_STEP_HEADER_INCLUDED
#define ACTIVE_VMS_SET_SCENE_STEP_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveVmsSetSceneStep.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */

#include "app/signs/vms_agent/IDL/src/SignControlCorbaDef.h"
#include "app/signs/vms_agent/IDL/src/SceneCorbaDef.h"


namespace TA_Base_App
{
	//Class declaration
	class ActiveVmsSetSceneStep : public ActiveStep
	{
	// Public methods
	public:

		ActiveVmsSetSceneStep( const TA_Base_Core::StepDetail& stepDetail,
						const TA_Base_Core::VmsSetSceneParameters& params,
						IActivePlanFlow& flowControl );
		
	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:

		ActiveVmsSetSceneStep& operator=( const ActiveVmsSetSceneStep& );
		ActiveVmsSetSceneStep( const ActiveVmsSetSceneStep& );

		const TA_Base_Core::VmsSetSceneParameters m_params;
		IActivePlanFlow& m_flowControl;
		
	}; // ActiveVmsSetSceneStep
}; // TA_Base_App

#endif // ACTIVE_VMS_SET_SCENE_STEP_HEADER_INCLUDED

#endif //  STEPTYPE_VMS_SET_SCENE
