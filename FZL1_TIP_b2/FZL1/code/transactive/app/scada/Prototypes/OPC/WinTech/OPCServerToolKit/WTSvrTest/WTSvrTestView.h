// WTSvrTestView.h : interface of the CWTSvrTestView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WTSVRTESTVIEW_H__E1407F9A_FCB2_11D2_80D5_0060975858BE__INCLUDED_)
#define AFX_WTSVRTESTVIEW_H__E1407F9A_FCB2_11D2_80D5_0060975858BE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class	CTag : public CObject
{
public:
	CTag::CTag();
	CTag::~CTag();

	HANDLE	Handle;
	CTime	Time;
	CString Name, Description, Units;
	VARIANT	Value;
	DWORD ReadAccessLevel, WriteAccessLevel;

// lolo, lo, hi, & hihi alarms
	float	alarms[4];
	DWORD	severity[4];
	BOOL	enabled[4];

};

class CWTSvrTestView : public CView
{
protected: // create from serialization only
	CWTSvrTestView();
	DECLARE_DYNCREATE(CWTSvrTestView)

	CObArray	TagList;

	int	ScreenX, ScreenY, cxChar, cyChar;
	int	SelectedIndex;

	BOOL	NTSecurityEnabled;

// Attributes
public:
	CWTSvrTestDoc* GetDocument();
	void	NotificationHandler(HANDLE Handle, VARIANT *pNewValue, DWORD *pDeviceError);
	void	UnknownTagHandler(LPSTR Path, LPSTR Name);
	void	RemovedTagHandler (HANDLE hTag, LPSTR Path, LPSTR Name);
	void	DeviceReadHandler(HANDLE Handle, VARIANT *pNewValue, WORD *pQuality, FILETIME *pTimestamp);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWTSvrTestView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWTSvrTestView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

void AutoAddtag(int tagno); 

// Generated message map functions
protected:
	//{{AFX_MSG(CWTSvrTestView)
	afx_msg void OnViewDllrevision();
	afx_msg void OnEditRegister();
	afx_msg void OnEditUnregister();
	afx_msg void OnEditAddtag();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnUpdateEditUpdatetag(CCmdUI* pCmdUI);
	afx_msg void OnEditUpdatetag();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnEditDynamictags();
	afx_msg void OnEditForceclientrefresh();
	afx_msg void OnEditUseraemsg();
	afx_msg void OnDestroy();
	afx_msg void OnEditDeletetag();
	afx_msg void OnUpdateEditDeletetag(CCmdUI* pCmdUI);
	afx_msg void OnFileSavetaglist();
	afx_msg void OnFileRestoretaglist();
	afx_msg void OnEditTagdelimiter();
	afx_msg void OnNtsecurityEnable();
	afx_msg void OnUpdateNtsecurityEnable(CCmdUI* pCmdUI);
	afx_msg void OnNtsecurityDisable();
	afx_msg void OnUpdateNtsecurityDisable(CCmdUI* pCmdUI);
	afx_msg void OnNtsecurityAdduser();
	afx_msg void OnUpdateNtsecurityAdduser(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in WTSvrTestView.cpp
inline CWTSvrTestDoc* CWTSvrTestView::GetDocument()
   { return (CWTSvrTestDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WTSVRTESTVIEW_H__E1407F9A_FCB2_11D2_80D5_0060975858BE__INCLUDED_)
