/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/ttis_manager/src/TrainSelectFreeTextPage.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * The free text page that resides on the train display tab.
  *
  */

#include "stdafx.h"
#include "TTISManager.h"
#include "TrainSelectFreeTextPage.h"
#include "TTISCommonConfiguration.h"
#include "TTISMessageManager.h"
#include "TTISPredefinedMessages.h"
#include "DisplayAttributeManager.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "cots/ssce/sdk/include/sscemfc.hpp"
#include "core/message/src/NameValuePair.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/TISAudit_MessageTypes.h"

#include "bus/application_types/src/apptypes.h"
#include "core/exceptions/src/UserSettingsException.h"

// owning page
#include "TrainDisplayPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_IRS_App::CTrainSelectFreeTextPage::AdHocMessage;

/////////////////////////////////////////////////////////////////////////////
// CTrainSelectFreeTextPage dialog

namespace TA_IRS_App
{

    const char* CTrainSelectFreeTextPage::FREE_TEXT_1_NAME = "TTISFreeTextMessage1";
    const char* CTrainSelectFreeTextPage::FREE_TEXT_2_NAME = "TTISFreeTextMessage2";
    const char* CTrainSelectFreeTextPage::FREE_TEXT_3_NAME = "TTISFreeTextMessage3";
    const char* CTrainSelectFreeTextPage::FREE_TEXT_4_NAME = "TTISFreeTextMessage4";
    const char* CTrainSelectFreeTextPage::FREE_TEXT_5_NAME = "TTISFreeTextMessage5";
    const char* CTrainSelectFreeTextPage::FREE_TEXT_6_NAME = "TTISFreeTextMessage6";

	CTrainSelectFreeTextPage::CTrainSelectFreeTextPage(CWnd* pParent /*=NULL*/)
		: CDialog(CTrainSelectFreeTextPage::IDD, pParent),
      m_userSettingsManager( TTISCommonConfiguration::getInstance().getSessionId(), TTIS_MANAGER_GUI_APPTYPE),
	  m_messageSelectionListener(NULL),
      m_ttisPredefinedMessages( NULL )
	{
		//{{AFX_DATA_INIT(CTrainSelectFreeTextPage)
		//}}AFX_DATA_INIT
	}


	void CTrainSelectFreeTextPage::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CTrainSelectFreeTextPage)
		DDX_Control(pDX, IDC_FREETEXT_LIST, m_freeTextList);
		DDX_Control(pDX, IDC_TRAIN_FREE_TEXT, m_messageContent);
		DDX_Control(pDX, IDC_DISPLAY_MODE, m_displayMode);
		DDX_Control(pDX, IDC_FONT_SIZE, m_fontSize);
		DDX_Control(pDX, IDC_JUSTIFICATION, m_justification);
		DDX_Control(pDX, IDC_INTENSITY, m_intensity);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CTrainSelectFreeTextPage, CDialog)
		//{{AFX_MSG_MAP(CTrainSelectFreeTextPage)
		ON_BN_CLICKED(IDC_SPELL_CHECK, OnSpellCheck)
		ON_CBN_SELCHANGE(IDC_DISPLAY_MODE, OnSelchangeDisplayMode)
		ON_BN_CLICKED(IDC_SAVE_FREETEXT, onSaveFreetext)
		ON_LBN_SELCHANGE(IDC_FREETEXT_LIST, onSelchangeFreetextList)
		ON_WM_SHOWWINDOW()
		ON_EN_CHANGE(IDC_TRAIN_FREE_TEXT, onChangeTrainFreeText)
		// TD17778
		ON_WM_DESTROY()
		ON_MESSAGE(WM_UPDATE_CURRENT_TTIS_VERSION, OnUpdateCurrentTTISVersion)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CTrainSelectFreeTextPage message handlers

	void CTrainSelectFreeTextPage::OnSpellCheck() 
	{
		CSentrySpellDlg spellDlg(this);

		spellDlg.SetKey(0x5B39C65E);
		SSCE_SetRegTreeName("HKEY_LOCAL_MACHINE\\Software\\Wintertree\\SSCE");
		spellDlg.Check(m_messageContent);
	}


	BOOL CTrainSelectFreeTextPage::OnInitDialog() 
	{
		CDialog::OnInitDialog();
		
		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}

	void CTrainSelectFreeTextPage::Init()
	{
		m_displayMode.AddString("Scroll Left");
		m_displayMode.AddString("Scroll Right");
		m_displayMode.AddString("Scroll Up");
		m_displayMode.AddString("Scroll Down");
		m_displayMode.AddString("Instant On");
		m_displayMode.AddString("Blinking");
		m_displayMode.AddString("Wiping");
		m_displayMode.AddString("Snow");

		m_justification.AddString("Left");
		m_justification.AddString("Centre");
		m_justification.AddString("Right");

		m_intensity.AddString("Normal");
		m_intensity.AddString("High");

		m_fontSize.AddString("Small");
//		m_fontSize.AddString("Medium");		//CL20135,hongzhi
		m_fontSize.AddString("Large");

		m_freeTextList.AddString("Free Text 1");
		m_freeTextList.AddString("Free Text 2");
		m_freeTextList.AddString("Free Text 3");
		m_freeTextList.AddString("Free Text 4");
		m_freeTextList.AddString("Free Text 5");
		m_freeTextList.AddString("Free Text 6");

        if ( NULL == m_ttisPredefinedMessages )
        {
            m_ttisPredefinedMessages = &( TTISPredefinedMessages::getInstance() );
        }

		m_ttisPredefinedMessages->registerCurrentVersionUser(this);

		m_messageContent.setDisallowedCharacters(ValidatingEdit::FREE_TEXT_INVALID_CHARACTERS);
		m_messageContent.SetLimitText(ValidatingEdit::MAX_FREE_TEXT_CHARACTERS);

		// enable/disable buttons
		onSelchangeFreetextList();

		// 2008-08-27 make default display
		SetDefaults();

	}

	// TD 17778
	void CTrainSelectFreeTextPage::OnDestroy() 
	{
		CDialog::OnDestroy();
		
        if ( NULL != m_ttisPredefinedMessages )
        {
            // deregister for pre-defined message library version update
            m_ttisPredefinedMessages->deregisterCurrentVersionUser(this);

            m_ttisPredefinedMessages = NULL;
            
            TTISPredefinedMessages::removeInstance();
        }
	}
	// TD 17778

	
	void CTrainSelectFreeTextPage::SetDefaults()
	{
		TA_Base_Core::TTISLedAttributes attributes = 
			m_ttisPredefinedMessages->getDefaultDisplayAttributes();

		setDisplayMode(attributes.displayMode);
		setJustification(attributes.justification);
		setIntensity(attributes.intensity);
		setFontSize(attributes.latinFontSize);
	}


	void CTrainSelectFreeTextPage::windowShown() 
	{
//    // the owning window
//    CWnd* parent = GetParent();
//    // down cast to the right window type
//    CTrainDisplayPage* trainDisplayPage = dynamic_cast<CTrainDisplayPage*>(parent);
//
    if (m_messageSelectionListener != NULL)
    {
        DisplayAttributeManager* displayAttributeManager = NULL;

        // get the object managing display attributes
        displayAttributeManager = m_messageSelectionListener->getDisplayAttributeManager();

        if (displayAttributeManager != NULL)
        {
            // enable the combo selection
            displayAttributeManager->enableTimeControls();

            // enable the priority
            displayAttributeManager->setPriority(4, true);

            // set the default repeat interval
            displayAttributeManager->
                setRepeatInterval( m_ttisPredefinedMessages->getDefaultDisplayAttributes().repeatInterval,
                                   true);

            // a message is selected
            m_messageSelectionListener->messageSelectionChanged(hasValidMessage());
        }
    }
}


	/** 
	  * hasValidMessage
	  *
	  * test if a message has been entered.
	  *
	  * @return true if a valid message has been entered.
	  */
	bool CTrainSelectFreeTextPage::hasValidMessage() const
	{
		CString message;
		m_messageContent.GetWindowText(message);

		// if there is some text
		if (!message.IsEmpty())
		{
			// there is a valid message
			return true;
		}

		return false;
	}


	void CTrainSelectFreeTextPage::onChangeTrainFreeText() 
	{
		notifyParentWindowOfMessage();
	}


	/** 
	  * getMessage
	  *
	  * Returns the message data set on this page.
	  * This can only be called when a valid message
	  * is entered, as determined by hasValidMessage
	  * @see hasValidMessage
	  *
	  * @return A structure with the message data
	  */
	AdHocMessage CTrainSelectFreeTextPage::getMessage() const
	{
		LOG_FUNCTION_ENTRY( SourceInfo, "CTrainSelectFreeTextPage::getMessage" );

		TA_ASSERT( hasValidMessage(),
				  "getMessage called when no valid message present");
    
		AdHocMessage message;
    
		// get the message content
		CString messageText;
		m_messageContent.GetWindowText(messageText);
		message.messageContent = messageText;

		// get the font size
		message.fontSize = getFontSize();   

		// get the justification
		// if it is disabled, it is still required to be set
		// in the structure, even though it is never used
		message.justification = getJustification();

		// get the intensity
		message.intensity = getIntensity();

		// get the display mode
		message.displayMode = getDisplayMode();
    
		LOG_FUNCTION_EXIT( SourceInfo, "CTrainSelectFreeTextPage::getMessage" );
		return message;
	}


	void CTrainSelectFreeTextPage::notifyParentWindowOfMessage()
	{
//    // the owning window
//    CWnd* parent = GetParent();
//    // down cast to the right window type
//    CTrainDisplayPage* trainDisplayPage = dynamic_cast<CTrainDisplayPage*>(parent);

    if (m_messageSelectionListener != NULL)
    {
        DisplayAttributeManager* displayAttributeManager = NULL;

        // get the object managing display attributes
        displayAttributeManager = m_messageSelectionListener->getDisplayAttributeManager();

        if (displayAttributeManager != NULL)
        {
            // a message is selected
            m_messageSelectionListener->messageSelectionChanged(hasValidMessage());
        }
    }
}


	TA_Base_Core::ELEDFontSize CTrainSelectFreeTextPage::getFontSize() const
	{
		int selected = m_fontSize.GetCurSel();
		switch (selected)
		{
		case 0:
			return TA_Base_Core::FONT_SIZE_SMALL;
			break;
		case 1:
		default:
			return TA_Base_Core::FONT_SIZE_LARGE;		//CL20135, hongzhi
			break;
		}
	}


	void CTrainSelectFreeTextPage::setFontSize(TA_Base_Core::ELEDFontSize fontSize)
	{
		switch (fontSize)
		{
		case TA_Base_Core::FONT_SIZE_SMALL:
			m_fontSize.SetCurSel(0);
			break;
		case TA_Base_Core::FONT_SIZE_LARGE:		//CL20135, hongzhi
		default:
			m_fontSize.SetCurSel(1);
			break;
		}
	}


	TA_Base_Core::EJustification CTrainSelectFreeTextPage::getJustification() const
	{
		int selected = m_justification.GetCurSel();
		switch (selected)
		{
		case 1:
			return TA_Base_Core::CENTRED;
			break;
		case 2:
			return TA_Base_Core::RIGHT;
			break;
		case 0:
		default:
			return TA_Base_Core::LEFT;
			break;
		}
	}


	void CTrainSelectFreeTextPage::setJustification(TA_Base_Core::EJustification justification)
	{
		switch (justification)
		{
		case TA_Base_Core::CENTRED:
			m_justification.SetCurSel(1);
			break;
		case TA_Base_Core::RIGHT:
			m_justification.SetCurSel(2);
			break;
		case TA_Base_Core::LEFT:
		default:
			m_justification.SetCurSel(0);
			break;
		}
	}


	TA_Base_Core::ETTISLEDIntensity CTrainSelectFreeTextPage::getIntensity() const
	{
		int selected = m_intensity.GetCurSel();
		switch (selected)
		{
		case 1:
			return TA_Base_Core::HIGH;
			break;
		case 0:
		default:
			return TA_Base_Core::NORMAL;
			break;
		}
	}


	void CTrainSelectFreeTextPage::setIntensity(TA_Base_Core::ETTISLEDIntensity intensity)
	{
		switch (intensity)
		{
		case TA_Base_Core::HIGH:
			m_intensity.SetCurSel(1);
			break;
		case TA_Base_Core::NORMAL:
		default:
			m_intensity.SetCurSel(0);
			break;
		}
	}


	TA_Base_Core::EDisplayMode CTrainSelectFreeTextPage::getDisplayMode() const
	{
		int selected = m_displayMode.GetCurSel();
		switch (selected)
		{
		case 1:
			return TA_Base_Core::SCROLL_RIGHT;
			break;
		case 2:
			return TA_Base_Core::SCROLL_UP;
			break;
		case 3:
			return TA_Base_Core::SCROLL_DOWN;
			break;
		case 4:
			return TA_Base_Core::INSTANT_ON;
			break;
		case 5:
			return TA_Base_Core::BLINKING;
			break;
		case 6:
			return TA_Base_Core::WIPING;
			break;
		case 7:
			return TA_Base_Core::SNOW;
			break;
		case 0:
		default:
			return TA_Base_Core::SCROLL_LEFT;
			break;
		}
	}


	void CTrainSelectFreeTextPage::setDisplayMode(TA_Base_Core::EDisplayMode displayMode)
	{
		switch (displayMode)
		{
		case TA_Base_Core::SCROLL_RIGHT:
			m_displayMode.SetCurSel(1);
			break;
		case TA_Base_Core::SCROLL_UP:
			m_displayMode.SetCurSel(2);
			break;
		case TA_Base_Core::SCROLL_DOWN:
			m_displayMode.SetCurSel(3);
			break;
		case TA_Base_Core::INSTANT_ON:
			m_displayMode.SetCurSel(4);
			break;
		case TA_Base_Core::BLINKING:
			m_displayMode.SetCurSel(5);
			break;
		case TA_Base_Core::WIPING:
			m_displayMode.SetCurSel(6);
			break;
		case TA_Base_Core::SNOW:
			m_displayMode.SetCurSel(7);
			break;
		case TA_Base_Core::SCROLL_LEFT:
		default:
			m_displayMode.SetCurSel(0);
			break;
		}

		// disable/enable the justification window
		OnSelchangeDisplayMode();
	}


	void CTrainSelectFreeTextPage::OnSelchangeDisplayMode() 
	{
		int currSel = m_displayMode.GetCurSel();

		// If the operator has selected a scrolling message, then disable the justification combo box
		if((currSel >= 0) && ( currSel <= 3))
		{
			m_justification.EnableWindow(FALSE);
		}
		else
		{
			m_justification.EnableWindow(TRUE);
		}
	}


	void CTrainSelectFreeTextPage::onSelchangeFreetextList() 
	{
		// if something is selected enable the save button
		// load the selection
    
		int currSel = m_freeTextList.GetCurSel();

		if (currSel > -1)
		{
			GetDlgItem(IDC_SAVE_FREETEXT)->EnableWindow(TRUE);

			// load the selected free text message into
			// the message box
			std::string newText("");
			try
			{
				switch (currSel)
				{
				case 0:
					newText = m_userSettingsManager.getSettingAsString(FREE_TEXT_1_NAME);
					break;
				case 1:
					newText = m_userSettingsManager.getSettingAsString(FREE_TEXT_2_NAME);
					break;
				case 2:
					newText = m_userSettingsManager.getSettingAsString(FREE_TEXT_3_NAME);
					break;
				case 3:
					newText = m_userSettingsManager.getSettingAsString(FREE_TEXT_4_NAME);
					break;
				case 4:
					newText = m_userSettingsManager.getSettingAsString(FREE_TEXT_5_NAME);
					break;
				case 5:
					newText = m_userSettingsManager.getSettingAsString(FREE_TEXT_6_NAME);
					break;
				}
			}
			catch ( const TA_Base_Core::UserSettingsException& use )
			{
				LOG( SourceInfo, DebugUtil::ExceptionCatch,
					 "UserSettingsException", use.what());

				// dont display a message, the setting may simply not be saved yet
			}

			m_messageContent.SetWindowText(newText.c_str());

			notifyParentWindowOfMessage();
		}
		else
		{
			GetDlgItem(IDC_SAVE_FREETEXT)->EnableWindow(FALSE);
		}
	}


	void CTrainSelectFreeTextPage::onSaveFreetext() 
	{
		// save the text in the message box to
		// the selected free text item

		// get the selected index
		int currSel = m_freeTextList.GetCurSel();

		CString text;
		m_messageContent.GetWindowText(text);

		//hongran++ TD17553
		std::string freeTextName;
		
		try
		{
			switch (currSel)
			{
			case 0:
				m_userSettingsManager.saveSetting(FREE_TEXT_1_NAME, std::string(text));
				freeTextName = FREE_TEXT_1_NAME;
				break;
			case 1:
				m_userSettingsManager.saveSetting(FREE_TEXT_2_NAME, std::string(text));
				freeTextName = FREE_TEXT_2_NAME;
				break;
			case 2:
				m_userSettingsManager.saveSetting(FREE_TEXT_3_NAME, std::string(text));
				freeTextName = FREE_TEXT_3_NAME;
				break;
			case 3:
				m_userSettingsManager.saveSetting(FREE_TEXT_4_NAME, std::string(text));
				freeTextName = FREE_TEXT_4_NAME;
				break;
			case 4:
				m_userSettingsManager.saveSetting(FREE_TEXT_5_NAME, std::string(text));
				freeTextName = FREE_TEXT_5_NAME;
				break;
			case 5:
				m_userSettingsManager.saveSetting(FREE_TEXT_6_NAME, std::string(text));
				freeTextName = FREE_TEXT_6_NAME;
				break;
			}
			
			std::string tempContent(text);
			TA_Base_Core::DescriptionParameters desc;
			TA_Base_Core::NameValuePair textName("MessageName", freeTextName );
			TA_Base_Core::NameValuePair textContent( "MessageContent", tempContent );

			desc.push_back(&textName);
			desc.push_back(&textContent);
			TA_Base_Core::AuditMessageSender* auditMessageSender = 
					TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::TISAudit::Context );

			std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
			TA_Base_Core::IEntityData* guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( entityName );
			TA_ASSERT ( guiEntity != NULL, "EntityAccessFactory returned a NULL entity and yet did not throw an exception." );

			auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::TTISFreeTextMessageSaved, guiEntity->getKey(),
				desc, 
				"", // Further description text
				TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),
				"", // alarm ID - not required
				"", // incident key - not required
				""); // event key - not required 

			delete auditMessageSender;
			//++hongran TD17553
		}
		catch ( const TA_Base_Core::UserSettingsException& use )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch,
				 "UserSettingsException", use.what());

			TTISMessageManager::getInstance().displayError(TTISMessageManager::ERROR_SAVING_FREE_TEXT);
		}
	}


	void CTrainSelectFreeTextPage::OnOK()
	{
	}


	void CTrainSelectFreeTextPage::OnCancel()
	{
	}


	BOOL CTrainSelectFreeTextPage::PreTranslateMessage(MSG* pMsg) 
	{
		switch ( pMsg->message )
		{
		case WM_KEYDOWN:
			{
				switch ( pMsg->wParam )
				{
				case VK_ESCAPE:
					{
						return TRUE;
					}
					break;
				case VK_RETURN:
					{
						return TRUE;
					}
					break;
				default:
					{
						//do nothing
					}
					break;
				}
			}
			break;
		default:
			{
				//do nothing
			}
		}
		
		return CDialog::PreTranslateMessage(pMsg);
	}

	//TD 15349
	//zhou yuan++
	bool CTrainSelectFreeTextPage::findAndSelectMessageNameInList(CListBox& list, const std::string& messageName)
	{
		bool ret = false;
		
		int num = list.GetCount();
		for(int i =0 ; i < num ; ++i)
		{
			CString msgContent;
			list.GetText(i, msgContent);
			if(stricmp(msgContent, messageName.c_str()) == 0)
			{
				list.SetCurSel(i);
				ret = true;
				break;
			}
		}
		return ret;
	}

	bool CTrainSelectFreeTextPage::findAndSelectStationMessage(const std::string& messageName)
	{
		bool ret = false;
		if(findAndSelectMessageNameInList(m_freeTextList, messageName))
		{
			//may need not to call the function, because the SetItemState may trigger this function
			onSelchangeFreetextList();
			ret = true;
		}
		return ret;
	}

	void CTrainSelectFreeTextPage::setMessageSelectionListener(IMessageSelectionListener* messageSelectionListener)
	{
		m_messageSelectionListener = messageSelectionListener;
	}
	//++zhou yuan

	
	// TD17778
	LRESULT CTrainSelectFreeTextPage::OnUpdateCurrentTTISVersion(WPARAM wParam, LPARAM lParam)
	{
		SetDefaults();

		return 0;
	}	
	// TD17778

} // TA_IRS_App

