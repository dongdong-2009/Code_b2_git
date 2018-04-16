#if defined ( STEPTYPE_PLAN_SET_TIMER )

#ifndef ACTIVEPLANSETTIMERSTEP_H_INCLUDED
#define ACTIVEPLANSETTIMERSTEP_H_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanSetTimerStep.h $
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
    class ActivePlanSetTimerStep: public ActiveStep
    {
    public:
        ActivePlanSetTimerStep( const TA_Base_Core::StepDetail& stepDetail,
            const TA_Base_Core::PlanSetTimerParameters& params, IActivePlanFlow& flowControl );

    protected:
        virtual TA_Base_Core::EActiveStepState executeImpl( void );

    private:
        ActivePlanSetTimerStep& operator=( const ActivePlanSetTimerStep& );
        ActivePlanSetTimerStep( const ActivePlanSetTimerStep& );

    private:
        const TA_Base_Core::PlanSetTimerParameters m_params;
    };
}


#endif // ActivePlanSetTimerStep_H_INCLUDED
#endif // ACTIVEPLANSETTIMERSTEP_H_INCLUDED