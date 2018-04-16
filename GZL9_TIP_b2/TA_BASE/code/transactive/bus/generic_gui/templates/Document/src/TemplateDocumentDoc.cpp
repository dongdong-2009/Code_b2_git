/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_gui/templates/Document/src/TemplateDocumentDoc.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  *
  */

#include "stdafx.h"
#include "TemplateDocument.h"

#include "TemplateDocumentDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTemplateDocumentDoc

IMPLEMENT_DYNCREATE(CTemplateDocumentDoc, CDocument)

BEGIN_MESSAGE_MAP(CTemplateDocumentDoc, CDocument)
	//{{AFX_MSG_MAP(CTemplateDocumentDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTemplateDocumentDoc construction/destruction

CTemplateDocumentDoc::CTemplateDocumentDoc()
{
	// TODO: add one-time construction code here

}

CTemplateDocumentDoc::~CTemplateDocumentDoc()
{
}

BOOL CTemplateDocumentDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTemplateDocumentDoc serialization

void CTemplateDocumentDoc::Serialize(CArchive& ar)
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
// CTemplateDocumentDoc diagnostics

#ifdef _DEBUG
void CTemplateDocumentDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTemplateDocumentDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTemplateDocumentDoc commands
