#if defined ( STEPTYPE_BACKGROUND_PROMPT )

#ifndef ACTIVE_BACKGROUND_PROMPTSTEP_H_INCLUDED
#define ACTIVE_BACKGROUND_PROMPTSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveBackgroundPromptStep.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2014/01/24 18:06:27 $
  *  Last modified by:  $Author: huangjian $
  *
  *  <description>
  */

#include "app/response_plans/plan_agent/src/ActiveStep.h"

namespace TA_Base_App
{
	//Class declaration
	class ActiveBackgroundPromptStep : public ActiveStep
	{
	// Public methods
	public:
		ActiveBackgroundPromptStep( const TA_Base_Core::StepDetail& stepDetail, const TA_Base_Core::BackgroundPromptParameters& params, IActivePlanFlow& flowControl );
		
	protected:
        virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveBackgroundPromptStep& operator=( const ActiveBackgroundPromptStep& );
		ActiveBackgroundPromptStep( const ActiveBackgroundPromptStep& );

        const TA_Base_Core::BackgroundPromptParameters m_params;
        unsigned short m_sendPopupMessageTryTimes;
	}; // ActiveBackgroundPromptStep
}; // TA_Base_App

#endif // ACTIVE_BACKGROUND_PROMPTSTEP_H_INCLUDED

#endif // STEPTYPE_BACKGROUND_PROMPT

