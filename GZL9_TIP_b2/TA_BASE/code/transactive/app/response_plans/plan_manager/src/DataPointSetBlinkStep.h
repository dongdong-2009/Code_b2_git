//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointSetBlinkStep.h $
// @author:  Liu Yong
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2016/11/6 13:35:44 $
// Last modified by:  $Author: builder $
//
// This step is used for set PSCADA control datapoint blink control status in its holding plan. 

#if !defined(AFX_DATAPOINT_SET_BLINK_STEP_H__C032447B_A828_4C1B_9037_F502C1C49DD1__INCLUDED_)
#define AFX_DATAPOINT_SET_BLINK_STEP_H__C032447B_A828_4C1B_9037_F502C1C49DD1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"
//#include "SOCPlanStepListCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // DataPointSetBlinkStep

    class DataPointSetBlinkStep : public PlanStep
    {
    // Operations
    public:
        DataPointSetBlinkStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        DataPointSetBlinkStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
       virtual ~DataPointSetBlinkStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }

		//virtual void addToStepList(CSOCPlanStepListCtrl& listCtrl);		//SOC

    protected:
        DataPointSetBlinkStep(const DataPointSetBlinkStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
                                              PlanStepParameterList &stepParameters);
        virtual void getUpdatedParameters(TA_Base_Core::Step& step);

        virtual bool canDeleteStep();

    private:
        DataPointSetBlinkStep &operator=(const DataPointSetBlinkStep &other);


    // Attributes
    private:

    };
}

#endif // !defined(AFX_DATAPOINT_SET_BLINK_STEP_H__C032447B_A828_4C1B_9037_F502C1C49DD1__INCLUDED_)
