//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditDataPointCheckPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// This class facilitates editing of parameters associated with a Data Point Check step.

#if !defined(AFX_PLANSTEPEDITDATAPOINTCHECKPAGE_H__50079BA7_CC43_44E4_9213_751F4C010B47__INCLUDED_)
#define AFX_PLANSTEPEDITDATAPOINTCHECKPAGE_H__50079BA7_CC43_44E4_9213_751F4C010B47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#include "resource.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DataPointConfigAccess.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepCommonDef.h"
#include "app/response_plans/plan_step_plugin/common/src/DataPointEnum.h"


namespace TA_Base_App
{
    class DataPointCheckStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditDataPointCheckPage dialog
    class CPlanStepEditDataPointCheckPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditDataPointCheckPage)

    // Construction
    public:
        CPlanStepEditDataPointCheckPage(DataPointCheckStep* step);
        ~CPlanStepEditDataPointCheckPage();


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();
		virtual void updateValue( const std::string& strName, CWnd& refDispWnd );

        void UpdateDataPointValueType();
		void UpdateDataPointValue();

        void ResetDataPointTestTypeDisplay(const std::string& datapointTestType);
		void ResetDataPointValueDisplay(const std::string& strDatapointName, const std::string& strDatapointValue);
        void ResetSuccessAndFailureStepDisplay();

    // Attributes
    private:
        DataPointConfigAccess::EDataPointType m_dataPointType;
		EDataPoint m_dataPointValueType;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditDataPointCheckPage)
        CEdit    m_dataPointValueTextEdit;
        CNumericEdit    m_dataPointValueNumberEdit;
        CComboBox    m_dataPointValueSetCombo;
        CComboBox    m_testTypeCombo;
        CEdit    m_failStepLabel;
        CEdit    m_successStepLabel;
        CEdit    m_dataPointLabel;
        CEdit    m_dataPointTypeLabel;
    // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditDataPointCheckPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditDataPointCheckPage)
        afx_msg void OnPickDataPointButton();
        afx_msg void OnPickFailStepButton();
        afx_msg void OnPickSuccessStepButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

}  // end namespace

#endif // !defined(AFX_PLANSTEPEDITDATAPOINTCHECKPAGE_H__50079BA7_CC43_44E4_9213_751F4C010B47__INCLUDED_)
