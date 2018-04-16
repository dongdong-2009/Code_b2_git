/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/SequenceFlowMap.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2014/07/11 17:19:04 $
  * Last modified by:  $Author: huangjian $
  *
  * This class is a container used by a PlanNode to store its PlanStep's. The steps are stored in a map
  * where the key is their step ID. This class allows steps to be added, deleted, copied and moved.
  *
  **/

#include "ace/Guard_T.h"

#include "app/response_plans/plan_manager/src/SequenceFlow.h"
#include "app/response_plans/plan_manager/src/SequenceFlowMap.h"
#include "app/response_plans/plan_manager/src/PlanAgentAccess.h"

/////////////////////////////////////////////////////////////////////////////
// SequenceFlowMap
namespace TA_Base_App
{

SequenceFlowMap::SequenceFlowMap( PlanNode* parent ) : 
m_parent( parent )
{
    FUNCTION_ENTRY( "SequenceFlowMap" );

    FUNCTION_EXIT;
}

SequenceFlowMap::~SequenceFlowMap()
{
    FUNCTION_ENTRY( "~SequenceFlowMap" );

    clearSequenceFlow();

    FUNCTION_EXIT;
}

void SequenceFlowMap::getSequenceFlows( SequenceFlowList& planSteps )
{
    FUNCTION_ENTRY( "getSequenceFlows" );

    planSteps.clear();

    ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxDataLock );

    for ( std::map<unsigned int, SequenceFlow*>::const_iterator itLoop = m_mapSequenceFlows.begin(); m_mapSequenceFlows.end() != itLoop; ++itLoop )
    {
        if ( NULL != itLoop->second )
        {
            planSteps.push_back( itLoop->second );
        }
    }

    FUNCTION_EXIT;
}

void SequenceFlowMap::getSequenceFlows( const unsigned int unStepId, std::map<unsigned int, unsigned int>& mapFlowIdToIncoming, std::map<unsigned int, unsigned int>& mapFlowIdToOutgoing )
{
    FUNCTION_ENTRY("getSequenceFlows");
    
    mapFlowIdToIncoming.clear();
    mapFlowIdToOutgoing.clear();

    ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxDataLock );
    std::map<unsigned int, std::set<SequenceFlow*>>::iterator itFound = m_linkedSequcenceFlows.find( unStepId );

    if ( m_linkedSequcenceFlows.end() != itFound )
    {
        for ( std::set<SequenceFlow*>::iterator itLoop = itFound->second.begin(); itFound->second.end() != itLoop; ++itLoop )
        {
            SequenceFlow* pFlow = *itLoop;

            if ( NULL == pFlow )
            {
                continue;
            }

            if ( pFlow->incomingStep() == unStepId )
            {
                mapFlowIdToOutgoing.insert( std::make_pair( pFlow->id(), pFlow->outgoingStep() ));
            }
            else if ( pFlow->outgoingStep() == unStepId )
            {
                mapFlowIdToIncoming.insert( std::make_pair( pFlow->id(), pFlow->incomingStep() ));
            }
        }
    }

    FUNCTION_EXIT;
}

void SequenceFlowMap::getSequenceFlows( const unsigned int unStepId, std::vector<unsigned int>& vecIncoming, std::vector<unsigned int>& vecOutgoing )
{
    FUNCTION_ENTRY("getSequenceFlows");
    
    vecIncoming.clear();
    vecOutgoing.clear();

    ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxDataLock );
    std::map<unsigned int, std::set<SequenceFlow*>>::iterator itFound = m_linkedSequcenceFlows.find( unStepId );

    if ( m_linkedSequcenceFlows.end() != itFound )
    {
        for ( std::set<SequenceFlow*>::iterator itLoop = itFound->second.begin(); itFound->second.end() != itLoop; ++itLoop )
        {
            SequenceFlow* pFlow = *itLoop;

            if ( NULL == pFlow )
            {
                continue;
            }

            if ( pFlow->incomingStep() == unStepId )
            {
                vecOutgoing.push_back( pFlow->outgoingStep() );
            }
            else if ( pFlow->outgoingStep() == unStepId )
            {
                vecIncoming.push_back( pFlow->incomingStep() );
            }
        }
    }

    FUNCTION_EXIT;
}

SequenceFlow* SequenceFlowMap::getSequenceFlow( unsigned int unId )
{
    FUNCTION_ENTRY( "getSequenceFlow" );

    SequenceFlow* pFoundFlow = NULL;

    ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxDataLock );

    std::map<unsigned int, SequenceFlow*>::const_iterator itFound = m_mapSequenceFlows.find( unId );

    if ( m_mapSequenceFlows.end() != itFound )
    {
        pFoundFlow = itFound->second;
    }

    FUNCTION_EXIT;
    return pFoundFlow;
}

SequenceFlow* SequenceFlowMap::createSequenceFlow( const unsigned int pkey, const unsigned int unId, const unsigned int unIncoming, const unsigned int unOutgoing, const std::string& strCondition, const std::string& strGraph )
{
    FUNCTION_ENTRY( "createSequenceFlow" );

    FUNCTION_EXIT;
    return new SequenceFlow( m_parent, pkey, unId, unIncoming, unOutgoing, strCondition, strGraph );
}

SequenceFlow* SequenceFlowMap::createNewSequenceFlow( unsigned int unId )
{
    FUNCTION_ENTRY( "createNewSequenceFlow" );

    FUNCTION_EXIT;
    return new SequenceFlow( m_parent, unId );
}

void SequenceFlowMap::deleteSequenceFlow( const unsigned int unId )
{
    FUNCTION_ENTRY( "deleteSequenceFlow" );

    ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxDataLock );

    std::map<unsigned int, SequenceFlow*>::iterator itFound = m_mapSequenceFlows.find( unId );

    if ( m_mapSequenceFlows.end() != itFound )
    {
        SequenceFlow* pToDelete = itFound->second;
        if ( NULL != itFound->second )
        {
            std::map<unsigned int, std::set<SequenceFlow*>>::iterator itInLinked = m_linkedSequcenceFlows.find( pToDelete->incomingStep() );

            if ( itInLinked != m_linkedSequcenceFlows.end() )
            {
                std::set<SequenceFlow*>::iterator itInFound = itInLinked->second.find( pToDelete );

                if ( itInFound != itInLinked->second.end() )
                {
                    itInLinked->second.erase( itInFound );
                }
            }

            std::map<unsigned int, std::set<SequenceFlow*>>::iterator itOutLinked = m_linkedSequcenceFlows.find( pToDelete->outgoingStep() );

            if ( itInLinked != m_linkedSequcenceFlows.end() )
            {
                std::set<SequenceFlow*>::iterator itOutFound = itOutLinked->second.find( pToDelete );

                if ( itOutFound != itOutLinked->second.end() )
                {
                    itOutLinked->second.erase( itOutFound );
                }
            }
        }

        m_mapSequenceFlows.erase( itFound );

        if ( NULL != pToDelete )
        {
            delete pToDelete;
            pToDelete = NULL;
        }
    }

    FUNCTION_EXIT;
}

void SequenceFlowMap::deleteSequenceFlow( const unsigned int unIncoming, const unsigned int unOutgoing )
{
    FUNCTION_ENTRY( "deleteSequenceFlow" );

    std::vector<unsigned int> vecIds;

    {
        ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxDataLock );
        std::map<unsigned int, std::set<SequenceFlow*>>::iterator itInLinked = m_linkedSequcenceFlows.find( unIncoming );

        if ( itInLinked != m_linkedSequcenceFlows.end() )
        {
            for ( std::set<SequenceFlow*>::iterator itLoop = itInLinked->second.begin(); itInLinked->second.end() != itLoop; ++itLoop )
            {
                SequenceFlow* pFlow = *itLoop;

                if ( NULL == pFlow )
                {
                    continue;
                }

                if ( unOutgoing == pFlow->outgoingStep() )
                {
                    vecIds.push_back( pFlow->id() );
                }
            }
        }
    }

    for ( std::vector<unsigned int>::iterator itLoop = vecIds.begin(); vecIds.end() != itLoop; ++itLoop )
    {
        deleteSequenceFlow( *itLoop );
    }

    FUNCTION_EXIT;
}

void SequenceFlowMap::clearSequenceFlow()
{
    FUNCTION_ENTRY( "clearSequenceFlow" );

    ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxDataLock );

    m_linkedSequcenceFlows.clear();

    for ( std::map<unsigned int, SequenceFlow*>::iterator itLoop = m_mapSequenceFlows.begin(); m_mapSequenceFlows.end() != itLoop; ++itLoop )
    {
        if ( NULL != itLoop->second )
        {
            delete itLoop->second;
            itLoop->second = NULL;
        }
    }

    m_mapSequenceFlows.clear();

    FUNCTION_EXIT;
}

void SequenceFlowMap::insertSequenceFlow( const unsigned int pkey, const unsigned int unId, const unsigned int unIncoming, const unsigned int unOutgoing, const std::string& strCondition, const std::string& strGraph )
{
    FUNCTION_ENTRY( "insertSequenceFlow" );

    SequenceFlow* pNewFlow = createSequenceFlow( pkey, unId, unIncoming, unOutgoing, strCondition, strGraph );

    insertSequenceFlow( *pNewFlow );

    FUNCTION_EXIT;
}

void SequenceFlowMap::insertSequenceFlow( const unsigned int unId )
{
    FUNCTION_ENTRY( "clearSequenceFlow" );

    SequenceFlow* pNewFlow = createNewSequenceFlow( unId );
    pNewFlow->pkey( PlanAgentAccess::getInstance().getNextStepId() ) ;

    insertSequenceFlow( *pNewFlow );

    FUNCTION_EXIT;
}

void SequenceFlowMap::insertSequenceFlow( const unsigned int unId, const unsigned int unIncoming, const unsigned int unOutgoing )
{
    FUNCTION_ENTRY( "clearSequenceFlow" );

    SequenceFlow* pNewFlow = createNewSequenceFlow( unId );
    pNewFlow->pkey( PlanAgentAccess::getInstance().getNextStepId() ) ;
    pNewFlow->incomingStep( unIncoming );
    pNewFlow->outgoingStep( unOutgoing );

    insertSequenceFlow( *pNewFlow );

    FUNCTION_EXIT;
}

void SequenceFlowMap::insertSequenceFlow( SequenceFlow& refFlow )
{
    ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxDataLock );

    m_mapSequenceFlows.insert( std::map<unsigned int, SequenceFlow*>::value_type( refFlow.id(), &refFlow ));

    m_linkedSequcenceFlows[refFlow.incomingStep()].insert( &refFlow );
    m_linkedSequcenceFlows[refFlow.outgoingStep()].insert( &refFlow );
}

unsigned long SequenceFlowMap::getNewId()
{
    unsigned int unNewId = m_mapSequenceFlows.empty() ? 1u : (m_mapSequenceFlows.rbegin()->first + 1u);

    return unNewId;
}

bool SequenceFlowMap::linkFlowFrom( const unsigned int unId, const unsigned int unIncoming, unsigned int& unPrevIncoming, unsigned int& unCurOutgoing, const bool bResetGraph )
{
    bool bLinkUpdated = false;

    ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxDataLock );

    std::map<unsigned int, SequenceFlow*>::iterator itFound = m_mapSequenceFlows.find( unId );
    if ( itFound != m_mapSequenceFlows.end() )
    {
        SequenceFlow* pFlow = itFound->second;
        if ( NULL != pFlow )
        {
            unPrevIncoming = pFlow->incomingStep();
            unCurOutgoing = pFlow->outgoingStep();
            bLinkUpdated = (unPrevIncoming != unIncoming);

            if ( bLinkUpdated )
            {
                std::map<unsigned int, std::set<SequenceFlow*>>::iterator itInLinked = m_linkedSequcenceFlows.find( pFlow->incomingStep() );

                if ( itInLinked != m_linkedSequcenceFlows.end() )
                {
                    itInLinked->second.erase( pFlow );
                }

                m_linkedSequcenceFlows[unIncoming].insert( pFlow );
            }

            pFlow->incomingStep( unIncoming );

            if ( bResetGraph )
            {
                pFlow->graph( "" );
            }
        }
    }

    return bLinkUpdated;
}

void SequenceFlowMap::linkFlowFrom( const unsigned int unId, const unsigned int unIncoming, const bool bResetGraph )
{
    unsigned int unPrevIncoming = 0u;
    unsigned int unCurOutgoing = 0u;

    linkFlowFrom( unId, unIncoming, unPrevIncoming, unCurOutgoing, bResetGraph );
}

bool SequenceFlowMap::linkFlowTo( const unsigned int unId, const unsigned int unOutgoing, unsigned int& unPrevOutgoing, unsigned int& unCurIncoming, const bool bResetGraph )
{
    bool bLinkUpdated = false;

    ACE_Write_Guard<ACE_RW_Mutex> guard( m_mtxDataLock );

    std::map<unsigned int, SequenceFlow*>::iterator itFound = m_mapSequenceFlows.find( unId );
    if ( itFound != m_mapSequenceFlows.end() )
    {
        SequenceFlow* pFlow = itFound->second;
        if ( NULL != pFlow )
        {
            unPrevOutgoing = pFlow->outgoingStep();
            unCurIncoming = pFlow->incomingStep();
            bLinkUpdated = (unPrevOutgoing != unOutgoing);

            if ( bLinkUpdated )
            {
                std::map<unsigned int, std::set<SequenceFlow*>>::iterator itOutLinked = m_linkedSequcenceFlows.find( pFlow->outgoingStep() );

                if ( itOutLinked != m_linkedSequcenceFlows.end() )
                {
                    itOutLinked->second.erase( pFlow );
                }

                m_linkedSequcenceFlows[unOutgoing].insert( pFlow );
            }

            pFlow->outgoingStep( unOutgoing );

            if ( bResetGraph )
            {
                pFlow->graph( "" );
            }
        }
    }

    return bLinkUpdated;
}

void SequenceFlowMap::linkFlowTo( const unsigned int unId, const unsigned int unOutgoing, const bool bResetGraph )
{
    unsigned int unPrevOutgoing = 0u;
    unsigned int unCurIncoming = 0u;

    linkFlowTo( unId, unOutgoing, unPrevOutgoing, unCurIncoming, bResetGraph );
}

void SequenceFlowMap::getSequenceFlowId( const unsigned int unIncoming, const unsigned int unOutgoing, unsigned int& unFlowId, bool& bFound )
{
    bFound = false;
    unFlowId = 0u;

    ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxDataLock );
    std::map<unsigned int, std::set<SequenceFlow*>>::iterator itInLinked = m_linkedSequcenceFlows.find( unIncoming );

    if ( itInLinked != m_linkedSequcenceFlows.end() )
    {
        for ( std::set<SequenceFlow*>::iterator itLoop = itInLinked->second.begin(); itInLinked->second.end() != itLoop; ++itLoop )
        {
            SequenceFlow* pFlow = *itLoop;

            if ( NULL == pFlow )
            {
                continue;
            }

            if ( unOutgoing == pFlow->outgoingStep() )
            {
                bFound = true;
                unFlowId = pFlow->id();
                break;
            }
        }
    }
}

size_t SequenceFlowMap::countSequenceFlow( const unsigned int unIncoming, const unsigned int unOutgoing )
{
    size_t szCount = 0u;

    ACE_Read_Guard<ACE_RW_Mutex> guard( m_mtxDataLock );
    std::map<unsigned int, std::set<SequenceFlow*>>::iterator itInLinked = m_linkedSequcenceFlows.find( unIncoming );

    if ( itInLinked != m_linkedSequcenceFlows.end() )
    {
        for ( std::set<SequenceFlow*>::iterator itLoop = itInLinked->second.begin(); itInLinked->second.end() != itLoop; ++itLoop )
        {
            SequenceFlow* pFlow = *itLoop;

            if ( NULL == pFlow )
            {
                continue;
            }

            if ( unOutgoing == pFlow->outgoingStep() )
            {
                ++szCount;
                break;
            }
        }
    }

    return szCount;
}

}
