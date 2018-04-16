//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepStationPAPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// <description>

#if !defined(AFX_PLANSTEPSTATIONPAPAGE_H__D1282C18_2991_4F94_B465_26CEA47B582A__INCLUDED_)
#define AFX_PLANSTEPSTATIONPAPAGE_H__D1282C18_2991_4F94_B465_26CEA47B582A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>

#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"
#include "StationPAStep.h"

namespace TA_Base_App
{

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStationPALogPage dialog

    class CPlanStepStationPAPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepStationPAPage)

    // Construction
    public:
        CPlanStepStationPAPage(StationPAStep* step);
        ~CPlanStepStationPAPage();


    // Operations
    protected:
       virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();


     private:
         void PopulateStationsCombo ();
    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStationPALogPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStationPALogPage)
        virtual BOOL OnInitDialog();
         afx_msg void OnStationBtn();
         afx_msg void OnStationChange();
         afx_msg void OnZonesBtn ();
         afx_msg void OnMessagesBtn ();
         afx_msg void OnCyclicBtn ();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    // Dialog Data
    private:
         //CUIntArray m_zoneIds;
		 //PAMessage m_paMessage;
        // {{AFX_DATA(CPlanStationPALogPage)
         CDateTimeCtrl      m_durationTime;
         CDateTimeCtrl      m_interval;
         CComboBox            m_stationCombo;
         CButton                m_stationBtn;
		 CButton                m_allStationBtn;
         CButton                m_cyclicBtn;
         CButton                m_TISSynchBtn;
         CButton                m_chimeBtn;
         CListBox           m_stationZonesLst;
         CEdit                m_messageNameTxt;
         std::map<unsigned long, std::string> m_mapLocationKeyToName;
        // }}AFX_DATA
    };

} // end namespace

#endif // !defined(AFX_PLANSTEPSTATIONPAPAGE_H__D1282C18_2991_4F94_B465_26CEA47B582A__INCLUDED_)
