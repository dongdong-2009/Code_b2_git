//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableFailureSummaryStep.h $
// @author:  Lizette Lingo
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2013/01/18 17:55:27 $
// Last modified by:  $Author: huangjian $
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
        ExecutableFailureSummaryStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);
        virtual ~ExecutableFailureSummaryStep();

    protected:
        virtual void launch();
    };

} // namespace TA_Base_App
