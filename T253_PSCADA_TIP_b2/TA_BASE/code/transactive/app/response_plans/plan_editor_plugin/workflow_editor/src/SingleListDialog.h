#pragma once

#include <string>
#include <vector>

struct ListItem;
// SingleListDialog dialog

class SingleListDialog : public CDialog
{
	DECLARE_DYNAMIC(SingleListDialog)

public:
    SingleListDialog( IListPicker& refPicker, const std::string& strName, const std::vector<ListItem>& vecSourceItems, const std::vector<unsigned int>& vecSelected, CWnd* pParent = NULL, const int nRow = 0 );   // standard constructor
    virtual ~SingleListDialog();

protected:
    void InitListItems();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    // Generated message map functions
    afx_msg virtual BOOL OnInitDialog();
    afx_msg virtual void OnOK();
    afx_msg virtual void OnSelectionChanged();

    DECLARE_MESSAGE_MAP()

protected:
    CListBox m_lstSourcePool;

public:
    std::string Title;
    std::string DisplayValue;
    bool ListUpdated;

private:
    const int m_nRow;
    const std::string m_strName;
    const std::vector<ListItem> m_vecSourceItems;
    std::vector<unsigned int> m_vecSelectedItems;
    IListPicker& m_refListPicker;
};
