#if !defined(AFX_PaTabCtrl_H__956D100E_81EA_40D3_82B3_83CE733E20AC__INCLUDED_)
#define AFX_PaTabCtrl_H__956D100E_81EA_40D3_82B3_83CE733E20AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaTabCtrl.h : header file
//

#include <map>

/////////////////////////////////////////////////////////////////////////////
// PaTabCtrl window

class PaTabCtrl : public CTabCtrl
{
// Construction
public:
	PaTabCtrl();

// Attributes
public:

// Operations
public:
    void addPage( CPropertyPage* pPage, UINT unTemplateId, const std::string& strCaption );
    void showPage( int nIndex );

// Overrides
	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(PaTabCtrl)
protected:
    virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~PaTabCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(PaTabCtrl)
	afx_msg BOOL OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnSelChanging(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
    std::map<int, CPropertyPage*> m_mapPages;
    RECT m_stPageRect;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PaTabCtrl_H__956D100E_81EA_40D3_82B3_83CE733E20AC__INCLUDED_)
