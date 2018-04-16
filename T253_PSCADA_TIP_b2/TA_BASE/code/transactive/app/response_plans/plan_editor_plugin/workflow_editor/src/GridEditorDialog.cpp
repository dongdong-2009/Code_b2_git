// src/GridEditorDialog.cpp : implementation file
//

#include "stdafx.h"

#include "resource.h"
#include "app/response_plans/plan_editor_plugin/workflow_element/src/IActivityParameter.h"
#include "TextEditDialog.h"
#include "GridEditorDialog.h"
#include "ListPickerDialog.h"
#include "TreePickerDialog.h"
#include "FontHelper.h"
#include "GridEditorDialog.h"


// GridEditorDialog dialog

IMPLEMENT_DYNAMIC( GridEditorDialog, CDialog )

GridEditorDialog::GridEditorDialog( IGridEditor& refEditor, const std::string& strName, CWnd* pParent ) :
CDialog( IDD_GRID_EDITOR_DIALOG, pParent ),
m_pgEditor( refEditor, strName, this ),
Title( "" ),
DisplayValue( m_pgEditor.DisplayValue )
{
}

GridEditorDialog::~GridEditorDialog()
{
}

void GridEditorDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange( pDX );
}

BEGIN_MESSAGE_MAP(GridEditorDialog, CDialog)
    ON_WM_DESTROY()
END_MESSAGE_MAP()

// GridEditorDialog message handlers

BOOL GridEditorDialog::OnInitDialog()
{
    CDialog::OnInitDialog();

	m_pgEditor.createWnd(*this);

    SetWindowText( Title.c_str() );

    FontHelper::adjustChildrenFont( GetSafeHwnd() );

    return FALSE;
}

void GridEditorDialog::OnDestroy()
{
    if (::IsWindow(m_pgEditor.GetSafeHwnd()))
    {
        m_pgEditor.DestroyWindow();
    }

    CDialog::OnDestroy();
}
