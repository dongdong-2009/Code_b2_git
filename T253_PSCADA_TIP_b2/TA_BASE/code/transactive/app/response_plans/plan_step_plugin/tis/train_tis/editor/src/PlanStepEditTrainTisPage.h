/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditTrainTisPage.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Facilitates the editing of Train TIS step parameters.
  *
  */

#if !defined(AFX_PLANSTEPEDITTRAINTISPAGE_H__AAA642A2_2026_428C_B77C_8ABDFD0769B4__INCLUDED_)
#define AFX_PLANSTEPEDITTRAINTISPAGE_H__AAA642A2_2026_428C_B77C_8ABDFD0769B4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"

namespace TA_Base_App
{
    class TrainTisStep;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditTrainTisPage dialog

    class CPlanStepEditTrainTisPage : public CPlanStepEditParameterPage
    {
    // Construction/destruction
    public:
        CPlanStepEditTrainTisPage(TrainTisStep* step);
        virtual ~CPlanStepEditTrainTisPage();

    // Operations
    public:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

    private:
        CPlanStepEditTrainTisPage(const CPlanStepEditTrainTisPage&);
        CPlanStepEditTrainTisPage& operator=(const CPlanStepEditTrainTisPage&);

		void UpdatePriorityCombo(const std::string& strPriority);
        void SetButtonState(CButton& button, int checkState);
        void SetComboSelection(CComboBox& combo, int item);

    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditTrainTisPage)
        CDateTimeCtrl    m_durationPicker;
        CEdit    m_messageEdit;
        CComboBox    m_priorityCombo;
        CComboBox    m_trainIdCombo;
        CButton    m_trainIdBtn;
        CButton    m_inboundTrainsBtn;
        CButton    m_outboundTrainsBtn;
        CButton    m_allTrainsBtn;
        CButton    m_pickMessageBtn;
        // }}AFX_DATA

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditTrainTisPage)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditTrainTisPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnTrainComboSelectionChanged();
        afx_msg void OnPriorityComboSelectionChanged();
        afx_msg void OnTrainIdRadio();
        afx_msg void OnTrainGroupRadio();
        afx_msg void OnPickMessageButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}

#endif // !defined(AFX_PLANSTEPEDITTRAINTISPAGE_H__AAA642A2_2026_428C_B77C_8ABDFD0769B4__INCLUDED_)
