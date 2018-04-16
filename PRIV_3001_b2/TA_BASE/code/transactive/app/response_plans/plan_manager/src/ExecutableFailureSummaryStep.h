//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableFailureSummaryStep.h $
// @author:  Lizette Lingo
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2018/03/14 13:51:20 $
// Last modified by:  $Author: lim.cy $
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
