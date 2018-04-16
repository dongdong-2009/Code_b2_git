//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/JumpStep.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_PARALLELGATEWAYSTEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
#define AFX_PARALLELGATEWAYSTEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStep.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_step_plugin/internal/src/ParallelGatewayParameter.h"
#include "app/response_plans/plan_manager/src/ParallelGatewayStepEditorParameter.h"

namespace TA_Base_App
{
    typedef std::pair<unsigned int, unsigned int> PathPair;

    /////////////////////////////////////////////////////////////////////////////
    // ParallelGatewayStep
    class ParallelGatewayStep : public PlanStep
    {
    // Operations
    public:
        ParallelGatewayStep(IPlanNode* parent, const BasicParameter &stepDetail);
        ParallelGatewayStep(IPlanNode* parent, const unsigned long stepId, const int stepType);  // For new plan steps
        virtual ~ParallelGatewayStep();

        virtual PlanStep* clone();
        virtual bool hasCustomIncomingStep() const;
        virtual bool hasCustomOutgoingStep() const;
        virtual bool canIgnoreFailure() const;
		virtual std::string getStepTypeName() const;
        virtual void incomingStepChanged( const unsigned int unPreIncoming, const unsigned int unNewIncoming );
        virtual void outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing );
        virtual bool retrieveIncomings( std::vector<unsigned int>& vecIncomings );
        virtual bool retrieveOutgoings( std::vector<unsigned int>& vecOutgoings );
        virtual void resetFlowParameters();
        virtual void clearFlowParameters();
        virtual void updateFlowParameters();
        virtual bool allowMultipleIncoming();
        virtual bool allowMultipleOutgoing();
		virtual bool canParallelHandle();

		virtual IActivityParameter& getActivityParameter();

    protected:
        ParallelGatewayStep(const ParallelGatewayStep &other);
		virtual void createPlanStepParameters();
		virtual bool isParametersValid( const std::vector<std::string>& parameters );
		virtual CPlanStepEditParameterPage* getParameterEditPage();

    private:
        size_t getShortestPathNum( const std::map<unsigned int, UintSet>& mapPaths, const unsigned int unStart, const unsigned int unEnd );
        void pairForCandidate( const bool bDirectionCandidateToMe, const std::map<unsigned int, UintSet>& mapAllFlows, std::set<unsigned int>& setRival, std::map<PathPair, size_t>& mapPathToPathNum, std::map<unsigned int, size_t>& mapPairCandidate );

    private:
        ParallelGatewayStep &operator=(const ParallelGatewayStep &other);

    // Attributes
    private:
		ParallelGatewayParameter m_params;
		ParallelGatewayStepEditorParameter m_parActivityParameter;
    };
}

#endif // !defined(AFX_PARALLELGATEWAYSTEP_H__4EDB2EEE_9B45_4649_A1B1_2802261ECB21__INCLUDED_)
