//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/AllPlanDoc.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2008/11/28 16:26:01 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_ALLPLANDOC_H__A7DD2889_AAFE_4498_9275_8E3674E6B1B7__INCLUDED_)
#define AFX_ALLPLANDOC_H__A7DD2889_AAFE_4498_9275_8E3674E6B1B7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanTreeDoc.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CAllPlanDoc document

    class CAllPlanDoc : public CPlanTreeDoc
    {
    protected:
        CAllPlanDoc();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CAllPlanDoc)

    // Operations
    public:
        virtual const std::string &GetRootNodeLabel();

        // ITreeNodeFilter interface
        virtual bool isRelevant(TreeNode &treeNode);


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CAllPlanDoc)
        // }}AFX_VIRTUAL

    // Implementation
    public:
        virtual ~CAllPlanDoc();
    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif

        // Generated message map functions
    protected:
        // {{AFX_MSG(CAllPlanDoc)
            // NOTE - the ClassWizard will add and remove member functions here.
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_ALLPLANDOC_H__A7DD2889_AAFE_4498_9275_8E3674E6B1B7__INCLUDED_)
