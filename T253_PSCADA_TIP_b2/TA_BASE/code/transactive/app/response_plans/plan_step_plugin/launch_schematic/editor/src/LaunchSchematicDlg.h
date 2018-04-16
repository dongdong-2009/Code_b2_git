/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/LaunchSchematicDlg.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Facilitates the launching of a display when executing a Launch Schematic step.
  *
  */

#if !defined(AFX_LaunchSchematicDLG_H__632F3386_7562_460E_B72C_2357C0311E1B__INCLUDED_)
#define AFX_LaunchSchematicDLG_H__632F3386_7562_460E_B72C_2357C0311E1B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/InteractiveStepDlg.h"
#include "app/response_plans/plan_step_plugin/launch_schematic/common/LaunchSchematicStepParameter.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CLaunchSchematicDlg dialog

    class CLaunchSchematicDlg : public CInteractiveStepDlg
    {
    // Construction
    public:
		CLaunchSchematicDlg(const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
			const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters);
        virtual ~CLaunchSchematicDlg();

    // Operations
    protected:
        virtual CString getTitlePrefix() const;
        virtual void reportCompletionStatus() const;


    // Dialog Data
    private:
        // {{AFX_DATA(CLaunchSchematicDlg)
        enum { IDD = IDD_LAUNCH_SCHEMATIC_DLG };
        CComboBox    m_screenNumberCombo;
		CString    m_instance;
		CString    m_plan;
		CString    m_location;
		CString    m_step;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CLaunchSchematicDlg)
    public:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        virtual BOOL OnInitDialog();
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CLaunchSchematicDlg)
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
	private:
		LaunchSchematicStepParameter m_params;
    };

} // namespace TA_App
#endif // !defined(AFX_LaunchSchematicDLG_H__632F3386_7562_460E_B72C_2357C0311E1B__INCLUDED_)
