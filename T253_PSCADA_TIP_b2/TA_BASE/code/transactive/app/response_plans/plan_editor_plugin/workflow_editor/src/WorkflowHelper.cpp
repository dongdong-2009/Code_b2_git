#include "StdAfx.h"
#include <algorithm>

#include "app/response_plans/plan_editor_plugin/workflow_element/src/ServiceTask.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/SequenceFlow.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/WorkflowActivity.h"
#include "ActivityLibrary.h"
#include "WorkflowHelper.h"

WorkflowHelper* WorkflowHelper::s_pInstance = 0;

WorkflowHelper::WorkflowHelper() 
{
}

WorkflowHelper::~WorkflowHelper()
{
    clear();
}

WorkflowHelper& WorkflowHelper::instance()
{
    if ( NULL == s_pInstance )
    {
        s_pInstance = new WorkflowHelper();
    }

    return *s_pInstance;
}

void WorkflowHelper::clear()
{
    m_mapLinkedFlows.clear();

    for ( std::map<unsigned int, SequenceFlow*>::iterator itLoop = m_mapFlows.begin(); m_mapFlows.end() != itLoop; ++itLoop )
    {
        if ( NULL != itLoop->second )
        {
            delete itLoop->second;
            itLoop->second = NULL;
        }
    }

    m_mapFlows.clear();

    for ( std::map<unsigned int, WorkflowActivity*>::iterator itLoop = m_mapActivities.begin(); m_mapActivities.end() != itLoop; ++itLoop )
    {
        if ( NULL != itLoop->second )
        {
            delete itLoop->second;
            itLoop->second = NULL;
        }
    }

    m_mapActivities.clear();
}

void WorkflowHelper::save( std::map<unsigned int, ActivityData>& mapTypeToData, std::map<unsigned int, SequenceFlowData>& mapSequenceFlows )
{
    mapTypeToData.clear();

    for (std::map<unsigned int, WorkflowActivity*>::iterator itLoop = m_mapActivities.begin(); m_mapActivities.end() != itLoop; ++itLoop)
    {
        ActivityData stNewData = { 0u, "", "" };
        std::map<unsigned int, ActivityData>::iterator itNew = mapTypeToData.insert( std::map<unsigned int, ActivityData>::value_type( itLoop->first, stNewData )).first;

        if ( mapTypeToData.end() != itNew )
        {
            itNew->second.Type = itLoop->second->Type;
            itNew->second.Graph = itLoop->second->serialize().c_str();
            itNew->second.InstanceName = itLoop->second->InstanceName.c_str();
        }
    }

    mapSequenceFlows.clear();

    for ( std::map<unsigned int, SequenceFlow*>::iterator itLoop = m_mapFlows.begin(); m_mapFlows.end() != itLoop; ++itLoop )
    {
        SequenceFlowData stNewData = { 0u, 0u, "", "" };
        std::map<unsigned int, SequenceFlowData>::iterator itNew = mapSequenceFlows.insert( std::map<unsigned int, SequenceFlowData>::value_type(itLoop->first, stNewData )).first;

        if ( mapSequenceFlows.end() != itNew )
        {
            itNew->second.Incoming = itLoop->second->IncomingId;
            itNew->second.Outgoing = itLoop->second->OutgoingId;
            itNew->second.Condition = itLoop->second->Condition.c_str();
            itNew->second.Graph = itLoop->second->serialize().c_str();
        }
    }

    clear();
}

void WorkflowHelper::load( const std::map<unsigned int, ActivityData>& mapTypeToData, const std::map<unsigned int, SequenceFlowData>& mapSequenceFlows )
{
    clear();

    // Create workflow activities
    for ( std::map<unsigned int, ActivityData>::const_iterator itLoop = mapTypeToData.begin(); mapTypeToData.end() != itLoop; ++itLoop )
    {
        WorkflowActivity* pNewElement = ActivityLibrary::instance().createActivity( itLoop->second.Type );

        if ( NULL == pNewElement )
        {
            continue;
        }

        pNewElement->deserialize( itLoop->second.Graph );
        pNewElement->ID = itLoop->first;
        pNewElement->InstanceName = itLoop->second.InstanceName.c_str();
        pNewElement->init( pNewElement->PositionX, pNewElement->PositionY );

        m_mapActivities.insert( std::map<unsigned int, WorkflowActivity*>::value_type( itLoop->first, pNewElement ));
    }

    // Create sequence flows
    for ( std::map<unsigned int, SequenceFlowData>::const_iterator itLoop = mapSequenceFlows.begin(); mapSequenceFlows.end() != itLoop; ++itLoop )
    {
        SequenceFlow* pNewElement = new SequenceFlow();

        if ( itLoop->second.Graph.empty() )
        {
            unsigned int unIncomingId = itLoop->second.Incoming;
            unsigned int unOutgoingId = itLoop->second.Outgoing;

            std::map<unsigned int, WorkflowActivity*>::iterator itIncoming = m_mapActivities.find( unIncomingId );
            if ( itIncoming != m_mapActivities.end() )
            {
                WorkflowActivity* pIncoming = itIncoming->second;

                if ( NULL != pIncoming )
                {
                    int nDockX = 0;
                    int nDockY = 0;
                    pIncoming->getDockPosition( nDockX, nDockY );
                    pNewElement->dockedToPosition( unIncomingId, nDockX, nDockY, pIncoming->FrameLeft, pIncoming->FrameTop, pIncoming->FrameRight, pIncoming->FrameBottom );
                }
            }

            std::map<unsigned int, WorkflowActivity*>::iterator itOutgoing = m_mapActivities.find( unOutgoingId );
            if ( itOutgoing != m_mapActivities.end() )
            {
                WorkflowActivity* pOutgoing = itOutgoing->second;

                if ( NULL != pOutgoing )
                {
                    int nDockX = 0;
                    int nDockY = 0;
                    pOutgoing->getDockPosition( nDockX, nDockY );
                    pNewElement->dockedToPosition( unOutgoingId, nDockX, nDockY, pOutgoing->FrameLeft, pOutgoing->FrameTop, pOutgoing->FrameRight, pOutgoing->FrameBottom );
                }
            }
        }
        else
        {
            pNewElement->deserialize( itLoop->second.Graph );
        }

        pNewElement->ID = itLoop->first;
        pNewElement->Condition = itLoop->second.Condition.c_str();

        m_mapFlows.insert( std::map<unsigned int, SequenceFlow*>::value_type( itLoop->first, pNewElement ));

        // Construct flow mappings
        if ( InvalidElementId != pNewElement->IncomingId )
        {
            m_mapLinkedFlows[pNewElement->IncomingId].insert( pNewElement );
        }
        if ( InvalidElementId != pNewElement->OutgoingId )
        {
            m_mapLinkedFlows[pNewElement->OutgoingId].insert( pNewElement );
        }
    }
}

void WorkflowHelper::position( const std::map<unsigned int, unsigned int>& mapPositionToStepId )
{
    // Position
    const int maxXSize = 1000;
    const int startPosX = 10;
    const int startPosY = 10;
    const int ySpace = 40;
    const int xSpace = 40;

    int posX = 0;
    int posY = 0;

    int maxElementHeight = 0;
    bool bInverse = false;
    WorkflowActivity* pLastElement = NULL;

    for (std::map<unsigned int, unsigned int>::const_iterator itLoop = mapPositionToStepId.begin(); mapPositionToStepId.end() != itLoop; ++itLoop)
	{
		if(m_mapActivities.end() == m_mapActivities.find(itLoop->second))
		{
			continue;
		}

		WorkflowActivity* pElement = m_mapActivities[itLoop->second];
		if(NULL == pElement)
		{
			continue;
		}

        if( NULL == pLastElement )
        {
            posX = startPosX;
            posY = startPosY;
        }
        else
        {
            if ( !bInverse )
            {
                posX = pLastElement->PositionX + xSpace + pLastElement->FrameRight - pLastElement->FrameLeft;
                if(( posX + pElement->FrameRight - pElement->FrameLeft ) > maxXSize )
                {
                    posX = maxXSize - ( pElement->FrameRight - pElement->FrameLeft );
                    posY += ( maxElementHeight + ySpace );
                    maxElementHeight = 0;
                    bInverse = true;
                }
            }
            else
            {
                posX = pLastElement->PositionX - xSpace - ( pElement->FrameRight - pElement->FrameLeft );
                if( posX < 0 )
                {
                    posX = startPosX;
                    posY += ( maxElementHeight + ySpace );
                    maxElementHeight = 0;
                    bInverse = false;
                }
            }
        }

        pElement->move( false, posX, posY );

        if (( pElement->FrameBottom - pElement->FrameTop ) > maxElementHeight)
        {
            maxElementHeight = pElement->FrameBottom - pElement->FrameTop;
        }

        pLastElement = pElement;
    }

    for( std::map<unsigned int, WorkflowActivity*>::iterator itLoop = m_mapActivities.begin(); m_mapActivities.end() != itLoop; ++itLoop )
    {
        WorkflowActivity* pElement = itLoop->second;

        if( NULL == pElement )
        {
            continue;
        }

        std::map<unsigned int, std::set<SequenceFlow*>>::iterator itFlowFound = m_mapLinkedFlows.find( pElement->ID );

        if( m_mapLinkedFlows.end() != itFlowFound )
        {
            for( std::set<SequenceFlow*>::iterator itFlowLoop = itFlowFound->second.begin(); itFlowFound->second.end() != itFlowLoop; ++itFlowLoop )
            {
                SequenceFlow* pFlow = *itFlowLoop;

                if( NULL == pFlow )
                {
                    continue;
                }

                int nDockX = 0;
                int nDockY = 0;
                pElement->getDockPosition( nDockX, nDockY );

				if(pFlow->IncomingId == pElement->ID)
				{
					pFlow->updateIncoming(pElement->ID, nDockX, nDockY, pElement->FrameLeft, pElement->FrameTop, pElement->FrameRight, pElement->FrameBottom);
				}
				else if(pFlow->OutgoingId == pElement->ID)
				{
					pFlow->updateOutgoing(pElement->ID, nDockX, nDockY, pElement->FrameLeft, pElement->FrameTop, pElement->FrameRight, pElement->FrameBottom);
				}

                pFlow->dockingMoved( pElement->ID, nDockX, nDockY, pElement->FrameLeft, pElement->FrameTop, pElement->FrameRight, pElement->FrameBottom );
            }
        }
    }
}

void WorkflowHelper::autoPositions( const std::map<unsigned int, unsigned int>& mapPositionToStepId, std::map<unsigned int, ActivityData>& mapTypeToData, std::map<unsigned int, SequenceFlowData>& mapSequenceFlows )
{
    load( mapTypeToData, mapSequenceFlows );

    position( mapPositionToStepId );

    save( mapTypeToData, mapSequenceFlows );
}
