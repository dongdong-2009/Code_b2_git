//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/TrainPAStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_TRAINPASTEP_H__EF80FEF7_9A8E_423E_8A7F_FC84A2A18F1B__INCLUDED_)
#define AFX_TRAINPASTEP_H__EF80FEF7_9A8E_423E_8A7F_FC84A2A18F1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/pa/train_pa/common/TrainPaStepParameter.h"
#include "app/response_plans/plan_step_plugin/pa/train_pa/editor/src/TrainPaStepEditorParameter.h"

namespace TA_Base_App
{
    class TrainStepParameter;
    class PaMessageStepParameter;
    class TimeSpanStepParameter;
    class YesNoStepParameter;
    /////////////////////////////////////////////////////////////////////////////
    // TrainPAStep

    class TrainPAStep : public PlanStep
    {
        // Operations
    public:
        TrainPAStep(IPlanNode* parent, BasicParameter&stepDetail);
        TrainPAStep(IPlanNode* parent,const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~TrainPAStep();
        PaMessageStepParameter* getPAMessageParameter()        { return m_messageParameter; }
        TrainStepParameter*        getTargetTrainParameter()    { return m_trainParameter;   }
        YesNoStepParameter*        getCyclicParameter()            { return m_cyclicParameter;  }
        TimeSpanStepParameter*  getDurationParameter()        { return m_durationParameter;}

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const { return false; }
        virtual bool canIgnoreFailure() const { return true; }
		virtual std::string getStepTypeName() const;
		virtual IActivityParameter& getActivityParameter();

    protected:
        TrainPAStep(const TrainPAStep &other);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
		virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        TrainPAStep &operator=(const TrainPAStep &other);


        // Attributes
    private:
        TrainStepParameter*       m_trainParameter;
        PaMessageStepParameter*      m_messageParameter;
        YesNoStepParameter*          m_cyclicParameter;
        TimeSpanStepParameter*    m_durationParameter;

		TrainPaStepParameter m_params;
		TrainPaStepEditorParameter m_parActivityParameter;
    };
}
#endif // !defined(AFX_TRAINPASTEP_H__EF80FEF7_9A8E_423E_8A7F_FC84A2A18F1B__INCLUDED_)
