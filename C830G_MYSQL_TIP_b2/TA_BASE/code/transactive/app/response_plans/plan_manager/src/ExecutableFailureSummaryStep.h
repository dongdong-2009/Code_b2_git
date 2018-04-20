//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableFailureSummaryStep.h $
// @author:  Lizette Lingo
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2008/11/28 16:26:01 $
// Last modified by:  $Author: builder $
//
// <description>
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InteractiveStep.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // ExecutableFailureSummaryStep

    class ExecutableFailureSummaryStep : public InteractiveStep
    {
    // Operations
    public:
        ExecutableFailureSummaryStep(const TA_Base_Bus::IPlanManagerCorbaDef::CurrentPlanStepDetail& stepDetail);
        virtual ~ExecutableFailureSummaryStep();

    protected:
        virtual void launch();
    };

} // namespace TA_Base_App
