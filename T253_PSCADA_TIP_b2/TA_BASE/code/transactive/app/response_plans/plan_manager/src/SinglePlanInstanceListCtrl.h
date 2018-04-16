//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/SinglePlanInstanceListCtrl.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// Displays the current state of a single active plan instance. Used by plan step detail page.
//

#if !defined(AFX_SINGLEPLANINSTANCELISTCTRL_H__17F208A1_3557_4F3F_977E_DD32A929C933__INCLUDED_)
#define AFX_SINGLEPLANINSTANCELISTCTRL_H__17F208A1_3557_4F3F_977E_DD32A929C933__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable: 4786)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CSinglePlanInstanceListCtrl window

    class CSinglePlanInstanceListCtrl : public CListCtrl
    {
    // Construction/destruction
    public:
        CSinglePlanInstanceListCtrl();
        virtual ~CSinglePlanInstanceListCtrl();

    // Operations
    public:
        BOOL SetReadOnly(BOOL readOnly = TRUE);
        void PopulateInstance(const TA_Base_Core::ActivePlanDetail &activePlanDetail);
        void UpdateInstance(const TA_Base_Core::ActivePlanDetail &activePlanDetail);


    // Attributes
    private:


    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CSinglePlanInstanceListCtrl)
        virtual void PreSubclassWindow();
        virtual BOOL PreCreateWindow( CREATESTRUCT& cs );
        // }}AFX_VIRTUAL

        // Generated message map functions
    protected:
        // {{AFX_MSG(CSinglePlanInstanceListCtrl)
        afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
        afx_msg void OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
        // }}AFX_MSG

        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // !defined(AFX_SINGLEPLANINSTANCELISTCTRL_H__17F208A1_3557_4F3F_977E_DD32A929C933__INCLUDED_)
