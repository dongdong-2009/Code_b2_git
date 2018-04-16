/**
  * The      code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/3001/transactive/bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationDialog.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/01/24 17:57:59 $
  * Last modified by:  $Author: CM $
  * 
  * This dialog is used by the AuthorisationCallFactory class.
  * It displays and takes input for information about Authorisation Requests.
  */

#ifdef _MSC_VER
	#pragma warning(disable: 4786)	// identifier was truncated to '255' characters in the debug information
#endif

#include "core/utilities/src/RunParams.h"
//#include "bus/radio/AuthorisationCallBannerLibrary/src/stdafx.h"
#include "bus/radio/AuthorisationCallBannerLibrary/src/resource.h"
#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationCallEntry.h"

#include "bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationDialog.h"
#include "bus/generic_gui/src/HelpLauncher.h"

using TA_Base_Core::RunParams;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_Bus
{

    /////////////////////////////////////////////////////////////////////////////
    // AuthorisationDialog dialog


    AuthorisationDialog::AuthorisationDialog(
        AuthorisationCallEntry* callEntry, CWnd* pParent /*=NULL*/)
        :
        CDialog(AuthorisationDialog::IDD, pParent),
        m_callEntry(callEntry)
    {
        TA_ASSERT(callEntry != NULL, "Call Entry must not be NULL");

        //{{AFX_DATA_INIT(AuthorisationDialog)
    //}}AFX_DATA_INIT

        Create(AuthorisationDialog::IDD, pParent);
    }


    void AuthorisationDialog::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(AuthorisationDialog)
    DDX_Control(pDX, IDC_AUTH_EDIT_TYPE, m_type);
    DDX_Control(pDX, IDC_AUTH_EDIT_SOURCE, m_source);
    DDX_Control(pDX, IDC_AUTH_EDIT_DESTINATION, m_destination);
    //}}AFX_DATA_MAP
    }


    BOOL AuthorisationDialog::OnInitDialog()
    {
        CDialog::OnInitDialog();

        refreshFields();

        CenterWindow();

        return TRUE;   // return TRUE unless you set the focus to a control
                       // EXCEPTION: OCX Property Pages should return FALSE
    }

    
    void AuthorisationDialog::setCallEntry(AuthorisationCallEntry* callEntry)
    {
        TA_ASSERT(callEntry != NULL, "Call entry must not be NULL");

        m_callEntry = callEntry;

        refreshFields();
    }


    AuthorisationCallEntry& AuthorisationDialog::getCallEntry()
    {
        return *m_callEntry;
    }


    void AuthorisationDialog::refreshFields()
    {
    }




    BEGIN_MESSAGE_MAP(AuthorisationDialog, CDialog)
        //{{AFX_MSG_MAP(AuthorisationDialog)
        ON_BN_CLICKED(IDC_AUTH_BUTTON_ACCEPT, OnButtonAccept)
        ON_BN_CLICKED(IDC_AUTH_BUTTON_REJECT, OnButtonReject)
        ON_BN_CLICKED(IDHELP, OnButtonHelp)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    /////////////////////////////////////////////////////////////////////////////
    // AuthorisationDialog message handlers


    void AuthorisationDialog::OnButtonAccept() 
    {
        BeginWaitCursor();
//        m_callEntry->dialogAcceptButtonPressed();
        EndWaitCursor();
    }

    void AuthorisationDialog::OnButtonReject() 
    {
        BeginWaitCursor();
//        m_callEntry->dialogRejectButtonPressed();
        EndWaitCursor();
    }


    void AuthorisationDialog::OnButtonHelp()
    {
        TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
    }


    void AuthorisationDialog::showErrorMessage(const std::string& message)
    {
        MessageBox(message.c_str(),
                   RunParams::getInstance().get(RPARAM_APPNAME).c_str(),
                   MB_OK|MB_ICONSTOP);
    }


    void AuthorisationDialog::showInformationMessage(const std::string& message)
    {
        MessageBox(message.c_str(),
                   RunParams::getInstance().get(RPARAM_APPNAME).c_str(),
                   MB_OK);
    }


}// TA_IRS_Bus

