/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/generic_gui/templates/Document/src/TemplateDocumentView.h $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  *
  */

#if !defined(AFX_TEMPLATEDOCUMENTVIEW_H__880DE812_3F92_49CF_88F5_B0449BE82537__INCLUDED_)
#define AFX_TEMPLATEDOCUMENTVIEW_H__880DE812_3F92_49CF_88F5_B0449BE82537__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTemplateDocumentView : public CView
{
protected: // create from serialization only
	CTemplateDocumentView();
	DECLARE_DYNCREATE(CTemplateDocumentView)

// Attributes
public:
	CTemplateDocumentDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateDocumentView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTemplateDocumentView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTemplateDocumentView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TemplateDocumentView.cpp
inline CTemplateDocumentDoc* CTemplateDocumentView::GetDocument()
   { return (CTemplateDocumentDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATEDOCUMENTVIEW_H__880DE812_3F92_49CF_88F5_B0449BE82537__INCLUDED_)
