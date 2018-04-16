/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/bus/response_plans/plan_agent/src/PlanAgentLibrary.cpp $
* @author:  Ripple
* @version: $Revision: #2 $
*
* Last modification: $DateTime: 2012/02/06 16:15:14 $
* Last modified by:  $Author: haijun.li $
*
*/

#include "bus/response_plans/plan_agent/src/SortableActiveStepId.h"

namespace TA_Base_Bus
{


    SortableActiveStepId::SortableActiveStepId( const unsigned long nodeId, const unsigned long planInstance, const unsigned long stepNumber ):
        m_nodeId(nodeId),
        m_planInstance(planInstance),
        m_stepPosition(stepNumber)
    {
    }

    SortableActiveStepId::SortableActiveStepId( const SortableActiveStepId& activeStepId):
        m_nodeId(activeStepId.m_nodeId),
        m_planInstance(activeStepId.m_planInstance),
        m_stepPosition(activeStepId.m_stepPosition)
    {
    }

    bool SortableActiveStepId::operator<( const SortableActiveStepId& rhs ) const
    {
        if (m_nodeId < rhs.m_nodeId)
        {
            return true;
        }
        else if(m_nodeId > rhs.m_nodeId)
        {
            return false;
        }
        else
        {
            if (m_planInstance < rhs.m_planInstance)
            {
                return true;
            }
            else if (m_planInstance > rhs.m_planInstance)
            {
                return false;
            }
            else
            {
                if (m_stepPosition < rhs.m_stepPosition)
                {
                    return true;
                }
                else if (m_stepPosition > rhs.m_stepPosition)
                {
                    return false;
                }
                else
                {
                    return false;
                }
            }
        }
    }

    void SortableActiveStepId::setNodeId( const unsigned long nodeId )
    {
        m_nodeId = nodeId;
    }

    void SortableActiveStepId::setPlanInstance( const unsigned long planInstance )
    {
        m_planInstance = planInstance;
    }

    void SortableActiveStepId::setStepNumber( const unsigned long stepNumber )
    {
        m_stepPosition = stepNumber;
    }

    unsigned long SortableActiveStepId::getNodeId() const
    {
        return m_nodeId;
    }

    unsigned long SortableActiveStepId::getPlanInstance() const
    {
        return m_planInstance;
    }

    unsigned long SortableActiveStepId::getStepNumber() const
    {
        return m_stepPosition;
    }

}