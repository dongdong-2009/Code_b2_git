#pragma once

#include <set>
#include <list>
#include <vector>
#include <map>

#include "SelectionFrame.h"
#include "WorkflowStructure.h"

class IWorkflowOwner;
class IActivityParameter;
class IDiagramViewer;
class WorkflowElement;
class WorkflowActivity;
class SequenceFlow;

class WorkflowDiagram
{
public:
    WorkflowDiagram();
    virtual ~WorkflowDiagram();

public:
    void attachViewer( IDiagramViewer& refViewer );
    void detachViewer( IDiagramViewer& refViewer );

    void attachOwner( IWorkflowOwner& refViewer );
    void detachOwner( IWorkflowOwner& refViewer );

    bool selectArea( const bool bEnableMultiple, const int nLeft, const int nTop, const int nRight, const int nBottom );
    bool buttonDown( const bool bEnableMultiple, const int nPosX, const int nPosY );
    void buttonUp( const int nPosX, const int nPosY );
    void refresh( IDrawingContext& refDrawing );
    void refreshArea( IDrawingContext& refDrawing, const int nLeft, const int nTop, const int nRight, const int nBottom );

    unsigned int addElement( const int nTypeId, const int nPosX, const int nPosY );
    unsigned int addFlow( const int nPosX, const int nPosY );
    void deleteActivity( const unsigned int unId );
    void deleteFlow( const unsigned int unId );

    void deleteElements();
    bool moveElements(const int nOffsetX, const int nOffsetY);
    bool elementSelected();
    void save( std::map<unsigned int, ActivityData>& mapTypeToData, std::map<unsigned int, SequenceFlowData>& mapSequenceFlows );
    void load( const std::map<unsigned int, ActivityData>& mapTypeToData, const std::map<unsigned int, SequenceFlowData>& mapSequenceFlows );

    void copy();
    void cut();
    void paste( const int nPosX, const int nPosY );

    void size( int& nWidth, int& nHeight );

    IActivityParameter& getActivityParameter( const unsigned int unId );
    void setActivityStatus( const unsigned int unId, const int eNewStatus );
    void setActivityName( const unsigned int unId, const std::string& strName );
    void activityUpdated( const unsigned int unId );
    void updateActivitiesStatus( const std::map<unsigned int, int>& mapStepIdToState );
    void clearDiagram();
	bool isActivitySelected();
	bool isFlowSelected();
	bool isActivityClipboardEmpty();
	bool isFlowClipboardEmpty();
	bool isUnEditableAcitivitySelected();

private:
    bool shortcutClicked( const int nPosX, const int nPosY, std::vector<int>& vecNewElements );
    bool selectElement( const bool bEnableMultiple, const int nPosX, const int nPosY );
    bool intersect( WorkflowElement& refElement );
    void elementInteraction(const int nPosX, const int nPosY);
    void createElements( const std::vector<int>& vecNewActivities );
	void repositionSelectionFrame();
    void interactionUpdate();
    void updateActivityOrderZ( const unsigned int unId, const int nCurOrderZ );
    void updateFlowOrderZ( const unsigned int unId, const int nCurOrderZ );
    void updateOrderZ( const unsigned int unId, const int nCurOrderZ, const bool bIsFlow );
    void dockingUpdated( const unsigned int unId, const int eType, const unsigned int unActivity );

public:
    int Width;
    int Height;
    bool ReadOnly;
    bool Updated;

private:
    IWorkflowOwner* m_pOwner;
    std::list<IDiagramViewer*> m_lstViewers;
    std::list<WorkflowActivity*> m_lstSelectedActivities;
    std::list<SequenceFlow*> m_lstSelectedFlows;
    std::map<unsigned int, WorkflowActivity*> m_mapActivities;
    std::map<unsigned int, SequenceFlow*> m_mapFlows;
    std::map<unsigned int, std::set<SequenceFlow*>> m_mapLinkedFlows;
    std::list<unsigned int> m_lstActivityIdsInClipboard;
    std::list<unsigned int> m_lstFlowIdsInClipboard;
	SelectionFrame m_frmSelection;
};
