/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/JumpStep.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "app/response_plans/plan_manager/src/ParallelGatewayStep.h"
#include "app/response_plans/plan_manager/src/PlanStepEditParallelGatewayPage.h"

namespace TA_Base_App
{

/////////////////////////////////////////////////////////////////////////////
// PlanStep

ParallelGatewayStep::ParallelGatewayStep(IPlanNode* parent, const BasicParameter &stepDetail) :
PlanStep(parent, m_params, false),
m_params(stepDetail),
m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "ParallelGatewayStep" );
    FUNCTION_EXIT;
}


ParallelGatewayStep::ParallelGatewayStep(IPlanNode* parent, const unsigned long stepId, const int stepType) :
PlanStep(parent, m_params,true),
m_params(stepId, stepType),
m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "ParallelGatewayStep" );
    FUNCTION_EXIT;
}


ParallelGatewayStep::ParallelGatewayStep(const ParallelGatewayStep& planStep) :
PlanStep(planStep.m_parent, m_params, true),
m_params(planStep.m_params),
m_parActivityParameter(m_params, *m_parent)
{
    FUNCTION_ENTRY( "ParallelGatewayStep" );

    createPlanStepParameters();

    FUNCTION_EXIT;
}


ParallelGatewayStep::~ParallelGatewayStep()
{
    FUNCTION_ENTRY( "~ParallelGatewayStep" );

    FUNCTION_EXIT;
}


PlanStep* ParallelGatewayStep::clone()
{
    FUNCTION_ENTRY( "clone" );

    // Ensure parameters are loaded
    reloadParameters();

    FUNCTION_EXIT;
    return new ParallelGatewayStep(*this);
}


void ParallelGatewayStep::createPlanStepParameters()
{
    FUNCTION_ENTRY( "createPlanStepParameters" );

    FUNCTION_EXIT;
}

bool ParallelGatewayStep::isParametersValid( const std::vector<std::string>& parameters )
{
	FUNCTION_ENTRY( "isParametersValid" );

	static const size_t ParallelGatewayParameterSize = 4u;

	bool bValid = ( ParallelGatewayParameterSize == parameters.size() );
	if ( !bValid )
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid parameter size [%u] for ParallelGateway step [%u].", parameters.size(), ParallelGatewayParameterSize );
	}

	FUNCTION_EXIT;
	return bValid;
}

CPlanStepEditParameterPage* ParallelGatewayStep::getParameterEditPage()
{
    FUNCTION_ENTRY( "getParameterEditPage" );
    FUNCTION_EXIT;
    return new CPlanStepEditParallelGatewayPage(this);
}

std::string ParallelGatewayStep::getStepTypeName() const
{
    return "Parallel Gateway";
}

IActivityParameter& ParallelGatewayStep::getActivityParameter()
{
    reloadParameters();

	return m_parActivityParameter;
}

void ParallelGatewayStep::incomingStepChanged( const unsigned int unPreIncoming, const unsigned int unNewIncoming )
{
    if ( 0u != unPreIncoming )
    {
        for ( std::vector<unsigned int>::iterator itLoop = m_params.incomingStepIds.begin(); itLoop != m_params.incomingStepIds.end(); ++itLoop )
        {
            if ( *itLoop == unPreIncoming )
            {
                if ( 0u == unNewIncoming )
                {
                    m_params.incomingStepIds.erase( itLoop );
                }
                else
                {
                    *itLoop = unNewIncoming;
                }

                break;
            }
        }
    }
    else if ( 0u != unNewIncoming )
    {
        m_params.incomingStepIds.push_back( unNewIncoming );
    }
}

void ParallelGatewayStep::outgoingStepChanged( const unsigned int unPrevOutgoing, const unsigned int unNewOutgoing )
{
    if ( 0u != unPrevOutgoing )
    {
        for ( std::vector<unsigned int>::iterator itLoop = m_params.outgoingStepIds.begin(); itLoop != m_params.outgoingStepIds.end(); ++itLoop )
        {
            if ( *itLoop == unPrevOutgoing )
            {
                if ( 0u == unNewOutgoing )
                {
                    m_params.outgoingStepIds.erase( itLoop );
                }
                else
                {
                    *itLoop = unNewOutgoing;
                }

                break;
            }
        }
    }
    else if ( 0u != unNewOutgoing )
    {
        m_params.outgoingStepIds.push_back( unNewOutgoing );
    }
}

bool ParallelGatewayStep::retrieveIncomings( std::vector<unsigned int>& vecIncomings )
{
    for ( std::vector<unsigned int>::iterator itLoop = m_params.incomingStepIds.begin(); itLoop != m_params.incomingStepIds.end(); ++itLoop )
    {
        if ( 0u != *itLoop )
        {
            vecIncomings.push_back( *itLoop );
        }
    }

    return true;
}

bool ParallelGatewayStep::retrieveOutgoings( std::vector<unsigned int>& vecOutgoings )
{
    for ( std::vector<unsigned int>::iterator itLoop = m_params.outgoingStepIds.begin(); itLoop != m_params.outgoingStepIds.end(); ++itLoop )
    {
        if ( 0u != *itLoop )
        {
            vecOutgoings.push_back( *itLoop );
        }
    }

    return true;
}

bool ParallelGatewayStep::hasCustomOutgoingStep() const
{
    return true;
}

bool ParallelGatewayStep::hasCustomIncomingStep() const
{
    return true;
}

bool ParallelGatewayStep::canIgnoreFailure() const
{
    return false;
}

bool ParallelGatewayStep::allowMultipleIncoming()
{
    return true;
}

bool ParallelGatewayStep::allowMultipleOutgoing()
{
    return true;
}

bool ParallelGatewayStep::canParallelHandle()
{
	return true;
}

size_t ParallelGatewayStep::getShortestPathNum( const std::map<unsigned int, UintSet>& mapPaths, const unsigned int unStart, const unsigned int unEnd )
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

void ParallelGatewayStep::updateFlowParameters()
{
    m_params.pairForkStep = 0u;
    m_params.pairJoinStep = 0u;

    std::vector<PlanStep*> vecAllSteps = m_parent->getPlanSteps();

    std::map<unsigned int, size_t> mapPairForkCandidate;
    std::set<unsigned int> setRivalForkStep;
    std::set<unsigned int> setRivalForkPair;
    std::map<unsigned int, size_t> mapPairJoinCandidate;
    std::set<unsigned int> setRivalJoinStep;
    std::set<unsigned int> setRivalJoinPair;

    bool bSeekPairFork = m_params.incomingStepIds.size() > 1u;
    bool bSeekPairJoin = m_params.outgoingStepIds.size() > 1u;

    for ( std::vector<PlanStep*>::iterator itLoop = vecAllSteps.begin(); vecAllSteps.end() != itLoop; ++itLoop )
    {
        PlanStep* pStep = *itLoop;

        if ( NULL == pStep )
        {
            continue;
        }

        // Filter step type (require ParallelGateway type) and step id (not this step)
        if (( getStepType() != pStep->getStepType() ) || ( getStepId() == pStep->getStepId() ))
        {
            continue;
        }

        ParallelGatewayParameter stParameters( pStep->getStepId(), pStep->getStepType() );
        stParameters.deserialize( pStep->compactParameter() );

        if ( bSeekPairJoin && ( stParameters.pairForkStep == getStepId() ))
        {
            m_params.pairJoinStep = pStep->getStepId();
            bSeekPairJoin = false;
        }

        if ( bSeekPairFork && ( stParameters.pairJoinStep == getStepId() ))
        {
            m_params.pairForkStep = pStep->getStepId();
            bSeekPairFork = false;
        }

        if ( !bSeekPairFork && !bSeekPairJoin )
        {
            break;
        }
        
        if ( bSeekPairFork )
        {
            if (( 0u == stParameters.pairJoinStep ) && ( stParameters.outgoingStepIds.size() == m_params.incomingStepIds.size() ))
            {
                if ( setRivalForkPair.end() == setRivalForkPair.find( pStep->getStepId() ))
                {
                    mapPairForkCandidate.insert( std::make_pair( pStep->getStepId(), 0u ));
                }
            }

            if ( stParameters.incomingStepIds.size() == m_params.incomingStepIds.size() )
            {
                if ( 0u == stParameters.pairForkStep )
                {
                    setRivalForkStep.insert( pStep->getStepId() );
                }
                else
                {
                    mapPairForkCandidate.erase( stParameters.pairForkStep );
                    setRivalForkPair.insert( stParameters.pairForkStep );
                }
            }
        }

        if ( bSeekPairJoin )
        {
            if (( 0u == stParameters.pairForkStep ) && ( stParameters.incomingStepIds.size() == m_params.outgoingStepIds.size() ))
            {
                if ( setRivalJoinPair.end() == setRivalJoinPair.find( pStep->getStepId() ))
                {
                    mapPairJoinCandidate.insert( std::make_pair( pStep->getStepId(), 0u ));
                }
            }

            if ( stParameters.outgoingStepIds.size() == m_params.outgoingStepIds.size() )
            {
                if ( 0u == stParameters.pairJoinStep )
                {
                    setRivalJoinStep.insert( pStep->getStepId() );
                }
                else
                {
                    mapPairJoinCandidate.erase( stParameters.pairJoinStep );
                    setRivalJoinPair.insert( stParameters.pairJoinStep );
                }
            }
        }
    }
    
    if ( !bSeekPairFork && !bSeekPairJoin )
    {
        return;
    }

    std::map<unsigned int, UintSet> mapAllFlows = m_parent->getSequenceFlows();

    std::map<PathPair, size_t> mapPathToPathNum;
    if ( bSeekPairFork )
    {
        pairForCandidate( true, mapAllFlows, setRivalForkStep, mapPathToPathNum, mapPairForkCandidate );

        size_t szPairPathNum = std::numeric_limits<size_t>::max();
        for ( std::map<unsigned int, size_t>::iterator itLoop = mapPairForkCandidate.begin(); mapPairForkCandidate.end() != itLoop; ++itLoop )
        {
            size_t szCurPathNum = getShortestPathNum( mapAllFlows, itLoop->first, getStepId() );

            if (( 0u != szCurPathNum ) && ( szCurPathNum < szPairPathNum ))
            {
                m_params.pairForkStep = itLoop->first;
                szPairPathNum = szCurPathNum;
            }
        }
    }

    if ( bSeekPairJoin )
    {
        pairForCandidate( false, mapAllFlows, setRivalJoinStep, mapPathToPathNum, mapPairJoinCandidate );

        size_t szPairPathNum = std::numeric_limits<size_t>::max();
        for ( std::map<unsigned int, size_t>::iterator itLoop = mapPairJoinCandidate.begin(); mapPairJoinCandidate.end() != itLoop; ++itLoop )
        {
            size_t szCurPathNum = getShortestPathNum( mapAllFlows, getStepId(), itLoop->first );

            if (( 0u != szCurPathNum ) && ( szCurPathNum < szPairPathNum ))
            {
                m_params.pairJoinStep = itLoop->first;
                szPairPathNum = szCurPathNum;
            }
        }
    }
}

void ParallelGatewayStep::resetFlowParameters()
{
    m_params.pairForkStep = 0u;
    m_params.pairJoinStep = 0u;
}

void ParallelGatewayStep::clearFlowParameters()
{
    m_params.pairForkStep = 0u;
    m_params.pairJoinStep = 0u;

    m_params.outgoingStepIds.clear();
    m_params.incomingStepIds.clear();
}

void ParallelGatewayStep::pairForCandidate( const bool bDirectionCandidateToMe, const std::map<unsigned int, UintSet>& mapAllFlows, 
    std::set<unsigned int>& setRival, std::map<PathPair, size_t>& mapPathToPathNum, std::map<unsigned int, size_t>& mapPairCandidate )
{
    while (!setRival.empty())
    {
        unsigned int unClosestRival = getStepId();
        std::multimap<size_t, unsigned int> mapPathNumToRivalCandidate;
        size_t szDeadLoopCounter = 0u;
        bool bDeadLoopOccur = false;

        // Find closest rival candidate
        while (0u != unClosestRival)
        {
            mapPathNumToRivalCandidate.clear();

            for (std::set<unsigned int>::iterator itLoop = setRival.begin(); setRival.end() != itLoop; ++itLoop)
            {
                // Avoid self checking and skip updated rival candidate
                if (*itLoop == unClosestRival)
                {
                    continue;
                }

                PathPair pirPath = bDirectionCandidateToMe ? std::make_pair( *itLoop, unClosestRival ) : std::make_pair( unClosestRival, *itLoop );

                size_t szPathNum = 0u;
                std::map<PathPair, size_t>::iterator itFound = mapPathToPathNum.find(pirPath);
                if (mapPathToPathNum.end() != itFound)
                {
                    szPathNum = itFound->second;
                }
                else
                {
                    if ( bDirectionCandidateToMe )
                    {
                        szPathNum = getShortestPathNum( mapAllFlows, *itLoop, unClosestRival );
                    }
                    else
                    {
                        szPathNum = getShortestPathNum( mapAllFlows, unClosestRival, *itLoop );
                    }
                    mapPathToPathNum.insert(std::make_pair(pirPath, szPathNum));
                }

                mapPathNumToRivalCandidate.insert(std::make_pair(szPathNum, *itLoop));
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

                    setRival.erase( itRivalLoop->second );
                }
            }
            mapPathNumToRivalCandidate.erase( 0u );

            // Update closest rival candidate
            if (!mapPathNumToRivalCandidate.empty())
            {
                unClosestRival = mapPathNumToRivalCandidate.begin()->second;
            }
            else
            {
                // No more rival candidate connected
                break;
            }

            ++szDeadLoopCounter;

            if (szDeadLoopCounter > setRival.size())
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
            return;
        }

        if ( 0u != unClosestRival )
        {
            // Find pair gateway for the closest rival candidate
            size_t szPairPathNum = std::numeric_limits<size_t>::max();
            unsigned int unPairStepId = 0u;
            for (std::map<unsigned int, size_t>::iterator itLoop = mapPairCandidate.begin(); mapPairCandidate.end() != itLoop; ++itLoop)
            {
                size_t szCurPathNum = 0u;
                if ( bDirectionCandidateToMe )
                {
                    szCurPathNum = getShortestPathNum( mapAllFlows, itLoop->first, unClosestRival );
                }
                else
                {
                    szCurPathNum = getShortestPathNum( mapAllFlows, unClosestRival, itLoop->first );
                }

                if ((0u != szCurPathNum) && (szCurPathNum < szPairPathNum))
                {
                    unPairStepId = itLoop->first;
                    szPairPathNum = szCurPathNum;
                }
            }

            // Remove rival pair candidate
            mapPairCandidate.erase( unPairStepId );
            setRival.erase( unClosestRival );
        }
    }
}

}
