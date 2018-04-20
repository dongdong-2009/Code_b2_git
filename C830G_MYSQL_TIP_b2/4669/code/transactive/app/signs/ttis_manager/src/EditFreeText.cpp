/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/EditFreeText.cpp $
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

#include "stdafx.h"
#include "TTISManager.h"
#include "EditFreeText.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "cots/ssce/sdk/include/sscemfc.hpp"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_IRS_App
{
/////////////////////////////////////////////////////////////////////////////
// CEditFreeText dialog


CEditFreeText::CEditFreeText(CWnd* pParent /*=NULL*/)
    : CDialog(CEditFreeText::IDD, pParent)
{
    FUNCTION_ENTRY( "CEditFreeText" );

    // {{AFX_DATA_INIT(CEditFreeText)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CEditFreeText::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CDialog::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CEditFreeText)
    DDX_Control(pDX, IDC_NAME, m_messageName);
    DDX_Control(pDX, IDC_CONTENT, m_messageContent);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CEditFreeText, CDialog)
    // {{AFX_MSG_MAP(CEditFreeText)
    ON_BN_CLICKED(IDC_SPELL_CHECK, OnSpellCheck)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditFreeText message handlers


BOOL CEditFreeText::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CDialog::OnInitDialog();

    m_messageChanged = false;

    // set the message name
    m_messageName.SetWindowText(m_messageTitle);
    m_messageName.SetLimitText(40);

    // set the message content
    m_messageContent.SetWindowText(m_messageText);

    // disallow invalid characters
    m_messageContent.setDisallowedCharacters(ValidatingEdit::FREE_TEXT_INVALID_CHARACTERS);
    m_messageContent.SetLimitText(ValidatingEdit::MAX_FREE_TEXT_CHARACTERS);

    m_messageContent.SetFocus();

    FUNCTION_EXIT;
    return FALSE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void CEditFreeText::displayDialog(CString messageName, CString messageContent)
{
    FUNCTION_ENTRY( "displayDialog" );

    // set the message name and text
    m_messageTitle = messageName;
    m_messageText = messageContent;

    // pop up the dialog
    DoModal();

    FUNCTION_EXIT;
}


void CEditFreeText::OnSpellCheck()
{
    FUNCTION_ENTRY( "OnSpellCheck" );

    CSentrySpellDlg spellDlg(this);

    spellDlg.SetKey(0x5B39C65E);
    SSCE_SetRegTreeName("HKEY_LOCAL_MACHINE\\Software\\Wintertree\\SSCE");
    spellDlg.Check(m_messageContent);

    FUNCTION_EXIT;
}


void CEditFreeText::OnOK()
{
    FUNCTION_ENTRY( "OnOK" );

    m_messageChanged = false;

    CString message;
    m_messageContent.GetWindowText(message);

    CString messageTitle;
    m_messageName.GetWindowText(messageTitle);

    // if the message content was actually changed
    if ( m_messageText != message )
    {
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
          "Message content changed");
        m_messageText = message;
        m_messageChanged = true;
    }


    // if the message title was actually changed
    if ( m_messageTitle != messageTitle )
    {
        LOG ( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
          "Message title changed");
        m_messageTitle = messageTitle;
        m_messageChanged = true;
    }


    CDialog::OnOK();

    FUNCTION_EXIT;
}


CString CEditFreeText::getMessageContent()
{
    FUNCTION_ENTRY( "getMessageContent" );
    FUNCTION_EXIT;
    return m_messageText;
}


CString CEditFreeText::getMessageName()
{
    FUNCTION_ENTRY( "getMessageName" );
    FUNCTION_EXIT;
    return m_messageTitle;
}


} // TA_IRS_App

