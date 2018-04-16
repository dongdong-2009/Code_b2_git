/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditLaunchSchematicParameterPage.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  **/

#if !defined(AFX_PLANSTEPEDITLaunchSchematicPARAMETERPAGE_H__E758CC5E_8F9A_4417_9A28_32AD27FE7798__INCLUDED_)
#define AFX_PLANSTEPEDITLaunchSchematicPARAMETERPAGE_H__E758CC5E_8F9A_4417_9A28_32AD27FE7798__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"


namespace TA_Base_App
{
    class LaunchSchematicStep;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditLaunchSchematicParameterPage dialog

    class CPlanStepEditLaunchSchematicParameterPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditLaunchSchematicParameterPage)

    // Construction/destruction
    public:
        CPlanStepEditLaunchSchematicParameterPage(LaunchSchematicStep* step);
        ~CPlanStepEditLaunchSchematicParameterPage();


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditLaunchSchematicParameterPage)
        UINT    m_screenNumber;
		CEdit   m_screenNumberEdit;
        CSpinButtonCtrl    m_screenNumberSpin;
        CComboBox    m_schematicNameCombo;
        CEdit   m_commandLineTxt;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditLaunchSchematicParameterPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditLaunchSchematicParameterPage)
        virtual BOOL OnInitDialog();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANSTEPEDITLaunchSchematicPARAMETERPAGE_H__E758CC5E_8F9A_4417_9A28_32AD27FE7798__INCLUDED_)
