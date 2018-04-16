//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/LaunchGuiAppDlg.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_LAUNCHGUIAPPDLG_H__AAC7FD87_5B36_44A5_8F08_4A587EF48B23__INCLUDED_)
#define AFX_LAUNCHGUIAPPDLG_H__AAC7FD87_5B36_44A5_8F08_4A587EF48B23__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/InteractiveStepDlg.h"
#include "app/response_plans/plan_step_plugin/launch_gui_app/common/LaunchGuiAppStepParameter.h"

namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CLaunchGuiAppDlg dialog

    class CLaunchGuiAppDlg : public CInteractiveStepDlg
    {
    // Construction
    public:
		CLaunchGuiAppDlg(const int eType, const unsigned int ulLocation, const unsigned int ulPlanId, 
			const unsigned int ulInstanceNum, const unsigned int ulStepNum, const std::vector<std::string>& vecParameters);
        virtual ~CLaunchGuiAppDlg();

    // Operations
    protected:
        virtual CString getTitlePrefix() const;
        virtual void reportCompletionStatus() const;


    // Dialog Data
    private:
        // {{AFX_DATA(CLaunchGuiAppDlg)
        enum { IDD = IDD_LAUNCH_GUI_APP_DLG };
		CString    m_instance;
		CString    m_plan;
		CString    m_location;
		CString    m_step;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CLaunchGuiAppDlg)
    public:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		virtual BOOL OnInitDialog();
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CLaunchGuiAppDlg)
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
	private:
		LaunchGuiAppStepParameter m_params;
    };

} // namespace TA_App

#endif // !defined(AFX_LAUNCHGUIAPPDLG_H__AAC7FD87_5B36_44A5_8F08_4A587EF48B23__INCLUDED_)
