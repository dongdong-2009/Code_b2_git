// MyTreeCtrl.h : header file
//

// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.


#ifndef INC_MYTREECTRL_H
#define INC_MYTREECTRL_H
/////////////////////////////////////////////////////////////////////////////
// CMyTreeCtrl window

const unsigned int WM_TREE_DRAG_RELEASE = WM_USER + 0x1001;

class DragTreeCtrl : public CTreeCtrl
{
// Construction
public:
	DragTreeCtrl();

// Attributes
public:
	BOOL        m_bDragging;
	HTREEITEM   m_hitemDrag;
	CImageList  *m_pimagelist;

// Operations
public:
    void DrawChlItem(CDC *pDC,HTREEITEM hTreeItem,CRect itemRect);
    void DrawParItem(CDC *pDC,HTREEITEM hTreeItem,CRect itemRect);
    void OnCustomDraw(NMHDR *pNMHDR,LRESULT *pResult);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyTreeCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~DragTreeCtrl();
	void SetNewStyle(long lStyleMask, BOOL bSetBits);
	void OnButtonUp(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CMyTreeCtrl)
	afx_msg void OnBeginDrag(LPNMHDR pnmhdr, LRESULT *pLResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	void OnButtonUp(CPoint point);
    CImageList* CreateDragImageEx( const HTREEITEM nItem );

    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif
