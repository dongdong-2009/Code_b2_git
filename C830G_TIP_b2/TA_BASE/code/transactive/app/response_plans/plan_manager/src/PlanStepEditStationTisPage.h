/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditStationTisPage.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Facilitates the editing of Station TIS step parameters.
  *
  */

#if !defined(AFX_PLANSTEPEDITSTATIONTISPAGE_H__937BE82B_C769_4426_8160_648D648318BE__INCLUDED_)
#define AFX_PLANSTEPEDITSTATIONTISPAGE_H__937BE82B_C769_4426_8160_648D648318BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#include "PlanStepEditParameterPage.h"
#include "TisPidListStepParameter.h"
#include "TisMessageStepParameter.h"
#include "TisPriorityStepParameter.h"


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

        void UpdatePidList();
        void UpdatePriorityCombo();
        void SetListEnableState(CListCtrl& list, BOOL enabled);
        void SetButtonState(CButton& button, int checkState);
        void SetComboSelection(CComboBox& combo, int item);
        void SetStationLibraryVersion();


    // Attributes
    private:
        unsigned short m_stationLibraryVersion;
        TisPidListStepParameter m_pidListParameter;
        TisMessageStepParameter m_messageParameter;
        TisPriorityStepParameter m_priorityParameter;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditStationTisPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_STATION_TIS_DLG };
        CListCtrl    m_pidList;
        CDateTimeCtrl    m_durationPicker;
        CEdit    m_messageEdit;
        CComboBox    m_priorityCombo;
        CComboBox    m_stationCombo;
        CComboBox    m_stationLevelCombo;
        CButton    m_stationBtn;
        CButton    m_allStationsBtn;
        CButton    m_stationLevelBtn;
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
        afx_msg void OnStationLevelCheck();
        afx_msg void OnPidListCheck();
        afx_msg void OnPickPidButton();
        afx_msg void OnPickMessageButton();
        afx_msg void OnPriorityComboSelectionChanged();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}

#endif // defined (STEPTYPE_STATION_TRAVELLER_INFO_SYSTEM)

#endif // !defined(AFX_PLANSTEPEDITSTATIONTISPAGE_H__937BE82B_C769_4426_8160_648D648318BE__INCLUDED_)
