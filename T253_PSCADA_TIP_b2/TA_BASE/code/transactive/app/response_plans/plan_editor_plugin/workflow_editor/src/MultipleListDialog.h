#pragma once

#include <string>
#include <vector>

// MultipleListDialog dialog

struct ListItem;
class IListPicker;

class MultipleListDialog : public CDialog
{
	DECLARE_DYNAMIC(MultipleListDialog)

public:
    MultipleListDialog( IListPicker& refPicker, const std::string& strName, const std::vector<ListItem>& vecSourceItems, const std::vector<unsigned int>& vecSelected, const unsigned int unMaxSel, CWnd* pParent = NULL, const int nRow = 0 );   // standard constructor
	virtual ~MultipleListDialog();

protected:
    void InitListItems();

protected:
    virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support

// Implementation
protected:
    // Generated message map functions
    afx_msg virtual BOOL OnInitDialog();
    afx_msg virtual void OnOK();
    afx_msg virtual void OnSelectItems();
    afx_msg virtual void OnDeSelectItems();
    afx_msg virtual void OnSrcSelAll();
    afx_msg virtual void OnDesSelAll();
    afx_msg virtual void OnSelectionPoolSelChanged();
    afx_msg virtual void OnSourcePoolSelChanged();

    DECLARE_MESSAGE_MAP()

protected:
    CListBox m_lstSourcePool;
    CListBox m_lstSelectionPool;
    CButton m_btnSrcSelectAll;
    CButton m_btnDesSelectAll;

public:
    std::string Title;
    std::string DisplayValue;
    bool ListUpdated;

private:
    const int m_nRow;
    const std::string m_strName;
    const unsigned int m_unMaxSelection;
    const std::vector<ListItem> m_vecSourceItems;
    std::vector<unsigned int> m_vecSelectedItems;
    IListPicker& m_refListPicker;
};
