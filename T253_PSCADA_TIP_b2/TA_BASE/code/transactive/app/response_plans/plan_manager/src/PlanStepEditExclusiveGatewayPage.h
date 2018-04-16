//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditActivePlanCheckPage.h $
// @author:  Andy Parker
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2012/02/06 16:15:14 $
// Last modified by:  $Author: haijun.li $
//
// <description>

#if !defined(AFX_PLANSTEPEDITEXCLUSIVEGATEWAYPAGE_H__7EE45DFB_6A62_4646_B92B_681BC7AC402C__INCLUDED_)
#define AFX_PLANSTEPEDITEXCLUSIVEGATEWAYPAGE_H__7EE45DFB_6A62_4646_B92B_681BC7AC402C__INCLUDED_

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"

class GridEditorPage;

namespace TA_Base_App
{
    class ExclusiveGatewayStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditExclusiveGatewayPage dialog

    class CPlanStepEditExclusiveGatewayPage : public CPlanStepEditParameterPage
    {
    // Construction
    public:
        CPlanStepEditExclusiveGatewayPage( ExclusiveGatewayStep* parameter );   // standard constructor
        virtual ~CPlanStepEditExclusiveGatewayPage();   // standard desstructor

    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditExclusiveGatewayPage)
        GridEditorPage* m_pNextStepGrid;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditExclusiveGatewayPage)
        afx_msg virtual void DoDataExchange( CDataExchange* pDX );    // DDX/DDV support
        afx_msg virtual BOOL OnInitDialog();
        afx_msg virtual void OnSize( UINT nType, int cx, int cy );
        afx_msg virtual void OnDestroy();
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditExclusiveGatewayPage)
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}

#endif // !defined(AFX_PLANSTEPEDITEXCLUSIVEGATEWAYPAGE_H__7EE45DFB_6A62_4646_B92B_681BC7AC402C__INCLUDED_)
