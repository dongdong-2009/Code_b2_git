//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointTimeCheckStep.h $
// @author:  Andy Parker
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2012/02/06 16:15:14 $
// Last modified by:  $Author: haijun.li $
//
// <description>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_DATA_POINT_TIME_CHECK)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanStep.h"
#include "PlanManagerCommonDefs.h"


namespace TA_Base_App
{
    class DataPointStepParameter;
    class StepNumberStepParameter;
    class TimeSpanStepParameter;


    /////////////////////////////////////////////////////////////////////////////
    // DataPointTimeCheckStep

    class DataPointTimeCheckStep : public PlanStep
    {
        // Operations
    public:
        DataPointTimeCheckStep(PlanNode* parent,const TA_Base_Core::StepDetail &stepDetail);
        DataPointTimeCheckStep(PlanNode* parent,PlanStepId stepId);  // For new plan steps
        virtual ~DataPointTimeCheckStep();

        virtual PlanStep* clone();
        virtual bool isStepBranching() const { return true; }
        virtual bool canIgnoreFailure() const { return true; }

        DataPointStepParameter*         getDataPointParameter() { return m_dataPoint;};
        TimeSpanStepParameter*          getTimeSpanParameter() {return m_timeSpanParameter;};
        StepNumberStepParameter*        getSuccessParameter() { return m_successParameter;};
        StepNumberStepParameter*        getFailParameter() { return m_failureParameter;};

    protected:
        DataPointTimeCheckStep(const DataPointTimeCheckStep &other);
        virtual void createPlanStepParameters(PlanStepParameterList &stepParameters);
        virtual void createPlanStepParameters(const TA_Base_Core::StepParameters &parameterSet,
            PlanStepParameterList &stepParameters);

        virtual void getUpdatedParameters(TA_Base_Core::Step& step);
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        DataPointTimeCheckStep &operator=(const DataPointTimeCheckStep &other);


        // Attributes
    private:
        DataPointStepParameter *m_dataPoint;
        TimeSpanStepParameter* m_timeSpanParameter;
        StepNumberStepParameter* m_successParameter;
        StepNumberStepParameter* m_failureParameter;
    };
}

#endif // defined (STEPTYPE_DATA_POINT_TIME_CHECK)
