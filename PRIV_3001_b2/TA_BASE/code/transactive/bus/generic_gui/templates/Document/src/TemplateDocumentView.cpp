/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/generic_gui/templates/Document/src/TemplateDocumentView.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  *
  */

#include "stdafx.h"
#include "TemplateDocument.h"

#include "TemplateDocumentDoc.h"
#include "TemplateDocumentView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplateDocumentView

IMPLEMENT_DYNCREATE(CTemplateDocumentView, CView)

BEGIN_MESSAGE_MAP(CTemplateDocumentView, CView)
	//{{AFX_MSG_MAP(CTemplateDocumentView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateDocumentView construction/destruction

CTemplateDocumentView::CTemplateDocumentView()
{
	// TODO: add construction code here

}

CTemplateDocumentView::~CTemplateDocumentView()
{
}

BOOL CTemplateDocumentView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTemplateDocumentView drawing

void CTemplateDocumentView::OnDraw(CDC* pDC)
{
	CTemplateDocumentDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CTemplateDocumentView diagnostics

#ifdef _DEBUG
void CTemplateDocumentView::AssertValid() const
{
	CView::AssertValid();
}

void CTemplateDocumentView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTemplateDocumentDoc* CTemplateDocumentView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTemplateDocumentDoc)));
	return (CTemplateDocumentDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTemplateDocumentView message handlers
