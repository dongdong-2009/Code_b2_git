// WTSvrTestDoc.h : interface of the CWTSvrTestDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_WTSVRTESTDOC_H__E1407F98_FCB2_11D2_80D5_0060975858BE__INCLUDED_)
#define AFX_WTSVRTESTDOC_H__E1407F98_FCB2_11D2_80D5_0060975858BE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


class CWTSvrTestDoc : public CDocument
{
protected: // create from serialization only
	CWTSvrTestDoc();
	DECLARE_DYNCREATE(CWTSvrTestDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWTSvrTestDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CWTSvrTestDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CWTSvrTestDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_WTSVRTESTDOC_H__E1407F98_FCB2_11D2_80D5_0060975858BE__INCLUDED_)
