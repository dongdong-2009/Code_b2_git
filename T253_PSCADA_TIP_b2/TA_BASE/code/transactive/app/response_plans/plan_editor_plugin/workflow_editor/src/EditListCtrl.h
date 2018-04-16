#pragma once

#include <map>
#include "IEditListOwner.h"

// EditListCtrl

class EditListCtrl : public CListCtrl
{
public:
	EditListCtrl();
	virtual ~EditListCtrl();

    void AttachOwner( IEditListOwner* pOwner );
    void SetId( const int nId );
    void SetGroupByColumn( const bool bCol );
    void SetEditType( const int nCol, const int nType );
    void StartEdit();
    void StopEdit();

    // Generated message map functions
protected:
    //{{AFX_MSG(CEditableListCtrl)
    afx_msg BOOL OnClick( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg BOOL OnToolNeedText( UINT id, NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnDateTimeLostFocus( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnItemCleared( NMHDR* pNMHDR, LRESULT* pResult );
    afx_msg void OnLvnItemchanging( NMHDR *pNMHDR, LRESULT *pResult );
    afx_msg void OnButtonClicked();
    afx_msg void OnCheckBoxClicked();
    afx_msg void OnEditLostFocus();
    afx_msg void PreSubclassWindow();
    //}}AFX_MSG

    DECLARE_MESSAGE_MAP()

private:
    void CellHitTest( const CPoint& pt, int& nRow, int& nCol ) const;

protected:
    bool m_bGroupByColumn;
    std::map<int, int> m_mapNumToEditType;
    CWnd* m_pCurEditWnd;
    int m_nId;
    int m_nCurEditRow;
    int m_nCurEditCol;
    IEditListOwner* m_pOwner;
};


