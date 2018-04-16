#pragma once

#include "GridEditorPage.h"

// GridEditorDialog dialog
class AFX_EXT_CLASS GridEditorDialog : public CDialog
{
	DECLARE_DYNAMIC(GridEditorDialog)

public:
    GridEditorDialog( IGridEditor& refEditor, const std::string& strName, CWnd* pParent = NULL );   // standard constructor
    virtual ~GridEditorDialog();

protected:
    virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

    // Implementation
protected:
    // Generated message map functions
    afx_msg virtual BOOL OnInitDialog();
    afx_msg virtual void OnDestroy();

    DECLARE_MESSAGE_MAP()

public:
    std::string Title;
    std::string& DisplayValue;

private:
    GridEditorPage m_pgEditor;
};
