
#include <set>
#include <algorithm>
#include <limits>

#include "app/response_plans/plan_editor_plugin/workflow_element/src/ServiceTask.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/SequenceFlow.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/WorkflowActivity.h"
#include "IWorkflowOwner.h"
#include "IDiagramViewer.h"
#include "WorkflowException.h"
#include "ActivityLibrary.h"
#include "WorkflowDiagram.h"

WorkflowDiagram::WorkflowDiagram() :
Width( 0u ),
Height( 0u ),
ReadOnly( false ),
Updated( false ),
m_pOwner( NULL )
{
}

WorkflowDiagram::~WorkflowDiagram()
{
    clearDiagram();
}

void WorkflowDiagram::attachViewer( IDiagramViewer& refViewer )
{
    std::list<IDiagramViewer*>::iterator itFound = std::find(m_lstViewers.begin(), m_lstViewers.end(), &refViewer);

    if (m_lstViewers.end() == itFound)
    {
        m_lstViewers.push_back(&refViewer);
    }
}

void WorkflowDiagram::detachViewer( IDiagramViewer& refViewer )
{
    std::list<IDiagramViewer*>::iterator itFound = std::find(m_lstViewers.begin(), m_lstViewers.end(), &refViewer);

    if (m_lstViewers.end() != itFound)
    {
        m_lstViewers.erase(itFound);
    }
}

unsigned int WorkflowDiagram::addElement( const int nTypeId, const int nPosX, const int nPosY )
{
    unsigned int unNewId = m_mapActivities.empty() ? 1u : (m_mapActivities.rbegin()->first + 1u);
    WorkflowActivity* pNewElement = ActivityLibrary::instance().createActivity( nTypeId );

    if (NULL == pNewElement)
    {
        return InvalidElementId;
    }

    pNewElement->ID = unNewId;
    pNewElement->init( nPosX, nPosY );
    m_mapActivities.insert( std::map<unsigned int, WorkflowActivity*>::value_type( unNewId, pNewElement ));

    updateActivityOrderZ( pNewElement->ID, std::numeric_limits<int>::max() );

    // Notify viewers
    for (std::list<IDiagramViewer*>::iterator itLoop = m_lstViewers.begin(); m_lstViewers.end() != itLoop; ++itLoop)
    {
        IDiagramViewer* pViewer = *itLoop;

        if (NULL == pViewer)
        {
            continue;
        }

        pViewer->elementAdded(unNewId);
    }

    if ( NULL != m_pOwner )
    {
        m_pOwner->activityCreated( nTypeId, unNewId );
    }

    return unNewId;
}

unsigned int WorkflowDiagram::addFlow( const int nPosX, const int nPosY )
{
    unsigned int unNewId = m_mapFlows.empty() ? 1u : (m_mapFlows.rbegin()->first + 1u);
    SequenceFlow* pNewElement = new SequenceFlow();

    if ( NULL == pNewElement )
    {
        return InvalidElementId;
    }

    pNewElement->ID = unNewId;
    pNewElement->init( nPosX, nPosY );
    m_mapFlows.insert( std::map<unsigned int, SequenceFlow*>::value_type( unNewId, pNewElement ) );

    updateFlowOrderZ( pNewElement->ID, std::numeric_limits<int>::max() );

    // Notify viewers
    for ( std::list<IDiagramViewer*>::iterator itLoop = m_lstViewers.begin(); m_lstViewers.end() != itLoop; ++itLoop )
    {
        IDiagramViewer* pViewer = *itLoop;

        if ( NULL == pViewer )
        {
            continue;
        }

        pViewer->elementAdded( unNewId );
    }

    if ( NULL != m_pOwner )
    {
        m_pOwner->flowCreated( unNewId );
    }

    return unNewId;
}

void WorkflowDiagram::deleteActivity( const unsigned int unId )
{
    std::map<unsigned int, WorkflowActivity*>::iterator itActivityFound = m_mapActivities.find( unId );
    if ( m_mapActivities.end() != itActivityFound )
    {
        delete itActivityFound->second;
        itActivityFound->second = NULL;
        m_mapActivities.erase( itActivityFound );

        std::map<unsigned int, std::set<SequenceFlow*>>::iterator itFlows = m_mapLinkedFlows.find( unId );
        if ( m_mapLinkedFlows.end() != itFlows )
        {
            for ( std::set<SequenceFlow*>::iterator itLoop = itFlows->second.begin(); itFlows->second.end() != itLoop; )
            {
                int eChanged = (*itLoop)->deleteDocking( unId );

                dockingUpdated( (*itLoop)->ID, eChanged, InvalidElementId );

                if ( eNoChange != eChanged )
                {
                    itLoop = itFlows->second.erase( itLoop );
                }
                else
                {
                    ++itLoop;
                }
            }
        }

        if ( NULL != m_pOwner )
        {
            m_pOwner->activityDeleted( unId );
        }
    }
}

void WorkflowDiagram::deleteFlow( const unsigned int unId )
{
    std::map<unsigned int, SequenceFlow*>::iterator itFlowFound = m_mapFlows.find( unId );
    if ( m_mapFlows.end() != itFlowFound )
    {
        std::map<unsigned int, std::set<SequenceFlow*>>::iterator itIncoming = m_mapLinkedFlows.find( itFlowFound->second->IncomingId );
        if ( m_mapLinkedFlows.end() != itIncoming )
        {
            std::set<SequenceFlow*>::iterator itInFlow = itIncoming->second.find( itFlowFound->second );
            if ( itIncoming->second.end() != itInFlow )
            {
                itIncoming->second.erase( *itInFlow );
            }
        }

        std::map<unsigned int, std::set<SequenceFlow*>>::iterator itOutgoing = m_mapLinkedFlows.find( itFlowFound->second->OutgoingId );
        if ( m_mapLinkedFlows.end() != itOutgoing )
        {
            std::set<SequenceFlow*>::iterator itOutFlow = itOutgoing->second.find( itFlowFound->second );
            if ( itOutgoing->second.end() != itOutFlow )
            {
                itOutgoing->second.erase( *itOutFlow );
            }
        }

        delete itFlowFound->second;
        itFlowFound->second = NULL;
        m_mapFlows.erase( itFlowFound );

        if ( NULL != m_pOwner )
        {
            m_pOwner->flowDeleted( unId );
        }
    }
}

bool WorkflowDiagram::selectElement( const bool bEnableMultiple, const int nPosX, const int nPosY )
{
    int nHitOrderZ = std::numeric_limits<int>::max();
    WorkflowElement* pCurHit = NULL;

    bool bActivitySelected = false;

    for ( std::map<unsigned int, WorkflowActivity*>::iterator itLoop = m_mapActivities.begin(); m_mapActivities.end() != itLoop; ++itLoop )
    {
        WorkflowActivity* pElement = itLoop->second;

        if (NULL == pElement)
        {
            continue;
        }

        if ( pElement->hitTest( nPosX, nPosY ))
        {
            if (( NULL == pCurHit ) || ( pElement->OrderZ < nHitOrderZ ))
            {
                // Update current hit and current z order of hit element
                pCurHit = pElement;
                bActivitySelected = true;
                nHitOrderZ = pElement->OrderZ;
            }
        }
    }

    for ( std::map<unsigned int, SequenceFlow*>::iterator itLoop = m_mapFlows.begin(); m_mapFlows.end() != itLoop; ++itLoop )
    {
        SequenceFlow* pElement = itLoop->second;

        if ( NULL == pElement )
        {
            continue;
        }

        if ( pElement->hitTest( nPosX, nPosY ) )
        {
            if (( NULL == pCurHit ) || ( pElement->OrderZ < nHitOrderZ ))
            {
                // Update current hit and current z order of hit element
                pCurHit = pElement;
                bActivitySelected = false;
                nHitOrderZ = pElement->OrderZ;
            }
        }
    }

    unsigned int unSelected = InvalidElementId;
    unsigned int unDeselected = InvalidElementId;
    std::vector<unsigned int> vecDeselected;
    bool bKeepSelections = false;

    if ( NULL != pCurHit )
    {
        if ( pCurHit->trySelect( bEnableMultiple, nPosX, nPosY ) )
        {
            // Hit element is selected
            if ( pCurHit->Selected )
            {
                unSelected = pCurHit->ID;
                if ( bActivitySelected )
                {
                    updateActivityOrderZ( pCurHit->ID, pCurHit->OrderZ );
                }
                else
                {
                    updateFlowOrderZ( pCurHit->ID, pCurHit->OrderZ );
                }
            }
            // Hit element is deselected
            else
            {
                unDeselected = pCurHit->ID;
                vecDeselected.push_back( pCurHit->ID );
            }

            bKeepSelections = bEnableMultiple;
        }
        // Hit but selection state keeps as selected
        else if ( pCurHit->Selected )
        {
            bKeepSelections = true;
        }
    }
    else
    {
        bKeepSelections = bEnableMultiple;
    }

    if ( !bKeepSelections )
    {
        for ( std::list<WorkflowActivity*>::iterator itLoop = m_lstSelectedActivities.begin(); m_lstSelectedActivities.end() != itLoop; ++itLoop )
        {
            WorkflowActivity* pElement = *itLoop;

            if ( NULL == pElement )
            {
                continue;
            }

            if ( bActivitySelected && (( pElement->ID == unDeselected ) || ( pElement->ID == unSelected )))
            {
                continue;
            }

            pElement->deselect();
            vecDeselected.push_back( pElement->ID );
        }

        m_lstSelectedActivities.clear();

        for ( std::list<SequenceFlow*>::iterator itLoop = m_lstSelectedFlows.begin(); m_lstSelectedFlows.end() != itLoop; ++itLoop )
        {
            SequenceFlow* pElement = *itLoop;

            if ( NULL == pElement )
            {
                continue;
            }

            if ( !bActivitySelected && (( pElement->ID == unDeselected ) || ( pElement->ID == unSelected )))
            {
                continue;
            }

            pElement->deselect();
            vecDeselected.push_back( pElement->ID );
        }

        m_lstSelectedFlows.clear();
    }

    if ( NULL != pCurHit )
    {
        if ( bActivitySelected )
        {
            std::list<WorkflowActivity*>::iterator itFound = std::find( m_lstSelectedActivities.begin(), m_lstSelectedActivities.end(), pCurHit );

            if ( m_lstSelectedActivities.end() == itFound )
            {
                if ( unSelected == pCurHit->ID )
                {
                    WorkflowActivity* pCurHitActivity = dynamic_cast<WorkflowActivity*>(pCurHit);
                    if ( NULL != pCurHitActivity )
                    {
                        m_lstSelectedActivities.push_back( pCurHitActivity );
                    }
                }
            }
            else if ( unDeselected == pCurHit->ID )
            {
                m_lstSelectedActivities.erase( itFound );
            }
        }
        else
        {
            std::list<SequenceFlow*>::iterator itFound = std::find( m_lstSelectedFlows.begin(), m_lstSelectedFlows.end(), pCurHit );

            if ( m_lstSelectedFlows.end() == itFound )
            {
                if ( unSelected == pCurHit->ID )
                {
                    SequenceFlow* pCurHitFlow = dynamic_cast<SequenceFlow*>(pCurHit);
                    if ( NULL != pCurHitFlow )
                    {
                        m_lstSelectedFlows.push_back( pCurHitFlow );
                    }
                }
            }
            else if ( unDeselected == pCurHit->ID )
            {
                m_lstSelectedFlows.erase( itFound );
            }
        }
    }

    if ((unSelected != InvalidElementId) || !vecDeselected.empty())
    {
        // Notify viewers
        for (std::list<IDiagramViewer*>::iterator itLoop = m_lstViewers.begin(); m_lstViewers.end() != itLoop; ++itLoop)
        {
            IDiagramViewer* pViewer = *itLoop;

            if (NULL == pViewer)
            {
                continue;
            }

            pViewer->elementSelected(unSelected, vecDeselected);
        }
    }

    repositionSelectionFrame();

    return (NULL != pCurHit);
}

bool WorkflowDiagram::selectArea( const bool bEnableMultiple, const int nLeft, const int nTop, const int nRight, const int nBottom )
{
    std::multimap<int, WorkflowActivity*> mapOrderedActivitiesHit;

    for ( std::map<unsigned int, WorkflowActivity*>::iterator itLoop = m_mapActivities.begin(); m_mapActivities.end() != itLoop; ++itLoop )
    {
        WorkflowActivity* pElement = itLoop->second;

        if ( NULL == pElement )
        {
            continue;
        }

        if ( pElement->intersect( nLeft, nTop, nRight, nBottom ) )
        {
            mapOrderedActivitiesHit.insert( std::multimap<int, WorkflowActivity*>::value_type( pElement->OrderZ, pElement ) );
        }
    }

    std::multimap<int, SequenceFlow*> mapOrderedFlowsHit;

    for ( std::map<unsigned int, SequenceFlow*>::iterator itLoop = m_mapFlows.begin(); m_mapFlows.end() != itLoop; ++itLoop )
    {
        SequenceFlow* pElement = itLoop->second;

        if ( NULL == pElement )
        {
            continue;
        }

        if ( pElement->intersect( nLeft, nTop, nRight, nBottom ) )
        {
            mapOrderedFlowsHit.insert( std::multimap<int, SequenceFlow*>::value_type( pElement->OrderZ, pElement ) );
        }
    }

    if ( !bEnableMultiple )
    {
        for ( std::list<WorkflowActivity*>::iterator itLoop = m_lstSelectedActivities.begin(); m_lstSelectedActivities.end() != itLoop; ++itLoop )
        {
            WorkflowActivity* pElement = *itLoop;

            if ( NULL == pElement )
            {
                continue;
            }

            pElement->deselect();
        }

        m_lstSelectedActivities.clear();

        for ( std::list<SequenceFlow*>::iterator itLoop = m_lstSelectedFlows.begin(); m_lstSelectedFlows.end() != itLoop; ++itLoop )
        {
            SequenceFlow* pElement = *itLoop;

            if ( NULL == pElement )
            {
                continue;
            }

            pElement->deselect();
        }

        m_lstSelectedActivities.clear();
    }

    for ( std::multimap<int, SequenceFlow*>::iterator itLoop = mapOrderedFlowsHit.begin(); mapOrderedFlowsHit.end() != itLoop; ++itLoop )
    {
        SequenceFlow* pElement = itLoop->second;

        if ( NULL == pElement )
        {
            continue;
        }

        pElement->select();
        updateFlowOrderZ( pElement->ID, pElement->OrderZ );

        std::list<SequenceFlow*>::iterator itFound = std::find( m_lstSelectedFlows.begin(), m_lstSelectedFlows.end(), pElement );

        if ( m_lstSelectedFlows.end() == itFound )
        {
            m_lstSelectedFlows.push_back( pElement );
        }
    }

    for ( std::multimap<int, WorkflowActivity*>::iterator itLoop = mapOrderedActivitiesHit.begin(); mapOrderedActivitiesHit.end() != itLoop; ++itLoop )
    {
        WorkflowActivity* pElement = itLoop->second;

        if ( NULL == pElement )
        {
            continue;
        }

        pElement->select();
        updateActivityOrderZ( pElement->ID, pElement->OrderZ );

        std::list<WorkflowActivity*>::iterator itFound = std::find( m_lstSelectedActivities.begin(), m_lstSelectedActivities.end(), pElement );

        if ( m_lstSelectedActivities.end() == itFound )
        {
            m_lstSelectedActivities.push_back( pElement );
        }
    }

    if ( !mapOrderedActivitiesHit.empty() || !mapOrderedFlowsHit.empty() )
    {
        // Notify viewers
        for ( std::list<IDiagramViewer*>::iterator itLoop = m_lstViewers.begin(); m_lstViewers.end() != itLoop; ++itLoop )
        {
            IDiagramViewer* pViewer = *itLoop;

            if ( NULL == pViewer )
            {
                continue;
            }

            pViewer->diagramUpdated();
        }
    }

    repositionSelectionFrame();

    return ( !mapOrderedActivitiesHit.empty() || !mapOrderedFlowsHit.empty() );
}

bool WorkflowDiagram::moveElements(const int nOffsetX, const int nOffsetY)
{
    bool bCanMove = true;

    bool bMultiMove = ((m_lstSelectedFlows.size() + m_lstSelectedActivities.size()) > 1);

    for ( std::list<WorkflowActivity*>::iterator itSelectedLoop = m_lstSelectedActivities.begin(); m_lstSelectedActivities.end() != itSelectedLoop; ++itSelectedLoop )
    {
        WorkflowElement* pSelectedElement = *itSelectedLoop;

        if (NULL == pSelectedElement)
        {
            continue;
        }

        pSelectedElement->move( bMultiMove, pSelectedElement->PositionX + nOffsetX, pSelectedElement->PositionY + nOffsetY );
    }

    for ( std::list<SequenceFlow*>::iterator itSelectedLoop = m_lstSelectedFlows.begin(); m_lstSelectedFlows.end() != itSelectedLoop; ++itSelectedLoop )
    {
        SequenceFlow* pSelectedElement = *itSelectedLoop;

        if ( NULL == pSelectedElement )
        {
            continue;
        }

        pSelectedElement->move( bMultiMove, pSelectedElement->PositionX + nOffsetX, pSelectedElement->PositionY + nOffsetY );
    }

    // Check whether activities position would be collided
    for ( std::list<WorkflowActivity*>::iterator itSelectedLoop = m_lstSelectedActivities.begin(); m_lstSelectedActivities.end() != itSelectedLoop; ++itSelectedLoop )
    {
        WorkflowActivity* pSelectedElement = *itSelectedLoop;

        if (NULL == pSelectedElement)
        {
            continue;
        }

        if ( intersect( *pSelectedElement ))
        {
            bCanMove = false;
            break;
        }
    }

    if ( m_lstSelectedActivities.empty() && ( m_lstSelectedFlows.size() == 1u ))
    {
        SequenceFlow* pSelectedElement = m_lstSelectedFlows.front();

        if ( pSelectedElement->Interactive )
        {
            int nPosX = 0;
            int nPosY = 0;
            pSelectedElement->getMovingPosition( nPosX, nPosY );
            elementInteraction( nPosX, nPosY );
        }
    }

    for ( std::list<WorkflowActivity*>::iterator itSelectedLoop = m_lstSelectedActivities.begin(); m_lstSelectedActivities.end() != itSelectedLoop; ++itSelectedLoop )
    {
        WorkflowActivity* pSelectedElement = *itSelectedLoop;

        if (NULL == pSelectedElement)
        {
            continue;
        }

        if ( !bCanMove )
        {
            pSelectedElement->move( bMultiMove, pSelectedElement->PositionX - nOffsetX, pSelectedElement->PositionY - nOffsetY );
        }
        else
        {
            std::map<unsigned int, std::set<SequenceFlow*>>::iterator itFlowFound = m_mapLinkedFlows.find( pSelectedElement->ID );

            if ( m_mapLinkedFlows.end() != itFlowFound )
            {
                for ( std::set<SequenceFlow*>::iterator itFlowLoop = itFlowFound->second.begin(); itFlowFound->second.end() != itFlowLoop; ++itFlowLoop )
                {
                    SequenceFlow* pFlow = *itFlowLoop;

                    if ( NULL == pFlow )
                    {
                        continue;
                    }

                    int nDockX = 0;
                    int nDockY = 0;
                    pSelectedElement->getDockPosition(nDockX, nDockY);
                    pFlow->dockingMoved( pSelectedElement->ID, nDockX, nDockY, pSelectedElement->FrameLeft, pSelectedElement->FrameTop, pSelectedElement->FrameRight, pSelectedElement->FrameBottom );
                }
            }
        }
    }

    repositionSelectionFrame();

    if ( bCanMove )
    {
        // Notify viewers
        for ( std::list<IDiagramViewer*>::iterator itLoop = m_lstViewers.begin(); m_lstViewers.end() != itLoop; ++itLoop )
        {
            IDiagramViewer* pViewer = *itLoop;

            if ( NULL == pViewer )
            {
                continue;
            }

            pViewer->diagramUpdated();
        }
    }
    
    if ( !Updated )
    {
        if ( NULL != m_pOwner )
        {
            m_pOwner->activityUpdated( 0u );
        }

        Updated = true;
    }

    return bCanMove;
}

void WorkflowDiagram::refresh( IDrawingContext& refDrawing )
{
    std::multimap<int, WorkflowElement*> mapOrderedElements;

    for ( std::map<unsigned int, SequenceFlow*>::iterator itLoop = m_mapFlows.begin(); m_mapFlows.end() != itLoop; ++itLoop )
    {
        SequenceFlow* pElement = itLoop->second;

        if ( NULL == pElement )
        {
            continue;
        }

        mapOrderedElements.insert( std::multimap<int, WorkflowElement*>::value_type( pElement->OrderZ, pElement ) );
    }

    for ( std::map<unsigned int, WorkflowActivity*>::iterator itLoop = m_mapActivities.begin(); m_mapActivities.end() != itLoop; ++itLoop )
    {
        WorkflowActivity* pElement = itLoop->second;

        if ( NULL == pElement )
        {
            continue;
        }

        mapOrderedElements.insert( std::multimap<int, WorkflowElement*>::value_type(pElement->OrderZ, pElement ));
    }

    for (std::multimap<int, WorkflowElement*>::reverse_iterator itLoop = mapOrderedElements.rbegin(); mapOrderedElements.rend() != itLoop; ++itLoop)
    {
        WorkflowElement* pElement = itLoop->second;

        if (NULL == pElement)
        {
            continue;
        }

        pElement->refresh( refDrawing );
    }

    m_frmSelection.draw( refDrawing );
}

void WorkflowDiagram::refreshArea( IDrawingContext& refDrawing, const int nLeft, const int nTop, const int nRight, const int nBottom )
{

}

bool WorkflowDiagram::elementSelected()
{
    return (!m_lstSelectedActivities.empty() || !m_lstSelectedFlows.empty());
}

bool WorkflowDiagram::shortcutClicked(const int nPosX, const int nPosY, std::vector<int>& vecNewElements)
{
    if ( m_lstSelectedActivities.empty() || !m_lstSelectedFlows.empty() || ( m_lstSelectedActivities.size() > 1u ))
    {
        // No shortcut displayed when no element selected or elements multi-selected or sequence flow selected
        return false;
    }

    WorkflowActivity* pElement = m_lstSelectedActivities.front();

    if (NULL == pElement)
    {
        return false;
    }

    return pElement->shortcutTriggered(nPosX, nPosY, vecNewElements);
}

bool WorkflowDiagram::buttonDown(const bool bEnableMultiple, const int nPosX, const int nPosY)
{
    bool bEventHandled = false;

    std::vector<int> vecNewElements;

    if (!shortcutClicked(nPosX, nPosY, vecNewElements))
    {
        // If nothing selected, event is not handled
        bEventHandled = !selectElement( bEnableMultiple, nPosX, nPosY );
    }
    else
    {
        createElements(vecNewElements);
    }

    return bEventHandled;
}

void WorkflowDiagram::buttonUp(const int nPosX, const int nPosY)
{
    interactionUpdate();
}

void WorkflowDiagram::createElements( const std::vector<int>& vecNewActivities )
{
    WorkflowActivity* pCurActivity = m_lstSelectedActivities.front();

    if (NULL == pCurActivity)
    {
        return;
    }

    for ( std::list<SequenceFlow*>::iterator itLoop = m_lstSelectedFlows.begin(); m_lstSelectedFlows.end() != itLoop; ++itLoop )
    {
        SequenceFlow* pElement = *itLoop;

        if ( NULL == pElement )
        {
            continue;
        }

        pElement->deselect();
    }
    m_lstSelectedFlows.clear();

    // Empty activities, create sequence flow only
    if ( vecNewActivities.empty() )
    {
        unsigned int unNewFlow = addFlow( pCurActivity->FrameRight, pCurActivity->FrameTop );

        std::map<unsigned int, SequenceFlow*>::iterator itFlow = m_mapFlows.find( unNewFlow );
        if ( m_mapFlows.end() != itFlow )
        {
            SequenceFlow* pFlow = itFlow->second;

            if ( NULL != pFlow )
            {
                for ( std::list<WorkflowActivity*>::iterator itLoop = m_lstSelectedActivities.begin(); m_lstSelectedActivities.end() != itLoop; ++itLoop )
                {
                    WorkflowActivity* pElement = *itLoop;

                    if ( NULL == pElement )
                    {
                        continue;
                    }

                    pElement->deselect();
                }
                m_lstSelectedActivities.clear();

                int nDockX = 0;
                int nDockY = 0;
                pCurActivity->getDockPosition( nDockX, nDockY );
                int eUpdated = pFlow->dockedToPosition( pCurActivity->ID, nDockX, nDockY, pCurActivity->FrameLeft, pCurActivity->FrameTop, pCurActivity->FrameRight, pCurActivity->FrameBottom );
                dockingUpdated( unNewFlow, eUpdated, pCurActivity->ID );

                m_mapLinkedFlows[pCurActivity->ID].insert( pFlow );

                pFlow->selectArrow();
                m_lstSelectedFlows.push_back( pFlow );
            }
        }
    }
    else
    {
        for ( std::vector<int>::const_iterator itLoop = vecNewActivities.begin(); vecNewActivities.end() != itLoop; ++itLoop )
        {
            unsigned int unNewFlow = addFlow( pCurActivity->FrameRight, pCurActivity->FrameTop );
            unsigned int unNewActivity = addElement( *itLoop, pCurActivity->FrameRight, pCurActivity->FrameTop );

            std::map<unsigned int, WorkflowActivity*>::iterator itActivity = m_mapActivities.find( unNewActivity );
            std::map<unsigned int, SequenceFlow*>::iterator itFlow = m_mapFlows.find( unNewFlow );
            if ( (m_mapActivities.end() == itActivity) || (m_mapFlows.end() == itFlow) )
            {
                break;
            }

            WorkflowActivity* pCreated = itActivity->second;
            SequenceFlow* pFlow = itFlow->second;

            if ( NULL != pCreated )
            {
                for ( std::list<WorkflowActivity*>::iterator itLoop = m_lstSelectedActivities.begin(); m_lstSelectedActivities.end() != itLoop; ++itLoop )
                {
                    WorkflowActivity* pElement = *itLoop;

                    if ( NULL == pElement )
                    {
                        continue;
                    }

                    pElement->deselect();
                }
                m_lstSelectedActivities.clear();

                pCreated->select();
                m_lstSelectedActivities.push_back( pCreated );
            }

            int nDockX = 0;
            int nDockY = 0;
            int eUpdated = eNoChange;

            pCurActivity->getDockPosition( nDockX, nDockY );
            eUpdated = pFlow->dockedToPosition( pCurActivity->ID, nDockX, nDockY, pCurActivity->FrameLeft, pCurActivity->FrameTop, pCurActivity->FrameRight, pCurActivity->FrameBottom );
            dockingUpdated( unNewFlow, eUpdated, pCurActivity->ID );

            pCreated->getDockPosition( nDockX, nDockY );
            eUpdated = pFlow->dockedToPosition( pCreated->ID, nDockX, nDockY, pCreated->FrameLeft, pCreated->FrameTop, pCreated->FrameRight, pCreated->FrameBottom );
            dockingUpdated( unNewFlow, eUpdated, pCurActivity->ID );

            m_mapLinkedFlows[pCurActivity->ID].insert( pFlow );
            m_mapLinkedFlows[pCreated->ID].insert( pFlow );

            pCurActivity = pCreated;
        }
    }

    repositionSelectionFrame();
}

void WorkflowDiagram::repositionSelectionFrame()
{
    int nFrameLeft = std::numeric_limits<int>::max();
    int nFrameTop = std::numeric_limits<int>::max();
    int nFrameRight = std::numeric_limits<int>::min();
    int nFrameBottom = std::numeric_limits<int>::min();

    for ( std::list<WorkflowActivity*>::iterator itLoop = m_lstSelectedActivities.begin(); m_lstSelectedActivities.end() != itLoop; ++itLoop )
    {
        WorkflowElement* pElement = *itLoop;

        if (NULL == pElement)
        {
            continue;
        }

        if (nFrameLeft > pElement->FrameLeft)
        {
            nFrameLeft = pElement->FrameLeft;
        }

        if (nFrameTop > pElement->FrameTop)
        {
            nFrameTop = pElement->FrameTop;
        }

        if (nFrameRight < pElement->FrameRight)
        {
            nFrameRight = pElement->FrameRight;
        }

        if (nFrameBottom < pElement->FrameBottom)
        {
            nFrameBottom = pElement->FrameBottom;
        }
    }

    for ( std::list<SequenceFlow*>::iterator itLoop = m_lstSelectedFlows.begin(); m_lstSelectedFlows.end() != itLoop; ++itLoop )
    {
        WorkflowElement* pElement = *itLoop;

        if ( NULL == pElement )
        {
            continue;
        }

        if ( nFrameLeft > pElement->FrameLeft )
        {
            nFrameLeft = pElement->FrameLeft;
        }

        if ( nFrameTop > pElement->FrameTop )
        {
            nFrameTop = pElement->FrameTop;
        }

        if ( nFrameRight < pElement->FrameRight )
        {
            nFrameRight = pElement->FrameRight;
        }

        if ( nFrameBottom < pElement->FrameBottom )
        {
            nFrameBottom = pElement->FrameBottom;
        }
    }

    if ( !m_lstSelectedActivities.empty() || !m_lstSelectedFlows.empty() )
    {
        m_frmSelection.Left = nFrameLeft;
        m_frmSelection.Right = nFrameRight;
        m_frmSelection.Top = nFrameTop;
        m_frmSelection.Bottom = nFrameBottom;
        m_frmSelection.Hidden = false;
    }
    else
    {
        m_frmSelection.Left = 0;
        m_frmSelection.Right = 0;
        m_frmSelection.Top = 0;
        m_frmSelection.Bottom = 0;
        m_frmSelection.Hidden = true;
    }
}

void WorkflowDiagram::interactionUpdate()
{
    if ( !m_lstSelectedActivities.empty() || ( m_lstSelectedFlows.size() != 1u ))
    {
        return;
    }

    SequenceFlow* pSelElement = m_lstSelectedFlows.front();

    if ( NULL == pSelElement )
    {
        return;
    }

    if ( !pSelElement->Interactive )
    {
        return;
    }

    int nPosX = 0;
    int nPosY = 0;
    pSelElement->getMovingPosition(nPosX, nPosY);

    bool bUpdated = false;
    WorkflowActivity* pPreDocked = NULL;
    WorkflowActivity* pNewDocked = NULL;

    for (std::map<unsigned int, WorkflowActivity*>::iterator itLoop = m_mapActivities.begin(); m_mapActivities.end() != itLoop; ++itLoop)
    {
        WorkflowActivity* pTargetElement = itLoop->second;

        if ( NULL == pTargetElement )
        {
            continue;
        }

        if ( pTargetElement->Hovered )
        {
            pTargetElement->Hovered = false;
            bUpdated = true;
        }

        bool bDockable = pTargetElement->isDockable( nPosX, nPosY );

        if (( NULL == pNewDocked ) && bDockable )
        {
            pNewDocked = pTargetElement;
        }
        else if (( NULL == pPreDocked ) && !bDockable )
        {
            if ( eNoChange != pSelElement->dockedTest( pTargetElement->ID ))
            {
                pPreDocked = pTargetElement;
            }
        }
        else if (( NULL != pNewDocked ) && ( NULL != pPreDocked ))
        {
            break;
        }
    }

    if ( NULL != pNewDocked )
    {
        int nDockX = 0;
        int nDockY = 0;
        pNewDocked->getDockPosition(nDockX, nDockY);
        int eUpdated = pSelElement->dockedToPosition( pNewDocked->ID, nDockX, nDockY, pNewDocked->FrameLeft, pNewDocked->FrameTop, pNewDocked->FrameRight, pNewDocked->FrameBottom );
            
        if ( eNoChange != eUpdated )
        {
            dockingUpdated( pSelElement->ID, eUpdated, pNewDocked->ID );
            m_mapLinkedFlows[pNewDocked->ID].insert( pSelElement );
        }

        if ( NULL != pPreDocked )
        {
            m_mapLinkedFlows[pPreDocked->ID].erase( pSelElement );
        }

        bUpdated = true;
    }
    else if ( NULL != pPreDocked )
    {
        int eUpdated = pSelElement->cancelDocking( pPreDocked->ID );

        if ( eNoChange != eUpdated )
        {
            dockingUpdated( pSelElement->ID, eUpdated, InvalidElementId );
            m_mapLinkedFlows[pPreDocked->ID].erase( pSelElement );
        }
    }

    if ( bUpdated )
    {
        repositionSelectionFrame();
    }

    if ( bUpdated )
    {
        // Notify viewers
        for (std::list<IDiagramViewer*>::iterator itLoop = m_lstViewers.begin(); m_lstViewers.end() != itLoop; ++itLoop)
        {
            IDiagramViewer* pViewer = *itLoop;

            if (NULL == pViewer)
            {
                continue;
            }

            pViewer->diagramUpdated();
        }
    }
}

bool WorkflowDiagram::intersect( WorkflowElement& refElement )
{
    bool bInterected = false;

    if ( refElement.Collision )
    {
        for (std::map<unsigned int, WorkflowActivity*>::iterator itLoop = m_mapActivities.begin(); m_mapActivities.end() != itLoop; ++itLoop)
        {
            WorkflowActivity* pTargetElement = itLoop->second;

            if (( NULL == pTargetElement ) || !pTargetElement->Collision || ( pTargetElement->ID == refElement.ID ))
            {
                continue;
            }

            if (( refElement.FrameLeft <= pTargetElement->FrameRight ) && ( refElement.FrameRight >= pTargetElement->FrameLeft ) && 
                ( refElement.FrameTop <= pTargetElement->FrameBottom ) && ( refElement.FrameBottom >= pTargetElement->FrameTop ))
            {
                bInterected = true;
                break;
            }
        }
    }

    return bInterected;
}

void WorkflowDiagram::updateOrderZ( const unsigned int unId, const int nCurOrderZ, const bool bIsFlow )
{
    for ( std::map<unsigned int, WorkflowActivity*>::iterator itLoop = m_mapActivities.begin(); m_mapActivities.end() != itLoop; ++itLoop )
    {
        WorkflowActivity* pElement = itLoop->second;

        if ( NULL == pElement )
        {
            continue;
        }

        if (( pElement->ID != unId ) || bIsFlow )
        {
            if ( pElement->OrderZ < nCurOrderZ )
            {
                pElement->OrderZ += 1;
            }
        }
        else if ( pElement->ID == unId )
        {
            pElement->OrderZ = 1;
        }
    }

    for ( std::map<unsigned int, SequenceFlow*>::iterator itLoop = m_mapFlows.begin(); m_mapFlows.end() != itLoop; ++itLoop )
    {
        SequenceFlow* pElement = itLoop->second;

        if ( NULL == pElement )
        {
            continue;
        }

        if (( pElement->ID != unId ) || !bIsFlow )
        {
            if ( pElement->OrderZ < nCurOrderZ )
            {
                pElement->OrderZ += 1;
            }
        }
        else if ( pElement->ID == unId )
        {
            pElement->OrderZ = 1;
        }
    }
}

void WorkflowDiagram::updateActivityOrderZ(const unsigned int unId, const int nCurOrderZ)
{
    updateOrderZ( unId, nCurOrderZ, false );
    return;
    for (std::map<unsigned int, WorkflowActivity*>::iterator itLoop = m_mapActivities.begin(); m_mapActivities.end() != itLoop; ++itLoop)
    {
        WorkflowActivity* pElement = itLoop->second;

        if (NULL == pElement)
        {
            continue;
        }

        if (pElement->ID != unId)
        {
            if ( pElement->OrderZ < nCurOrderZ )
            {
                pElement->OrderZ += 1;
            }
        }
        else
        {
            pElement->OrderZ = 1;
        }
    }
}

void WorkflowDiagram::updateFlowOrderZ( const unsigned int unId, const int nCurOrderZ )
{
    updateOrderZ( unId, nCurOrderZ, true );
    return;
    for ( std::map<unsigned int, SequenceFlow*>::iterator itLoop = m_mapFlows.begin(); m_mapFlows.end() != itLoop; ++itLoop )
    {
        SequenceFlow* pElement = itLoop->second;

        if ( NULL == pElement )
        {
            continue;
        }

        if ( pElement->ID != unId )
        {
            if ( pElement->OrderZ < nCurOrderZ )
            {
                pElement->OrderZ += 1;
            }
        }
        else
        {
            pElement->OrderZ = 1;
        }
    }
}

void WorkflowDiagram::elementInteraction( const int nPosX, const int nPosY )
{
    int nCurOrder = std::numeric_limits<int>::max();

    for (std::map<unsigned int, WorkflowActivity*>::iterator itLoop = m_mapActivities.begin(); m_mapActivities.end() != itLoop; ++itLoop)
    {
        WorkflowActivity* pElement = itLoop->second;

        if (NULL == pElement)
        {
            continue;
        }

        if ( pElement->isHover( nPosX, nPosY ) && pElement->OrderZ < nCurOrder )
        {
            pElement->Hovered = true;
            nCurOrder = pElement->OrderZ;
        }
        else
        {
            pElement->Hovered = false;
        }
    }
}

void WorkflowDiagram::save( std::map<unsigned int, ActivityData>& mapTypeToData, std::map<unsigned int, SequenceFlowData>& mapSequenceFlows )
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
}

void WorkflowDiagram::load( const std::map<unsigned int, ActivityData>& mapTypeToData, const std::map<unsigned int, SequenceFlowData>& mapSequenceFlows )
{
    clearDiagram();

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
    
    // Notify viewers
    for (std::list<IDiagramViewer*>::iterator itLoop = m_lstViewers.begin(); m_lstViewers.end() != itLoop; ++itLoop)
    {
        IDiagramViewer* pViewer = *itLoop;

        if (NULL == pViewer)
        {
            continue;
        }

        pViewer->diagramUpdated();
    }
}

void WorkflowDiagram::deleteElements()
{
    if ( m_lstSelectedActivities.empty() && m_lstSelectedFlows.empty() )
    {
        return;
    }

    std::list<unsigned int> lstToDelete;

    for ( std::list<WorkflowActivity*>::iterator itSelectedLoop = m_lstSelectedActivities.begin(); m_lstSelectedActivities.end() != itSelectedLoop; ++itSelectedLoop )
    {
        WorkflowActivity* pSelectedElement = *itSelectedLoop;

        if ( NULL == pSelectedElement )
        {
            continue;
        }

        std::list<unsigned int>::iterator itClipFound = std::find( m_lstActivityIdsInClipboard.begin(), m_lstActivityIdsInClipboard.end(), pSelectedElement->ID );
        if ( m_lstActivityIdsInClipboard.end() != itClipFound )
        {
            m_lstActivityIdsInClipboard.erase( itClipFound );
        }

        deleteActivity( pSelectedElement->ID );
    }

    m_lstSelectedActivities.clear();

    for ( std::list<SequenceFlow*>::iterator itSelectedLoop = m_lstSelectedFlows.begin(); m_lstSelectedFlows.end() != itSelectedLoop; ++itSelectedLoop )
    {
        SequenceFlow* pSelectedElement = *itSelectedLoop;

        if ( NULL == pSelectedElement )
        {
            continue;
        }

        std::list<unsigned int>::iterator itClipFound = std::find( m_lstFlowIdsInClipboard.begin(), m_lstFlowIdsInClipboard.end(), pSelectedElement->ID );
        if ( m_lstFlowIdsInClipboard.end() != itClipFound )
        {
            m_lstFlowIdsInClipboard.erase( itClipFound );
        }

        deleteFlow( pSelectedElement->ID );
    }

    m_lstSelectedFlows.clear();

    m_frmSelection.Hidden = true;

    // Notify viewers
    for ( std::list<IDiagramViewer*>::iterator itLoop = m_lstViewers.begin(); m_lstViewers.end() != itLoop; ++itLoop )
    {
        IDiagramViewer* pViewer = *itLoop;

        if ( NULL == pViewer )
        {
            continue;
        }

        pViewer->diagramUpdated();
    }
}

void WorkflowDiagram::copy()
{
    if ( m_lstSelectedActivities.empty() )
    {
        return;
    }

    m_lstActivityIdsInClipboard.clear();

    for ( std::list<WorkflowActivity*>::iterator itLoop = m_lstSelectedActivities.begin(); m_lstSelectedActivities.end() != itLoop; ++itLoop )
    {
        WorkflowActivity* pElement = *itLoop;

        if ( NULL == pElement )
        {
            continue;
        }

        m_lstActivityIdsInClipboard.push_back( pElement->ID );
    }

    m_lstFlowIdsInClipboard.clear();

    for ( std::list<SequenceFlow*>::iterator itLoop = m_lstSelectedFlows.begin(); m_lstSelectedFlows.end() != itLoop; ++itLoop )
    {
        SequenceFlow* pElement = *itLoop;

        if ( NULL == pElement )
        {
            continue;
        }

        m_lstFlowIdsInClipboard.push_back( pElement->ID );
    }
}

void WorkflowDiagram::cut()
{
    copy();
}

void WorkflowDiagram::paste( const int nPosX, const int nPosY )
{
    // Pasting would refresh selection with new pasted activities and sequence flows
    for ( std::list<WorkflowActivity*>::iterator itLoop = m_lstSelectedActivities.begin(); m_lstSelectedActivities.end() != itLoop; ++itLoop )
    {
        WorkflowActivity* pElement = *itLoop;

        if ( NULL == pElement )
        {
            continue;
        }

        pElement->deselect();
    }
    m_lstSelectedActivities.clear();

    // Pasting would refresh selection with new pasted activities and sequence flows
    for ( std::list<SequenceFlow*>::iterator itLoop = m_lstSelectedFlows.begin(); m_lstSelectedFlows.end() != itLoop; ++itLoop )
    {
        SequenceFlow* pElement = *itLoop;

        if ( NULL == pElement )
        {
            continue;
        }

        pElement->deselect();
    }
    m_lstSelectedFlows.clear();

    // Initialize frame with maximum numbers
    int nFrameLeft = std::numeric_limits<int>::max();
    int nFrameTop = std::numeric_limits<int>::max();
    int nFrameRight = std::numeric_limits<int>::min();
    int nFrameBottom = std::numeric_limits<int>::min();

    std::set<unsigned int> setActivitySelected;
    std::set<unsigned int> setFlowSelected;

    // Create an activity selection pool for faster index
    for ( std::list<unsigned int>::iterator itLoop = m_lstActivityIdsInClipboard.begin(); m_lstActivityIdsInClipboard.end() != itLoop; ++itLoop )
    {
        std::map<unsigned int, WorkflowActivity*>::iterator itActivityFound = m_mapActivities.find( *itLoop );

        if ( m_mapActivities.end() == itActivityFound )
        {
            continue;
        }

        WorkflowActivity* pElement = itActivityFound->second;

        if ( NULL == pElement )
        {
            continue;
        }

        if ( nFrameLeft > pElement->FrameLeft )
        {
            nFrameLeft = pElement->FrameLeft;
        }

        if ( nFrameTop > pElement->FrameTop )
        {
            nFrameTop = pElement->FrameTop;
        }

        if ( nFrameRight < pElement->FrameRight )
        {
            nFrameRight = pElement->FrameRight;
        }

        if ( nFrameBottom < pElement->FrameBottom )
        {
            nFrameBottom = pElement->FrameBottom;
        }

        setActivitySelected.insert( *itLoop );
    }

    // Create a sequence flow selection pool for faster index
    for ( std::list<unsigned int>::iterator itLoop = m_lstFlowIdsInClipboard.begin(); m_lstFlowIdsInClipboard.end() != itLoop; ++itLoop )
    {
        std::map<unsigned int, SequenceFlow*>::iterator itFlowFound = m_mapFlows.find( *itLoop );

        if ( m_mapFlows.end() == itFlowFound )
        {
            continue;
        }

        SequenceFlow* pElement = itFlowFound->second;

        if ( NULL == pElement )
        {
            continue;
        }

        if ( nFrameLeft > pElement->FrameLeft )
        {
            nFrameLeft = pElement->FrameLeft;
        }

        if ( nFrameTop > pElement->FrameTop )
        {
            nFrameTop = pElement->FrameTop;
        }

        if ( nFrameRight < pElement->FrameRight )
        {
            nFrameRight = pElement->FrameRight;
        }

        if ( nFrameBottom < pElement->FrameBottom )
        {
            nFrameBottom = pElement->FrameBottom;
        }

        setFlowSelected.insert( *itLoop );
    }

    int nOffsetX = nPosX - ( nFrameLeft + nFrameRight ) / 2;
    int nOffsetY = nPosY - ( nFrameTop + nFrameBottom ) / 2;

    nOffsetX = 30;
    nOffsetY = 30;

    std::map<unsigned int, unsigned int> mapActivityCopyedToNew;

    for ( std::list<unsigned int>::iterator itLoop = m_lstActivityIdsInClipboard.begin(); m_lstActivityIdsInClipboard.end() != itLoop; ++itLoop )
    {
        std::map<unsigned int, WorkflowActivity*>::iterator itFound = m_mapActivities.find( *itLoop );

        if ( m_mapActivities.end() == itFound )
        {
            continue;
        }

        WorkflowActivity* pElement = itFound->second;

        if ( NULL == pElement )
        {
            continue;
        }

        unsigned int unNewId = addElement( pElement->Type, pElement->PositionX + nOffsetX, pElement->PositionY + nOffsetY );

        std::map<unsigned int, WorkflowActivity*>::iterator itNew = m_mapActivities.find( unNewId );

        if ( m_mapActivities.end() != itNew )
        {
            int nOldOrder = itNew->second->OrderZ;
            itNew->second->deserialize( pElement->serialize() );
            itNew->second->offset( nOffsetX, nOffsetY );
            itNew->second->ID = unNewId;
            itNew->second->OrderZ = nOldOrder;
            itNew->second->InstanceName = pElement->InstanceName.c_str();
            mapActivityCopyedToNew.insert( std::map<unsigned int, unsigned int>::value_type( pElement->ID, unNewId ));

            itNew->second->select();
            m_lstSelectedActivities.push_back( itNew->second );
            updateActivityOrderZ( unNewId, nOldOrder );
        }

        if (NULL != m_pOwner)
        {
            m_pOwner->activityCopied( pElement->ID, unNewId );
        }
    }

    std::map<unsigned int, unsigned int> mapFlowCopyedToNew;

    for ( std::list<unsigned int>::iterator itLoop = m_lstFlowIdsInClipboard.begin(); m_lstFlowIdsInClipboard.end() != itLoop; ++itLoop )
    {
        std::map<unsigned int, SequenceFlow*>::iterator itFound = m_mapFlows.find( *itLoop );

        if ( m_mapFlows.end() == itFound )
        {
            continue;
        }

        SequenceFlow* pElement = itFound->second;

        if ( NULL == pElement )
        {
            continue;
        }

        unsigned int unNewId = addFlow( pElement->PositionX + nOffsetX, pElement->PositionY + nOffsetY );

        std::map<unsigned int, SequenceFlow*>::iterator itNew = m_mapFlows.find( unNewId );

        if ( m_mapFlows.end() != itNew )
        {
            int nOldOrder = itNew->second->OrderZ;
            itNew->second->deserialize( pElement->serialize() );
            itNew->second->offset( nOffsetX, nOffsetY );
            itNew->second->ID = unNewId;
            itNew->second->OrderZ = nOldOrder;
            itNew->second->IncomingId = InvalidElementId;
            itNew->second->OutgoingId = InvalidElementId;
            mapFlowCopyedToNew.insert( std::map<unsigned int, unsigned int>::value_type( pElement->ID, unNewId ) );

            itNew->second->select();
            m_lstSelectedFlows.push_back( itNew->second );
            updateFlowOrderZ( unNewId, nOldOrder );
        }
    }

    for ( std::set<unsigned int>::iterator itLoop = setActivitySelected.begin(); setActivitySelected.end() != itLoop; ++itLoop )
    {
        std::map<unsigned int, std::set<SequenceFlow*>>::iterator itFound = m_mapLinkedFlows.find( *itLoop );

        if ( m_mapLinkedFlows.end() == itFound )
        {
            continue;
        }

        for ( std::set<SequenceFlow*>::iterator itDocking = itFound->second.begin(); itFound->second.end() != itDocking; ++itDocking )
        {
            SequenceFlow* pElement = *itDocking;

            if ( NULL == pElement )
            {
                continue;
            }

            if ( setFlowSelected.end() == setFlowSelected.find( pElement->ID ) )
            {
                continue;
            }

            std::map<unsigned int, unsigned int>::iterator itMapedActivityNewId = mapActivityCopyedToNew.find( *itLoop );

            if ( mapActivityCopyedToNew.end() == itMapedActivityNewId )
            {
                continue;
            }

            std::map<unsigned int, unsigned int>::iterator itMapedFlowNewId = mapFlowCopyedToNew.find( pElement->ID );

            if ( mapFlowCopyedToNew.end() == itMapedFlowNewId )
            {
                continue;
            }

            std::map<unsigned int, WorkflowActivity*>::iterator itMappedActivityNew = m_mapActivities.find( itMapedActivityNewId->second );

            if ( m_mapActivities.end() == itMappedActivityNew )
            {
                continue;
            }

            std::map<unsigned int, SequenceFlow*>::iterator itMappedFlowNew = m_mapFlows.find( itMapedFlowNewId->second );

            if ( m_mapFlows.end() == itMappedFlowNew )
            {
                continue;
            }

            if ( pElement->IncomingId == *itLoop )
            {
                itMappedFlowNew->second->updateIncoming( itMappedActivityNew->second->ID, itMappedActivityNew->second->PositionX, itMappedActivityNew->second->PositionY, itMappedActivityNew->second->FrameLeft,
                    itMappedActivityNew->second->FrameTop, itMappedActivityNew->second->FrameRight, itMappedActivityNew->second->FrameBottom );

                if ( NULL != m_pOwner )
                {
                    m_pOwner->linkFlowFrom( itMappedFlowNew->second->ID, itMappedActivityNew->second->ID );
                }
            }
            else if ( pElement->OutgoingId == *itLoop )
            {
                itMappedFlowNew->second->updateOutgoing( itMappedActivityNew->second->ID, itMappedActivityNew->second->PositionX, itMappedActivityNew->second->PositionY, itMappedActivityNew->second->FrameLeft,
                    itMappedActivityNew->second->FrameTop, itMappedActivityNew->second->FrameRight, itMappedActivityNew->second->FrameBottom );

                if ( NULL != m_pOwner )
                {
                    m_pOwner->linkFlowTo( itMappedFlowNew->second->ID, itMappedActivityNew->second->ID );
                }
            }

            m_mapLinkedFlows[itMappedActivityNew->second->ID].insert( itMappedFlowNew->second );
        }
    }

    repositionSelectionFrame();
}

void WorkflowDiagram::size( int& nWidth, int& nHeight )
{
    nWidth = 1;
    nHeight = 1;

    int nActivityTop = 0;
    int nActivityLeft = 0;
    int nActivityRight = 0;
    int nActivityBottom = 0;

    for ( std::map<unsigned int, WorkflowActivity*>::iterator itLoop = m_mapActivities.begin(); m_mapActivities.end() != itLoop; ++itLoop )
    {
        itLoop->second->getValidRectangle( nActivityLeft, nActivityTop, nActivityRight, nActivityBottom );

        if ( nWidth < nActivityRight )
        {
            nWidth = nActivityRight;
        }

        if ( nHeight < nActivityBottom )
        {
            nHeight = nActivityBottom;
        }
    }

    for ( std::map<unsigned int, SequenceFlow*>::iterator itLoop = m_mapFlows.begin(); m_mapFlows.end() != itLoop; ++itLoop )
    {
        if ( nWidth < itLoop->second->FrameRight )
        {
            nWidth = itLoop->second->FrameRight;
        }

        if ( nHeight < itLoop->second->FrameBottom )
        {
            nHeight = itLoop->second->FrameBottom;
        }
    }
}

IActivityParameter& WorkflowDiagram::getActivityParameter( const unsigned int unId )
{
    if ( NULL == m_pOwner )
    {
        throw WorkflowException( "Work flow owner has not been attached yet" );
    }

    return m_pOwner->getActivityParameter( unId );
}

void WorkflowDiagram::setActivityStatus( const unsigned int unId, const int eNewStatus )
{
    std::map<unsigned int, WorkflowActivity*>::iterator itActivityFound = m_mapActivities.find( unId );

    if ( itActivityFound == m_mapActivities.end() )
    {
        throw WorkflowException("Invalid activity ID");
    }

    itActivityFound->second->status( eNewStatus );

    // Notify viewers
    for ( std::list<IDiagramViewer*>::iterator itLoop = m_lstViewers.begin(); m_lstViewers.end() != itLoop; ++itLoop )
    {
        IDiagramViewer* pViewer = *itLoop;

        if ( NULL == pViewer )
        {
            continue;
        }

        pViewer->elementUpdated( unId );
    }
}

void WorkflowDiagram::setActivityName(const unsigned int unId, const std::string& strName)
{
    std::map<unsigned int, WorkflowActivity*>::iterator itActivityFound = m_mapActivities.find( unId );

    if ( itActivityFound == m_mapActivities.end() )
    {
        throw WorkflowException("Invalid activity ID");
    }

    itActivityFound->second->InstanceName = strName.c_str();

    // Notify viewers
    for ( std::list<IDiagramViewer*>::iterator itLoop = m_lstViewers.begin(); m_lstViewers.end() != itLoop; ++itLoop )
    {
        IDiagramViewer* pViewer = *itLoop;

        if ( NULL == pViewer )
        {
            continue;
        }

        pViewer->elementUpdated( unId );
    }
}

void WorkflowDiagram::updateActivitiesStatus( const std::map<unsigned int, int>& mapStepIdToState )
{
    for ( std::map<unsigned int, WorkflowActivity*>::iterator itLoop = m_mapActivities.begin(); m_mapActivities.end() != itLoop; ++itLoop )
    {
        std::map<unsigned int, int>::const_iterator itActivityFound = mapStepIdToState.find( itLoop->first );

        if ( itActivityFound == mapStepIdToState.end() )
        {
            itLoop->second->status( eUnknown );
        }
        else
        {
            itLoop->second->status( itActivityFound->second );
        }
    }

    // Notify viewers
    for ( std::list<IDiagramViewer*>::iterator itLoop = m_lstViewers.begin(); m_lstViewers.end() != itLoop; ++itLoop )
    {
        IDiagramViewer* pViewer = *itLoop;

        if ( NULL == pViewer )
        {
            continue;
        }

        pViewer->diagramUpdated();
    }
}

void WorkflowDiagram::dockingUpdated( const unsigned int unId, const int eType, const unsigned int unActivity )
{
    if ( eNoChange == eType )
    {
        return;
    }

    if ( NULL == m_pOwner )
    {
        return;
    }

    if ( eOutgoingUpdated == eType )
    {
        m_pOwner->linkFlowTo( unId, unActivity );
    }
    else if ( eIncomingUpdated == eType )
    {
        m_pOwner->linkFlowFrom( unId, unActivity );
    }
}

void WorkflowDiagram::attachOwner( IWorkflowOwner& refViewer )
{
    m_pOwner = &refViewer;
}

void WorkflowDiagram::detachOwner( IWorkflowOwner& refViewer )
{
    if ( &refViewer == m_pOwner )
    {
        m_pOwner = NULL;
    }
}

void WorkflowDiagram::clearDiagram()
{
    m_lstSelectedActivities.clear();
    m_lstSelectedFlows.clear();
    m_lstActivityIdsInClipboard.clear();
    m_lstFlowIdsInClipboard.clear();

    m_frmSelection.Left = 0;
    m_frmSelection.Right = 0;
    m_frmSelection.Top = 0;
    m_frmSelection.Bottom = 0;
    m_frmSelection.Hidden = true;

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

void WorkflowDiagram::activityUpdated(const unsigned int unId)
{
    if ( NULL != m_pOwner )
    {
        m_pOwner->activityUpdated( unId );
    }
}

bool WorkflowDiagram::isActivityClipboardEmpty()
{
	return m_lstActivityIdsInClipboard.empty();
}

bool WorkflowDiagram::isActivitySelected()
{
	return !m_lstSelectedActivities.empty();
}

bool WorkflowDiagram::isFlowSelected()
{
	return !m_lstSelectedFlows.empty();
}

bool WorkflowDiagram::isFlowClipboardEmpty()
{
	return m_lstFlowIdsInClipboard.empty();
}

bool WorkflowDiagram::isUnEditableAcitivitySelected()
{
    for ( std::list<WorkflowActivity*>::iterator itLoop = m_lstSelectedActivities.begin(); m_lstSelectedActivities.end() != itLoop; ++itLoop )
    {
        WorkflowActivity* pElement = *itLoop;

        if ( NULL == pElement )
        {
            continue;
        }
		if(!pElement->Editable)
		{
			return true;
		}
    }
	return false;
}