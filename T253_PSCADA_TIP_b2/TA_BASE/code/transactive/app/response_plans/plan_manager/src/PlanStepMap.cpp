/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepMap.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #25 $
  *
  * Last modification: $DateTime: 2015/10/26 13:03:00 $
  * Last modified by:  $Author: qi.huang $
  *
  * This class is a container used by a PlanNode to store its PlanStep's. The steps are stored in a map
  * where the key is their step ID. This class allows steps to be added, deleted, copied and moved.
  *
  **/

#include "StdAfx.h"

#if defined(_MSC_VER)
    #pragma warning(disable:4250)
#endif // defined (_MSC_VER)

#include <algorithm>
#include "core/exceptions/src/PlanStepException.h"
#include "app/response_plans/plan_step_plugin/common/src/BasicParameter.h"
#include "PlanAgentAccess.h"
#include "PlanStepMap.h"
#include "PlanNode.h"
#include "MftStepImplementationFactory.h"

namespace TA_Base_App
{

    struct PlanStepComparator
    {
        bool operator()(PlanStep *&step1, PlanStep *&step2) const
        {
            return step1->getStepPosition() < step2->getStepPosition();
        }
    };

/////////////////////////////////////////////////////////////////////////////
// PlanStepMap

PlanStepMap::PlanStepMap(PlanNode* parent,const TA_Base_Core::StepDetails &stepDetails) : m_parent(parent)
{
    FUNCTION_ENTRY( "PlanStepMap" );

    for (CORBA::ULong i = 0; i < stepDetails.length(); i++)
    {
        PlanStep *planStep = createPlanStep(stepDetails[i]);

        if ( NULL == planStep )
        {
            // [ToDo] Error handling, clear and delete exist step in map and throw exception
        }
        else
        {
            m_planStepIdMap.insert( planStep->getStepId(), planStep );
            m_planStepPosMap.insert( planStep->getStepPosition(), planStep );
        }
    }

    FUNCTION_EXIT;
}

PlanStepMap::~PlanStepMap()
{
    FUNCTION_ENTRY( "~PlanStepMap" );

    while (!m_planStepIdMap.empty())
    {
        PlanStep* planStep = m_planStepIdMap.front();

        m_planStepIdMap.erase( m_planStepIdMap.frontKey() );

        if (NULL != planStep)
        {
            delete planStep;
            planStep = NULL;
        }
    }

    m_planStepPosMap.clear();

    FUNCTION_EXIT;
}


void PlanStepMap::getPlanSteps(PlanStepList &planSteps)
{
    FUNCTION_ENTRY( "getPlanSteps" );

    m_planStepPosMap.getItems(planSteps);

    FUNCTION_EXIT;
}


PlanStep *PlanStepMap::getPlanStepByPos( unsigned long stepPos )
{
    FUNCTION_ENTRY( "getPlanStepByPos" );

    PlanStep* pStep = NULL;

    m_planStepPosMap.find( stepPos, pStep );

    FUNCTION_EXIT;
    return pStep;
}

PlanStep* PlanStepMap::getPlanStepById( unsigned long stepId )
{
    FUNCTION_ENTRY( "getPlanStepById" );

    PlanStep* pStep = NULL;

    if ( 0u != stepId )
    {
        m_planStepIdMap.find( stepId, pStep );
    }

    FUNCTION_EXIT;
    return pStep;
}

void PlanStepMap::getAdjacentSteps( const unsigned long stepPos, PlanStep*& pInStep, PlanStep*& pOutStep )
{
    pInStep = NULL;
    m_planStepPosMap.find( stepPos - 1u, pInStep );

    pOutStep = NULL;
    m_planStepPosMap.find( stepPos + 1u, pOutStep );
}

PlanStep* PlanStepMap::createPlanStep( const TA_Base_Core::StepDetail& refDetail )
{
    FUNCTION_ENTRY( "createPlanStep" );

    BasicParameter oParameter( refDetail.type, refDetail.pkey, refDetail.id, refDetail.position, refDetail.delay,
        0u, refDetail.name.in(), refDetail.description.in(), refDetail.graph.in(), refDetail.skip, refDetail.skippable, refDetail.ignoreFailure );

    PlanStep* pPlanStep = EditorStepFactory::instance()->createStep( m_parent, oParameter );

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[Pointer] Step [%p] created by Step Map", pPlanStep );

	FUNCTION_EXIT;
	return pPlanStep;
}

PlanStep *PlanStepMap::createNewPlanStep( const int stepType, unsigned long stepId )
{
    FUNCTION_ENTRY( "createNewPlanStep" );

	PlanStep* pPlanStep = EditorStepFactory::instance()->createStep(m_parent, stepType, stepId);

	FUNCTION_EXIT;
	return pPlanStep;
}


void PlanStepMap::insertPlanStep( const int stepType, const unsigned long position, const unsigned long id )
{
    FUNCTION_ENTRY( "insertPlanStep" );

    // Get a step ID for the new step
    unsigned long stepPkey = PlanAgentAccess::getInstance().getNextStepId();

    // Create the new step
    PlanStep* newPlanStep = createNewPlanStep( stepType, stepPkey );

    if ( NULL == newPlanStep )
    {
        return;
    }

    newPlanStep->setStepId( id );
    newPlanStep->setStepPosition( position );

    // Renumber all plan steps below the new step. We don't want to insert the new step prior to doing this
    // because its position would clash with the position of a step already in the map, i.e. getPlanSteps()
    // could not guarantee returning all steps in the correct order.
    PlanStepList planSteps;
    getPlanSteps( planSteps );

    // The step list is sorted by step position. Start at the first step affected by the addition of the new step.
    for (PlanStepList::const_iterator iter = planSteps.begin() + (position - 1); iter != planSteps.end(); iter++)
    {
        PlanStep *planStepToRenumber = *iter;

        if ( NULL == planStepToRenumber )
        {
            continue;
        }

        // Increment the position
        unsigned long newPosition = planStepToRenumber->getStepPosition() + 1u;
        planStepToRenumber->setStepPosition( newPosition );
        m_planStepPosMap[newPosition] = planStepToRenumber;
    }

    // Now add the new step to the map.
    m_planStepIdMap.insert( newPlanStep->getStepId(), newPlanStep );
    m_planStepPosMap[position] = newPlanStep;

    FUNCTION_EXIT;
}

void PlanStepMap::deletePlanStep( unsigned long id )
{
    FUNCTION_ENTRY( "deletePlanStep" );

    PlanStep* pToDelete = NULL;
    if ( !m_planStepIdMap.find( id, pToDelete ))
    {
        return;
    }

    unsigned long position = pToDelete->getStepPosition();

    // Remove the specified steps, determining the position of topmost one at the same time.
    unsigned long firstRemovedPosition = getLastStepPosition();
    m_planStepIdMap.erase( pToDelete->getStepId() );
    m_planStepPosMap.erase( position );
    firstRemovedPosition = __min( firstRemovedPosition, position );

    delete pToDelete;
    pToDelete = NULL;

    // Renumber all plan steps following the first removed step.
    PlanStepList planSteps;
    getPlanSteps( planSteps );

    int newPosition = firstRemovedPosition;
    for ( PlanStepList::const_iterator iter = planSteps.begin() + (firstRemovedPosition - 1); iter != planSteps.end(); iter++ )
    {
        PlanStep *planStepToRenumber = *iter;

        // Adjust the position
        planStepToRenumber->setStepPosition( newPosition );
        m_planStepPosMap[newPosition] = planStepToRenumber;
        newPosition++;
    }

    m_planStepPosMap.erase( newPosition );

    FUNCTION_EXIT;
}

unsigned long PlanStepMap::deletePlanSteps(PlanStepList &deleteSteps)
{
    FUNCTION_ENTRY( "deletePlanSteps" );

    // Remove the specified steps, determining the position of topmost one at the same time.
    unsigned long firstRemovedPosition = getLastStepPosition();
    size_t szDelNum = deleteSteps.size();
    for (PlanStepList::const_iterator iter = deleteSteps.begin(); iter != deleteSteps.end(); iter++)
    {
        PlanStep *planStep = *iter;

        m_planStepIdMap.erase(planStep->getStepId());
        m_planStepPosMap.erase(planStep->getStepPosition());
        firstRemovedPosition = __min(firstRemovedPosition, planStep->getStepPosition());

        delete planStep;
        planStep = NULL;
    }
    deleteSteps.clear();

    // Renumber all plan steps following the first removed step.
    PlanStepList planSteps;
    getPlanSteps( planSteps );

    int newPosition = firstRemovedPosition;
    for (PlanStepList::const_iterator iter = planSteps.begin() + (firstRemovedPosition - 1); iter != planSteps.end(); iter++)
    {
        PlanStep *planStepToRenumber = *iter;

        // Adjust the position
        planStepToRenumber->setStepPosition( newPosition );
        m_planStepPosMap[newPosition] = planStepToRenumber;
        newPosition++;
    }

    for ( size_t unEraseLoop = 0u; unEraseLoop < szDelNum; ++unEraseLoop )
    {
        m_planStepPosMap.erase( newPosition++ );
    }

    FUNCTION_EXIT;
    return firstRemovedPosition;
}


void PlanStepMap::copyPlanSteps( const PlanStepList &copySteps, const unsigned long position, std::vector<unsigned int>& newStepIds )
{
    FUNCTION_ENTRY( "copyPlanSteps" );

    // Get step IDs for the copied steps
    std::vector<unsigned long> stepPkeys;
    stepPkeys.reserve(copySteps.size());
    for (unsigned int i = 0; i < copySteps.size(); i++)
    {
        stepPkeys.push_back(PlanAgentAccess::getInstance().getNextStepId());
    }

    // Renumber all plan steps below the plan copy insertion point. We don't want to insert the copied steps
    // prior to doing this because their position would clash with the position of steps already in the map,
    // i.e. getPlanSteps() could not guarantee returning all steps in the correct order.
    PlanStepList planSteps;
    getPlanSteps(planSteps);

    for (PlanStepList::const_iterator iter = planSteps.begin() + (position - 1); iter != planSteps.end(); iter++)
    {
        PlanStep *planStepToRenumber = *iter;

        // Adjust the position
        unsigned long newPosition = planStepToRenumber->getStepPosition() + copySteps.size();
        planStepToRenumber->setStepPosition( newPosition );
        m_planStepPosMap[newPosition] = planStepToRenumber;
    }

    // Sort the steps being copied so that we can duplicate and insert them in the correct order.
    PlanStepList sortedCopySteps(copySteps);
    std::sort(sortedCopySteps.begin(), sortedCopySteps.end(), PlanStepComparator());

    // Clone the steps being copied
    int stepIdIdx = 0;
    int newPosition = position;
    newStepIds.reserve( sortedCopySteps.size() );
    for (PlanStepList::const_iterator iter = sortedCopySteps.begin(); iter != sortedCopySteps.end(); iter++)
    {
        PlanStep *planStep = *iter;
        PlanStep *planStepCopy = planStep->clone();
        TA_ASSERT(planStepCopy, "Failed to make copy of plan step");

        planStepCopy->setStepPkey( stepPkeys[stepIdIdx++] );
        planStepCopy->setStepPosition( newPosition );
        planStepCopy->setStepId( getNewId() );
        planStepCopy->setStepGraph( "" );
        planStepCopy->clearFlowParameters();

        // Add to the step map
        m_planStepIdMap.insert(planStepCopy->getStepId(), planStepCopy);
        m_planStepPosMap[newPosition] = planStepCopy;
        newStepIds.push_back( planStepCopy->getStepId() );

        newPosition++;
    }

    FUNCTION_EXIT;
}


unsigned long PlanStepMap::movePlanSteps(const PlanStepList &moveSteps, unsigned long position)
{
    FUNCTION_ENTRY( "movePlanSteps" );

    unsigned long firstPosition = getLastStepPosition();  // position of the first moved step
    unsigned long lastPosition = 1;                       // position of the last moved step

    unsigned long precedingTarget = 0;  // number of steps being moved that are before the target position
    unsigned long followingTarget = 0;  // number of steps being moved that are after the target position

    // Determine the positions of the first and last step being moved.
    // Determine the number of steps being moved from before and after the target position.
    for (PlanStepList::const_iterator iter = moveSteps.begin(); iter != moveSteps.end(); iter++)
    {
        PlanStep *planStep = *iter;

        firstPosition = __min(firstPosition, planStep->getStepPosition());
        lastPosition = __max(lastPosition, planStep->getStepPosition());

        if (planStep->getStepPosition() < position)
        {
            precedingTarget++;
        }
        else
        {
            followingTarget++;
        }

        // Remove the moved steps from our step map so that the remaining ones can be renumbered
        m_planStepIdMap.erase(planStep->getStepId());
        m_planStepPosMap.erase(planStep->getStepPosition());
    }

    // Get the steps not being moved. They are sorted by their existing position.
    PlanStepList planSteps;
    getPlanSteps(planSteps);

    // If steps are being moved from before the target position, renumber the remaining steps
    // between the original position of the first step being moved and the target position.
    if (precedingTarget > 0)
    {
        PlanStepList::const_iterator beginIter = planSteps.begin() + (firstPosition - 1);
        PlanStepList::const_iterator endIter = planSteps.begin() + (position - precedingTarget - 1);

        unsigned long newPosition = firstPosition;
        for (PlanStepList::const_iterator iter = beginIter; iter != endIter; iter++)
        {
            PlanStep *planStepToRenumber = *iter;

            planStepToRenumber->setStepPosition( newPosition );
            m_planStepPosMap[newPosition] = planStepToRenumber;
            newPosition++;
        }
    }

    // If steps are being moved from after the target position, renumber the remaining steps
    // between the target position and the original position of the last step being moved.
    if (followingTarget > 0)
    {
        PlanStepList::const_iterator beginIter = planSteps.begin() + (position - precedingTarget - 1);
        PlanStepList::const_iterator endIter = planSteps.begin() + (lastPosition - moveSteps.size());

        unsigned long newPosition = position + followingTarget;
        for (PlanStepList::const_iterator iter = beginIter; iter != endIter; iter++)
        {
            PlanStep *planStepToRenumber = *iter;

            planStepToRenumber->setStepPosition( newPosition );
            m_planStepPosMap[newPosition] = planStepToRenumber;
            newPosition++;
        }
    }

    // Sort the steps being moved so that we can reassign their positions in the correct order.
    PlanStepList sortedMoveSteps(moveSteps);
    std::sort(sortedMoveSteps.begin(), sortedMoveSteps.end(), PlanStepComparator());

    unsigned long newPosition = position - precedingTarget;
    for (PlanStepList::const_iterator iter = sortedMoveSteps.begin(); iter != sortedMoveSteps.end(); iter++)
    {
        PlanStep *planStep = *iter;

        planStep->setStepPosition( newPosition );

        // Add back to the step map
        m_planStepIdMap.insert(planStep->getStepId(), planStep);
        m_planStepPosMap[newPosition] = planStep;
        newPosition++;
    }

    FUNCTION_EXIT;
    return (position - precedingTarget);
}

unsigned long PlanStepMap::getLastStepPosition()
{
    FUNCTION_ENTRY( "getLastStepPosition" );
    FUNCTION_EXIT;
    return m_planStepIdMap.size();
}

unsigned long PlanStepMap::getNewId()
{
    unsigned int unNewId = m_planStepIdMap.empty() ? 1u : (m_planStepIdMap.backKey() + 1u);

    return unNewId;
}

unsigned long PlanStepMap::getNewPostion()
{
    unsigned int unNewPos = m_planStepPosMap.empty() ? 1u : (m_planStepPosMap.backKey());

    return unNewPos;
}

}
