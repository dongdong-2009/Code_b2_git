//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL9_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditBackgroundLaunchGuiAppParameterPage.h $
// @author:  Bart Golab
// @version: $Revision: #4 $
//
// Last modification: $DateTime: 2018/01/30 14:25:25 $
// Last modified by:  $Author: hoa.le $
//
// <description>

#if !defined(AFX_PLANSTEPEDITBRACKGROUNDLAUNCHAPPGUIPARAMETERPAGE_H__1F769611_744C_4F51_BAE3_D31208953B3C__INCLUDED_)
#define AFX_PLANSTEPEDITBRACKGROUNDLAUNCHAPPGUIPARAMETERPAGE_H__1F769611_744C_4F51_BAE3_D31208953B3C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_BACKGROUND_LAUNCH_GUI_APP)

#include "PlanStepEditParameterPage.h"
#include "DataPointStepParameter.h"
#include "NumericStepParameter.h"

namespace TA_Base_App
{
    class BackgroundLaunchGuiAppStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditBackgroundLaunchGuiAppParameterPage dialog

    class CPlanStepEditBackgroundLaunchGuiAppParameterPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditBackgroundLaunchGuiAppParameterPage)

        // Construction/destruction
    public:
        CPlanStepEditBackgroundLaunchGuiAppParameterPage(BackgroundLaunchGuiAppStep* step);
        ~CPlanStepEditBackgroundLaunchGuiAppParameterPage();

        // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

        // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditBackgroundLaunchGuiAppParameterPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_BACKGROUND_LAUNCH_GUI_APP_DLG };
        UINT    m_yPositionEdit;
        UINT    m_xPositionEdit;
        CString    m_widthEdit;
        CString    m_heightEdit;

		CSpinButtonCtrl	   m_yPositionSpin;
		CSpinButtonCtrl    m_xPositionSpin;
		CSpinButtonCtrl    m_widthSpin;
		CSpinButtonCtrl    m_heightSpin;

		typedef struct
		{
			UINT min;
			UINT max;
		} Range;

		Range m_yRange;
		Range m_xRange;
		Range m_wRange;
		Range m_hRange;

        CComboBox    m_appTypeCombo;
        CEdit    m_commandLineEdit;
        BOOL    m_triggerEntity;
        CComboBox    m_actionTypeCombo;
        CString    m_dataPointLabel;
        BOOL    m_triggerEntityCheckAction;
        DataPointStepParameter m_dataPointParameter;
        NumericStepParameter m_actionTypeKey;
        CComboBox    m_operatorProfileCombo;
        NumericStepParameter m_operatorProfileParameter;
        // }}AFX_DATA

        // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditBackgroundLaunchGuiAppParameterPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

        // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditBackgroundLaunchGuiAppParameterPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnPickDataPointButton();
        afx_msg void OnTriggerEntityActionCheckClick();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
} // namespace TA_Base_App

#endif // defined (STEPTYPE_BACKGROUND_LAUNCH_GUI_APP)

#endif // !defined(AFX_PLANSTEPEDITBRACKGROUNDLAUNCHAPPGUIPARAMETERPAGE_H__1F769611_744C_4F51_BAE3_D31208953B3C__INCLUDED_)