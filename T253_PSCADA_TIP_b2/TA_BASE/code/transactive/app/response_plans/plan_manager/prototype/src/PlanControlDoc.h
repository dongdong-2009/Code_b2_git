/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanControlDoc.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  */
// PlanControlDoc.h
//

#if !defined(AFX_PLANCONTROLDOC_H__24FA27DA_B3B3_11D5_B614_0078BFB1E967__INCLUDED_)
#define AFX_PLANCONTROLDOC_H__24FA27DA_B3B3_11D5_B614_0078BFB1E967__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPlanControlDoc : public CDocument
{
protected: // create from serialization only
    CPlanControlDoc();
    DECLARE_DYNCREATE(CPlanControlDoc)

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanControlDoc)
    public:
    virtual BOOL OnNewDocument();
    // }}AFX_VIRTUAL

// Implementation
public:
    virtual ~CPlanControlDoc();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    // {{AFX_MSG(CPlanControlDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
        // DO NOT EDIT what you see in these blocks of generated code !
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // AFX_PLANCONTROLDOC_H__24FA27DA_B3B3_11D5_B614_0078BFB1E967__INCLUDED_
