//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/DataPointCheckStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#ifndef AFX_DATAPOINTCHECKSTEP_H__INCLUDED_
#define AFX_DATAPOINTCHECKSTEP_H__INCLUDED_

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/data_point_check/common/DataPointCheckStepParameter.h"
#include "app/response_plans/plan_step_plugin/data_point_check/editor/src/DataPointCheckStepEditorParameter.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // DataPointCheckStep

    class DataPointCheckStep : public PlanStep
    {
    // Operations
    public:
        DataPointCheckStep(IPlanNode* parent,const BasicParameter &stepDetail);
        DataPointCheckStep(IPlanNode* parent,const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~DataPointCheckStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const;
        virtual bool canIgnoreFailure() const;
		virtual std::string getStepTypeName() const;
        virtual void outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing );
        virtual bool retrieveOutgoings( std::vector<unsigned int>& vecOutgoings );
        virtual void clearFlowParameters();
        virtual bool allowMultipleIncoming();
        virtual bool allowMultipleOutgoing();

		virtual IActivityParameter& getActivityParameter();

    protected:
        DataPointCheckStep(const DataPointCheckStep &other);
        virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        DataPointCheckStep &operator=(const DataPointCheckStep &other);

    // Attributes
    private:
		DataPointCheckStepParameter m_params;
		DataPointCheckStepEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_DATAPOINTCHECKSTEP_H__INCLUDED_)
