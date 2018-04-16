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

#if defined (STEPTYPE_DATA_POINT_TIME_CHECK)

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"
#include "PlanStepEditParameterPage.h"
#include "DataPointStepParameter.h"
#include "StepNumberStepParameter.h"
#include "DataPointConfigAccess.h"
#include "TimeSpanStepParameter.h"


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
        void UpdateDataPointType();
        void ResetStepDisplay();


        // Attributes
    private:
        TimeSpanStepParameter m_timespanParameter;
        DataPointStepParameter m_dataPointParameter;
        StepNumberStepParameter m_lessThanTimeSpanParameter;
        StepNumberStepParameter m_moreThanTimeSpanParameter;
        DataPointConfigAccess::EDataPointType m_dataPointType;

        // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditDataPointTimeCheckPage)
        enum { IDD = IDD_PLAN_STEP_EDIT_DATA_POINT_TIME_CHECK_DLG };
        CString    m_lessThanTimeSpanStepLabel;
        CString    m_moreThanTimeSpanStepLabel;
        CString    m_dataPointLabel;
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

#endif // defined (STEPTYPE_DATA_POINT_TIME_CHECK)

#endif // !defined(AFX_PLANSTEPEDITDATAPOINTCHECKPAGE_H__92FE4835_990A_47b5_8130_19647311EC56__INCLUDED_)
