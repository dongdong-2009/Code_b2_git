#if !defined(AFX_DECISIONDIALOG_H__65DAB917_BAD3_44CA_9F30_AF2F1411DB08__INCLUDED_)
#define AFX_DECISIONDIALOG_H__65DAB917_BAD3_44CA_9F30_AF2F1411DB08__INCLUDED_

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ecs_manager/src/DecisionDialog.h $
  * @author:  Robert Young
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Derivation of CDialog with the purpose of displaying listbox of possible actions for user
  * to choose from. The users choice is returned by index from DoModal. -1 is used to indicate
  * user has opted to cancel/choose none.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
#include <string>

namespace TA_IRS_App
{
    class DecisionDialog : public CDialog
    {

    public:

        /** 
          * DecisionDialog Constructor
          *
          * Constructs a decision dialog with specified message and
          * list of choices to display to user.
          *
          * @param pParent - The parent CWnd.
          * @param decisionMessage - string to display to user along with list of messages.
          * @param choices - vector of strings corresponding to choices to present user with.
          */
        DecisionDialog( CWnd* pParent, const std::string& decisionMessage, 
            const std::vector< std::string >& choices );

    // Dialog Data
        //{{AFX_DATA(DecisionDialog)
        enum { IDD = IDD_DECISION_DIALOG };
        CStatic m_decisionMessageStatic;
        CStatic m_decisionIconStatic;
        CButton m_oKButton;
        CListBox    m_decisionList;
        //}}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(DecisionDialog)
        public:
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        //{{AFX_MSG(DecisionDialog)
        afx_msg void onCancel();
        afx_msg void onOK();
        afx_msg void onDecisionListSelectionChange();
        virtual BOOL OnInitDialog();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

        std::string m_decisionStr;
        std::vector< std::string > m_choices;
    };
} // namespace TA_IRS_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DECISIONDIALOG_H__65DAB917_BAD3_44CA_9F30_AF2F1411DB08__INCLUDED_)
