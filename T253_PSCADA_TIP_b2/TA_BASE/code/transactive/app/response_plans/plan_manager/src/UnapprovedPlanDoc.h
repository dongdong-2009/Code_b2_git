//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/UnapprovedPlanDoc.h $
// @author:  Bart Golab
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2015/09/15 15:58:53 $
// Last modified by:  $Author: huang.wenbo $
//
// <description>

#if !defined(AFX_UNAPPROVEDPLANDOC_H__49F7A51A_2FE1_477E_B5B0_2406E7F9F160__INCLUDED_)
#define AFX_UNAPPROVEDPLANDOC_H__49F7A51A_2FE1_477E_B5B0_2406E7F9F160__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanTreeDoc.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CUnapprovedPlanDoc document

    class CUnapprovedPlanDoc : public CPlanTreeDoc
    {
    protected:
        CUnapprovedPlanDoc();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CUnapprovedPlanDoc)

    // Operations
    public:
        virtual const std::string &GetRootNodeLabel();

        // ITreeNodeFilter interface
        virtual bool isRelevant(TreeNode &treeNode);


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CUnapprovedPlanDoc)
        // }}AFX_VIRTUAL

    // Implementation
    public:
        virtual ~CUnapprovedPlanDoc();
        // Generated message map functions
    protected:
        // {{AFX_MSG(CUnapprovedPlanDoc)
            // NOTE - the ClassWizard will add and remove member functions here.
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_UNAPPROVEDPLANDOC_H__49F7A51A_2FE1_477E_B5B0_2406E7F9F160__INCLUDED_)
