/**
  * The      code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/radio/AuthorisationCallBannerLibrary/src/AuthorisationDialog.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164

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
        //
        // Reread the text fields from the call entry
        //
        m_type.SetWindowText(        m_callEntry->getAuthorisationType().c_str() );
        m_source.SetWindowText(      m_callEntry->getSource().c_str()            );
        m_destination.SetWindowText( m_callEntry->getDestination().c_str()       );
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
        m_callEntry->dialogAcceptButtonPressed();
        EndWaitCursor();
    }

    void AuthorisationDialog::OnButtonReject() 
    {
        BeginWaitCursor();
        m_callEntry->dialogRejectButtonPressed();
        EndWaitCursor();
    }


    void AuthorisationDialog::OnButtonHelp()
    {
        TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
    }


    void AuthorisationDialog::showErrorMessage(const std::string& message)
    {
		// TD14164++ 
		TA_Base_Bus::TransActiveMessage userMsg;
		CString reason = message.c_str();
		userMsg << reason;
		CString errMsg = userMsg.constructMessage(IDS_UE_020071);
        /*MessageBox(message.c_str(),
                   RunParams::getInstance().get(RPARAM_APPNAME).c_str(),
                   MB_OK|MB_ICONSTOP);*/
        MessageBox(errMsg,
                   RunParams::getInstance().get(RPARAM_APPNAME).c_str(),
                   MB_OK|MB_ICONSTOP);
		// ++TD14164 
    }


    void AuthorisationDialog::showInformationMessage(const std::string& message)
    {
		// TD14164 azenitha++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString reason = message.c_str();
		userMsg << reason;
		CString errMsg = userMsg.constructMessage(IDS_UW_050018);
		/*MessageBox(message.c_str(),
                   RunParams::getInstance().get(RPARAM_APPNAME).c_str(),
                   MB_OK);*/
        MessageBox(errMsg,
                   RunParams::getInstance().get(RPARAM_APPNAME).c_str(),
                   MB_OK);
		// TD14164 ++azenitha
    }


}// TA_IRS_Bus

