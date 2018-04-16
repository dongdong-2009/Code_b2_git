/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_agent/src/PlanAccessFactory.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #33 $
  *
  *  Last modification: $DateTime: 2015/10/26 13:03:00 $
  *  Last modified by:  $Author: qi.huang $
  *
  *  Implementation of PlanAccessFactory.
  *  TODO: put hard coded column names and SQL in a config file or in the data
  *  access classes outlined in the DataAccessInterface usage guide.
  */

#include "core/utilities/src/DebugUtil.h"
#include "bus/response_plans/PlanDataAccess/src/PlanDataReader.h"
#include "app/response_plans/plan_step_plugin/common/src/BasicParameter.h"
#include "app/response_plans/plan_step_plugin/common_active/src/IActivePlan.h"
#include "app/response_plans/plan_step_plugin/common_active/src/ActiveStep.h"
#include "app/response_plans/plan_agent/src/ActiveStartStep.h"
#include "app/response_plans/plan_agent/src/ActiveEndStep.h"
#include "app/response_plans/plan_agent/src/PlanAccessFactory.h"
#include "app/response_plans/plan_agent/src/StepImplementationFactory.h"
#include "app/response_plans/plan_agent/src/PlanInfoCache.h"


namespace TA_Base_App
{

	PlanAccessFactoryImpl::PlanAccessFactoryImpl()
	{
	}
	
    // Improve performance, load all steps in one sql
	void PlanAccessFactoryImpl::getActiveStepList( const unsigned long plan, IActivePlan& planInfo, std::map<unsigned long, ActiveStep*>& mapSteps )
 	{
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin to load steps" );

        std::string reason = "";

		TA_Base_Core::Steps_var varAllSteps;

		boost::shared_ptr<PlanStepInfo> psi = PlanInfoCache::instance()->findPlanInfo(plan);
		if (psi.get() != NULL)
		{
			if (psi->stepDetails.get() != NULL)
			{
				TA_Base_Core::Steps* tmpSteps = new TA_Base_Core::Steps(*psi->stepDetails);
				varAllSteps = tmpSteps;
			}
			else
			{
				varAllSteps = TA_Base_Bus::PlanDataReader::instance()->getAllSteps(plan);
				psi->stepDetails.reset(new TA_Base_Core::Steps(varAllSteps.in()));
			}
		}
		else
		{
			varAllSteps = TA_Base_Bus::PlanDataReader::instance()->getAllSteps(plan);
		}

        TA_Base_Core::StepNumber stepIndex = 1;

        ActiveStep* pPreviousStep = NULL;
        ActiveStep* pCurrentStep = NULL;

        bool stepDefined = true;

        for( stepIndex = 1; stepIndex <= varAllSteps->length(); stepIndex++ )
        {
            stepDefined = false;
            TA_Base_Core::StepDetail& refDetail = varAllSteps[stepIndex - 1].detail;
            BasicParameter oParameter( refDetail.type, refDetail.pkey, refDetail.id, refDetail.position, refDetail.delay,
                0u, refDetail.name.in(), refDetail.description.in(), refDetail.graph.in(), refDetail.skip, refDetail.skippable, refDetail.ignoreFailure );
            pCurrentStep = ActiveStepFactory::instance()->createStep( oParameter, planInfo );

            if ( NULL != pCurrentStep )
            {
				stepDefined = true;
                TA_Base_Core::StepParameters& params = varAllSteps[stepIndex - 1].parameters;

				std::vector<std::string> vecParams;
                for ( size_t nIndex = 0u; nIndex < params.length(); ++nIndex )
                {
                    vecParams.push_back( params[nIndex].in() );
                }

                pCurrentStep->populateParameter( vecParams );
            }

            if ( !stepDefined )
            {
                break;
            }

            if ( NULL == pCurrentStep )
            {
                continue;
            }

            mapSteps.insert( std::pair<unsigned long, ActiveStep*>(pCurrentStep->id(), pCurrentStep) );
		}

        if ( stepDefined )
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Loaded %lu steps", mapSteps.size() );
        }
        else
        {
		    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Failed to load all steps, unusable steptype encountered: %s", reason.c_str() );
        }

		TA_Base_Core::SequenceFlows_var varFlows = NULL;

		if (psi.get() != NULL)
		{
			if (psi->sequenceFlows.get() != NULL)
			{
				TA_Base_Core::SequenceFlows* tmpFlows = new TA_Base_Core::SequenceFlows(*psi->sequenceFlows);
				varFlows = tmpFlows;
			}
			else
			{
				varFlows = TA_Base_Bus::PlanDataReader::instance()->getSequenceFlows(plan);
				psi->sequenceFlows.reset(new TA_Base_Core::SequenceFlows(varFlows.in()));
			}
		}
		else
		{
			varFlows = TA_Base_Bus::PlanDataReader::instance()->getSequenceFlows(plan);
		}

        if ( 0 < varFlows->length() )
        {
            // Populate incoming and outgoing steps
            for ( size_t nIndex = 0u; nIndex < varFlows->length(); ++nIndex )
            {
                TA_Base_Core::SequenceFlow& refFlow = varFlows[nIndex];

				std::map<unsigned long, ActiveStep*>::iterator itOutgoingFound = mapSteps.find( refFlow.outgoingStep );
				std::map<unsigned long, ActiveStep*>::iterator itIncomingFound = mapSteps.find( refFlow.incomingStep );

				if (( mapSteps.end() != itIncomingFound ) && ( mapSteps.end() != itOutgoingFound ))
				{
				    itIncomingFound->second->pushOutgoingStep( *(itOutgoingFound->second) );
				    itOutgoingFound->second->pushIncomingStep( *(itIncomingFound->second) );
				}
            }
        }
        else
        {
            // Compatible with previous versions
            createDefaultFlow( mapSteps );
        }
	}

    void PlanAccessFactoryImpl::createDefaultFlow( const std::map<unsigned long, ActiveStep*>& mapSteps ) const
    {
        // Should has at least start step and end step
        static const size_t MiniStepCount = 2u;
        if ( mapSteps.size() < MiniStepCount )
        {
            return;
        }

        ActiveStep* pPreStep = NULL;
        ActiveStep* pCurStep = NULL;

        for ( std::map<unsigned long, ActiveStep*>::const_iterator itLoop = mapSteps.begin(); mapSteps.end() != itLoop; ++itLoop )
        {
            pCurStep = itLoop->second;

            if ( NULL == pCurStep )
            {
                continue;
            }

            if ( NULL != pPreStep )
            {
                pCurStep->pushIncomingStep( *pPreStep );
                pPreStep->pushOutgoingStep( *pCurStep );
            }

            pPreStep = pCurStep;
        }
    }
} // TA_Base_App
