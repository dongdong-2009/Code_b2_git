#pragma once

#include "WorkflowEditorDlg.h"

struct ActivityItem;
class WorkflowDiagram;

class AFX_EXT_CLASS WorkflowEditorForm
{
public:
    WorkflowEditorForm( WorkflowDiagram& refDiagram );
    ~WorkflowEditorForm();

    void attachParent( void* pParent );
    void detachParent();
    void moveInParent( const int nLeft, const int nTop, const int nWidth, const int nHeight );
    void setReadOnly( const bool bReadOnly );
    void refresh();
    void onCmd( const int cmdType );

private:
    void init();
    void uninit();
    void plugActivities( const std::string& strCategory, const std::vector<ActivityItem>& vecActivities );

private:
    CWorkflowEditorDlg* m_pWorkflowDialog;
};

