#if defined ( STEPTYPE_EXECUTE_SCRIPT )

#ifndef ACTIVEEXECUTESCRIPTSTEP_H_INCLUDED
#define ACTIVEEXECUTESCRIPTSTEP_H_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/3001_TIP/TA_BASE/transactive/app/response_plans/plan_agent/src/ActiveLogStep.h $
  *  @author:  Mahaveer Pareek
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2009/12/29 14:17:48 $
  *  Last modified by:  $Author: grace.koh $
  *
  *  <description>
  */
#include "app/response_plans/plan_agent/src/ActiveStep.h"

namespace TA_Base_App
{
    class ActiveExecuteScriptStep: public ActiveStep
    {
    public:

        ActiveExecuteScriptStep( const TA_Base_Core::StepDetail& stepDetail,
            const TA_Base_Core::ExecuteScriptParameters& params,
            IActivePlanFlow& flowControl );
    protected:
        virtual TA_Base_Core::EActiveStepState executeImpl( void );
    private:
        ActiveExecuteScriptStep& operator=( const ActiveExecuteScriptStep& );
        ActiveExecuteScriptStep( const ActiveExecuteScriptStep& );

        const TA_Base_Core::ExecuteScriptParameters m_params;

        IActivePlanFlow& m_flowControl;
    };
}


#endif // ACTIVEEXECUTESCRIPTSTEP_H_INCLUDED
#endif // STEPTYPE_EXECUTE_SCRIPT