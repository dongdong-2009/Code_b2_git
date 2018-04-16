#if !defined(AFX_PADVATABCTRL_H__956D100E_81EA_40D3_82B3_83CE733E20AC__INCLUDED_)
#define AFX_PADVATABCTRL_H__956D100E_81EA_40D3_82B3_83CE733E20AC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PaDvaTabCtrl.h : header file
//

#include <map>

/////////////////////////////////////////////////////////////////////////////
// PaDvaTabCtrl window

class PaDvaTabCtrl : public CTabCtrl
{
// Construction
public:
	PaDvaTabCtrl();

// Attributes
public:

// Operations
public:
    void addPage( CPropertyPage* pPage, UINT unTemplateId, const std::string& strCaption );
    void showPage( int nIndex );
    void showPage( CPropertyPage* pPage );

// Overrides
	// ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(PaDvaTabCtrl)
protected:
    virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~PaDvaTabCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(PaDvaTabCtrl)
	afx_msg void OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

protected:
    std::map<int, CPropertyPage*> m_mapPages;
    RECT m_stPageRect;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PADVATABCTRL_H__956D100E_81EA_40D3_82B3_83CE733E20AC__INCLUDED_)
