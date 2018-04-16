/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/PlanStepParameter.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2014/06/03 09:20:18 $
  * Last modified by:  $Author: huangjian $
  *
  * <description>
  *
  **/

#ifndef IPLANSTEPPARAMETER_H
#define IPLANSTEPPARAMETER_H

#include <string>

namespace TA_Base_App
{

  /////////////////////////////////////////////////////////////////////////////
    // IPlanStepParameter
    class IPlanStepParameter
    {
    // Operations
    public:
        virtual ~IPlanStepParameter() {};
        virtual const std::string& name() const = 0;
        virtual const std::string displayValue() const = 0;
    };
}

#endif // !defined(IPLANSTEPPARAMETER_H)
