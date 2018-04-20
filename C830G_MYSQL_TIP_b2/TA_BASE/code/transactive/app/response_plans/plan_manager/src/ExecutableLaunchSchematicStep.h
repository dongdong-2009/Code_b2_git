/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/ExecutableLaunchSchematicStep.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Facilitates the launching of a display when executing a Launch Schematic step.
  *
  */

#if !defined(AFX_EXECUTABLELAUNCHSCHEMATICSTEP_H__1958B41C_198B_4BEC_97DF_BBDC94A86BC6__INCLUDED_)
#define AFX_EXECUTABLELAUNCHSCHEMATICSTEP_H__1958B41C_198B_4BEC_97DF_BBDC94A86BC6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InteractiveStep.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // ExecutableLaunchSchematicStep

    class ExecutableLaunchSchematicStep : public InteractiveStep
    {
    // Operations
    public:
        ExecutableLaunchSchematicStep(const TA_Base_Bus::IPlanManagerCorbaDef::CurrentPlanStepDetail& stepDetail);
        virtual ~ExecutableLaunchSchematicStep();

    protected:
        virtual void launch();
    };

} // namespace TA_Base_App

#endif // !defined(AFX_EXECUTABLELAUNCHSCHEMATICSTEP_H__1958B41C_198B_4BEC_97DF_BBDC94A86BC6__INCLUDED_)
