
#include "GlobalStructure.h"
#include "ActivityLibrary.h"
#include "WorkflowDiagram.h"
#include "WorkflowEditorForm.h"
#include "WorkflowCmd.h"
#include "app/response_plans/plan_editor_plugin/workflow_drawing/src/ResourcePool.h"

WorkflowEditorForm::WorkflowEditorForm( WorkflowDiagram& refDiagram ) :
m_pWorkflowDialog( new CWorkflowEditorDlg( refDiagram, NULL ))
{
    init();
}

WorkflowEditorForm::~WorkflowEditorForm()
{
    uninit();
}

void WorkflowEditorForm::init()
{
}

void WorkflowEditorForm::uninit()
{
    if ( NULL != m_pWorkflowDialog )
    {
        delete m_pWorkflowDialog;
        m_pWorkflowDialog = NULL;
    }
}

void WorkflowEditorForm::attachParent( void* pParent )
{
    CWnd* pParentWnd = reinterpret_cast<CWnd*>(pParent);

    if ( NULL == pParentWnd )
    {
        return;
    }

    if ( NULL != m_pWorkflowDialog )
    {
        if ( !::IsWindow( m_pWorkflowDialog->GetSafeHwnd() ))
        {
            m_pWorkflowDialog->create( *pParentWnd );
        }

        m_pWorkflowDialog->SetParent( pParentWnd );
        m_pWorkflowDialog->ShowWindow( SW_SHOW );

        CRect rtClient;
        pParentWnd->GetClientRect( &rtClient );
        m_pWorkflowDialog->SetWindowPos( NULL, 0, 0, rtClient.Width(), rtClient.Height(), SWP_NOZORDER );
    }

    plugActivities( ActivityLibrary::instance().getCategoryName(), ActivityLibrary::instance().getActivityItems() );
}

void WorkflowEditorForm::detachParent()
{
    if ( NULL != m_pWorkflowDialog )
    {
        if ( ::IsWindow( m_pWorkflowDialog->GetSafeHwnd() ))
        {
            m_pWorkflowDialog->DestroyWindow();
        }
    }
}

void WorkflowEditorForm::moveInParent( const int nLeft, const int nTop, const int nWidth, const int nHeight )
{
    if (( NULL == m_pWorkflowDialog ) || !::IsWindow( m_pWorkflowDialog->GetSafeHwnd() ))
    {
        return;
    }

    m_pWorkflowDialog->MoveWindow( nLeft, nTop, nWidth, nHeight, TRUE );
}

void WorkflowEditorForm::plugActivities( const std::string& strCategory, const ActivityItems& vecActivities )
{
    if ( NULL == m_pWorkflowDialog )
    {
        return;
    }

    HTREEITEM hCategory = m_pWorkflowDialog->getActivityTree().InsertItem( strCategory.c_str(), 0, 0, TVI_ROOT );

    for ( ActivityItems::const_iterator itItem = vecActivities.begin(); vecActivities.end() != itItem; ++itItem )
    {
        if ( !itItem->Visable )
        {
            continue;
        }

        HTREEITEM hActivity = m_pWorkflowDialog->getActivityTree().InsertItem( itItem->Name.c_str(), hCategory );
        m_pWorkflowDialog->getActivityTree().SetItemData( hActivity, static_cast<DWORD>(itItem->ID) );
        HGDIOBJ hRes = ResourcePool::instance().loadBitmap( itItem->Resource ).GetSafeHandle();
        m_pWorkflowDialog->getActivityTree().SetItemImage( hActivity, reinterpret_cast<int>(hRes), 0 );
    }

    m_pWorkflowDialog->getActivityTree().Expand( hCategory, TVE_EXPAND );
}

void WorkflowEditorForm::setReadOnly( const bool bReadOnly )
{
    if ( NULL == m_pWorkflowDialog )
    {
        return;
    }

    m_pWorkflowDialog->setReadOnly( bReadOnly );
}

void WorkflowEditorForm::refresh()
{
    if (NULL == m_pWorkflowDialog)
    {
        return;
    }

    m_pWorkflowDialog->refresh();
}

void WorkflowEditorForm::onCmd( const int cmdType )
{
	switch ( cmdType )
	{
	case eCmdDelete:
		m_pWorkflowDialog->onCmdDelete();
		break;
	case eCmdZoomIn:
		m_pWorkflowDialog->onCmdZoomIn();
		break;
	case eCmdZoomOut:
		m_pWorkflowDialog->onCmdZoomOut();
		break;
	case eCmdZoomActual:
		m_pWorkflowDialog->onCmdActualSize();
		break;
	case eCmdFitToWin:
		m_pWorkflowDialog->onCmdFitToWin();
		break;
	case eCmdCopy:
		m_pWorkflowDialog->onCmdCopy();
		break;
	case eCmdPaste:
		m_pWorkflowDialog->onCmdPaste();
		break;
	default:
		break;
	}
}