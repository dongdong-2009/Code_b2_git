/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/EditFreeText.h $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This is the dialog that pops up when the user clicks the edit
  * button on the train time schedule page. It is used to edit a
  * free text message.
  *
  */

#if !defined(AFX_EDITFREETEXT_H__0329812E_6526_4152_A826_AED879B783D1__INCLUDED_)
#define AFX_EDITFREETEXT_H__0329812E_6526_4152_A826_AED879B783D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ValidatingEdit.h"

/////////////////////////////////////////////////////////////////////////////
// CEditFreeText dialog

namespace TA_IRS_App
{
    class CEditFreeText : public CDialog
    {
    // Construction
    public:
        CEditFreeText(CWnd* pParent = NULL);   // standard constructor

    // Dialog Data
        // {{AFX_DATA(CEditFreeText)
        enum { IDD = IDD_EDIT_FREE_TEXT };
        ValidatingEdit    m_messageContent;
        CEdit    m_messageName;
        // }}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CEditFreeText)
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

    // Implementation
    protected:

        // Generated message map functions
        // {{AFX_MSG(CEditFreeText)
        virtual BOOL OnInitDialog();
    afx_msg void OnSpellCheck();
    // }}AFX_MSG
        DECLARE_MESSAGE_MAP()

    public:


        /**
          * displayDialog
          *
          * Display the free text dialog with the given details.
          *
          * @param messageName  The name of the message, eg "Free Text 1"
          * @param messageContent   The message content itself
          *
          */
        void displayDialog(CString messageName, CString messageContent);

        /**
          * messageWasChanged
          *
          * Test whether the message was actually changed
          *
          * @return true if the message was edited, and save was clicked
          */
        bool messageWasChanged() {return m_messageChanged;}

        /**
          * getMessageName
          *
          * Gets the message name. If it wasn't changed,
          * it will be the same as what was given.
          *
          * @return the message name.
          */
        CString getMessageName();

        /**
          * getMessageContent
          *
          * Gets the message content. If it wasn't changed,
          * it will be the same as what was given.
          *
          * @return the message text.
          */
        CString getMessageContent();

        /**
          * OnOK
          *
          * Called when the save button is clicked.
          * updates the message in the member variable, and the m_messageChanged flag.
          *
          */
        virtual void OnOK();

    private:

        // true if the message content changed,
        // and the user clicked save
        bool m_messageChanged;

        // the message name and title
        CString m_messageTitle;
        CString m_messageText;

    };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // TA_IRS_App

#endif // !defined(AFX_EDITFREETEXT_H__0329812E_6526_4152_A826_AED879B783D1__INCLUDED_)
