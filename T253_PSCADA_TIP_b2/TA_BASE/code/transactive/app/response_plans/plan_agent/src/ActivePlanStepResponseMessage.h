#ifndef ACTIVEPLANSTEPRESPONSEMESSAGE_HEADER_INCLUDED
#define ACTIVEPLANSTEPRESPONSEMESSAGE_HEADER_INCLUDED
/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/ActivePlanStepResponseMessage.h $
  *  @author:  Bart Golab
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2014/10/24 16:32:43 $
  *  Last modified by:  $Author: huang.wenbo $
  *
  *  Generated when the result of a Run Plan interactive step execution request
  *  is received back from the Plan Manager.
  */

#include <map>
#include <string>

#include "app/response_plans/plan_agent/src/IActivePlanMessage.h"

namespace TA_Base_App
{
    class ActivePlan;
    class IActiveStepMessage;

    class ActivePlanStepResponseMessage : public IActivePlanMessage
    {
    //public methods
    public:
        ActivePlanStepResponseMessage( IActiveStepMessage* pStepMessage );
        virtual ~ActivePlanStepResponseMessage();

        virtual void execute( ActivePlan& activePlan ) const;

    private:
        ActivePlanStepResponseMessage& operator=(const ActivePlanStepResponseMessage&);
        ActivePlanStepResponseMessage( const ActivePlanStepResponseMessage& );

        IActiveStepMessage* m_pStepMessage;
    };

}

#endif //ACTIVEPLANSTEPRESPONSEMESSAGE_HEADER_INCLUDED
