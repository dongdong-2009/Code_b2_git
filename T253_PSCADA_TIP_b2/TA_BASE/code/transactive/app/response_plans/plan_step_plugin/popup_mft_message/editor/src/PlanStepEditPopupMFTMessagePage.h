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

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"

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
        std::string getWindowsTextAsStdString(UINT controlID);
        void calibrateControlByMessageboxStyle(unsigned long messageboxStyle);
        void setButtonControlVisible(unsigned long controlGroupIdex, bool bVisible);

		void SetFirstToThirdStep(unsigned long messageboxStyle,const std::string& yesStep,const std::string& noStep,const std::string& abortStep,const std::string& ignoreStep,const std::string& retryStep,const std::string& okStep,const std::string& cancelStep);

        // Attributes
    private:
        CDateTimeCtrl      m_timeSpan;

        // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditPopupMFTMessagePage)
        CEdit   m_decisionMessage;
        CEdit    m_firstButtonStep;
        CEdit    m_secondButtonStep;
        CEdit    m_thirdButtonStep;
        CEdit    m_pendingStep;
        CComboBox    m_operatorProfileCombo;
        CComboBox    m_messageboxStyleCombo;
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
#endif // !defined(AFX_PLANSTEPEDITDATAPOINTCHECKPAGE_H__77564D38_CECE_41df_B367_1A76A9439E20__INCLUDED_)
