/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/ActivePlanCheckStep.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * <description>
  *
  **/
#include <limits>

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/PlanStepException.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/IPlanNode.h"
#include "app/response_plans/plan_manager/src/PlanStepEditExclusiveGatewayPage.h"
#include "app/response_plans/plan_manager/src/ExclusiveGatewayStep.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

ExclusiveGatewayStep::ExclusiveGatewayStep( IPlanNode* parent, const BasicParameter& stepDetail ) :
PlanStep( parent, m_params, false ),
m_params( stepDetail ),
m_parActivityParameter( m_params, *parent )
{
    FUNCTION_ENTRY( "ExclusiveGatewayStep" );
    FUNCTION_EXIT;
}

ExclusiveGatewayStep::ExclusiveGatewayStep( IPlanNode* parent, const unsigned long stepId, const int stepType ) :
PlanStep( parent, m_params, true ),
m_params( stepId, stepType ),
m_parActivityParameter( m_params, *parent )
{
    FUNCTION_ENTRY( "ExclusiveGatewayStep" );
    FUNCTION_EXIT;
}

ExclusiveGatewayStep::ExclusiveGatewayStep(const ExclusiveGatewayStep& planStep) :
PlanStep(planStep.m_parent, m_params, true),
m_params(planStep.m_params),
m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "ExclusiveGatewayStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}

ExclusiveGatewayStep::~ExclusiveGatewayStep()
{
    FUNCTION_ENTRY( "~ExclusiveGatewayStep" );

    // The parameters are deleted by PlanStep.

    FUNCTION_EXIT;
}

PlanStep* ExclusiveGatewayStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new ExclusiveGatewayStep(*this);
}

void ExclusiveGatewayStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );
    
    FUNCTION_EXIT;
}

CPlanStepEditParameterPage* ExclusiveGatewayStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditExclusiveGatewayPage(this);
}

bool ExclusiveGatewayStep::isParametersValid( const std::vector<std::string>& parameters )
{
    FUNCTION_ENTRY( "isParametersValid" );

    static const size_t ParameterSize = 1u;

    bool bValid = (ParameterSize == parameters.size());
    if ( !bValid )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for ExclusiveGateway step [%u].", parameters.size(), ParameterSize );
    }

    FUNCTION_EXIT;
    return bValid;
}

std::string ExclusiveGatewayStep::getStepTypeName() const
{
	return "Exclusive gateway";
}

IActivityParameter& ExclusiveGatewayStep::getActivityParameter()
{
    reloadParameters();

	return m_parActivityParameter;
}

void ExclusiveGatewayStep::outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing )
{
    if ( 0u != unPrevOutgoing )
    {
        for ( std::vector<ExpressionPair>::iterator itLoop = m_params.stepExpressionPairs.begin(); itLoop != m_params.stepExpressionPairs.end(); ++itLoop )
        {
            if ( itLoop->nextStep == unPrevOutgoing )
            {
                if ( 0u == unNewOutgoing )
                {
                    m_params.stepExpressionPairs.erase( itLoop );
                }
                else
                {
                    itLoop->nextStep = unNewOutgoing;
                }

                break;
            }
        }
    }
    else if ( 0u != unNewOutgoing )
    {
        ExpressionPair stNewExpression;
        m_params.stepExpressionPairs.push_back( stNewExpression );
        m_params.stepExpressionPairs.back().nextStep = unNewOutgoing;
    }
}

bool ExclusiveGatewayStep::retrieveOutgoings( std::vector<unsigned int>& vecOutgoings )
{
    for ( std::vector<ExpressionPair>::iterator itLoop = m_params.stepExpressionPairs.begin(); itLoop != m_params.stepExpressionPairs.end(); ++itLoop )
    {
        if ( 0u != itLoop->nextStep )
        {
            vecOutgoings.push_back( itLoop->nextStep );
        }
    }

    return true;
}

size_t ExclusiveGatewayStep::getShortestPathNum( const std::map<unsigned int, UintSet>& mapPaths, const unsigned int unStart, const unsigned int unEnd )
{
    size_t szShortest = 0u;
    size_t szDepth = 0u;
    std::set<unsigned int> setCurDepth;
    std::set<unsigned int> setLastDepth;
    setCurDepth.insert( unStart );

    // Breadth-First algorithm
    while ( !setCurDepth.empty() )
    {
        ++szDepth;
        setLastDepth = setCurDepth;
        setCurDepth.clear();

        for ( std::set<unsigned int>::iterator itLoop = setLastDepth.begin(); setLastDepth.end() != itLoop; ++itLoop )
        {
            std::map<unsigned int, UintSet>::const_iterator itFound = mapPaths.find( *itLoop );

            if ( itFound != mapPaths.end() )
            {
                if ( itFound->second.end() != itFound->second.find( unEnd ))
                {
                    szShortest = szDepth;
                    break;
                }
                else
                {
                    setCurDepth.insert( itFound->second.begin(), itFound->second.end() );
                }
            }
        }

        if ( szDepth > mapPaths.size() )
        {
            // Avoid dead loop
            break;
        }
    }

    return szShortest;
}

void ExclusiveGatewayStep::updateFlowParameters()
{
    bool bInvertSeek = !m_params.isConverge();

    m_params.pairStep = 0u;

    std::vector<PlanStep*> vecAllSteps = m_parent->getPlanSteps();

    std::map<unsigned int, size_t> mapPairCandidate;
    std::set<unsigned int> setRivalCandidate;

    for ( std::vector<PlanStep*>::iterator itLoop = vecAllSteps.begin(); vecAllSteps.end() != itLoop; ++itLoop )
    {
        PlanStep* pStep = *itLoop;

        if ( NULL == pStep )
        {
            continue;
        }

        // Filter step type (require ExclusiveGateway type) and step id (not this step)
        if (( getStepType() != pStep->getStepType() ) || ( getStepId() == pStep->getStepId() ))
        {
            continue;
        }

        ConditionalGatewayParameter stParameters( pStep->getStepId(), pStep->getStepType() );
        stParameters.deserialize( pStep->compactParameter() );

        if ( stParameters.pairStep == getStepId() )
        {
            m_params.pairStep = pStep->getStepId();
            break;
        }

        bool bConverge = stParameters.isConverge();

        if ( bInvertSeek )
        {
            if (( 0u == stParameters.pairStep ) && bConverge )
            {
                mapPairCandidate.insert( std::make_pair( pStep->getStepId(), 0u ));
            }

            if (( 0u == stParameters.pairStep ) && !bConverge )
            {
                setRivalCandidate.insert( pStep->getStepId() );
            }
        }
        else
        {
            if (( 0u == stParameters.pairStep ) && !bConverge )
            {
                mapPairCandidate.insert( std::make_pair( pStep->getStepId(), 0u ));
            }

            if (( 0u == stParameters.pairStep ) && bConverge )
            {
                setRivalCandidate.insert( pStep->getStepId() );
            }
        }
    }

    if ( 0u != m_params.pairStep )
    {
        return;
    }

    std::map<unsigned int, UintSet> mapAllFlows = m_parent->getSequenceFlows();

    typedef std::pair<unsigned int, unsigned int> PathPair;
    std::map<PathPair, size_t> mapPathToPathNum;

    while ( !setRivalCandidate.empty() )
    {
        unsigned int unClosestRival = getStepId();
        std::multimap<size_t, unsigned int> mapPathNumToRivalCandidate;
        size_t szDeadLoopCounter = 0u;
        bool bDeadLoopOccur = false;

        // Find closest rival candidate
        while ( 0u != unClosestRival )
        {
            mapPathNumToRivalCandidate.clear();

            for ( std::set<unsigned int>::iterator itLoop = setRivalCandidate.begin(); setRivalCandidate.end() != itLoop; ++itLoop )
            {
                // Avoid self checking and skip updated rival candidate
                if ( *itLoop == unClosestRival )
                {
                    continue;
                }

                PathPair pirPath = bInvertSeek ? std::make_pair( unClosestRival, *itLoop ) : std::make_pair( *itLoop, unClosestRival );

                size_t szPathNum = 0u;
                std::map<PathPair, size_t>::iterator itFound = mapPathToPathNum.find( pirPath );
                if ( mapPathToPathNum.end() != itFound )
                {
                    szPathNum = itFound->second;
                }
                else
                {
                    if ( bInvertSeek )
                    {
                        szPathNum = getShortestPathNum( mapAllFlows, unClosestRival, *itLoop );
                    }
                    else
                    {
                        szPathNum = getShortestPathNum( mapAllFlows, *itLoop, unClosestRival );
                    }
                    mapPathToPathNum.insert( std::make_pair( pirPath, szPathNum ));
                }

                mapPathNumToRivalCandidate.insert( std::make_pair( szPathNum, *itLoop ));
            }
            
            if ( unClosestRival == getStepId() )
            {
                // Remove non-connected rival candidate
                for ( std::multimap<size_t, unsigned int>::iterator itRivalLoop = mapPathNumToRivalCandidate.begin();
                    mapPathNumToRivalCandidate.end() != itRivalLoop; ++itRivalLoop )
                {
                    if ( itRivalLoop->first > 0u )
                    {
                        break;
                    }

                    setRivalCandidate.erase( itRivalLoop->second );
                }
            }
            mapPathNumToRivalCandidate.erase( 0u );

            // Update closest rival candidate
            if ( !mapPathNumToRivalCandidate.empty() )
            {
                unClosestRival = mapPathNumToRivalCandidate.begin()->second;
            }
            else
            {
                // No more rival candidate connected
                break;
            }

            ++szDeadLoopCounter;

            if ( szDeadLoopCounter > setRivalCandidate.size() )
            {
                // Dead loop occur
                bDeadLoopOccur = true;
                break;
            }
        }

        if ( bDeadLoopOccur )
        {
            return;
        }

        if ( getStepId() == unClosestRival )
        {
            break;
        }

        if ( 0u != unClosestRival )
        {
            // Find pair gateway for the closest rival candidate
            size_t szPairPathNum = std::numeric_limits<size_t>::max();
            unsigned int unPairStepId = 0u;
            for (std::map<unsigned int, size_t>::iterator itLoop = mapPairCandidate.begin(); mapPairCandidate.end() != itLoop; ++itLoop)
            {
                size_t szCurPathNum = 0u; 
                if ( bInvertSeek )
                {
                    szCurPathNum = getShortestPathNum( mapAllFlows, unClosestRival, itLoop->first );
                }
                else
                {
                    szCurPathNum = getShortestPathNum( mapAllFlows, itLoop->first, unClosestRival );
                }

                if (( 0u != szCurPathNum ) && ( szCurPathNum < szPairPathNum ))
                {
                    unPairStepId = itLoop->first;
                    szPairPathNum = szCurPathNum;
                }
            }

            // Remove rival pair candidate
            mapPairCandidate.erase( unPairStepId );
            setRivalCandidate.erase( unClosestRival );
        }
    }

    size_t szPairPathNum = std::numeric_limits<size_t>::max();
    for ( std::map<unsigned int, size_t>::iterator itLoop = mapPairCandidate.begin(); mapPairCandidate.end() != itLoop; ++itLoop )
    {
        size_t szCurPathNum = 0u; 
        if ( bInvertSeek )
        {
            szCurPathNum = getShortestPathNum( mapAllFlows, getStepId(), itLoop->first );
        }
        else
        {
            szCurPathNum = getShortestPathNum( mapAllFlows, itLoop->first, getStepId() );
        }

        if (( 0u != szCurPathNum ) && ( szCurPathNum < szPairPathNum ))
        {
            m_params.pairStep = itLoop->first;
            szPairPathNum = szCurPathNum;
        }
    }
}

void ExclusiveGatewayStep::resetFlowParameters()
{
    m_params.pairStep = 0u;
}

void ExclusiveGatewayStep::clearFlowParameters()
{
    m_params.stepExpressionPairs.clear();

    m_params.pairStep = 0u;
}

bool ExclusiveGatewayStep::hasCustomOutgoingStep() const
{
    return true;
}

bool ExclusiveGatewayStep::canIgnoreFailure() const
{
    return false;
}

bool ExclusiveGatewayStep::allowMultipleIncoming()
{
    return true;
}

bool ExclusiveGatewayStep::allowMultipleOutgoing()
{
    return true;
}

}
