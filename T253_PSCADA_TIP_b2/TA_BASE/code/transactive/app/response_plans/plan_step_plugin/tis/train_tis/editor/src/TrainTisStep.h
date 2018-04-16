/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TrainTisStep.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Represents a Train Traveller Info System step.
  *
  **/

#if !defined(AFX_TRAINTISSTEP_H__B56D2E47_86BD_4B4E_8455_8BB3DA730B57__INCLUDED_)
#define AFX_TRAINTISSTEP_H__B56D2E47_86BD_4B4E_8455_8BB3DA730B57__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/tis/train_tis/common/TrainTisStepParameter.h"
#include "app/response_plans/plan_step_plugin/tis/train_tis/editor/src/TrainTisStepEditorParameter.h"


namespace TA_Base_App
{
    class TrainStepParameter;
    class TisMessageStepParameter;
    class TisPriorityStepParameter;
    class TimeSpanStepParameter;

    /////////////////////////////////////////////////////////////////////////////
    // TrainTisStep

    class TrainTisStep : public PlanStep
    {
    // Operations
    public:
        TrainTisStep(IPlanNode* parent,const BasicParameter& stepDetail);
        TrainTisStep(IPlanNode* parent, const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~TrainTisStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const  { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual std::string getStepTypeName() const;

        TrainStepParameter* getTrainParameter()          { return m_trainParameter; }
        TisMessageStepParameter* getMessageParameter()   { return m_messageParameter; }
        TisPriorityStepParameter* getPriorityParameter() { return m_priorityParameter; }
        TimeSpanStepParameter* getDurationParameter()    { return m_durationParameter; }

		virtual IActivityParameter& getActivityParameter();
    protected:
        TrainTisStep(const TrainTisStep& other);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
		virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        TrainTisStep& operator=(const TrainTisStep& other);


    // Attributes
    private:
        TrainStepParameter*       m_trainParameter;
        TisMessageStepParameter*  m_messageParameter;
        TisPriorityStepParameter* m_priorityParameter;
        TimeSpanStepParameter*    m_durationParameter;

		TrainTisStepParameter m_params;
		TrainTisStepEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_TRAINTISSTEP_H__B56D2E47_86BD_4B4E_8455_8BB3DA730B57__INCLUDED_)
