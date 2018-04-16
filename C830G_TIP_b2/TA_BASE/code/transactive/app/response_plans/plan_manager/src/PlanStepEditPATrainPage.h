/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditPATrainPage.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Facilitates the editing of Train PA step parameters.
  *
  */

#if !defined(AFX_PLANSTEPEDITPATRAINPAGE_H__855D836B_008F_46CC_B7F4_51D5F9C7F44B__INCLUDED_)
#define AFX_PLANSTEPEDITPATRAINPAGE_H__855D836B_008F_46CC_B7F4_51D5F9C7F44B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined (STEPTYPE_TRAIN_PUBLIC_ADDRESS)

#include "PlanStepEditParameterPage.h"
#include "PaMessagesDlg.h"
#include "TrainStepParameter.h"

namespace TA_Base_App
{
    class TrainPAStep;
    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditPATrainPage dialog

    class CPlanStepEditPATrainPage : public CPlanStepEditParameterPage
    {


        // Construction
        public:
            CPlanStepEditPATrainPage(TrainPAStep* step);
            virtual ~CPlanStepEditPATrainPage();

        // Operations
        public:
            virtual void DisplayParameters();
            virtual void UpdateParameters();

        private:
            CPlanStepEditPATrainPage(const CPlanStepEditPATrainPage&);
         CPlanStepEditPATrainPage& operator=(const CPlanStepEditPATrainPage&);

            void PopulateTrainGroupsCombo ();

        // Attributes
        private:
            TrainStepParameter        m_trainParameter;
            PaMessageStepParameter    m_messageParameter;

            // Dialog Data
            // {{AFX_DATA(CPlanStepEditPATrainPage)
            enum { IDD = IDD_PLAN_STEP_PA_TRAIN_DLG };
            CComboBox    m_trainsCombo;
            CButton    m_messageBtn;
            CEdit    m_messageTxt;
            CComboBox    m_trainGroupsCombo;
            CDateTimeCtrl    m_durationDTPicker;
            CButton    m_cyclicBtn;
            BOOL    m_cyclic;
            CTime    m_duration;
            int        m_trainGroups;
            CString    m_message;
            int        m_trains;
            // }}AFX_DATA

            // Overrides
            // ClassWizard generated virtual function overrides
            // {{AFX_VIRTUAL(CPlanStepEditPATrainPage)
        protected:
            virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
            // }}AFX_VIRTUAL

            // Implementation
        protected:
            // Generated message map functions
            // {{AFX_MSG(CPlanStepEditPATrainPage)
            virtual BOOL OnInitDialog();
            afx_msg void OnSelchangePaTrainGroupesCombo();
            afx_msg void OnPaTrainMsgDlgBtn();
    // }}AFX_MSG
            DECLARE_MESSAGE_MAP()
    };

}
#endif //define ((STEPTYPE_TRAIN_PUBLIC_ADDRESS)


#endif // !defined(AFX_PLANSTEPEDITPATRAINPAGE_H__855D836B_008F_46CC_B7F4_51D5F9C7F44B__INCLUDED_)
