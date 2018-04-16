/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Product_ECSPlan/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepParameterTisMessageEditDlg.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/07/17 15:33:29 $
  * Last modified by:  $Author: CM $
  *
  * Dialog for selecting a TIS message for the Station/Train TIS steps.
  *
  **/

#if !defined(AFX_PLANSTEPPARAMETERTISMESSAGEEDITDLG_H__21A3606A_6DA2_4703_A271_2F3CF35265B3__INCLUDED_)
#define AFX_PLANSTEPPARAMETERTISMESSAGEEDITDLG_H__21A3606A_6DA2_4703_A271_2F3CF35265B3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "resource.h"
#include "TisConfigAccess.h"
#include "app/response_plans/plan_step_plugin/common_editor/src/PlanStepParameterEditDlg.h"
#include "TisMessageStepParameter.h"

namespace TA_Base_App
{
    //class TisMessageStepParameter;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepParameterTisMessageEditDlg dialog

    class CPlanStepParameterTisMessageEditDlg : public CPlanStepParameterEditDlg
    {
    // Construction/Destruction
    public:
        CPlanStepParameterTisMessageEditDlg(unsigned short libraryVersion, TisMessageStepParameter& parameter, bool canEdit);
        virtual ~CPlanStepParameterTisMessageEditDlg();

    // Operations
    public:
        TisConfigAccess::PredefinedTisMessage GetSelectedMessage() const  { return m_selectedMessage; }

    private:
        CPlanStepParameterTisMessageEditDlg(const CPlanStepParameterTisMessageEditDlg&);
        CPlanStepParameterTisMessageEditDlg& operator=(const CPlanStepParameterTisMessageEditDlg&);

        void SetMsgListItemData(int item, const TisConfigAccess::PredefinedTisMessage& message);
        const TisConfigAccess::PredefinedTisMessage& GetMsgListItemData(int item) const;
		std::string convertToString(const EMessageType messageType);
		std::string convertToString(const TA_Base_Core::ELibrarySection librarySection);

    // Attributes
    private:
        TisMessageStepParameter& m_messageParameter;
        TisConfigAccess::PredefinedTisMessageList m_predefinedMessages;
        TisConfigAccess::PredefinedTisMessage m_selectedMessage;


    // Dialog Data
    private:
        // {{AFX_DATA(CPlanStepParameterTisMessageEditDlg)
        enum { IDD = IDD_PLAN_STEP_PARAMETER_TRAIN_TIS_MESSAGE_EDIT_DLG };
        CButton    m_okBtn;
        CString    m_parameterName;
        CString    m_libraryType;
        UINT    m_destinationLibraryVersion; // station's or train's current library version, might not
        CString    m_messageLibraryVersion;
        CListCtrl    m_messageList;
        // }}AFX_DATA

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepParameterTisMessageEditDlg)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CPlanStepParameterTisMessageEditDlg)
        virtual BOOL OnInitDialog();
        afx_msg void OnMessageListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
        virtual void OnOK();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANSTEPPARAMETERTISMESSAGEEDITDLG_H__21A3606A_6DA2_4703_A271_2F3CF35265B3__INCLUDED_)
