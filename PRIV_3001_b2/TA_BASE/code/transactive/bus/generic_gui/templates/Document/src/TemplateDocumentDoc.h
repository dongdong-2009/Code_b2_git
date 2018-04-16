/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/bus/generic_gui/templates/Document/src/TemplateDocumentDoc.h $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  *
  */

#if !defined(AFX_TEMPLATEDOCUMENTDOC_H__D0E5B0FF_ECF2_4F0F_AC0E_F01265E93D30__INCLUDED_)
#define AFX_TEMPLATEDOCUMENTDOC_H__D0E5B0FF_ECF2_4F0F_AC0E_F01265E93D30__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTemplateDocumentDoc : public CDocument
{
protected: // create from serialization only
	CTemplateDocumentDoc();
	DECLARE_DYNCREATE(CTemplateDocumentDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTemplateDocumentDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTemplateDocumentDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTemplateDocumentDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEMPLATEDOCUMENTDOC_H__D0E5B0FF_ECF2_4F0F_AC0E_F01265E93D30__INCLUDED_)
