//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutablePromptStep.h $
// @author:  Bart Golab
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2013/01/18 17:55:27 $
// Last modified by:  $Author: huangjian $
//
// <description>

#if !defined(AFX_EXECUTABLEPROMPTSTEP_H__20522C28_336C_4624_9E69_0BA656E5A139__INCLUDED_)
#define AFX_EXECUTABLEPROMPTSTEP_H__20522C28_336C_4624_9E69_0BA656E5A139__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InteractiveStep.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // ExecutablePromptStep

    class ExecutablePromptStep : public InteractiveStep
    {
    // Operations
    public:
        ExecutablePromptStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);
        virtual ~ExecutablePromptStep();

    protected:
        virtual void launch();
    };

} // namespace TA_Base_App

#endif // !defined(AFX_EXECUTABLEPROMPTSTEP_H__20522C28_336C_4624_9E69_0BA656E5A139__INCLUDED_)
