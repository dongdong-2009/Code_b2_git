// WorkflowEditorDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "DragTreeCtrl.h"
#include "EditListCtrl.h"
#include "WorkflowViewer.h"
#include "WorkflowStructure.h"

class WorkflowDiagram;

// CWorkflowEditorDlg dialog
class CWorkflowEditorDlg : public CDialog, IEditListOwner
{
// Construction
public:
    CWorkflowEditorDlg( WorkflowDiagram& refDiagram, CWnd* pParent = NULL );	// standard constructor
    ~CWorkflowEditorDlg();

public:
    // Interface of IEditListOwner
    virtual void valueChanged( const int nId, const int nRow, const int nCol, const std::string& strDisplayValue );
    virtual void timeValueChanged( const int nId, const int nRow, const int nCol, const time_t tmNewValue, std::string& strDisplayValue );
    virtual void booleanValueChanged( const int nId, const int nRow, const int nCol, const bool bNewValue, std::string& strDisplayValue );
    virtual void getBooleanValue( const int nId, const int nRow, const int nCol, bool& bCurValue );
    virtual void getTimeValue( const int nId, const int nRow, const int nCol, time_t& tmCurValue );
    virtual void getStringValue( const int nId, const int nRow, const int nCol, std::string& strCurValue, size_t& szMaxLength );
    virtual void popupList( const int nId, const int nRow, const int nCol );
    virtual void popupTree( const int nId, const int nRow, const int nCol );
    virtual void popupGrid( const int nId, const int nRow, const int nCol );
    virtual void popupText( const int nId, const int nRow, const int nCol );

    void onCmdSave();
    void onCmdLoad();
    void onCmdZoomIn();
    void onCmdZoomOut();
    void onCmdActualSize();
    void onCmdFitToWin();
    void onCmdDelete();
    void onCmdCopy();
    void onCmdCut();
    void onCmdPaste();

protected:
    void refreshBasicAttributeList( const unsigned int idCurActivity );
    void refreshCustomAttributeList( const unsigned int idCurActivity );

public:
    DragTreeCtrl& getActivityTree();
    void setReadOnly( const bool bReadOnly );
    void create( CWnd& refParent );
    void resize();
    void refresh();

// Dialog Data
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	// Generated message map functions
    afx_msg virtual BOOL OnInitDialog();
    afx_msg virtual void OnSize( UINT nType, int cx, int cy );
    afx_msg virtual LRESULT OnTreeControlDragRelease( WPARAM wParam, LPARAM lParam );
    afx_msg virtual LRESULT OnActivityClicked( WPARAM wParam, LPARAM lParam );
    afx_msg virtual void OnOK();
    afx_msg virtual void OnCancel();
    afx_msg virtual void OnDestroy();
	DECLARE_MESSAGE_MAP()

protected:
    WorkflowDiagram& m_refDiagram;
    WorkflowViewer& m_viwWorkflowViewer;
    DragTreeCtrl m_tcActivityList;
    EditListCtrl m_lcBasicAttributes;
    EditListCtrl m_lcCustomAttributes;
    CStatic m_stcGeneral;
    CStatic m_stcParameters;

public:
    unsigned int m_idCurSelActivity;
    bool m_bReadonly;
    std::map<unsigned int, ActivityData> mapTypeToString;
    std::map<unsigned int, SequenceFlowData> mapDockings;
};
