//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableRunPlanStep.h $
// @author:  Bart Golab
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2013/01/18 17:55:27 $
// Last modified by:  $Author: huangjian $
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
        ExecutableRunPlanStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);
        virtual ~ExecutableRunPlanStep();

    protected:
        virtual void launch();
    };

} // namespace TA_Base_App

#endif // !defined(AFX_EXECUTABLERUNPLANSTEP_H__A83A1B95_8238_4E25_BF29_C954D9C787DD__INCLUDED_)
