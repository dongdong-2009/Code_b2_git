// WorkflowEditorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/WorkflowElement.h"
#include "WorkflowDiagram.h"
#include "WorkflowEditor.h"
#include "TextEditDialog.h"
#include "GridEditorDialog.h"
#include "ListPickerDialog.h"
#include "TreePickerDialog.h"
#include "WorkflowEditorDlg.h"

enum AttritubeType
{
    eBasicAttribute = 1,
    eCustomAttribute
};

// CWorkflowEditorDlg dialog

CWorkflowEditorDlg::CWorkflowEditorDlg( WorkflowDiagram& refDiagram, CWnd* pParent ) : CDialog( IDD_WORKFLOWEDITOR_DIALOG, pParent ),
m_refDiagram( refDiagram ),
// View would be self-deleted, cannot call delete explicit or used as member object
m_viwWorkflowViewer( *(new WorkflowViewer(m_refDiagram)) )
{
}

CWorkflowEditorDlg::~CWorkflowEditorDlg()
{
    if ( ::IsWindow( GetSafeHwnd() ))
    {
        DestroyWindow();
    }
}

void CWorkflowEditorDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_TREE_ACTIVITY, m_tcActivityList);
    DDX_Control(pDX, IDC_LIST_BASIC_ATTRIBUTES, m_lcBasicAttributes);
    DDX_Control(pDX, IDC_LIST_ATTRIBUTES, m_lcCustomAttributes);
    DDX_Control(pDX, IDC_STATIC_GENERAL, m_stcGeneral);
    DDX_Control(pDX, IDC_STATIC_PARAMETERS, m_stcParameters);
}

BEGIN_MESSAGE_MAP(CWorkflowEditorDlg, CDialog)
    ON_WM_SIZE()
    ON_WM_DESTROY()
    ON_MESSAGE( WM_TREE_DRAG_RELEASE, OnTreeControlDragRelease )
    ON_MESSAGE( WM_ACTIVITY_CLICKED, OnActivityClicked )
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CWorkflowEditorDlg message handlers

BOOL CWorkflowEditorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

    m_lcCustomAttributes.SetGroupByColumn( false );
    m_lcCustomAttributes.AttachOwner( this );
    m_lcCustomAttributes.SetId( eCustomAttribute );

    m_lcBasicAttributes.SetGroupByColumn( false );
    m_lcBasicAttributes.AttachOwner( this );
    m_lcBasicAttributes.SetId( eBasicAttribute );

    RECT rtClient = { 0, 0, 0, 0 };
    m_viwWorkflowViewer.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, rtClient, this, 0x100010);

    resize();

	// Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog

    m_lcCustomAttributes.InsertColumn( 0, "Name", LVCFMT_LEFT, 130 );
    m_lcCustomAttributes.InsertColumn( 1, "Value", LVCFMT_LEFT, 150 );
    m_lcCustomAttributes.InsertColumn( 2, "UniqueName", LVCFMT_LEFT, 0 );

    m_lcBasicAttributes.InsertColumn( 0, "Name", LVCFMT_LEFT, 130 );
    m_lcBasicAttributes.InsertColumn( 1, "Value", LVCFMT_LEFT, 150 );
    m_lcBasicAttributes.InsertColumn( 2, "UniqueName", LVCFMT_LEFT, 0 );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.

LRESULT CWorkflowEditorDlg::OnTreeControlDragRelease( WPARAM wParam, LPARAM lParam )
{
    CPoint ptAction;
    ::GetCursorPos(&ptAction);
    CRect rtViewer;
    m_viwWorkflowViewer.GetWindowRect( &rtViewer );
    if ( rtViewer.PtInRect( ptAction ))
    {
        m_viwWorkflowViewer.screenToDiagram(ptAction);
        m_refDiagram.addElement(static_cast<int>(wParam), ptAction.x, ptAction.y);
    }

    return 0u;
}

LRESULT CWorkflowEditorDlg::OnActivityClicked( WPARAM wParam, LPARAM lParam )
{
    unsigned int idNewSelActivity = static_cast<unsigned int>(wParam);

    if ( idNewSelActivity != m_idCurSelActivity )
    {
        m_lcBasicAttributes.StopEdit();
        m_lcCustomAttributes.StopEdit();

        refreshBasicAttributeList(idNewSelActivity);
        refreshCustomAttributeList(idNewSelActivity);

        m_idCurSelActivity = idNewSelActivity;
    }

    return 0u;
}

void CWorkflowEditorDlg::onCmdSave()
{
    m_refDiagram.save( mapTypeToString, mapDockings );
}

void CWorkflowEditorDlg::onCmdLoad()
{
    m_refDiagram.load( mapTypeToString, mapDockings );
}


void CWorkflowEditorDlg::onCmdZoomIn()
{
    m_viwWorkflowViewer.zoomIn();
}


void CWorkflowEditorDlg::onCmdZoomOut()
{
    m_viwWorkflowViewer.zoomOut();
}


void CWorkflowEditorDlg::onCmdActualSize()
{
    m_viwWorkflowViewer.actualZoom();
}


void CWorkflowEditorDlg::onCmdFitToWin()
{
    m_viwWorkflowViewer.fitToWin();
}


void CWorkflowEditorDlg::onCmdDelete()
{
    m_refDiagram.deleteElements();
}


void CWorkflowEditorDlg::onCmdCopy()
{
    m_refDiagram.copy();
}


void CWorkflowEditorDlg::onCmdCut()
{
    m_refDiagram.cut();
}


void CWorkflowEditorDlg::onCmdPaste()
{
    CPoint ptAction;
    ::GetCursorPos( &ptAction );
    m_viwWorkflowViewer.screenToDiagram( ptAction );

    m_refDiagram.paste( ptAction.x, ptAction.y );
}

DragTreeCtrl& CWorkflowEditorDlg::getActivityTree()
{
    return m_tcActivityList;
}

void CWorkflowEditorDlg::valueChanged( const int nId, const int nRow, const int nCol, const std::string& strDisplayValue )
{
    if ( eCustomAttribute == nId )
    {
        std::string strName = m_lcCustomAttributes.GetItemText( nRow, 2 ).operator LPCSTR();
        IStringEditor* pStringEditor = m_refDiagram.getActivityParameter( m_idCurSelActivity ).getStringEditor( strName );
        bool bValid = false;
        bool bListUpdated = false;

        if ( NULL != pStringEditor )
        {
            bListUpdated = pStringEditor->updateValue( strName, strDisplayValue, bValid );
        }

        if ( bListUpdated )
        {
            refreshCustomAttributeList( m_idCurSelActivity );
        }
    }
    else if ( eBasicAttribute == nId )
    {
        std::string strName = m_lcBasicAttributes.GetItemText( nRow, 2 ).operator LPCSTR();
        IBasicEditor& refEditor = m_refDiagram.getActivityParameter( m_idCurSelActivity ).basicEditor();
        bool bValid = false;
        bool bListUpdated = refEditor.updateBasicValue( strName, strDisplayValue, bValid );

        if ( bListUpdated )
        {
            refreshBasicAttributeList( m_idCurSelActivity );
        }

        if ( 0 == strName.compare( "Name" ))
        {
            m_refDiagram.setActivityName( m_idCurSelActivity, strDisplayValue );
        }
    }

    m_refDiagram.activityUpdated( m_idCurSelActivity );
}

void CWorkflowEditorDlg::booleanValueChanged( const int nId, const int nRow, const int nCol, const bool bNewValue, std::string& strDisplayValue )
{
    if ( eCustomAttribute == nId )
    {
        std::string strName = m_lcCustomAttributes.GetItemText( nRow, 2 ).operator LPCSTR();
        IBoolEditor* pBoolEditor = m_refDiagram.getActivityParameter( m_idCurSelActivity ).getBoolEditor( strName );
        bool bValid = false;
        bool bListUpdated = false;

        if ( NULL != pBoolEditor )
        {
            bListUpdated = pBoolEditor->updateBooleanValue( strName, bNewValue, strDisplayValue, bValid );
        }

        if ( bListUpdated )
        {
            refreshCustomAttributeList( m_idCurSelActivity );
        }
    }
    else if ( eBasicAttribute == nId )
    {
        std::string strName = m_lcBasicAttributes.GetItemText( nRow, 2 ).operator LPCSTR();
        IBasicEditor& refEditor = m_refDiagram.getActivityParameter( m_idCurSelActivity ).basicEditor();
        bool bValid = false;
        bool bListUpdated = refEditor.updateBasicBooleanValue( strName, bNewValue, strDisplayValue, bValid );

        if ( bListUpdated )
        {
            refreshBasicAttributeList( m_idCurSelActivity );
        }
    }

    m_refDiagram.activityUpdated( m_idCurSelActivity );
}

void CWorkflowEditorDlg::timeValueChanged( const int nId, const int nRow, const int nCol, const time_t tmNewValue, std::string& strDisplayValue )
{
    if ( eCustomAttribute == nId )
    {
        std::string strName = m_lcCustomAttributes.GetItemText( nRow, 2 ).operator LPCSTR();
        ITimeEditor* pTimeEditor = m_refDiagram.getActivityParameter( m_idCurSelActivity ).getTimeEditor( strName );
        bool bValid = false;
        bool bListUpdated = false;

        if ( NULL != pTimeEditor )
        {
            bListUpdated = pTimeEditor->updateTimeValue( strName, tmNewValue, strDisplayValue, bValid );
        }

        if ( bListUpdated )
        {
            refreshCustomAttributeList( m_idCurSelActivity );
        }
    }
    else if ( eBasicAttribute == nId )
    {
        // Basic parameter not support
    }

    m_refDiagram.activityUpdated( m_idCurSelActivity );
}

void CWorkflowEditorDlg::getBooleanValue( const int nId, const int nRow, const int nCol, bool& bCurValue )
{
    if ( eCustomAttribute == nId )
    {
        std::string strName = m_lcCustomAttributes.GetItemText( nRow, 2 ).operator LPCSTR();
        IBoolEditor* pBoolEditor = m_refDiagram.getActivityParameter( m_idCurSelActivity ).getBoolEditor( strName );

        if ( NULL != pBoolEditor )
        {
            pBoolEditor->getBooleanValue( strName, bCurValue );
        }
    }
    else if ( eBasicAttribute == nId )
    {
        std::string strName = m_lcBasicAttributes.GetItemText( nRow, 2 ).operator LPCSTR();
        IBasicEditor& refEditor = m_refDiagram.getActivityParameter( m_idCurSelActivity ).basicEditor();

        refEditor.getBasicBooleanValue( strName, bCurValue );
    }
}

void CWorkflowEditorDlg::getTimeValue( const int nId, const int nRow, const int nCol, time_t& tmCurValue )
{
    if ( eCustomAttribute == nId )
    {
        std::string strName = m_lcCustomAttributes.GetItemText( nRow, 2 ).operator LPCSTR();
        ITimeEditor* pTimeEditor = m_refDiagram.getActivityParameter( m_idCurSelActivity ).getTimeEditor( strName );

        if ( NULL != pTimeEditor )
        {
            pTimeEditor->getTimeValue( strName, tmCurValue, nRow );
        }
    }
    else if ( eBasicAttribute == nId )
    {
        // Basic parameter not support
    }
}

void CWorkflowEditorDlg::getStringValue( const int nId, const int nRow, const int nCol, std::string& strCurValue, size_t& szMaxLength )
{
    if ( eCustomAttribute == nId )
    {
        std::string strName = m_lcCustomAttributes.GetItemText( nRow, 2 ).operator LPCSTR();
        IStringEditor* pStringEditor = m_refDiagram.getActivityParameter( m_idCurSelActivity ).getStringEditor( strName );

        if ( NULL != pStringEditor )
        {
            pStringEditor->getValue( strName, strCurValue, szMaxLength, nRow );
        }
    }
    else if ( eBasicAttribute == nId )
    {
        std::string strName = m_lcBasicAttributes.GetItemText( nRow, 2 ).operator LPCSTR();
        IBasicEditor& refEditor = m_refDiagram.getActivityParameter( m_idCurSelActivity ).basicEditor();
        
        refEditor.getBasicValue( strName, strCurValue, szMaxLength );
    }
}

void CWorkflowEditorDlg::popupList( const int nId, const int nRow, const int nCol )
{
    if ( eCustomAttribute == nId )
    {
        std::string strName = m_lcCustomAttributes.GetItemText( nRow, 2 ).operator LPCSTR();

        IListPicker* pListPicker = m_refDiagram.getActivityParameter( m_idCurSelActivity ).getListPicker( strName );

        if ( NULL == pListPicker )
        {
            return;
        }

        ListPickerDialog dlgPopup( *pListPicker, strName, this );

        dlgPopup.Title = m_lcCustomAttributes.GetItemText( nRow, 0 ).operator LPCSTR();

        int nAction = dlgPopup.DoModal();

        if ( IDOK == nAction )
        {
            m_lcCustomAttributes.SetItemText( nRow, nCol, dlgPopup.DisplayValue.c_str( ) );
            m_lcCustomAttributes.StopEdit();

            if ( dlgPopup.ListUpdated )
            {
                refreshCustomAttributeList( m_idCurSelActivity );
            }

            m_refDiagram.activityUpdated( m_idCurSelActivity );
        }
    }
    else if ( eBasicAttribute == nId )
    {
        // Basic parameter not support
    }
}

void CWorkflowEditorDlg::popupTree( const int nId, const int nRow, const int nCol )
{
    if ( eCustomAttribute == nId )
    {
        std::string strName = m_lcCustomAttributes.GetItemText( nRow, 2 ).operator LPCSTR();

        ITreePicker* pTreePicker = m_refDiagram.getActivityParameter( m_idCurSelActivity ).getTreePicker( strName );

        if ( NULL == pTreePicker )
        {
            return;
        }

        TreePickerDialog dlgPopup( *pTreePicker, strName, this );

        dlgPopup.Title = m_lcCustomAttributes.GetItemText( nRow, 0 ).operator LPCSTR();

        int nAction = dlgPopup.DoModal();

        if ( IDOK == nAction )
        {
            m_lcCustomAttributes.SetItemText( nRow, nCol, dlgPopup.DisplayValue.c_str( ) );
            m_lcCustomAttributes.StopEdit();

            if ( dlgPopup.ListUpdated )
            {
                refreshCustomAttributeList( m_idCurSelActivity );
            }

            m_refDiagram.activityUpdated( m_idCurSelActivity );
        }
    }
    else if ( eBasicAttribute == nId )
    {
        // Basic parameter not support
    }
}

void CWorkflowEditorDlg::popupGrid( const int nId, const int nRow, const int nCol )
{
    if ( eCustomAttribute == nId )
    {
        std::string strName = m_lcCustomAttributes.GetItemText( nRow, 2 ).operator LPCSTR();

        IGridEditor* pGridEditor = m_refDiagram.getActivityParameter( m_idCurSelActivity ).getGridEditor( strName );

        if ( NULL == pGridEditor )
        {
            return;
        }

        GridEditorDialog dlgPopup( *pGridEditor, strName, this );

        dlgPopup.Title = m_lcCustomAttributes.GetItemText( nRow, 0 ).operator LPCSTR();

        int nAction = dlgPopup.DoModal();

        if ( IDOK == nAction )
        {
            m_lcCustomAttributes.SetItemText( nRow, nCol, dlgPopup.DisplayValue.c_str() );
            m_lcCustomAttributes.StopEdit();

            m_refDiagram.activityUpdated( m_idCurSelActivity );
        }
    }
    else if ( eBasicAttribute == nId )
    {
        // Basic parameter not support
    }
}

void CWorkflowEditorDlg::popupText( const int nId, const int nRow, const int nCol )
{
    if ( eCustomAttribute == nId )
    {
        TextEditDialog dlgPopup( this );

        dlgPopup.Title = m_lcCustomAttributes.GetItemText( nRow, 0 ).operator LPCSTR();
        dlgPopup.Content = m_lcCustomAttributes.GetItemText( nRow, 1 ).operator LPCSTR();

        int nAction = dlgPopup.DoModal();

        if ( IDOK == nAction )
        {
            std::string strName = m_lcCustomAttributes.GetItemText( nRow, 2 ).operator LPCSTR();

            IStringEditor* pStringEditor = m_refDiagram.getActivityParameter( m_idCurSelActivity ).getStringEditor( strName );
            bool bValid = false;
            bool bListUpdated = false;

            if ( NULL != pStringEditor )
            {
                bListUpdated = pStringEditor->updateValue( strName, dlgPopup.Content, bValid );
            }

            m_lcCustomAttributes.SetItemText( nRow, nCol, dlgPopup.Content.c_str() );
            m_lcCustomAttributes.StopEdit();

            if ( bListUpdated )
            {
                refreshCustomAttributeList( m_idCurSelActivity );
            }

            m_refDiagram.activityUpdated( m_idCurSelActivity );
        }
    }
    else if ( eBasicAttribute == nId )
    {
        TextEditDialog dlgPopup( this );

        dlgPopup.Title = m_lcBasicAttributes.GetItemText( nRow, 0 ).operator LPCSTR();
        dlgPopup.Content = m_lcBasicAttributes.GetItemText( nRow, 1 ).operator LPCSTR();

        int nAction = dlgPopup.DoModal();

        if ( IDOK == nAction )
        {
            std::string strName = m_lcBasicAttributes.GetItemText( nRow, 2 ).operator LPCSTR();

            IBasicEditor& refEditor = m_refDiagram.getActivityParameter( m_idCurSelActivity ).basicEditor();
            bool bValid = false;
            bool bListUpdated = refEditor.updateBasicValue( strName, dlgPopup.Content, bValid );

            m_lcBasicAttributes.SetItemText( nRow, nCol, dlgPopup.Content.c_str() );
            m_lcBasicAttributes.StopEdit();

            if ( bListUpdated )
            {
                refreshBasicAttributeList( m_idCurSelActivity );
            }

            m_refDiagram.activityUpdated( m_idCurSelActivity );
        }
    }
}

void CWorkflowEditorDlg::refreshBasicAttributeList( const unsigned int idCurActivity )
{
    m_lcBasicAttributes.DeleteAllItems();

    if ( 0u != idCurActivity )
    {
        std::vector<ItemPair> vecNameValues;

        try
        {
            m_refDiagram.getActivityParameter( idCurActivity ).basicEditor().getBasicNameValuePairs( vecNameValues );
        }
        catch ( std::exception& expWhat )
        {
        }
        catch ( ... )
        {
        }

        int nIndex = 0;
        for ( std::vector<ItemPair>::iterator itLoop = vecNameValues.begin(); vecNameValues.end() != itLoop; ++itLoop )
        {
            nIndex = m_lcBasicAttributes.InsertItem( m_lcBasicAttributes.GetItemCount(), itLoop->DisplayName.c_str() );
            m_lcBasicAttributes.SetItemText( nIndex, 1, itLoop->Item.c_str() );
            m_lcBasicAttributes.SetItemText(nIndex, 2, itLoop->Name.c_str());
            m_lcBasicAttributes.SetEditType(nIndex, itLoop->Type);
        }
    }
}

void CWorkflowEditorDlg::refreshCustomAttributeList( const unsigned int idCurActivity )
{
    m_lcCustomAttributes.DeleteAllItems();

    if ( 0u != idCurActivity )
    {
        std::vector<ItemPair> vecNameValues;

        try
        {
            m_refDiagram.getActivityParameter( idCurActivity ).getNameValuePairs( vecNameValues );
        }
        catch ( std::exception& expWhat )
        {
        }
        catch ( ... )
        {
        }

        int nIndex = 0;
        for ( std::vector<ItemPair>::iterator itLoop = vecNameValues.begin(); vecNameValues.end() != itLoop; ++itLoop )
        {
            nIndex = m_lcCustomAttributes.InsertItem( m_lcCustomAttributes.GetItemCount(), itLoop->DisplayName.c_str() );
            m_lcCustomAttributes.SetItemText( nIndex, 1, itLoop->Item.c_str() );
            m_lcCustomAttributes.SetItemText( nIndex, 2, itLoop->Name.c_str() );
            m_lcCustomAttributes.SetEditType( nIndex, itLoop->Type );
        }
    }
}

void CWorkflowEditorDlg::setReadOnly( const bool bReadOnly )
{
    m_bReadonly = bReadOnly;

    resize();
    m_tcActivityList.ShowWindow( m_bReadonly ? SW_HIDE : SW_SHOW );
    m_lcBasicAttributes.ShowWindow( m_bReadonly ? SW_HIDE : SW_SHOW );
    m_lcCustomAttributes.ShowWindow( m_bReadonly ? SW_HIDE : SW_SHOW );
    m_stcGeneral.ShowWindow( m_bReadonly ? SW_HIDE : SW_SHOW );
    m_stcParameters.ShowWindow( m_bReadonly ? SW_HIDE : SW_SHOW );
}

void CWorkflowEditorDlg::create( CWnd& refParent )
{
    Create( IDD_WORKFLOWEDITOR_DIALOG, &refParent );
    ShowWindow( SW_HIDE );
}

void CWorkflowEditorDlg::OnSize( UINT nType, int cx, int cy )
{
    CDialog::OnSize( nType, cx, cy );

    if ( SIZE_MINIMIZED != nType && SIZE_MAXHIDE != nType )
    {
        resize();
    }
}

void CWorkflowEditorDlg::resize()
{
    if ( !::IsWindow( m_tcActivityList.GetSafeHwnd() ) || !::IsWindow( m_lcCustomAttributes.GetSafeHwnd() ) || !::IsWindow( m_viwWorkflowViewer.GetSafeHwnd() ))
    {
        return;
    }

    CRect rtLeftPanel;
    m_tcActivityList.GetWindowRect( &rtLeftPanel );
    ScreenToClient( rtLeftPanel );

    CRect rtRightPanel;
    m_lcCustomAttributes.GetWindowRect( &rtRightPanel );
    ScreenToClient( rtRightPanel );

    CRect rtRightBasicPanel;
    m_lcBasicAttributes.GetWindowRect( &rtRightBasicPanel );
    ScreenToClient( rtRightBasicPanel );

    CRect rtRightTitle;
    m_stcParameters.GetWindowRect( &rtRightTitle );
    ScreenToClient( rtRightTitle );

    CRect rtRightBasicTitle;
    m_stcGeneral.GetWindowRect( &rtRightBasicTitle );
    ScreenToClient( rtRightBasicTitle );

    CRect rtClient;
    GetClientRect( &rtClient );
    rtLeftPanel.bottom = rtClient.bottom;
    rtRightPanel.bottom = rtClient.bottom;
    rtRightBasicTitle.left = rtClient.right - rtRightBasicTitle.Width();
    rtRightBasicPanel.left = rtClient.right - rtRightPanel.Width();
    rtRightBasicPanel.right = rtClient.right;
    rtRightBasicTitle.right = rtClient.right;
    rtRightTitle.left = rtClient.right - rtRightTitle.Width();
    rtRightPanel.left = rtClient.right - rtRightPanel.Width();
    rtRightPanel.right = rtClient.right;
    rtRightTitle.right = rtClient.right;

    rtClient.left += ( m_bReadonly ? 0 : rtLeftPanel.Width() );
    rtClient.right -= ( m_bReadonly ? 0 : rtRightPanel.Width() );

    m_tcActivityList.MoveWindow( &rtLeftPanel );
    m_lcBasicAttributes.MoveWindow( &rtRightBasicPanel );
    m_lcCustomAttributes.MoveWindow( &rtRightPanel );
    m_stcGeneral.MoveWindow( &rtRightBasicTitle );
    m_stcParameters.MoveWindow( &rtRightTitle );
    m_viwWorkflowViewer.MoveWindow( &rtClient );
}

void CWorkflowEditorDlg::refresh()
{
    m_lcCustomAttributes.DeleteAllItems();
    m_lcBasicAttributes.DeleteAllItems();
}

void CWorkflowEditorDlg::OnOK()
{
}

void CWorkflowEditorDlg::OnCancel()
{
}

void CWorkflowEditorDlg::OnDestroy()
{
    if ( ::IsWindow( m_viwWorkflowViewer.GetSafeHwnd() ) )
    {
        m_viwWorkflowViewer.DestroyWindow();
    }

    CDialog::OnDestroy();
}
