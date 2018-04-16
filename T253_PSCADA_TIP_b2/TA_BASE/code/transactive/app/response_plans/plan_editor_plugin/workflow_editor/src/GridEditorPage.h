#pragma once

#include <vector>
#include <map>
#include <set>

#include "EditListCtrl.h"

class CWnd;
class IGridEditor;

// GridEditorPage dialog
struct GridItem;
struct GridColumn;

class AFX_EXT_CLASS GridEditorPage : public CDialog, IEditListOwner
{
	DECLARE_DYNAMIC(GridEditorPage)

public:
    GridEditorPage( IGridEditor& refEditor, const std::string& strName, CWnd* pParent = NULL );   // standard constructor
    virtual ~GridEditorPage();

protected:
    void InitGridItems();

public:
    // IEditListOwner
    virtual void valueChanged( const int nId, const int nRow, const int nCol, const std::string& strDisplayValue );
    virtual void booleanValueChanged( const int nId, const int nRow, const int nCol, const bool bNewValue, std::string& strDisplayValue );
    virtual void timeValueChanged( const int nId, const int nRow, const int nCol, const time_t tmNewValue, std::string& strDisplayValue );
    virtual void getBooleanValue( const int nId, const int nRow, const int nCol, bool& bCurValue );
    virtual void getTimeValue( const int nId, const int nRow, const int nCol, time_t& tmCurValue );
    virtual void getStringValue( const int nId, const int nRow, const int nCol, std::string& strCurValue, size_t& szMaxLength );
    virtual void popupList( const int nId, const int nRow, const int nCol );
    virtual void popupTree( const int nId, const int nRow, const int nCol );
    virtual void popupGrid( const int nId, const int nRow, const int nCol );
    virtual void popupText( const int nId, const int nRow, const int nCol );
	virtual bool isRowEditable(const int nRow);

    void createWnd( CWnd& refParent, const bool bForcedRowEditable = false );

protected:
    virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
	virtual void OnOK();
	virtual void OnCancel();
	void insertAddIcon();
	void onDeleteIconClicked(const int row);
	void onAddIconClicked();

    // Implementation
protected:
    // Generated message map functions
    afx_msg virtual BOOL OnInitDialog();
    afx_msg virtual void OnDestroy();
    afx_msg virtual void OnListCtrlClick( NMHDR* pNMHDR, LRESULT* pResult );

    DECLARE_MESSAGE_MAP()

protected:
    EditListCtrl m_lcAttributes;

public:
    std::string DisplayValue;

private:
    std::string m_strName;
    std::vector<GridColumn> m_vecGridColumns;
    std::vector<GridItem> m_vecSourceItems;
    std::map<int, std::string> m_mapColNames;
    IGridEditor& m_refEditor;
    bool m_bRowFixed;
    bool m_bForcedRowEditable;
	CBitmap m_bitMapAdd, m_bitMapDelete;
	CImageList m_imageList;
};
