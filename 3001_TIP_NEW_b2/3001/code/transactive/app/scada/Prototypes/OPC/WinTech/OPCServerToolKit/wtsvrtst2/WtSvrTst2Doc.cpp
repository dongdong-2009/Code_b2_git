// WtSvrTst2Doc.cpp : implementation of the CWtSvrTst2Doc class
//

#include "stdafx.h"
#include "WtSvrTst2.h"

#include "WtSvrTst2Doc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWtSvrTst2Doc

IMPLEMENT_DYNCREATE(CWtSvrTst2Doc, CDocument)

BEGIN_MESSAGE_MAP(CWtSvrTst2Doc, CDocument)
	//{{AFX_MSG_MAP(CWtSvrTst2Doc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWtSvrTst2Doc construction/destruction

CWtSvrTst2Doc::CWtSvrTst2Doc()
{
	// TODO: add one-time construction code here

}

CWtSvrTst2Doc::~CWtSvrTst2Doc()
{
}

BOOL CWtSvrTst2Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CWtSvrTst2Doc serialization

void CWtSvrTst2Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CWtSvrTst2Doc diagnostics

#ifdef _DEBUG
void CWtSvrTst2Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWtSvrTst2Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWtSvrTst2Doc commands
