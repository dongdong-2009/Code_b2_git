/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/radio_manager_gui/src/SelectTextDialog.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class presents a list of pre-defined text messages the user can select from.
  * It shows the associated text with each pre-defined message and permits it to be
  * altered.
  *
  */


// SelectTextDialog.cpp : implementation file
//

#include "stdafx.h"
#include "RadioManager.h"
#include "RadioManagerDlg.h"
#include "SelectTextDialog.h"
#include "NewTextMessageDlg.h"
#include "core/data_access_interface/radio/src/RAPredefinedSDSMessageAccessFactory.h"
#include "core/data_access_interface/radio/src/RAPredefinedSDSMessage.h"
#include <vector>
#include "core/utilities/src/TAAssert.H"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/RadioAudit_MessageTypes.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectTextDialog dialog


CSelectTextDialog::CSelectTextDialog(CWnd* pParent,
                                        unsigned long radioEntityKey,
                                        CString messageName, 
                                        CString messageText,
                                        CString customSDSName)
	: CDialog(CSelectTextDialog::IDD, pParent), 
	m_radioEntityKey(radioEntityKey),
	m_messageName(messageName),
    m_customSDSMessageName(customSDSName),
    m_isCustom(false)
{
 	FUNCTION_ENTRY("Constructor");

	//{{AFX_DATA_INIT(CSelectTextDialog)
	m_messageText = messageText;
    //}}AFX_DATA_INIT

    // MUST CALL THIS AFTER SETTING m_messageText (above)
    // If the message name is not present, must be a custom message
    setIsCustom(messageName.GetLength() == 0);

	// Create access to the rights library
//	TA_Base_Bus::RightsLibraryFactory rlf;
//  m_rightsLibrary = rlf.buildRightsLibrary();
//	TA_ASSERT(m_rightsLibrary!=NULL,"Rights library failed to create.");
	FUNCTION_EXIT;
}


CSelectTextDialog::~CSelectTextDialog()
{
 	FUNCTION_ENTRY("Destructor");
//	delete m_rightsLibrary;
	FUNCTION_EXIT;
}

void CSelectTextDialog::DoDataExchange(CDataExchange* pDX)
{
 	FUNCTION_ENTRY("Constructor");
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectTextDialog)
	DDX_Control(pDX, IDC_DELETE_TEXT, m_deleteButton);
	DDX_Control(pDX, IDC_UPDATE_TEXT, m_updateButton);
	DDX_Control(pDX, IDC_NEW_TEXT, m_newButton);
	DDX_Control(pDX, IDC_MESSAGELIST, m_TextList);
    DDX_Control(pDX, IDC_MESSAGETEXT, m_messageTextEdit);
	DDX_Text(pDX, IDC_MESSAGETEXT, m_messageText);
	DDV_MaxChars(pDX, m_messageText, 140);
	//}}AFX_DATA_MAP
	FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CSelectTextDialog, CDialog)
	//{{AFX_MSG_MAP(CSelectTextDialog)
	ON_BN_CLICKED(IDC_FREETEXT, OnFreetext)
	ON_LBN_SELCHANGE(IDC_MESSAGELIST, onSelChangeMessageList)
	ON_BN_CLICKED(IDC_NEW_TEXT, onNewText)
	ON_BN_CLICKED(IDC_UPDATE_TEXT, onUpdateText)
	ON_BN_CLICKED(IDC_DELETE_TEXT, onDeleteText)
	ON_EN_CHANGE(IDC_MESSAGETEXT, onChangeMessageText)
	ON_BN_CLICKED(IDOK, onApply)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectTextDialog message handlers


BOOL CSelectTextDialog::OnInitDialog() 
{
 	FUNCTION_ENTRY("OnInitDialog");

	CDialog::OnInitDialog();

	// Set some messages in the list
	LoadPredefinedList();

	// Set specified message
	if (!m_messageName.IsEmpty())
	{
		m_TextList.SelectString(-1,m_messageName);
        onSelChangeMessageList();
	}
	CheckDlgButton(IDC_FREETEXT,BST_CHECKED);
	OnFreetext();

	// Enable buttons
	EnableButtons();

	FUNCTION_EXIT;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSelectTextDialog::onSelChangeMessageList() 
{
 	FUNCTION_ENTRY("onSelChangeMessageList");
    static const char errorStr[] = "Error retrieving Predefined SDS Message";

	if (m_TextList.GetCurSel()!=LB_ERR)
	{
		std::string messageName = getSelectedPredefinedSDS();
		TA_ASSERT (!messageName.empty(),"No pre-defined message selected.")
        bool textUpdated = false;

		// Get the message associated with the pre-defined item selected
        try
        {
		    TA_Base_Core::IRAPredefinedSDSMessage* sdsMessage = TA_Base_Core::RAPredefinedSDSMessageAccessFactory::getInstance().getByShortName(messageName);
		    TA_ASSERT(sdsMessage!=NULL,"Predefined SDS message not created.");
    
		    m_messageText = sdsMessage->getMessage().c_str();
		    this->UpdateData(false);
    
		    delete sdsMessage;

            textUpdated = true;
        }
        SILENT_RM_CATCH_HANDLER(errorStr);        

        if (!textUpdated)
        {
			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString actionName = "retrieving Predefined";
			userMsg << actionName;
			CString errMsg = userMsg.constructMessage(IDS_UW_632027);
            /*RadioManagerErrorHandler::getInstance().AfxMessageBox(errorStr);*/
			RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
			// ++ TD14164 
        }

		// Re-check if buttons should be enabled
        setIsCustom(false);
		
        EnableButtons();
	}
	FUNCTION_EXIT;
}

void CSelectTextDialog::onNewText() 
{
 	FUNCTION_ENTRY("onNewText");

	CNewTextMessageDlg   dlg;

    // TES #472, continue bringing dialog back until cancel is hit, or valid name entered
	while (dlg.DoModal() == IDOK)
	{
		UpdateData(true);

		std::string  messageName = static_cast<LPCSTR>(dlg.m_SDSShortName);
		std::string  messageText = static_cast<LPCSTR>(m_messageText);

        if (strcmp(m_customSDSMessageName.GetBuffer(0), messageName.c_str()) == 0)
        {
			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString errMsg = userMsg.constructMessage(IDS_UW_632028);
            /*RadioManagerErrorHandler::getInstance().AfxMessageBox(
                    "This message name is reserved for system use.  Please try another.");*/
			RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
			// ++ TD14164 

            // TES #472, bring up name entry dialog for user again
			continue;
        }

		// Check pre-defined message does not already exist (is it in the list?)
		if (m_TextList.FindStringExact(0,messageName.c_str())!=CB_ERR)
		{
			// Duplicate name selected

			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString fieldName = "Pre-defined message name";
			userMsg << fieldName;
			CString errMsg = userMsg.constructMessage(IDS_UW_632029);
			/*RadioManagerErrorHandler::getInstance().AfxMessageBox("Pre-defined message name already exists.");*/
			RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
			// ++ TD14164 

			// TES #472, bring up name entry dialog for user again
			continue;
		}

        try
        {
    		// Save the new entry
    		TA_Base_Core::RAPredefinedSDSMessage sdsMessage(messageName,messageText,true);
    		sdsMessage.applyChanges();    
        }
        catch (...)
        {
			// TD14164 ++
			TA_Base_Bus::TransActiveMessage userMsg;
			CString actionName = "saving";
			userMsg << actionName;
			CString errMsg = userMsg.constructMessage(IDS_UW_632027);
            /*RadioManagerErrorHandler::getInstance().AfxMessageBox("Error saving SDS Message");*/
			RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
			// ++ TD14164 

            // TES #472, bring up name entry dialog for user again
            continue;
        }

    	// Add to list
       	m_TextList.AddString(messageName.c_str());
    	m_TextList.SelectString(-1,messageName.c_str());
        // This is invoked when doing the manual selection change above (cause of TD #3113)
        onSelChangeMessageList();
    
    	m_messageName = messageName.c_str();
    
		// Re-check if buttons should be enabled
		EnableButtons();

    	// Audit the addition
        try
        {
    	    TA_Base_Core::DescriptionParameters parameters;
    	    TA_Base_Core::NameValuePair  param("MessageName", messageName );
    	    parameters.push_back(&param);
    	    AuditOperation(TA_Base_Core::RadioAudit::RadioPredefinedMessageCreated,parameters);
        }
        SILENT_RM_CATCH_HANDLER("Failed to submit audit message for SDS Message");
        
        // Applied new name, break immediately
        break;
	}
	FUNCTION_EXIT;
}

// Save the update text as the new pre-defined text
void CSelectTextDialog::onUpdateText() 
{
 	FUNCTION_ENTRY("onUpdateText");

    try
    {
    	std::string messageName = getSelectedPredefinedSDS();
    	TA_ASSERT (!messageName.empty(),"No pre-defined message selected.")
    
    	// Confirm update is required
    	const char * messageFormat = "Are you sure you wish to save changes to message \"%s\"?";
    	char messageText[255] = {0};
    	TA_ASSERT(sizeof(messageText)>(sizeof(*messageFormat)+messageName.size()),"Buffer too small for message.");
    	sprintf(messageText,messageFormat,messageName.c_str());
    	
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName = "save changes to";
		CString fieldName = messageName.c_str();
		userMsg << actionName;
		userMsg << fieldName;
		CString errMsg = userMsg.constructMessage(IDS_UW_632030);
		// ++ TD14164 

		// TD14164 ++
		/*if (RadioManagerErrorHandler::getInstance().AfxMessageBox(messageText,MB_YESNO)!=IDYES)*/
		if (RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg,MB_YESNO)!=IDYES)
		// ++ TD14164 
    	{
    		FUNCTION_EXIT;
    		return;
    	}
    
    	// Get the message associated with the pre-defined item selected
    	TA_Base_Core::IRAPredefinedSDSMessage* sdsMessage = TA_Base_Core::RAPredefinedSDSMessageAccessFactory::getInstance().getByShortName(messageName);
    	TA_ASSERT(sdsMessage!=NULL,"Predefined SDS message not created.");
    
    	// Get the new text
    	UpdateData(true);
    	sdsMessage->setMessage(std::string(m_messageText)); 
    
    	// Update the database
    	sdsMessage->applyChanges();
    
    	delete sdsMessage;
    
    	// Re-check if buttons should still be enabled
        setIsCustom(false);
    	EnableButtons();
    
    	// Audit the event
    	TA_Base_Core::DescriptionParameters parameters;
    	TA_Base_Core::NameValuePair param1("MessageName", messageName );
    	TA_Base_Core::NameValuePair param2("MessageText", std::string((LPCSTR)m_messageText));
    	parameters.push_back(&param1);
    	parameters.push_back(&param2);
    	AuditOperation(TA_Base_Core::RadioAudit::RadioPredefinedMessageUpdated,parameters);
    }
    catch (...)
    {
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName = "updating";
		userMsg << actionName;
		CString errMsg = userMsg.constructMessage(IDS_UW_632027);
        /*RadioManagerErrorHandler::getInstance().AfxMessageBox("Error updating SDS Message");*/
		RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
		// ++ TD14164 
    }

	FUNCTION_EXIT;
}

void CSelectTextDialog::onDeleteText() 
{
 	FUNCTION_ENTRY("onDeleteText");

    try
    {
    	std::string messageName = getSelectedPredefinedSDS();
    	TA_ASSERT (!messageName.empty(),"No pre-defined message selected.")
    
    	// Confirm delete is required
    	const char * messageFormat = "Are you sure you wish to delete message \"%s\"?";
    	char messageText[255] = {0};
    	TA_ASSERT(sizeof(messageText)>(sizeof(*messageFormat)+messageName.size()),"Buffer too small for message.");
    	sprintf(messageText,messageFormat,messageName.c_str());

		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName = "delete";
		CString fieldName = messageName.c_str();
		userMsg << actionName;
		userMsg << fieldName;
		CString errMsg = userMsg.constructMessage(IDS_UW_632030);
		// ++ TD14164 
    	
		// TD14164 ++
		/*if (RadioManagerErrorHandler::getInstance().AfxMessageBox(messageText,MB_YESNO)!=IDYES)*/
		if (RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg,MB_YESNO)!=IDYES)
		// ++ TD14164 
    	{
    		FUNCTION_EXIT;
    		return;
    	}
    
    	// Get the message associated with the pre-defined item selected
    	TA_Base_Core::IRAPredefinedSDSMessage* sdsMessage = TA_Base_Core::RAPredefinedSDSMessageAccessFactory::getInstance().getByShortName(messageName);
    	TA_ASSERT(sdsMessage!=NULL,"Predefined SDS message not created.");
    
    	// Delete it
    	TA_Base_Core::RAPredefinedSDSMessageAccessFactory::getInstance().deleteMessage(sdsMessage);
    	delete sdsMessage;
    
    	// Delete selected message
    	int position = m_TextList.GetCurSel();
    	TA_ASSERT(position!=LB_ERR,"No message selected");
    	m_TextList.DeleteString(position);
		m_messageText.Empty();
		UpdateData(false);

    	// Audit the event
    	TA_Base_Core::DescriptionParameters parameters;
    	TA_Base_Core::NameValuePair  param("MessageName", messageName );
    	parameters.push_back(&param);
    	AuditOperation(TA_Base_Core::RadioAudit::RadioPredefinedMessageDeleted,parameters);
    
    	// Re-check if buttons should still be enabled
    	EnableButtons();
    }
    catch (...)
    {
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName = "deleting";
		userMsg << actionName;
		CString errMsg = userMsg.constructMessage(IDS_UW_632027);
        /*RadioManagerErrorHandler::getInstance().AfxMessageBox("Error deleting SDS Message");*/
		RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
		// ++ TD14164 
    }

	FUNCTION_EXIT;
}

void CSelectTextDialog::OnFreetext() 
{
 	FUNCTION_ENTRY("OnFreetext");

	BOOL checked = IsDlgButtonChecked(IDC_FREETEXT);

	// Enable edit box when free text checked
    // TES #485 - set read only state (so text doesn't grey out)
    m_messageTextEdit.SetReadOnly(!checked);
	//GetDlgItem(IDC_MESSAGETEXT)->EnableWindow(checked);

    // Disable select box when free text checked
	GetDlgItem(IDC_MESSAGELIST)->EnableWindow(!checked);

	// Re-check if buttons should still be enabled
	EnableButtons();

	FUNCTION_EXIT;
}


void CSelectTextDialog::onChangeMessageText() 
{
 	FUNCTION_ENTRY("onChangeMessageText");

    bool customLast = m_isCustom;

    // Collect text changes
    UpdateData(TRUE);

    // Only flag as custom message if text has changed from original text
    // (or no message name selected)
    if (getSelectedPredefinedSDS().size() == 0 ||
            m_messageText != m_originalMessageText)
    {
        setIsCustom(true);
    }
    else
    {
        // Not custom text - no changes in string
        setIsCustom(false);
    }
	
    if (customLast != m_isCustom)
    {
        EnableButtons();
	}

	FUNCTION_EXIT;
}


//******************
// Private Methods *
//******************

void CSelectTextDialog::LoadPredefinedList()
{
 	FUNCTION_ENTRY("LoadPredefinedList");

    try
    {
        std::vector<TA_Base_Core::IRAPredefinedSDSMessage*> sdsList = TA_Base_Core::RAPredefinedSDSMessageAccessFactory::getInstance().getList(false);
	    while (sdsList.size()>0)
	    {
       		m_TextList.AddString(sdsList.at(0)->getShortName().c_str());
		    delete sdsList.at(0);
		    sdsList.erase(sdsList.begin());  // delete them as we go through
	    }
    }
    catch (...)
    {
		// TD14164 ++
		TA_Base_Bus::TransActiveMessage userMsg;
		CString actionName = "retrieving";
		userMsg << actionName;
		CString errMsg = userMsg.constructMessage(IDS_UW_632027);
        /*RadioManagerErrorHandler::getInstance().AfxMessageBox("Error retrieving SDS Messages");*/
		RadioManagerErrorHandler::getInstance().AfxMessageBox(errMsg);
		// ++ TD14164 
    }
    
	FUNCTION_EXIT;
}


bool CSelectTextDialog::IsOperationPermitted(TA_Base_Bus::AccessControlledAction action)
{
 	FUNCTION_ENTRY("IsOperationPermitted");

	// Get our session - needed for rights check
    CRadioManagerDialog* manager = dynamic_cast<CRadioManagerDialog*>(GetParent());
	bool enabled = manager->isFunctionPermitted(action);

	FUNCTION_EXIT;
    return enabled;
}


void CSelectTextDialog::AuditOperation(const TA_Base_Core::MessageType& messageType,
										TA_Base_Core::DescriptionParameters parameters)
{
 	FUNCTION_ENTRY("AuditOperation");

    try
    {
    	// Get our session - needed for rights check
        std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
        TA_ASSERT(!sessionId.empty(), "SessionId parameter is not set");
    
    	// Always add operator name - this should be done by sender in the future
    	//TA_Base_Core::ISession* session = TA_Base_Core::SessionAccessFactory::getInstance().getSession(sessionId);
    	//TA_Base_Core::NameValuePair operatorNameDescriptionPair("OperatorName", session->getOperatorName());
        //parameters.push_back(&operatorNameDescriptionPair);
    	//delete session;
    
    	TA_Base_Core::AuditMessageSender*  auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(
    					                          TA_Base_Core::RadioAudit::Context);
    	TA_ASSERT(auditMessageSender!=NULL, "Cannot create audit message sender");
    
    	auditMessageSender->sendAuditMessage(messageType,m_radioEntityKey,parameters,
    											"",				// No extra details
    											sessionId,
    											"",				// No associated alarm
    											"",				// No associated incident
    											"",				// No parent event
    											NULL);
    }
    SILENT_RM_CATCH_HANDLER("CSelectedTextDialog: Error submitting audit message");

	FUNCTION_EXIT;
}


void CSelectTextDialog::EnableButtons()
{
 	FUNCTION_ENTRY("EnableButtons");

	bool isMessageSelected = (m_TextList.GetCurSel()!=LB_ERR);
	BOOL isFreeTextButtonChecked = IsDlgButtonChecked(IDC_FREETEXT);

	// Setup the button security
	m_newButton.EnableWindow(isFreeTextButtonChecked &&
                                IsOperationPermitted(TA_Base_Bus::aca_RADIO_ADD_PREDEFINED_TEXT));

    // TES #484: Edit button should be disabled when free text button unchecked
	m_updateButton.EnableWindow(isFreeTextButtonChecked && 
                                    m_isCustom && 
                                    isMessageSelected && 
                                    IsOperationPermitted(TA_Base_Bus::aca_RADIO_MODIFY_PREDEFINED_TEXT));

	m_deleteButton.EnableWindow(isMessageSelected &&
                                    IsOperationPermitted(TA_Base_Bus::aca_RADIO_MODIFY_PREDEFINED_TEXT));
	FUNCTION_EXIT;
}

std::string CSelectTextDialog::getSelectedPredefinedSDS()
{
 	FUNCTION_ENTRY("getSelectedPredefinedSDS");

	CString selectedMessage;

	int position = m_TextList.GetCurSel();
	if (position!=LB_ERR)
	{
		m_TextList.GetText(position,selectedMessage);
	}

	FUNCTION_EXIT;
	return std::string(selectedMessage);
}


std::string CSelectTextDialog::getPredefinedSDSText(CString messageName)
{
	// Get the message associated with the pre-defined item selected
    try
    {
	    TA_Base_Core::IRAPredefinedSDSMessage* sdsMessage = TA_Base_Core::RAPredefinedSDSMessageAccessFactory::getInstance().getByShortName(static_cast<LPCSTR>(messageName));
	    TA_ASSERT(sdsMessage!=NULL,"Predefined SDS message not created.");
	    std::string messageText = sdsMessage->getMessage();
	    delete sdsMessage;

	    return messageText;
    }
    SILENT_RM_CATCH_HANDLER("CSelectTextDialog::getPredefinedSDSText");   
    return ""; 
}


void CSelectTextDialog::loadPredefinedMessageCombo(CComboBox& combo)
{
	// Remove any existing choices
	combo.ResetContent();
    
    try
    {
        // Add the choices
        std::vector<TA_Base_Core::IRAPredefinedSDSMessage*> sdsList = TA_Base_Core::RAPredefinedSDSMessageAccessFactory::getInstance().getList(false);
    	while (sdsList.size()>0)
    	{
       		combo.AddString(sdsList.at(0)->getShortName().c_str());
    		delete sdsList.at(0);
    		sdsList.erase(sdsList.begin());  // delete them as we go through
    	}
    }
    SILENT_RM_CATCH_HANDLER("CSelectTextDialog::loadPredefinedMessageCombo");    

}


void CSelectTextDialog::onApply() 
{
	// Save selected name
	m_messageName = getSelectedPredefinedSDS().c_str();
	UpdateData(true);

	// Normal exit
	EndDialog(IDOK);
	
}


void CSelectTextDialog::setIsCustom(bool isCustom)
{
    if (!isCustom)
    {
        m_originalMessageText = m_messageText;
    }

    m_isCustom = isCustom;
}
	
