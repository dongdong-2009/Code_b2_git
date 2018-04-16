// WorkflowEditor.h : main header file for the PROJECT_NAME application
//

#pragma once

#include <string>
#include <vector>
#include <map>

#include "WorkflowEditorForm.h"
#include "WorkflowDiagram.h"

class IActivityComponent;
class IWorkflowOwner;

class AFX_EXT_CLASS WorkflowEditor
{
public:
    WorkflowEditor();
    ~WorkflowEditor();

    WorkflowEditorForm& getWorkflowForm();
    void resetModified();
    void setReadOnly( const bool bReadOnly );
    void attachWorkflowOwner( IWorkflowOwner& refOwner );
    void setActivityStatus( const unsigned int unId, const int eNewStatus );
    void updateActivitiesStatus( const std::map<unsigned int, int>& mapStepIdToState );
    void save( std::map<unsigned int, ActivityData>& mapTypeToData, std::map<unsigned int, SequenceFlowData>& mapSequenceFlows );
    void load( const std::map<unsigned int, ActivityData>& mapTypeToData, const std::map<unsigned int, SequenceFlowData>& mapSequenceFlows );
	void onCmd( const int cmdType );
	bool isCmdEnable( const int cmdType );
	void autoPositions(const std::map<unsigned int, unsigned int>& mapPositionToStepId);

    static void plugActivities( const std::string& strCategory, const std::vector<IActivityComponent*>& vecActivityComponent );

private:
    void init();
    void uninit();

private:
    WorkflowDiagram m_digWorkflow;
    WorkflowEditorForm m_dlgWorkflowForm;
};
