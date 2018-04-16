// WtSvrTst2View.h : interface of the CWtSvrTst2View class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WTSVRTST2VIEW_H__47BED7AB_6C4F_11D4_80FF_00C04F790F3B__INCLUDED_)
#define AFX_WTSVRTST2VIEW_H__47BED7AB_6C4F_11D4_80FF_00C04F790F3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class	CTag : public CObject
{
public:

	HANDLE	Handle;
	CString Name;
	VARIANT	Value;

};

class CWtSvrTst2View : public CFormView
{
protected: // create from serialization only
	CWtSvrTst2View();
	DECLARE_DYNCREATE(CWtSvrTst2View)

public:
	//{{AFX_DATA(CWtSvrTst2View)
	enum{ IDD = IDD_WTSVRTST2_FORM };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

// Attributes
public:
	CWtSvrTst2Doc* GetDocument();

	void	EventHandler (char *pMsg);
	void	WriteNotificationHandler (HANDLE hItem, VARIANT *pNewValue, DWORD *pDeviceError);

// Operations
public:
	CFile	LogFile;
	CFileException	e; 

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWtSvrTst2View)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWtSvrTst2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CTag		*pTagList;

	DWORD		StartTime;
	BOOL		IsSuspended;

	int			nAttr, nTags, nNodes, nTrains, nUnits;
	BOOL		Attr[4];
	BOOL		RandomUpdates;
	BOOL		DoListUpdates;

	int			UpdateFrequency;
	int			TagCount;
	int			TicCount;
	float		AvgUpdateTime;

	void		UpdateTree();

// Generated message map functions
protected:
	//{{AFX_MSG(CWtSvrTst2View)
	afx_msg void OnServerRegister();
	afx_msg void OnServerUnregister();
	afx_msg void OnServerCreatetags();
	afx_msg void OnUpdateServerCreatetags(CCmdUI* pCmdUI);
	afx_msg void OnDestroy();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSuspend();
	afx_msg void OnServerSetdelimiter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in WtSvrTst2View.cpp
inline CWtSvrTst2Doc* CWtSvrTst2View::GetDocument()
   { return (CWtSvrTst2Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WTSVRTST2VIEW_H__47BED7AB_6C4F_11D4_80FF_00C04F790F3B__INCLUDED_)
