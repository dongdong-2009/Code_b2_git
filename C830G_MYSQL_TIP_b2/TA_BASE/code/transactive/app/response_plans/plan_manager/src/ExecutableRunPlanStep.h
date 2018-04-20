//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableRunPlanStep.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2008/11/28 16:26:01 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_EXECUTABLERUNPLANSTEP_H__A83A1B95_8238_4E25_BF29_C954D9C787DD__INCLUDED_)
#define AFX_EXECUTABLERUNPLANSTEP_H__A83A1B95_8238_4E25_BF29_C954D9C787DD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InteractiveStep.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // ExecutableRunPlanStep

    class ExecutableRunPlanStep : public InteractiveStep
    {
    // Operations
    public:
        ExecutableRunPlanStep(const TA_Base_Bus::IPlanManagerCorbaDef::CurrentPlanStepDetail& stepDetail);
        virtual ~ExecutableRunPlanStep();

    protected:
        virtual void launch();
    };

} // namespace TA_Base_App

#endif // !defined(AFX_EXECUTABLERUNPLANSTEP_H__A83A1B95_8238_4E25_BF29_C954D9C787DD__INCLUDED_)
