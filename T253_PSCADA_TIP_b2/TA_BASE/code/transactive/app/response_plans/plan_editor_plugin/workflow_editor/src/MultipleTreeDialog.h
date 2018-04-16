#pragma once

#include <string>
#include <vector>

// MultipleListDialog dialog

struct TreeItem;

class MultipleTreeDialog : public CDialog
{
	DECLARE_DYNAMIC(MultipleTreeDialog)

public:
    MultipleTreeDialog( ITreePicker& refPicker, const std::string& strName, const std::vector<TreeItem>& vecSourceItems, std::vector<unsigned int> vecSelected, const unsigned int unMaxSelection, CWnd* pParent = NULL, const int nRow = 0 );   // standard constructor
    virtual ~MultipleTreeDialog();

protected:
    void InitTreeItems();
    HTREEITEM InsertItem( CTreeCtrl& refTree, const std::string& strName, const bool bHasChild, HTREEITEM hParent = NULL );

protected:
    virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

// Implementation
protected:
    // Generated message map functions
    afx_msg virtual BOOL OnInitDialog();
    afx_msg virtual void OnOK();
    afx_msg virtual void OnSelectItems();
    afx_msg virtual void OnSrcSelAll();
    afx_msg virtual void OnSelectionPoolSelChanged();
    afx_msg virtual void OnSourcePoolSelChanged();
    afx_msg virtual void OnTreeSourceExpanding( NMHDR *pNMHDR, LRESULT *pResult );

    DECLARE_MESSAGE_MAP()

protected:
    CTreeCtrl m_treSourcePool;
    CListBox m_lstSelectionPool;
    CButton m_btnSrcSelectAll;

public:
    std::string Title;
    std::string DisplayValue;
    bool ListUpdated;

private:
    const int m_nRow;
    const std::string m_strName;
    const unsigned int m_unMaxSelection;
    const std::vector<TreeItem> m_vecSourceItems;
    std::vector<unsigned int> m_vecSelectedItems;
    ITreePicker& m_refTreePicker;
};
