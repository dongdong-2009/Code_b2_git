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

#if !defined(AFX_PLANSTEPEDITDATAPOINTCHECKPAGE_H__92FE4835_990A_47b5_8130_19647311EC56__INCLUDED_)
#define AFX_PLANSTEPEDITDATAPOINTCHECKPAGE_H__92FE4835_990A_47b5_8130_19647311EC56__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DataPointConfigAccess.h"

namespace TA_Base_App
{
    class DataPointTimeCheckStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditDataPointTimeCheckPage dialog
    class CPlanStepEditDataPointTimeCheckPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditDataPointTimeCheckPage)

        // Construction
    public:
        CPlanStepEditDataPointTimeCheckPage(DataPointTimeCheckStep* step);
        ~CPlanStepEditDataPointTimeCheckPage();


        // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();
        void ResetStepDisplay();


        // Attributes
    private:
        DataPointConfigAccess::EDataPointType m_dataPointType;

        // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditDataPointTimeCheckPage)
        CEdit    m_lessThanTimeSpanStepLabel;
        CEdit    m_moreThanTimeSpanStepLabel;
        CEdit    m_dataPointLabel;
        CDateTimeCtrl m_timespanEdit;
        // }}AFX_DATA

        // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditDataPointTimeCheckPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

        // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditDataPointTimeCheckPage)
        afx_msg void OnPickDataPointButton();
        afx_msg void OnPickMoreThanTimeSpanStepButton();
        afx_msg void OnPickLessThanTimeSpanStepButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

}  // end namespace

#endif // !defined(AFX_PLANSTEPEDITDATAPOINTCHECKPAGE_H__92FE4835_990A_47b5_8130_19647311EC56__INCLUDED_)
