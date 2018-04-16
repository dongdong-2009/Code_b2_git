/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/CRadioSendTextPage.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/resource.h"
#include "app/radio/radio_manager_gui_icdv4/src/CRadioManagerDlg.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/CRadioSendTextPage.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/UnicodeCharacterCalculationUtility.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioTextMessageData.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioTextMessageFacade.h"

#include "core/utilities/src/DebugUtil.h"

#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRadioSendTextPage property page

IMPLEMENT_DYNCREATE(CRadioSendTextPage, CPropertyPage)


const int CRadioSendTextPage::MAX_ASCII_CHAR_SET   = 140;
const int CRadioSendTextPage::MAX_CHINESE_CHAR_SET = MAX_ASCII_CHAR_SET / 2;


CRadioSendTextPage::CRadioSendTextPage() 
: CPropertyPage(CRadioSendTextPage::IDD),
  m_bModifyRights(false),
  m_bAddRights(false)
{
	//{{AFX_DATA_INIT(CRadioSendTextPage)
	//}}AFX_DATA_INIT
}

CRadioSendTextPage::~CRadioSendTextPage()
{
    The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PREDEFINEDMSG_ADDED);
    The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PREDEFINEDMSG_MODIFIED);
    The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_PREDEFINEDMSG_REMOVED);
	
}
 
void CRadioSendTextPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRadioSendTextPage)
	DDX_Control(pDX, IDC_CHAR_SET_TEXT, m_staticCharSetStr);
	DDX_Control(pDX, IDC_CHARS_USED, m_staticCharsUsed);
	DDX_Control(pDX, IDC_CHARS_AVAILABLE, m_staticCharsAvailable);
	DDX_Control(pDX, IDC_MSG_SELECTION, m_msgSelectionLB);
	DDX_Control(pDX, IDC_TEXT_EDIT, m_textEditBox);
	DDX_Control(pDX, IDC_SAVE, m_saveButton);
	DDX_Control(pDX, IDC_REMOVE, m_removeButton);
	DDX_Control(pDX, IDC_CANCEL, m_cancelButton);
	DDX_Control(pDX, IDC_ADD, m_addButton);
	DDX_Control(pDX, IDC_PREDEFINED, m_predefinedButton);
	DDX_Control(pDX, IDC_ADHOC, m_adHocButton);
	DDX_Control(pDX, IDC_CLOSE, m_closeButton);
	DDX_Control(pDX, IDC_SEND, m_sendButton);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRadioSendTextPage, CPropertyPage)
	//{{AFX_MSG_MAP(CRadioSendTextPage)
	ON_BN_CLICKED(IDC_ADHOC, onAdHocButtonClicked )
	ON_BN_CLICKED(IDC_PREDEFINED, onPredefinedButtonClicked )
	ON_BN_CLICKED(IDC_ADD, OnAddButtonClicked)
	ON_BN_CLICKED(IDC_CANCEL, OnCancelButtonClicked)
	ON_BN_CLICKED(IDC_CLOSE, OnCloseButtonClicked)
	ON_BN_CLICKED(IDC_REMOVE, OnRemoveButtonClicked)
	ON_BN_CLICKED(IDC_SAVE, OnSaveButtonClicked)
	ON_BN_CLICKED(IDC_SEND, OnSendButtonClicked)
	ON_MESSAGE(WM_PREDEFINEDMSG_ADDED,onNotifiedPredefineMessageAdded)
	ON_MESSAGE(WM_PREDEFINEDMSG_MODIFIED,onNotifiedPredefineMessageModified)
	ON_MESSAGE(WM_PREDEFINEDMSG_REMOVED,onNotifiedPredefineMessageRemoved)
	ON_LBN_SELCHANGE(IDC_MSG_SELECTION, OnSelectionChange)
	ON_EN_CHANGE(IDC_TEXT_EDIT, OnChangeTextEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioSendTextPage message handlers


BOOL CRadioSendTextPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();

	// determine operator privileges
	CRadioManagerDlg* pMainWnd = dynamic_cast<CRadioManagerDlg*>(AfxGetMainWnd());    
	ASSERT_VALID(pMainWnd);
	
	m_bModifyRights = pMainWnd->checkRights(TA_Base_Bus::aca_RADIO_SDS_MODIFY_PREDEFINED);
	m_bAddRights	= pMainWnd->checkRights(TA_Base_Bus::aca_RADIO_SDS_ADD_PREDEFINED);

	setAdHoc(false);

	refreshListBox();
	
	// Display character set used
	CString sText;
	m_staticCharSetStr.GetWindowText(sText);

	UnicodeCharacterCalculationUtility::StringCharacterSetUsed(m_CharSetText);

	sText = sText + " (" + m_CharSetText + ")";

	m_staticCharSetStr.SetWindowText(sText);

	// Display maximum characters available
	int nMaxChar = 0;
	if ( m_CharSetText == "ASCII" )
	{
		nMaxChar = MAX_ASCII_CHAR_SET;
	}
	else
	{
		nMaxChar = MAX_CHINESE_CHAR_SET;
	}

	std::ostringstream sCharsMax;
	sCharsMax << nMaxChar;

	m_textCount = sCharsMax.str().c_str();
	m_staticCharsAvailable.SetWindowText(m_textCount);

	// set text limit, in bytes
	m_textEditBox.SetLimitText(MAX_ASCII_CHAR_SET);

	// register to observer the following user-defined messages
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PREDEFINEDMSG_ADDED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PREDEFINEDMSG_MODIFIED);
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_PREDEFINEDMSG_REMOVED);
	
	//fixed TD14308
	GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDC_SEND)->EnableWindow(FALSE);
	GetDlgItem(IDC_REMOVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
	//fixed TD14308

	return TRUE;
}


void CRadioSendTextPage::onAdHocButtonClicked() 
{
    setAdHoc(true);
}


void CRadioSendTextPage::onPredefinedButtonClicked() 
{
    setAdHoc(false);
	//fixed the potential defect which happends in switch the ad-hoc and predefined button. 
	GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
	GetDlgItem(IDC_CANCEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
	GetDlgItem(IDC_REMOVE)->EnableWindow(FALSE);

	refreshListBox();
	m_msgSelectionLB.SetCurSel(-1);
	setMessageText("");
	//fixed the potential defect which happends in switch the ad-hoc and predefined button. 
}


void CRadioSendTextPage::OnAddButtonClicked() 
{
	CString text;
	m_textEditBox.GetWindowText(text);

	if ( text.GetLength() != 0 )
	{
		std::string sText = text;
		// add the new predefined message to the list
		PredefinedMessage message;
		message.key = 0;
		message.messageText = sText;
		message.shortName = "New Message"; // max 20 chars!!
		message.type = false; //Fixed TD14312

        if (RadioTextMessageFacade::addMessage(message))
		{
			refreshListBox();
			m_msgSelectionLB.SelectString(0, text);
			updateTextCount(); 
		}
		else
		{
			::MessageBox(this->m_hWnd, "Failed to add predefined message", "Transactive", MB_OK|MB_ICONINFORMATION);
		}
	}
	else
    {
		::MessageBox(this->m_hWnd, "Please enter the text message", "Transactive", MB_OK|MB_ICONINFORMATION);
        m_textEditBox.SetFocus();
    }
}

void CRadioSendTextPage::OnCancelButtonClicked() 
{
	// replace the text entered in the m_textEditBox with the text 
	// from the selected predefined message. 


    int nIndex = m_msgSelectionLB.GetCurSel();

    // if something is selected
    if(nIndex != LB_ERR )
    {
		CString text;
		m_msgSelectionLB.GetText(nIndex,text);
        setMessageText(text.GetBuffer(0));
    }
	else
	{
        setMessageText("");
	}
}


void CRadioSendTextPage::OnCloseButtonClicked() 
{
    ::PostMessage(::GetParent(m_hWnd), WM_CLOSE, 0, 0);
}


void CRadioSendTextPage::OnRemoveButtonClicked() 
{
    int nIndex = m_msgSelectionLB.GetCurSel();

    // if something is selected
    if(nIndex != LB_ERR )
    {
		CString text;
		m_msgSelectionLB.GetText(nIndex,text);
	
		// delete text from Message Selection

		std::vector<PredefinedMessage>::iterator iter;

		// delete text from m_predefineMsgList
		for (iter=m_predefinedMsgList.begin(); iter!=m_predefinedMsgList.end(); iter++)
		{
			if ( (*iter).messageText.c_str() == text )
			{
                if ( RadioTextMessageFacade::removeMessage(*iter))
				{
					refreshListBox();
					//m_msgSelectionLB.SetCurSel(-1);
					m_msgSelectionLB.SetCurSel(nIndex - 1);
					//GetDlgItem(IDC_REMOVE)->EnableWindow(FALSE);
					//setMessageText("");
					updateTextCount(); 
				}
				break;
			}
		}
    }
}


void CRadioSendTextPage::OnSaveButtonClicked() 
{
	// It will update the selected predefined message with the text entered in the m_textEditBox. 
    int nIndex = m_msgSelectionLB.GetCurSel();
    // if something is selected
    if(nIndex == LB_ERR ) return;

	CString text;
	m_textEditBox.GetWindowText(text);
	
	// edit text in Listbox
	if ( !text.IsEmpty() )
	{
		CString oldText;
		m_msgSelectionLB.GetText(nIndex, oldText);
		
		// edit text in m_predefineMsgList
		std::vector<PredefinedMessage>::iterator iter;
		
		for (iter=m_predefinedMsgList.begin(); iter!=m_predefinedMsgList.end(); iter++)
		{
			if (oldText.Compare( (*iter).messageText.c_str()) != 0 ) continue; 

			PredefinedMessage message = (*iter);
			message.messageText = text;
			
			if ( RadioTextMessageFacade::modifyMessage(message))
			{
				refreshListBox();
				m_msgSelectionLB.SelectString(0, text);
				updateTextCount(); 
			}
			break;
		}
	}
}


void CRadioSendTextPage::OnSendButtonClicked() 
{
    FUNCTION_ENTRY("OnSendButtonClicked");
    
	CString text;
	m_textEditBox.GetWindowText(text);

	RadioTextMessageData msgData;

	msgData.MessageSource = "";
	msgData.MessageSourceDescription = "";
	msgData.MessageText = text;

	time_t now;
	time(&now);
	msgData.MessageTime = now;


	// check for operator privileges
	CRadioManagerDlg* pMainWnd = dynamic_cast<CRadioManagerDlg*>(AfxGetMainWnd());    
	ASSERT_VALID(pMainWnd);

	bool bHasRights = false;

	if (m_predefinedButton.GetCheck()==1)
		bHasRights = pMainWnd->checkRights(TA_Base_Bus::aca_RADIO_SDS_SEND);
	else
		bHasRights = pMainWnd->checkRights(TA_Base_Bus::aca_RADIO_SDS_EDIT_ADHOC);

	if (bHasRights)
	{
        if (!RadioTextMessageFacade::sendMessage(msgData,m_RadioResource) )
		{
			AfxMessageBox("Failed to send message");
		}
        else
        {
            setMessageText("");
        }
	}

	FUNCTION_EXIT;
}


void CRadioSendTextPage::setRadioResource(RadioResource& resource)
{
	m_RadioResource = resource;	
}


afx_msg LRESULT CRadioSendTextPage::onNotifiedPredefineMessageAdded(WPARAM pdetails, LPARAM unused)
{
    FUNCTION_ENTRY("onNotifiedPredefineMessageAdded");
    
	/*/fixed TD14308, using refreshListBox() instead of the below codes
	PredefinedMessage* predefinedMsg = (PredefinedMessage*)pdetails;

	if ( predefinedMsg != NULL )
	{
		// add text to predefinedMsgList
		m_predefinedMsgList.push_back(*predefinedMsg);

		// add text to Message Selection
		int nIndex = m_msgSelectionLB.GetCount();
		m_msgSelectionLB.InsertString(nIndex, predefinedMsg->messageText.c_str() );

		delete predefinedMsg;
	}
	//*/
	FUNCTION_EXIT;
	return 0;
}


afx_msg LRESULT CRadioSendTextPage::onNotifiedPredefineMessageModified(WPARAM pdetails, LPARAM unused)
{
    FUNCTION_ENTRY("onNotifiedPredefineMessageModified");

	/*/fixed TD14308, using refreshListBox() instead of the below codes
	PredefinedMessage* predefinedMsg = (PredefinedMessage*)pdetails;

	if ( predefinedMsg != NULL )
	{
		// edit text in m_predefineMsgList
		std::vector<PredefinedMessage>::iterator iter;

		int nIndex = 0;
		for (iter=m_predefinedMsgList.begin(); iter!=m_predefinedMsgList.end(); iter++)
		{
			if ((*iter).key == predefinedMsg->key)
			{
				(*iter).messageText = predefinedMsg->messageText;

				// delete text in Message Selection
				m_msgSelectionLB.DeleteString(nIndex);
				m_msgSelectionLB.InsertString(nIndex,predefinedMsg->messageText.c_str());

				break;
			}
			nIndex++;
		}

		delete predefinedMsg;
	}
	//*/
	FUNCTION_EXIT;
	return 0;
}


afx_msg LRESULT CRadioSendTextPage::onNotifiedPredefineMessageRemoved(WPARAM pdetails, LPARAM unused)
{
    FUNCTION_ENTRY("onNotifiedPredefineMessageRemoved");

	/*/fixed TD14308, using refreshListBox() instead of the below codes
	PredefinedMessage* predefinedMsg = (PredefinedMessage*)pdetails;
	if ( predefinedMsg != NULL )
	{
		// delete text in m_predefineMsgList
		std::vector<PredefinedMessage>::iterator iter;

		int nIndex = 0;
		for (iter=m_predefinedMsgList.begin(); iter!=m_predefinedMsgList.end(); iter++)
		{
			if ( (*iter).key == predefinedMsg->key)
			{
				m_predefinedMsgList.erase(iter);

				// delete text in Message Selection
				m_msgSelectionLB.DeleteString(nIndex);

				break;
			}
			nIndex++;
		}
		
		delete predefinedMsg;
	}
	//*/
	
	FUNCTION_EXIT;
	return 0;
}


void CRadioSendTextPage::setAdHoc(bool pEnable)
{
    m_msgSelectionLB.ResetContent();
    setMessageText("");

    if (pEnable)
    {
        m_adHocButton.SetCheck(1);
        m_predefinedButton.SetCheck(0);
        m_addButton.EnableWindow(FALSE);
        m_removeButton.EnableWindow(FALSE);
        m_saveButton.EnableWindow(FALSE);
        m_cancelButton.EnableWindow(FALSE);

        m_msgSelectionLB.EnableWindow(FALSE);
		m_msgSelectionLB.ShowWindow(SW_HIDE);
    }
    else
    {
        m_adHocButton.SetCheck(0);
        m_predefinedButton.SetCheck(1);
        m_cancelButton.EnableWindow(TRUE);
        m_msgSelectionLB.EnableWindow(TRUE);
		m_msgSelectionLB.ShowWindow(SW_SHOW);

		if (m_bAddRights)
			m_addButton.EnableWindow(TRUE);
		else
			m_addButton.EnableWindow(FALSE);

		if (m_bModifyRights)
		{
			m_removeButton.EnableWindow(TRUE);
			m_saveButton.EnableWindow(TRUE);
		}
		else
		{
			m_removeButton.EnableWindow(FALSE);
			m_saveButton.EnableWindow(FALSE);
		}
    }
}


void CRadioSendTextPage::OnSelectionChange() 
{
	// Display selected message on Text area
	int nIndex = m_msgSelectionLB.GetCurSel();
    
	// if something is selected
    if(nIndex != LB_ERR )
    {
		CString text;		
		m_msgSelectionLB.GetText(nIndex,text);
        setMessageText(text.GetBuffer(0));

		//fixed TD14308
		GetDlgItem(IDC_REMOVE)->EnableWindow(TRUE);
		GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CANCEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
		//fixed TD14308
	}
	else
	{
		GetDlgItem(IDC_REMOVE)->EnableWindow(TRUE);
	}
}

void CRadioSendTextPage::OnChangeTextEdit() 
{
    updateTextCount();
}

void CRadioSendTextPage::setMessageText(const std::string& msgText)
{
    m_textEditBox.SetWindowText(msgText.c_str());
    updateTextCount();
}

void CRadioSendTextPage::updateTextCount() 
{
	// get Character Set used
	int nMaxChar;

	if ( m_CharSetText == "ASCII" )
	{
		nMaxChar = MAX_ASCII_CHAR_SET;
	}
	else
	{
		nMaxChar = MAX_CHINESE_CHAR_SET;
	}

	// no. of chars used
	CString text;
	m_textEditBox.GetWindowText(text);

	int nCharsUsed = UnicodeCharacterCalculationUtility::StringSizeInBytes(text);

	// no. of chars available
	int nChars = nMaxChar - nCharsUsed;
	
	if ( nCharsUsed <= nMaxChar)
	{
		std::ostringstream sCharsUsed;
		sCharsUsed << nCharsUsed;

		m_textCount = sCharsUsed.str().c_str();

		m_staticCharsUsed.SetWindowText(m_textCount);

		std::ostringstream sTemp;
		sTemp << nChars;

		m_staticCharsAvailable.SetWindowText(sTemp.str().c_str());
	}

	//fixed TD14308
	if (nCharsUsed == 0 )  // empty text
	{
		GetDlgItem(IDC_SEND)->EnableWindow(FALSE);

		GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CANCEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
		GetDlgItem(IDC_REMOVE)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_SEND)->EnableWindow(TRUE);

		if (m_predefinedButton.GetCheck())// in predefined msg
		{
			int nIndex = m_msgSelectionLB.GetCurSel();

			if(nIndex != LB_ERR )
			{
				//list item selected
				CString strMsg;
				m_textEditBox.GetWindowText(strMsg);
				CString strSelected;
				m_msgSelectionLB.GetText(nIndex, strSelected);

				if (strMsg.Compare(strSelected) == 0)  
				{
					//text not changed. select mode.
					GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
					GetDlgItem(IDC_CANCEL)->EnableWindow(FALSE);
					GetDlgItem(IDC_ADD)->EnableWindow(FALSE);
					GetDlgItem(IDC_REMOVE)->EnableWindow(TRUE);
				}
				else
				{
					//text changed, and size >0. edit mode.
					GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);
					GetDlgItem(IDC_CANCEL)->EnableWindow(TRUE);
					GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
					GetDlgItem(IDC_REMOVE)->EnableWindow(FALSE);
				}
			}
			else //no list item selected, save button be disable.
			{
				GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);

				GetDlgItem(IDC_CANCEL)->EnableWindow(TRUE);
				GetDlgItem(IDC_ADD)->EnableWindow(TRUE);
				GetDlgItem(IDC_REMOVE)->EnableWindow(FALSE);
			}
		}
	}
	//fixed TD14308
}

//fixed TD14308
void CRadioSendTextPage::refreshListBox()
{
	m_predefinedMsgList.clear();
    m_predefinedMsgList = RadioTextMessageFacade::getPredefinedMessageList();
	
	m_msgSelectionLB.ResetContent();

	// populate Message selection
	std::vector<PredefinedMessage>::iterator iter = m_predefinedMsgList.begin();
	for (; iter != m_predefinedMsgList.end(); ++iter)
	{
		if ( (*iter).type ) continue; //Fixed TD14312
		m_msgSelectionLB.AddString((*iter).messageText.c_str());		
	}
}