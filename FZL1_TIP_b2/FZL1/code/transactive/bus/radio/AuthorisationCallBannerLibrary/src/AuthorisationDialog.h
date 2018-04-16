/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationDialog.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  * 
  * This dialog is used by the AuthorisationCallFactory and AuthorisationCallEntry classes.
  * It displays and takes input for information about Authorisation messages.
  */



#ifndef CALL_BANNER_AUTHORISATION_DIALOG_H
#define CALL_BANNER_AUTHORISATION_DIALOG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AuthorisationDialog.h : header file
//

#include <string>


#include "bus/radio/AuthorisationCallBannerLibrary/src/resource.h"



namespace TA_IRS_Bus
{

    // Forward declaration
    class AuthorisationCallEntry;


    /////////////////////////////////////////////////////////////////////////////
    // AuthorisationDialog dialog

    class AuthorisationDialog : public CDialog
    {

    public:
        
        /**
         *  Constructs a new dialog with the given call entry being displayed
         */
        AuthorisationDialog(
            AuthorisationCallEntry* callEntry, CWnd* pParent = NULL);


        /**
         *  Sets the call entry object that is being displayed by this dialog.
         */
        void setCallEntry(AuthorisationCallEntry* callEntry);


        /**
         *  Gets the call entry object that is being displayed by this dialog
         */
        AuthorisationCallEntry& getCallEntry();


        /**
         *  Refreshes the dialog's text fields to reflect what is in the
         *  call entry object.
         */
        void refreshFields();


        
        /**
         *  Shows a message box with the given error message.
         */
        void showErrorMessage(const std::string& message);

        /**
         *  Shows a message box with the given information message.
         */
        void showInformationMessage(const std::string& message);




    // Dialog Data
        //{{AFX_DATA(AuthorisationDialog)
        enum { IDD = IDD_AUTHORISATION };
        CEdit   m_type;
        CEdit   m_source;
        CEdit   m_destination;
    //}}AFX_DATA


    // Overrides
        // ClassWizard generated virtual function overrides
        //{{AFX_VIRTUAL(AuthorisationDialog)
        public:
        virtual BOOL OnInitDialog();
        protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        //}}AFX_VIRTUAL


    // Implementation
    protected:

        // Generated message map functions
        //{{AFX_MSG(AuthorisationDialog)
        afx_msg void OnButtonAccept();
        afx_msg void OnButtonReject();
        afx_msg void OnButtonHelp();
        //}}AFX_MSG
        DECLARE_MESSAGE_MAP()

    private:

        // The call entry whose details are currently being displayed
        AuthorisationCallEntry* m_callEntry;
    };

}// TA_IRS_Bus;


//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // CALL_BANNER_AUTHORISATION_DIALOG_H
