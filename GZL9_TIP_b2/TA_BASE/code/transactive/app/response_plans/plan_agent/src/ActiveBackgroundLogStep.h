#if defined ( STEPTYPE_BACKGROUND_LOG )


#ifndef ACTIVE_BACKGROUNDLOG_STEP_HEADER_INCLUDED
#define ACTIVE_BACKGROUNDLOG_STEP_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/app/response_plans/plan_agent/src/ActiveBackgroundLogStep.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2016/01/22 14:37:26 $
  *  Last modified by:  $Author: CM $
  *
  *  <description>
  */

#include "app/response_plans/plan_agent/src/ActiveStep.h"

namespace TA_Base_App
{

	class ActiveBackgroundLogStep : public ActiveStep
	{
	public:
		ActiveBackgroundLogStep( const TA_Base_Core::StepDetail& stepDetail,
            const TA_Base_Core::BackgroundLogParameters& params, IActivePlanFlow& flowControl );

	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveBackgroundLogStep& operator=( const ActiveBackgroundLogStep& );
		ActiveBackgroundLogStep( const ActiveBackgroundLogStep& );

        const TA_Base_Core::BackgroundLogParameters m_params;
        unsigned short m_sendPopupMessageTryTimes;
	};

}
#endif //ACTIVE_BACKGROUNDLOG_STEP_HEADER_INCLUDED


#endif //  STEPTYPE_BACKGROUND_LOG
