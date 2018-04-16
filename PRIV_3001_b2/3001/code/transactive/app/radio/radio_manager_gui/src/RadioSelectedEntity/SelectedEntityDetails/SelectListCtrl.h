#if !defined(AFX_SELECTLISTCTRL_H__92CA5999_9434_11D1_88D5_444553540000__INCLUDED_)
#define AFX_SelectLISTCTRL_H__92CA5999_9434_11D1_88D5_444553540000__INCLUDED_

#include "app/radio/radio_manager_gui/src/stdafx.h"
#include "app/radio/radio_manager_gui/src/resource.h"
#include "app/radio/radio_manager_gui/src/RadioOperations/RadioOperations.h"
#include "bus/radio/radio_bus/src/RadioTypes.h"

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// SelectListCtrl.h : header file


/////////////////////////////////////////////////////////////////////////////
// CSelectListCtrl window

class CSelectListCtrl : public CListCtrl
{
// Construction
public:
	CSelectListCtrl();

	  
// Attributes
public:
	
// Operations
private:
  
protected:
	
	//variables to hold current item
	int m_iRow;
    int m_iCol;
	
	

public:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectListCtrl)
	public:
//}}AFX_VIRTUAL

// Implementation
public:
	
	virtual ~CSelectListCtrl();
protected: 

	// Generated message map functions
protected:
	//{{AFX_MSG(CSelectListCtrl)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg void OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* hWnd);

public:
	afx_msg void OnNMClick(NMHDR *pNMHDR, LRESULT *pResult);
public:
	void setReadOnly(bool val);
	bool isReadOnly();
	
	// implement Original ListBox Function so no change on 
	// the Caller Class
	int GetCount ();
	void ResetContent ();
	void SetDefColWidth  ();
	int GetCurSel ();
	int InsertString (int nIndex,LPCTSTR lpszItem);
	void DeleteString (int nIndex) ;
	int FindStringExact ( int nIndexStart,LPCTSTR lpszFind, bool bSubStringSearch = true);
	void InsertDefColoumn ();
	int AddString (LPCTSTR lpszItem);

private:
	bool readOnly;

	bool m_bColInitialized;
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTLISTCTRL_H__92CA5999_9434_11D1_88D5_444553540000__INCLUDED_)
