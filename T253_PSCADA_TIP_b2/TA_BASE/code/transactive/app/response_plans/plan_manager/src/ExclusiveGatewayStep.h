//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/ActivePlanCheckStep.h $
// @author:  Andy Parker
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2012/02/06 16:15:14 $
// Last modified by:  $Author: haijun.li $
//
// <description>

#if !defined(AFX_EXCLUSIVEGATEWAYSTEP_H__F0B425B6_5C8B_4A24_B5A7_7B08792FB6C6__INCLUDED_)
#define AFX_EXCLUSIVEGATEWAYSTEP_H__F0B425B6_5C8B_4A24_B5A7_7B08792FB6C6__INCLUDED_

#include <set>

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/internal/src/ConditionalGatewayParameter.h"
#include "app/response_plans/plan_manager/src/ExclusiveGatewayEditorParameter.h"

class IActivityParameter;

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // ExclusiveGatewayStep

    class ExclusiveGatewayStep : public PlanStep
    {
    // Operations
    public:
        ExclusiveGatewayStep( IPlanNode* parent, const BasicParameter &stepDetail );
        ExclusiveGatewayStep( IPlanNode* parent, const unsigned long stepId, const int stepType );  // For new plan steps
        virtual ~ExclusiveGatewayStep();

        virtual PlanStep* clone();
        virtual bool hasCustomOutgoingStep() const;
        virtual bool canIgnoreFailure() const;
		virtual std::string getStepTypeName() const;
        virtual void outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing );
        virtual bool retrieveOutgoings( std::vector<unsigned int>& vecOutgoings );
        virtual void resetFlowParameters();
        virtual void clearFlowParameters();
        virtual void updateFlowParameters();
        virtual bool allowMultipleIncoming();
        virtual bool allowMultipleOutgoing();

        virtual IActivityParameter& getActivityParameter();

    protected:
        ExclusiveGatewayStep(const ExclusiveGatewayStep &other);

        virtual void createPlanStepParameters();
        virtual bool isParametersValid( const std::vector<std::string>& parameters );
        virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        size_t getShortestPathNum( const std::map<unsigned int, std::set<unsigned int> >& mapPaths, const unsigned int unStart, const unsigned int unEnd );

    private:
        ExclusiveGatewayStep &operator=(const ExclusiveGatewayStep &other);

    // Attributes
    private:
		ConditionalGatewayParameter m_params;
        ExclusiveGatewayEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_EXCLUSIVEGATEWAYSTEP_H__F0B425B6_5C8B_4A24_B5A7_7B08792FB6C6__INCLUDED_)
