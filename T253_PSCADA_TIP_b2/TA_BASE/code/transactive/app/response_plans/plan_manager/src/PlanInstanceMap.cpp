//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanInstanceMap.cpp $
// @author:  Bart Golab
// @version: $Revision: #5 $
//
// Last modification: $DateTime: 2015/05/14 18:57:50 $
// Last modified by:  $Author: qi.huang $
//
// <description>

#include "StdAfx.h"
#include "PlanInstanceMap.h"
#include "PlanInstance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// PlanInstanceMap

PlanInstanceMap::PlanInstanceMap()
{
    FUNCTION_ENTRY( "PlanInstanceMap" );

    m_planInstanceMap.clear();

    FUNCTION_EXIT;
}


PlanInstanceMap::PlanInstanceMap(const TA_Base_Core::ActivePlanDetails &activePlanDetails)
{
    FUNCTION_ENTRY( "PlanInstanceMap" );

    addPlanInstances(activePlanDetails);

    FUNCTION_EXIT;
}


PlanInstanceMap::~PlanInstanceMap()
{
    FUNCTION_ENTRY( "~PlanInstanceMap" );

    clearInstances();

    FUNCTION_EXIT;
}


void PlanInstanceMap::addPlanInstances(const TA_Base_Core::ActivePlanDetails &activePlanDetails)
{
    FUNCTION_ENTRY( "addPlanInstances" );

    for (CORBA::ULong i = 0; i < activePlanDetails.length(); i++)
    {
        addPlanInstance(activePlanDetails[i]);
    }

    FUNCTION_EXIT;
}

void TA_Base_App::PlanInstanceMap::clearInstances()
{
    FUNCTION_ENTRY( "clearInstances" );

    while (!m_planInstanceMap.empty())
    {
        PlanInstanceSharedPtr planInstance = m_planInstanceMap.front();
        m_planInstanceMap.erase(planInstance->getInstanceId());
    }

    FUNCTION_EXIT;
}

void PlanInstanceMap::getPlanInstances(PlanInstanceList &planInstances)
{
    FUNCTION_ENTRY( "getPlanInstances" );

    m_planInstanceMap.getItems(planInstances);

    FUNCTION_EXIT;
}


PlanInstanceSharedPtr PlanInstanceMap::getPlanInstance(const PlanInstanceId &instanceId)
{
    FUNCTION_ENTRY( "getPlanInstance" );

    if (m_planInstanceMap.find(instanceId))
    {
        FUNCTION_EXIT;
        return m_planInstanceMap[instanceId];
    }

    FUNCTION_EXIT;
    return PlanInstanceSharedPtr();
}


PlanInstanceSharedPtr PlanInstanceMap::getPlanInstance(PlanInstanceNumber instanceNumber, LocationKey locationKey)
{
    FUNCTION_ENTRY( "getPlanInstance" );
    FUNCTION_EXIT;
    return getPlanInstance(PlanInstanceId(instanceNumber, locationKey));
}

void PlanInstanceMap::updatePlanInstance(const TA_Base_Core::PlanExecuteStepUpdate &activeStepDetail)
{
    FUNCTION_ENTRY( "updatePlanInstance" );

    PlanInstanceSharedPtr planInstance = getPlanInstance(activeStepDetail.plan.instance, activeStepDetail.plan.location);

    if (NULL != planInstance.get())
    {
        planInstance->update(activeStepDetail);
    }

    FUNCTION_EXIT;
}

void PlanInstanceMap::updatePlanInstance(const TA_Base_Core::ActivePlanDetail &activePlanDetail)
{
    FUNCTION_ENTRY( "updatePlanInstance" );

    if (activePlanDetail.activePlanState == TA_Base_Core::EXPIRED_ACTIVE_PLAN_STATE)
    {
        removePlanInstance(activePlanDetail);

        FUNCTION_EXIT;
        return;
    }

    PlanInstanceSharedPtr planInstance = getPlanInstance(activePlanDetail.plan.instance, activePlanDetail.plan.location);

    if (NULL != planInstance.get())
    {
        planInstance->update(activePlanDetail);
    }
    else
    {
        addPlanInstance(activePlanDetail);
    }

    FUNCTION_EXIT;
}


void PlanInstanceMap::updatePlanInstance(const TA_Base_Core::PlanCustomisationUpdate &planCustomDetail)
{
    FUNCTION_ENTRY( "updatePlanInstance" );

    PlanInstanceSharedPtr planInstance = getPlanInstance(planCustomDetail.plan.instance, planCustomDetail.plan.location);

    if (NULL != planInstance.get())
    {
        planInstance->update(planCustomDetail);
    }

    FUNCTION_EXIT;
}


void PlanInstanceMap::addPlanInstance(const TA_Base_Core::ActivePlanDetail &activePlanDetail)
{
    FUNCTION_ENTRY( "addPlanInstance" );

    PlanInstance *planInstance = new PlanInstance(activePlanDetail);

    if (NULL != planInstance)
    {
        PlanInstanceSharedPtr ptrNewInstance(planInstance);
        m_planInstanceMap.insert(planInstance->getInstanceId(), ptrNewInstance);
    }
    else
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed to add plan instance [%lu, %lu]", 
            activePlanDetail.plan.plan, activePlanDetail.plan.instance );
    }

    FUNCTION_EXIT;
}


void PlanInstanceMap::removePlanInstance(const TA_Base_Core::ActivePlanDetail &activePlanDetail)
{
    FUNCTION_ENTRY( "removePlanInstance" );

    PlanInstanceId planInstanceId(activePlanDetail.plan.instance, activePlanDetail.plan.location);

    if (m_planInstanceMap.find(planInstanceId))
    {
        m_planInstanceMap.erase(planInstanceId);
    }

    FUNCTION_EXIT;
}


