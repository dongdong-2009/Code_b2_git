// WTSvrTestDoc.cpp : implementation of the CWTSvrTestDoc class
//

#include "stdafx.h"
#include "WTSvrTest.h"

#include "WTSvrTestDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWTSvrTestDoc

IMPLEMENT_DYNCREATE(CWTSvrTestDoc, CDocument)

BEGIN_MESSAGE_MAP(CWTSvrTestDoc, CDocument)
	//{{AFX_MSG_MAP(CWTSvrTestDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWTSvrTestDoc construction/destruction

CWTSvrTestDoc::CWTSvrTestDoc()
{
	// TODO: add one-time construction code here

}

CWTSvrTestDoc::~CWTSvrTestDoc()
{
}

BOOL CWTSvrTestDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CWTSvrTestDoc serialization

void CWTSvrTestDoc::Serialize(CArchive& ar)
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
// CWTSvrTestDoc diagnostics

#ifdef _DEBUG
void CWTSvrTestDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CWTSvrTestDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CWTSvrTestDoc commands
