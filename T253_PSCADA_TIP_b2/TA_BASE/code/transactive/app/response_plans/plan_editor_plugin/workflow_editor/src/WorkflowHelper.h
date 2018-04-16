#pragma once

#include <set>
#include <map>

#include "WorkflowStructure.h"

class WorkflowElement;
class WorkflowActivity;
class SequenceFlow;

class AFX_EXT_CLASS WorkflowHelper
{
public:
    ~WorkflowHelper();
    static WorkflowHelper& instance();

private:
    WorkflowHelper();

public:
	void autoPositions( const std::map<unsigned int, unsigned int>& mapPositionToStepId, std::map<unsigned int, ActivityData>& mapTypeToData, std::map<unsigned int, SequenceFlowData>& mapSequenceFlows );

private:
    void save( std::map<unsigned int, ActivityData>& mapTypeToData, std::map<unsigned int, SequenceFlowData>& mapSequenceFlows );
    void load( const std::map<unsigned int, ActivityData>& mapTypeToData, const std::map<unsigned int, SequenceFlowData>& mapSequenceFlows );
	void position( const std::map<unsigned int, unsigned int>& mapPositionToStepId );
    void clear();

private:
    static WorkflowHelper* s_pInstance;
    std::map<unsigned int, WorkflowActivity*> m_mapActivities;
    std::map<unsigned int, SequenceFlow*> m_mapFlows;
    std::map<unsigned int, std::set<SequenceFlow*>> m_mapLinkedFlows;
};
