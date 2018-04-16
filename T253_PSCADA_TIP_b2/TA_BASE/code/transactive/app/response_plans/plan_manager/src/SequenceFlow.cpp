//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/SequenceFlow.cpp $
// @author:  Bart Golab
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2014/05/26 14:33:17 $
// Last modified by:  $Author: huangjian $
//
// <description>


#include "core/utilities/src/DebugUtil.h"
#include "SequenceFlow.h"

namespace TA_Base_App
{

SequenceFlow::SequenceFlow( PlanNode* parent, const unsigned int unId ) :
m_parent( parent ),
m_unPkey( 0u ),
m_unId( unId ),
m_unIncomingStep( 0u ),
m_unOutgoingStep( 0u ),
m_strCondition( "" ),
m_strGraph( "" )
{
    FUNCTION_ENTRY( "SequenceFlow" );
    FUNCTION_EXIT;
}


SequenceFlow::SequenceFlow( PlanNode* parent, const unsigned int pkey, const unsigned int unId, const unsigned int unIncoming, const unsigned int unOutgoing, const std::string& strCondition, const std::string& strGraph ) :
m_parent( parent ),
m_unPkey( pkey ),
m_unId( unId ),
m_unIncomingStep( unIncoming ),
m_unOutgoingStep( unOutgoing ),
m_strCondition( strCondition.c_str() ),
m_strGraph( strGraph.c_str() )
{
    FUNCTION_ENTRY( "SequenceFlow" );
    FUNCTION_EXIT;
}


SequenceFlow::SequenceFlow(const SequenceFlow& SequenceFlow) : 
m_parent( SequenceFlow.m_parent ),
m_unId( SequenceFlow.m_unId ),
m_unIncomingStep( SequenceFlow.m_unIncomingStep ),
m_unOutgoingStep( SequenceFlow.m_unOutgoingStep ),
m_strCondition( SequenceFlow.m_strCondition.c_str() ),
m_strGraph( SequenceFlow.m_strGraph.c_str() )
{
    FUNCTION_ENTRY( "SequenceFlow" );
    FUNCTION_EXIT;
}


SequenceFlow::~SequenceFlow()
{
    FUNCTION_ENTRY( "~SequenceFlow" );
    FUNCTION_EXIT;
}

std::string SequenceFlow::graph() const
{
    FUNCTION_ENTRY( "graph" );

    FUNCTION_EXIT;
    return m_strGraph;
}

void SequenceFlow::graph( const std::string& graph )
{
    FUNCTION_ENTRY( "graph" );

    m_strGraph = graph.c_str();

    FUNCTION_EXIT;
}

unsigned int SequenceFlow::pkey() const
{
    FUNCTION_ENTRY( "pkey" );

    FUNCTION_EXIT;
    return m_unPkey;
}

void SequenceFlow::pkey( const unsigned int pkey )
{
    FUNCTION_ENTRY( "pkey" );

    m_unPkey = pkey;

    FUNCTION_EXIT;
}

unsigned int SequenceFlow::id() const
{
    FUNCTION_ENTRY( "id" );

    FUNCTION_EXIT;
    return m_unId;
}

void SequenceFlow::id( const unsigned int stepId )
{
    FUNCTION_ENTRY( "id" );

    m_unId = stepId;

    FUNCTION_EXIT;
}

unsigned int SequenceFlow::incomingStep() const
{
    FUNCTION_ENTRY( "incomingStep" );

    FUNCTION_EXIT;
    return m_unIncomingStep;
}

void SequenceFlow::incomingStep( const unsigned int stepId )
{
    FUNCTION_ENTRY( "incomingStep" );

    m_unIncomingStep = stepId;

    FUNCTION_EXIT;
}

unsigned int SequenceFlow::outgoingStep() const
{
    FUNCTION_ENTRY( "outgoingStep" );

    FUNCTION_EXIT;
    return m_unOutgoingStep;
}

void SequenceFlow::outgoingStep( const unsigned int stepId )
{
    FUNCTION_ENTRY( "outgoingStep" );

    m_unOutgoingStep = stepId;

    FUNCTION_EXIT;
}

std::string SequenceFlow::condition() const
{
    FUNCTION_ENTRY( "condition" );

    FUNCTION_EXIT;
    return m_strCondition;
}

void SequenceFlow::condition( const std::string& strCondition )
{
    FUNCTION_ENTRY( "condition" );

    m_strCondition = strCondition.c_str();

    FUNCTION_EXIT;
}

}

