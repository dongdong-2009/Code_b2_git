/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/4677_TIP/4677/transactive/app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/CRadioInboxPage.cpp $
 * @author:  F. Stuart
 * @version: $Revision: #2 $
 *
 * Last modification: $DateTime: 2009/12/15 19:31:45 $
 * Last modified by:  $Author: grace.koh $
 * 
 */

#include "app/radio/radio_manager_gui_icdv4/src/stdafx.h"
#include "app/radio/radio_manager_gui_icdv4/src/resource.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioTextMessagingDialog/CRadioInboxPage.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioUserDefinedEvents.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioTextMessageFacade.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioOperations/RadioOperations.h"
#include "app/radio/radio_manager_gui_icdv4/src/RadioHelper.h"
#include "app/radio/radio_mft_agent_icdv4/src/RadioMFTAgentHelper.h"
#include "core/message/types/RadioAudit_MessageTypes.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/src/NameValuePair.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/RadioComms_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "bus/radio/radio_agent/IDL/src/RadioServerAgentIDL.h"
#include "bus/radio/radio_agent/IDL/src/CommonRadioAgentIDL.h"

#include <sstream>
#include <iomanip> 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRadioInboxPage property page

IMPLEMENT_DYNCREATE(CRadioInboxPage, CPropertyPage)

CRadioInboxPage::CRadioInboxPage() : CPropertyPage(CRadioInboxPage::IDD),m_commsSender(NULL)
{
	//{{AFX_DATA_INIT(CRadioInboxPage)
	//}}AFX_DATA_INIT
	
	The_RadioOperations::instance()->m_observerHelper.addChildObserver(this,WM_TEXTMSG_RECEIVED);
}

CRadioInboxPage::~CRadioInboxPage()
{
	The_RadioOperations::instance()->m_observerHelper.removeChildObserver(this,WM_TEXTMSG_RECEIVED);

	if (m_commsSender != NULL)
	{
		delete m_commsSender;
		m_commsSender = NULL;
    }
}

void CRadioInboxPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRadioInboxPage)
	DDX_Control(pDX, IDC_MSG_TEXT, m_msgTextEL);
	DDX_Control(pDX, IDC_MSG_HISTORY, m_msgHistoryLB);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRadioInboxPage, CPropertyPage)
	//{{AFX_MSG_MAP(CRadioInboxPage)
	ON_MESSAGE(WM_TEXTMSG_RECEIVED,onTextMessageReceived)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_MSG_HISTORY, OnItemChanged)
	ON_BN_CLICKED(IDC_CLOSE, OnClose)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRadioInboxPage message handlers


BOOL CRadioInboxPage::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	m_msgHistoryLB.SetExtendedStyle( m_msgHistoryLB.GetExtendedStyle() | 
									 LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES );

	// Create columns in the list control and give each column a header string
	LV_COLUMN lv;  
	lv.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lv.fmt = LVCFMT_LEFT;

    UINT columnId[] = {IDS_HISTORYCOLUMN_HISTORY, IDS_HISTORYCOLUMN_SOURCE, IDS_HISTORYCOLUMN_TEXT};
    UINT columnWidth[] = {150, 110, 100};

    // load the column title from resource
    CString title;
	for (int i = 0; i < 3; ++i )
    {
        title.LoadString(columnId[i]);

		lv.pszText = (LPTSTR) (LPCTSTR) title;
        lv.cx = columnWidth[i];
		
		// insert the column of the list control
		m_msgHistoryLB.InsertColumn(i, &lv);
	}

    
	refreshInbox();

	m_commsSender = TA_Base_Core::MessagePublicationManager::getInstance().
    getCommsMessageSender(TA_Base_Core::RadioComms::Context);

	TA_Base_Core::IEntityData* entity = RadioMFTAgentHelper::getRadioMFTAgentEntity();
	m_entityKey = entity->getKey();
	m_entitySubsystem = entity->getSubsystem();
	m_entityLocation  = entity->getLocation();

	delete entity;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


afx_msg LRESULT CRadioInboxPage::onTextMessageReceived(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY("onTextMessageReceived");
    
	refreshInbox();

	FUNCTION_EXIT;

	return 0;
}


void CRadioInboxPage::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
    FUNCTION_ENTRY("OnItemChanged");
    
    int nIndex = -1;
    POSITION currSel = m_msgHistoryLB.GetFirstSelectedItemPosition();

    // if something is selected
    if(currSel != NULL)
    {
        // get the selected index
        nIndex = m_msgHistoryLB.GetNextSelectedItem(currSel);

		if (nIndex != -1)
		{
			std::vector<MessageInbox>::iterator selectedText = m_msgHistoryList.begin()+nIndex;
			m_msgTextEL.SetWindowText((*selectedText).messageText.c_str());

			if (!selectedText->isRead)
			{
				// 1. setRead in DAI	
				// 2. change text color of read messages
				// 3. notify call banner to delete sds(call) entry
				RadioTextMessageFacade::updateTextMessage(selectedText->key,true);

				m_msgHistoryLB.setItemColour(nIndex,RGB(85,85,85), TA_Base_Bus::ListCtrlSelNoFocus::I_INDEX);

				TA_Base_Core::DescriptionParameters descParams;

				std::ostringstream sKey;
				sKey << selectedText->key;

				sendNotification(*selectedText);
			}
		}
    }
	FUNCTION_EXIT;
}

void CRadioInboxPage::refreshInbox()
{
	m_msgHistoryLB.DeleteAllItems();

    m_msgHistoryList = RadioTextMessageFacade::getHistoricalMessages(); 

	std::vector<MessageInbox>::iterator iter;
	int index = 0;

	std::string strTemp;
	for(iter = m_msgHistoryList.begin(); iter != m_msgHistoryList.end();++iter)
	{
		if (iter->isRead)
		{
            m_msgHistoryLB.setItemColour(index,RGB(85,85,85), TA_Base_Bus::ListCtrlSelNoFocus::I_INDEX);
		}
		else
		{
			m_msgHistoryLB.setItemColour(index,RGB(0,0,0), TA_Base_Bus::ListCtrlSelNoFocus::I_INDEX);
		}

        m_msgHistoryLB.InsertItem(index, CRadioHelper::timeToString((*iter).date).c_str());		
		m_msgHistoryLB.SetItemText(index,1,(*iter).messageSource.c_str());

		strTemp = (*iter).messageText.substr(0,16);
		m_msgHistoryLB.SetItemText(index,2,strTemp.c_str());
		++index;
	}	
}


void CRadioInboxPage::OnClose() 
{
    ::PostMessage(::GetParent(m_hWnd), WM_CLOSE, 0, 0);
}

void CRadioInboxPage::sendNotification(const MessageInbox& message)
{
	TA_IRS_Bus::RadioServerAgentIDL::recvText tData;			

	TA_IRS_Bus::Radio::Message msgData;
	msgData.key = message.key;
	msgData.time = message.date;
	msgData.isRead = true;
	msgData.messageText = CORBA::string_dup(message.messageText.c_str());

	tData.messageData = msgData;
	
	// send notification
	CORBA::Any msg;
    msg <<= tData;

    m_commsSender->sendCommsMessage(TA_Base_Core::RadioComms::ReceiveText,
        m_entityKey, msg,m_entitySubsystem, m_entityLocation);	
}
