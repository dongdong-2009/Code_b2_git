/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditStationTisPage.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Facilitates the editing of Station TIS step parameters.
  *
  */

#if !defined(AFX_PLANSTEPEDITSTATIONTISPAGE_H__937BE82B_C769_4426_8160_648D648318BE__INCLUDED_)
#define AFX_PLANSTEPEDITSTATIONTISPAGE_H__937BE82B_C769_4426_8160_648D648318BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"

namespace TA_Base_App
{
    class StationTisStep;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditStationTisPage dialog

    class CPlanStepEditStationTisPage : public CPlanStepEditParameterPage
    {
    // Construction/destruction
    public:
        CPlanStepEditStationTisPage(StationTisStep* step);
        virtual ~CPlanStepEditStationTisPage();

    // Operations
    public:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

    private:
        CPlanStepEditStationTisPage(const CPlanStepEditStationTisPage&);
        CPlanStepEditStationTisPage& operator=(const CPlanStepEditStationTisPage&);

        void UpdatePidList(const std::vector<std::string>& pids);
        void UpdatePriorityCombo(const std::string& strPriority);
        void SetListEnableState(CListCtrl& list, BOOL enabled);
        void SetButtonState(CButton& button, int checkState);
        void SetComboSelection(CComboBox& combo, int item);

    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditStationTisPage)
        CListCtrl    m_pidList;
        CDateTimeCtrl    m_durationPicker;
        CEdit    m_messageEdit;
        CComboBox    m_priorityCombo;
        CComboBox    m_stationCombo;
        CButton    m_stationBtn;
        CButton    m_allStationsBtn;
        CButton    m_pidListBtn;
        CButton    m_pickPidBtn;
        CButton    m_pickMessageBtn;
        // }}AFX_DATA

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditStationTisPage)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditStationTisPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnStationComboSelectionChanged();
        afx_msg void OnStationRadio();
        afx_msg void OnAllStationsRadio();
        afx_msg void OnPidListCheck();
        afx_msg void OnPickPidButton();
        afx_msg void OnPickMessageButton();
        afx_msg void OnPriorityComboSelectionChanged();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}

#endif // !defined(AFX_PLANSTEPEDITSTATIONTISPAGE_H__937BE82B_C769_4426_8160_648D648318BE__INCLUDED_)
