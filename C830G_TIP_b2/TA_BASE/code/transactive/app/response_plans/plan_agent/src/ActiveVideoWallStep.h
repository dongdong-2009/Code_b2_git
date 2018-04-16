#if defined ( STEPTYPE_VIDEOWALL )

#ifndef ACTIVE_VIDEO_WALL_STEP_HEADER_INCLUDED
#define ACTIVE_VIDEO_WALL_STEP_HEADER_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveVideoWallStep.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2008/11/28 16:26:01 $
  *  Last modified by:  $Author: builder $
  *
  *  <description>
  */

#include "app/cctv/video_wall_agent/IDL/src/IVideoWallAgentCorbaDef.h"

namespace TA_Base_App
{
	//Class declaration
	class ActiveVideoWallStep : public ActiveStep
	{
	// Public methods
	public:

		ActiveVideoWallStep( const TA_Base_Core::StepDetail& stepDetail,
						const TA_Base_Core::VideoWallParameters& params,
						IActivePlanFlow& flowControl );
		
	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:

		ActiveVideoWallStep& operator=( const ActiveVideoWallStep& );
		ActiveVideoWallStep( const ActiveVideoWallStep& );

		const TA_Base_Core::VideoWallParameters m_params;
		IActivePlanFlow& m_flowControl;
		
	}; // ActiveVideoWallStep
}; // TA_Base_App

#endif // ACTIVE_VIDEO_WALL_STEP_HEADER_INCLUDED

#endif //  STEPTYPE_VIDEOWALL
