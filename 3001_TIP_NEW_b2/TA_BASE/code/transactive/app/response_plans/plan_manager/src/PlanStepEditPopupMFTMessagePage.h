//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditDataPointCheckPage.h $
// @author:  Andy Parker
// @version: $Revision: #2 $
//
// Last modification: $DateTime: 2012/02/06 16:15:14 $
// Last modified by:  $Author: haijun.li $
//
// This class facilitates editing of parameters associated with a Data Point Check step.

#if !defined(AFX_PLANSTEPEDITDATAPOINTCHECKPAGE_H__77564D38_CECE_41df_B367_1A76A9439E20__INCLUDED_)
#define AFX_PLANSTEPEDITDATAPOINTCHECKPAGE_H__77564D38_CECE_41df_B367_1A76A9439E20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_POPUP_MFT_MESSAGE)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"
#include "PlanStepEditParameterPage.h"
#include "StepNumberStepParameter.h"
#include "NumericStepParameter.h"
#include "TimeSpanStepParameter.h"

namespace TA_Base_App
{
    class PopupMFTMessageStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditPopupMFTMessagePage dialog
    class CPlanStepEditPopupMFTMessagePage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditPopupMFTMessagePage)

        // Construction
    public:
        CPlanStepEditPopupMFTMessagePage(PopupMFTMessageStep* step);
        ~CPlanStepEditPopupMFTMessagePage();


        // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();
        void UpdateDataPointType();
        void ResetStepDisplay();

    private:
        void populateButtonStepParameterMap();
        std::string getWindowsTextAsStdString(UINT controlID);
        void calibrateControlByMessageboxStyle(unsigned long messageboxStyle);
        void setButtonControlVisible(unsigned long controlGroupIdex, bool bVisible);
        std::string getStepDestinationStepDescriptionByButtonLabel(std::string buttonLebal);

        // Attributes
    private:
        StepNumberStepParameter  m_abortStepParameter;
        StepNumberStepParameter  m_retryStepParameter;
        StepNumberStepParameter  m_ignoreStepParameter;
        StepNumberStepParameter  m_okStepParameter;
        StepNumberStepParameter  m_cancelStepParameter;
        StepNumberStepParameter  m_noStepParameter;
        StepNumberStepParameter  m_yesStepParameter;
        StepNumberStepParameter  m_pendingStepParameter;
        NumericStepParameter m_operatorProfileParameter;
        NumericStepParameter m_messageboxStyleParameter;

        TimeSpanStepParameter m_timeSpanParameter;
        CDateTimeCtrl      m_timeSpan;

        // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditPopupMFTMessagePage)
        enum { IDD = IDD_PLAN_STEP_EDIT_POPUP_MFT_MESSAGE_DLG };
        CString    m_decisionMessage;
        CString    m_firstButtonStep;
        CString    m_secondButtonStep;
        CString    m_thirdButtonStep;
        CString    m_pendingStep;
        CComboBox    m_operatorProfileCombo;
        CComboBox    m_messageboxStyleCombo;
        std::map<std::string, StepNumberStepParameter*> m_buttonStepParameterMap;
        // }}AFX_DATA

        // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditPopupMFTMessagePage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

        // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditPopupMFTMessagePage)
        virtual BOOL OnInitDialog();
        afx_msg void OnFirstPickStepButton();
        afx_msg void OnSecondPickStepButton();
        afx_msg void OnThirdPickStepButton();
        afx_msg void OnPendingPickStepButton();
        afx_msg void OnMessageboxStyleChange();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

}  // end namespace

#endif // defined (STEPTYPE_POPUP_MFT_MESSAGE)

#endif // !defined(AFX_PLANSTEPEDITDATAPOINTCHECKPAGE_H__77564D38_CECE_41df_B367_1A76A9439E20__INCLUDED_)
