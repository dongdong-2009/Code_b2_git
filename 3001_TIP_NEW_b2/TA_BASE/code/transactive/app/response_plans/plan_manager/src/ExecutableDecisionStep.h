//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableDecisionStep.h $
// @author:  Bart Golab
// @version: $Revision: #3 $
//
// Last modification: $DateTime: 2013/01/18 17:55:27 $
// Last modified by:  $Author: huangjian $
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
        ExecutableDecisionStep(const TA_Base_Core::CurrentPlanStepDetail& stepDetail);
        virtual ~ExecutableDecisionStep();

    protected:
        virtual void launch();
    };

} // namespace TA_Base_App

#endif // !defined(AFX_EXECUTABLEDECISIONSTEP_H__C069CC69_C838_462D_8579_BAC17D459DD9__INCLUDED_)
