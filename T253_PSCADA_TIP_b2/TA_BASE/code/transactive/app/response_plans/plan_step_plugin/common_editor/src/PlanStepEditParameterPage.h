//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/plan_step_common/src/PlanStepEditParameterPage.h $
// @author:  Andy Parker
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2014/06/03 09:20:18 $
// Last modified by:  $Author: huangjian $
//
// <description>

#if !defined(AFX_PLANSTEPEDITPARAMETERPAGE_H__45479BA5_8ED8_4A8F_965E_250A6A0FAEAB__INCLUDED_)
#define AFX_PLANSTEPEDITPARAMETERPAGE_H__45479BA5_8ED8_4A8F_965E_250A6A0FAEAB__INCLUDED_

#include <afxext.h>
#include <xstring>
#include "app/response_plans/plan_step_plugin/common_editor/src/DllExport.h"

class IActivityParameter;
class CButton;
class CWnd;
class CDateTimeCtrl;

namespace TA_Base_App
{
    class PlanStep;

    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditParameterPage dialog

    class PLAN_COMM_DLL CPlanStepEditParameterPage : public CPropertyPage
    {
        DECLARE_DYNAMIC(CPlanStepEditParameterPage)

    // Construction
    public:
        CPlanStepEditParameterPage(UINT dialogIDD, PlanStep* step); // For derived dialogs
        CPlanStepEditParameterPage(PlanStep* step);
        ~CPlanStepEditParameterPage();

    // Operations
    public:
        PlanStep* GetPlanStep()            { return m_step; }
        void EnableEditing(bool canEdit)   { m_canEdit = canEdit; }
        bool IsEditingEnabled()            { return m_canEdit; }
        void SaveParameters();

    protected:
        virtual void EnableParameterControls(bool enable);
        virtual void DisplayParameters();
        virtual void UpdateParameters();

        virtual void valueChanged( IActivityParameter& refParameter, const std::string& strName, const std::string& strDisplayValue );
        virtual void booleanValueChanged( IActivityParameter& refParameter, const std::string& strName, const bool bNewValue );
        virtual void timeValueChanged( IActivityParameter& refParameter, const std::string& strName, const time_t tmNewValue );
        virtual void getBooleanValue( IActivityParameter& refParameter, const std::string& strName, bool& bCurValue );
        virtual void popupList( IActivityParameter& refParameter, const std::string& strName, const std::string& strTitle, std::string& strDisplayValue );
        virtual void popupTree( IActivityParameter& refParameter, const std::string& strName, const std::string& strTitle, std::string& strDisplayValue );
        virtual void popupGrid( IActivityParameter& refParameter, const std::string& strName, const std::string& strTitle, std::string& strDisplayValue );
        virtual void popupText( IActivityParameter& refParameter, const std::string& strName, const std::string& strTitle, std::string& strDisplayValue );

        virtual void updateValue( const std::string& strName, CWnd& refDispWnd );
        virtual void updateBooleanValue( const std::string& strName, CButton& refDispWnd );
        virtual void updateTimeValue( const std::string& strName, CDateTimeCtrl& refDispWnd );
        virtual void fillCheckbox( const std::string& strName, CButton& refDispWnd );
        virtual void fillTimePicker( const std::string& strName, CDateTimeCtrl& refDispWnd );
        virtual void popupList( const std::string& strName, const std::string& strTitle, CWnd& refDispWnd );
        virtual void popupTree( const std::string& strName, const std::string& strTitle, CWnd& refDispWnd );
        virtual void popupGrid( const std::string& strName, const std::string& strTitle, CWnd& refDispWnd );
        virtual void popupText( const std::string& strName, const std::string& strTitle, CWnd& refDispWnd );

    // Attributes
    private:
        PlanStep* m_step;
        bool m_canEdit;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepEditParameterPage)
            // NOTE - ClassWizard will add data members here.
            // DO NOT EDIT what you see in these blocks of generated code !
        // }}AFX_DATA

    // Overrides
    protected:
        // ClassWizard generate virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditParameterPage)
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepEditParameterPage)
        virtual BOOL OnInitDialog();
        afx_msg void OnDisplayParameters();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

}  // end namespace

#endif // !defined(AFX_PLANSTEPEDITPARAMETERPAGE_H__45479BA5_8ED8_4A8F_965E_250A6A0FAEAB__INCLUDED_)
