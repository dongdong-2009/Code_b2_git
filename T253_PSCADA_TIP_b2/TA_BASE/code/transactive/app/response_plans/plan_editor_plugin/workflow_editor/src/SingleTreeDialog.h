#pragma once

#include <string>
#include <vector>
#include "afxcmn.h"

// TreeSelectionDialog dialog

struct TreeItem;
class ITreePicker;

class SingleTreeDialog : public CDialog
{
	DECLARE_DYNAMIC(SingleTreeDialog)

public:
    SingleTreeDialog( ITreePicker& refPicker, const std::string& strName, const std::vector<TreeItem>& vecSourceItems, std::vector<unsigned int> vecSelected, CWnd* pParent = NULL, const int nRow = 0 );   // standard constructor
    virtual ~SingleTreeDialog();

protected:
    void InitTreeItems();
    HTREEITEM InsertItem( CTreeCtrl& refTree, const std::string& strName, const bool bHasChild, HTREEITEM hParent = NULL );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
    // Generated message map functions
    afx_msg virtual BOOL OnInitDialog();
    afx_msg virtual void OnOK();
    afx_msg virtual void OnTreeSourceExpanding( NMHDR *pNMHDR, LRESULT *pResult );
	afx_msg void OnTreeSelChanged(NMHDR *pNMHDR, LRESULT *pResult);

    DECLARE_MESSAGE_MAP()

protected:
    CTreeCtrl m_treSourcePool;

public:
    std::string Title;
    std::string DisplayValue;
    bool ListUpdated;

private:
    const int m_nRow;
    const std::string m_strName;
    const std::vector<TreeItem> m_vecSourceItems;
    std::vector<unsigned int> m_vecSelectedItems;
    ITreePicker& m_refTreePicker;
};
