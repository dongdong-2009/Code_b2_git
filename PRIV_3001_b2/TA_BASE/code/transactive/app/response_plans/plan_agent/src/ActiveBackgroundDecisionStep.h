#if defined ( STEPTYPE_BACKGROUND_DECISION )

#ifndef ACTIVE_BACKGROUND_DECISIONSTEP_H_INCLUDED
#define ACTIVE_BACKGROUND_DECISIONSTEP_H_INCLUDED

/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveBackgroundDecisionStep.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #3 $
  *
  *  Last modification: $DateTime: 2014/01/24 18:06:27 $
  *  Last modified by:  $Author: huangjian $
  *
  *  <description>
  */

#include "app/response_plans/plan_agent/src/ActiveStep.h"
#include "app/response_plans/plan_agent/src/IDecisionStep.h"

namespace TA_Base_App
{
	//Class declaration
	class ActiveBackgroundDecisionStep : public ActiveStep, public IDecisionStep
	{
	// Public methods
	public:
		ActiveBackgroundDecisionStep( const TA_Base_Core::StepDetail& stepDetail, const TA_Base_Core::BackgroundDecisionParameters& params, IActivePlanFlow& flowControl );

        virtual TA_Base_Core::StepNumber getNextDecisionStep(const int status);

	protected:
		virtual TA_Base_Core::EActiveStepState executeImpl( void );

	private:
		ActiveBackgroundDecisionStep& operator=( const ActiveBackgroundDecisionStep& );
		ActiveBackgroundDecisionStep( const ActiveBackgroundDecisionStep& );

        const TA_Base_Core::BackgroundDecisionParameters m_params;

        unsigned short m_sendPopupMessageTryTimes;
	}; // ActiveBackgroundDecisionStep
}; // TA_Base_App

#endif // ACTIVEDECISIONSTEP_H_INCLUDED


#endif //  ACTIVE_BACKGROUND_DECISIONSTEP_H_INCLUDED

