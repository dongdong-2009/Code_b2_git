// WorkflowEditor.cpp : Defines the class behaviors for the application.
//

#include "WorkflowEditor.h"
#include "WorkflowCmd.h"
#include "ActivityLibrary.h"

// CWorkflowEditorApp construction

WorkflowEditor::WorkflowEditor() :
m_digWorkflow(),
m_dlgWorkflowForm( m_digWorkflow )
{
    init();
}

WorkflowEditor::~WorkflowEditor()
{
    uninit();
}

WorkflowEditorForm& WorkflowEditor::getWorkflowForm()
{
    return m_dlgWorkflowForm;
}

void WorkflowEditor::init()
{
}

void WorkflowEditor::uninit()
{
}

void WorkflowEditor::plugActivities( const std::string& strCategory, const std::vector<IActivityComponent*>& vecActivityComponent )
{
    ActivityLibrary::instance().plugActivities( strCategory, vecActivityComponent );
}

void WorkflowEditor::attachWorkflowOwner( IWorkflowOwner& refOwner )
{
    m_digWorkflow.attachOwner( refOwner );
}

void WorkflowEditor::setActivityStatus( const unsigned int unId, const int eNewStatus )
{
    m_digWorkflow.setActivityStatus( unId, eNewStatus );
}

void WorkflowEditor::updateActivitiesStatus( const std::map<unsigned int, int>& mapStepIdToState )
{
    m_digWorkflow.updateActivitiesStatus( mapStepIdToState );
}

void WorkflowEditor::setReadOnly( const bool bReadOnly )
{
    m_digWorkflow.ReadOnly = bReadOnly;

    m_dlgWorkflowForm.setReadOnly( bReadOnly );
}

void WorkflowEditor::save( std::map<unsigned int, ActivityData>& mapTypeToData, std::map<unsigned int, SequenceFlowData>& mapSequenceFlows )
{
    m_digWorkflow.save( mapTypeToData, mapSequenceFlows );
}

void WorkflowEditor::load( const std::map<unsigned int, ActivityData>& mapTypeToData, const std::map<unsigned int, SequenceFlowData>& mapSequenceFlows )
{
    m_digWorkflow.load( mapTypeToData, mapSequenceFlows );

    m_dlgWorkflowForm.refresh();
}

void WorkflowEditor::resetModified()
{
    m_digWorkflow.Updated = false;
}

void WorkflowEditor::onCmd( const int cmdType )
{
	m_dlgWorkflowForm.onCmd(cmdType);
}

bool WorkflowEditor::isCmdEnable( const int cmdType )
{
	switch (cmdType)
	{
		case eCmdZoomIn:
		case eCmdZoomOut:
		case eCmdZoomActual:
		case eCmdFitToWin:
			return true;
		case eCmdDelete:
			return (m_digWorkflow.isActivitySelected() || m_digWorkflow.isFlowSelected()) && !m_digWorkflow.isUnEditableAcitivitySelected();
		case eCmdCopy:
			return m_digWorkflow.isActivitySelected() && !m_digWorkflow.isUnEditableAcitivitySelected();
		case eCmdPaste:
			return !m_digWorkflow.isFlowClipboardEmpty() || !m_digWorkflow.isActivityClipboardEmpty();
		default:
			return false;
	}
}