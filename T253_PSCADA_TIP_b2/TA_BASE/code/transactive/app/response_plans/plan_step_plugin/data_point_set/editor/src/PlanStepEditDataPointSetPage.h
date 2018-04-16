//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditDataPointSetPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2013/07/17 15:33:29 $
// Last modified by:  $Author: CM $
//
// This class facilitates editing of parameters associated with a Data Point Set step.

#if !defined(AFX_PLANSTEPEDITDATAPOINTSETPAGE_H__INCLUDED_)
#define AFX_PLANSTEPEDITDATAPOINTSETPAGE_H__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepEditParameterPage.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/DataPointConfigAccess.h"
#include "app/response_plans/plan_step_plugin/common/src/DataPointEnum.h"

namespace TA_Base_App
{
    class DataPointSetStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditDataPointSetPage dialog
    class CPlanStepEditDataPointSetPage : public CPlanStepEditParameterPage
    {
        DECLARE_DYNAMIC(CPlanStepEditDataPointSetPage)

    // Construction
    public:
        CPlanStepEditDataPointSetPage(DataPointSetStep* step);
        ~CPlanStepEditDataPointSetPage();


    // Operations
    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();
        virtual void updateValue( const std::string& strName, CWnd& refDispWnd );

        void UpdateDataPointValueType();
		void UpdateDataPointValue();

		void ResetDataPointValueDisplay(const std::string& strDatapointName, const std::string& strDatapointNewValue);
        
    // Attributes
    private:
        DataPointConfigAccess::EDataPointType m_dataPointType;
		EDataPoint m_dataPointValueType;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditDataPointSetPage)
        CEdit    m_dataPointValueTextEdit;
        CNumericEdit    m_dataPointValueNumberEdit;
        CComboBox    m_dataPointValueSetCombo;
        CEdit    m_dataPointLabel;
        CEdit    m_dataPointTypeLabel;
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditDataPointSetPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditDataPointSetPage)
        afx_msg void OnPickDataPointButton();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

}  // end namespace

#endif // !defined(AFX_PLANSTEPEDITDATAPOINTSETPAGE_H__INCLUDED_)
