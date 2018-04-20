//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableDecisionStep.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2008/11/28 16:26:01 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_EXECUTABLEDECISIONSTEP_H__C069CC69_C838_462D_8579_BAC17D459DD9__INCLUDED_)
#define AFX_EXECUTABLEDECISIONSTEP_H__C069CC69_C838_462D_8579_BAC17D459DD9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InteractiveStep.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // ExecutableDecisionStep

    class ExecutableDecisionStep : public InteractiveStep
    {
    // Operations
    public:
        ExecutableDecisionStep(const TA_Base_Bus::IPlanManagerCorbaDef::CurrentPlanStepDetail& stepDetail);
        virtual ~ExecutableDecisionStep();

    protected:
        virtual void launch();
    };

} // namespace TA_Base_App

#endif // !defined(AFX_EXECUTABLEDECISIONSTEP_H__C069CC69_C838_462D_8579_BAC17D459DD9__INCLUDED_)
