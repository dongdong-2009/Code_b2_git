//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanEditorDoc.h $
// @author:  Bart Golab
// @version: $Revision: #8 $
//
// Last modification: $DateTime: 2015/09/15 15:58:53 $
// Last modified by:  $Author: huang.wenbo $
//
// Plan Editor GUI is based on an SDI. This class is a representation of the document associated with the
// plan editor frame and its plan node view.

#if !defined(AFX_PLANEDITORDOC_H__B252484F_15AB_4A84_9B40_AF6117E95A2B__INCLUDED_)
#define AFX_PLANEDITORDOC_H__B252484F_15AB_4A84_9B40_AF6117E95A2B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanManagerCommonDefs.h"
#include "PlanBrowserDoc.h"
#include "PlanNode.h"


namespace TA_Base_App
{
    class PlanStep;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanEditorDoc document

    class CPlanEditorDoc : public CPlanBrowserDoc
    {
    protected:
        CPlanEditorDoc();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CPlanEditorDoc)

    // Operations
    public:
        virtual void SetCurrentNode(TreeNode *node);
        virtual bool EditingEnabled() const;


	private:
        bool isEnable(PlanStep& planStep, const int nAction);

    // Attributes
    private:
        typedef enum { SCA_CUT_STEP, SCA_COPY_STEP, SCA_NO_ACTION } EStepCopyAction;

        EStepCopyAction m_copyStepAction;
        PlanStepList m_copyStepList;


    // Overrides
    public:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanEditorDoc)
        virtual BOOL OnNewDocument();
        // }}AFX_VIRTUAL

    // Implementation
    public:
        virtual ~CPlanEditorDoc();
        virtual BOOL DoFileSave();


    // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanEditorDoc)
        afx_msg void OnUpdateCmdUI(CCmdUI* pCmdUI);
        afx_msg void OnUpdatePlanSave(CCmdUI* pCmdUI);
		afx_msg void OnUpdatePlanUpgrade(CCmdUI* pCmdUI);
        afx_msg void OnPlanSave();
		afx_msg void OnPlanUpgrade();
        afx_msg void OnStepEdit();
        afx_msg void OnStepDelete();
        afx_msg void OnStepNew();
        afx_msg void OnStepCopy();
        afx_msg void OnStepCut();
        afx_msg void OnStepPaste();
        afx_msg void OnFileExit();
        afx_msg void OnUpdateFlowCharEditorCmdUI(CCmdUI* pCmdUI);
		afx_msg void OnFlowCharEditorCmd(UINT id);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };


    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANEDITORDOC_H__B252484F_15AB_4A84_9B40_AF6117E95A2B__INCLUDED_)
