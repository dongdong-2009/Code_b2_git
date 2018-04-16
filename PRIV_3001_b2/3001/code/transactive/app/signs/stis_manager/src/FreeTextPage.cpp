/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/stis_manager/src/FreeTextPage.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * The free text message selection page
  *
  */

#include "stdafx.h"
#include "stismanager.h"
#include "FreeTextPage.h"
#include "STISPredefinedMessages.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "bus/application_types/src/apptypes.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/TISAudit_MessageTypes.h"
#include "cots/ssce/sdk/include/sscemfc.hpp"
#include "bus/generic_gui/src/TransactiveMessage.h"




#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{
    const char* CFreeTextPage::FREE_TEXT_1_NAME = "STISFreeTextMessage1";
    const char* CFreeTextPage::FREE_TEXT_2_NAME = "STISFreeTextMessage2";
    const char* CFreeTextPage::FREE_TEXT_3_NAME = "STISFreeTextMessage3";
    const char* CFreeTextPage::FREE_TEXT_4_NAME = "STISFreeTextMessage4";
    const char* CFreeTextPage::FREE_TEXT_5_NAME = "STISFreeTextMessage5";
    const char* CFreeTextPage::FREE_TEXT_6_NAME = "STISFreeTextMessage6";
	const int CFreeTextPage::MAX_FREE_TEXT_CHARACTERS = 255; 
	const CString CFreeTextPage::VALID_CHARACTERS = 
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        " !\"#$%&'()*+,-./" //ASCII code 32 to 47
        ":;<=>?\x03\x08\x16\x18"; //ASCII code 58 to 63, and allow ctrl-c(0x03),backspace(0x08),ctrl-v(0x16), ctrl-v(0x18)

    CFreeTextPage::CFreeTextPage(CWnd* pParent /*=NULL*/)
	    : HandledDlg(CFreeTextPage::IDD, pParent),
          m_messageSelectionListener(NULL),
          m_validMessage(false), 
		  m_isNonScrollApplicable(false),
		  m_userSettingsManager( TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID), STIS_MANAGER_GUI_APPTYPE),
		  m_stisPredefinedMessages(NULL),
		  m_defaultDisplayAttributes(DisplayAttributes()),
		  m_defaultLcdAttributes(LCDAttributesSTIS()),
		  m_defaultLedAttributes(LEDAttributes())
    {

    }


    void CFreeTextPage::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(CFreeTextPage)
	    DDX_Control(pDX, IDC_FREETEXT_LIST, m_freeTextList);
		DDX_Control(pDX, IDC_DISPLAY_TIME_LABEL, m_displayTimeLabel);
	    DDX_Control(pDX, IDC_LCD_FONT_COLOUR, m_lcdFontColour);
	    DDX_Control(pDX, IDC_DISPLAY_TIME, m_displayTime);
	    DDX_Control(pDX, IDC_LCD_FONT_SIZE, m_lcdFontSize);
	    DDX_Control(pDX, IDC_LED_FONT_SIZE, m_LEDFontSize);
		DDX_Control(pDX, IDC_LED_FONT_COLOUR, m_LEDFontColor);
	    DDX_Control(pDX, IDC_STATION_FREE_TEXT, m_freeTextContent);
	    DDX_Control(pDX, IDC_SCROLL_SPEED, m_scrollSpeed);
	    DDX_Control(pDX, IDC_JUSTIFICATION, m_justification);
	    DDX_Control(pDX, IDC_DISPLAY_MODE, m_displayMode);
		DDX_Control(pDX, IDC_JUSTIFICATION_LABEL, m_justificationLabel);
		DDX_Control(pDX, IDC_SCROLLSPEED_LABEL, m_scrollSpeedLabel);
	    //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(CFreeTextPage, CDialog)
	    //{{AFX_MSG_MAP(CFreeTextPage)
	    ON_EN_CHANGE(IDC_STATION_FREE_TEXT, onChangeFreeText)
        ON_MESSAGE(WM_UPDATE_CURRENT_STIS_VERSION, OnUpdateCurrentSTISVersion)
	    ON_WM_DESTROY()
		ON_BN_CLICKED(IDC_SAVE, OnSave)
		ON_LBN_SELCHANGE(IDC_FREETEXT_LIST, OnSelchangeFreetextList)
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CFreeTextPage message handlers

    BOOL CFreeTextPage::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
		m_displayAttributesManager = new DisplayAttributesManager(m_displayMode,
			m_displayTimeLabel,
			m_displayTime,
			m_scrollSpeed,
			m_justification,
			m_lcdFontColour,
			m_lcdFontSize,
			m_LEDFontColor,
			m_LEDFontSize,
			m_justificationLabel,
			m_scrollSpeedLabel);

	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }
		
	DisplayAttributesManager* CFreeTextPage::getDisplayAttributesManager()
	{
		return m_displayAttributesManager;
	}

	void CFreeTextPage::init()
	{
		m_stisPredefinedMessages = STISPredefinedMessages::getInstance();  // TD11310
		m_displayAttributesManager->init();

		m_freeTextList.AddString("Ad Hoc Message 1");
		m_freeTextList.AddString("Ad Hoc Message 2");
		m_freeTextList.AddString("Ad Hoc Message 3");
		m_freeTextList.AddString("Ad Hoc Message 4");
		m_freeTextList.AddString("Ad Hoc Message 5");
		m_freeTextList.AddString("Ad Hoc Message 6");
		
		m_freeTextContent.SetLimitText(MAX_FREE_TEXT_CHARACTERS);
		m_freeTextContent.setAllowedCharacters(VALID_CHARACTERS);
		// register with the pre-defined message singleton to be updated of a change
		m_stisPredefinedMessages->registerCurrentVersionUser(this);
	  
		// Force load the default adhoc default display attributes
		//OnUpdateCurrentSTISVersion(0, 0);

		// Set default display effect and led & lcd, it's not get from predefined message library
	    setDisplayAttributes(m_defaultDisplayAttributes);
        setLcdAttributes(m_defaultLcdAttributes);
        setLedAttributes(m_defaultLedAttributes);
	}


    void CFreeTextPage::OnDestroy() 
    {
	    
	    delete m_displayAttributesManager;
	    // no longer accepting updates
		if(NULL != m_stisPredefinedMessages)
		{
	        m_stisPredefinedMessages->deregisterCurrentVersionUser(this);
		}
	    CDialog::OnDestroy();
    }
	
	CFreeTextPage::~CFreeTextPage()
	{
		if(NULL != m_stisPredefinedMessages)
		{
	        m_stisPredefinedMessages->removeInstance();
			m_stisPredefinedMessages = NULL;
		}
	}

    FreeTextMessage CFreeTextPage::getMessage() const
    {
        CString messageText;
	    m_freeTextContent.GetWindowText(messageText);

        FreeTextMessage message;
        message.messageContent = messageText.GetBuffer(0);
        message.displayAttributes = getDisplayAttributes();
		message.lcdAttributes = getLcdAttributes();
		message.ledAttributes = getLedAttributes();

        return message;
    }

    
    void CFreeTextPage::windowShown()
    {
        if (m_messageSelectionListener != NULL)
        {
            CString messageText;
	        m_freeTextContent.GetWindowText(messageText);

            // if there is some text there is a valid message
            m_validMessage = !messageText.IsEmpty();
            m_messageSelectionListener->adHocMessageSelected(true, m_validMessage, m_defaultDisplayAttributes.repeatInterval);
        }
    }


    void CFreeTextPage::onChangeFreeText() 
    {
	    if (m_messageSelectionListener != NULL)
        {
            CString messageText;
	        m_freeTextContent.GetWindowText(messageText);

            // if there is some text there is a valid message
            bool validMessage = !messageText.IsEmpty();

            if (m_validMessage != validMessage)
            {
                m_validMessage = validMessage;
                m_messageSelectionListener->adHocMessageSelected(false, m_validMessage, m_defaultDisplayAttributes.repeatInterval);
            }
        }
    }


    void CFreeTextPage::setMessageSelectionListener( IMessageSelectionListener* messageSelectionListener)
    {
        m_messageSelectionListener = messageSelectionListener;
    }


    LRESULT CFreeTextPage::OnUpdateCurrentSTISVersion(WPARAM wParam, LPARAM lParam)
    {
        // get the new default display attributes
        m_defaultLedAttributes = m_stisPredefinedMessages->getDefaultLedDisplayAttributes();
		m_defaultLcdAttributes = m_stisPredefinedMessages->getDefaultLcdDisplayAttributes();
		m_defaultDisplayAttributes = m_stisPredefinedMessages->getDefaultStisAttributes();

		// Set default display effect and led & lcd, it's get from predefined message library
	    setDisplayAttributes(m_defaultDisplayAttributes);
        setLcdAttributes(m_defaultLcdAttributes);
        setLedAttributes(m_defaultLedAttributes);

		CTisDialogCtrl* tab = getHandleTab();
		if( (NULL != tab) && (tab->getActiveDialog() == this) )
		{
	        if (m_messageSelectionListener != NULL)
	        {
	            m_messageSelectionListener->adHocMessageSelected(false, m_validMessage, m_defaultDisplayAttributes.repeatInterval);
	        }
		}

        return 0;
    }


    TA_Base_Core::DisplayAttributes CFreeTextPage::getDisplayAttributes() const
    {
        TA_Base_Core::DisplayAttributes attributes;
		attributes.displayMode = m_displayAttributesManager->getDisplayMode();
        attributes.justification = m_displayAttributesManager->getJustification();
		attributes.scrollSpeed = m_displayAttributesManager->getScrollSpeed();
		attributes.displayTime = m_displayAttributesManager->getDisplayTime();

        return attributes;
    }


    void CFreeTextPage::setDisplayAttributes(const TA_Base_Core::DisplayAttributes& displayAttributes)
    {
		m_displayAttributesManager->setJustification(displayAttributes.justification);
        m_displayAttributesManager->setScrollSpeed(displayAttributes.scrollSpeed);
		m_displayAttributesManager->setDisplayTime(displayAttributes.displayTime);
		m_displayAttributesManager->setDisplayMode(displayAttributes.displayMode);
    }

	void CFreeTextPage::setLcdAttributes(const TA_Base_Core::LCDAttributesSTIS& lcdAttributes)
	{
		m_displayAttributesManager->setLcdFontSize(lcdAttributes.fontSize);
		m_displayAttributesManager->setLcdFontColour(lcdAttributes.fontColour);
	}

	void CFreeTextPage::setLedAttributes(const TA_Base_Core::LEDAttributes& ledAttributes)
	{
		m_displayAttributesManager->setLedFontSize(ledAttributes.fontSize);
		m_displayAttributesManager->setLedFontColour(ledAttributes.fontColour);
	}

	TA_Base_Core::LCDAttributesSTIS CFreeTextPage::getLcdAttributes() const
	{
		TA_Base_Core::LCDAttributesSTIS attributes;
		attributes.fontSize = m_displayAttributesManager->getLcdFontSize();
		attributes.fontColour = m_displayAttributesManager->getLcdFontColour();
		return attributes;
	}

    TA_Base_Core::LEDAttributes CFreeTextPage::getLedAttributes() const
    {
        TA_Base_Core::LEDAttributes attributes;
        attributes.fontSize = m_displayAttributesManager->getLedFontSize();
		attributes.fontColour = m_displayAttributesManager->getLedFontColour();
        return attributes;
    }

    void CFreeTextPage::OnOK()
    {
    }


    void CFreeTextPage::OnCancel()
    {
    }

    
	void CFreeTextPage::OnSave() 
	{
		// save the text in the message box to
		// the selected free text item

		// get the selected index
		int currSel = m_freeTextList.GetCurSel();

		CString text;
		m_freeTextContent.GetWindowText(text);
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
			default:
				// User not select a message to save
				return;
			}
		}
		catch ( const TA_Base_Core::UserSettingsException& use )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UserSettingsException", use.what() );

			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_070124);
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown" );
		}
	}

	void CFreeTextPage::OnSelchangeFreetextList() 
	{
		// if something is selected enable the save button
		// load the selection
    
		int currSel = m_freeTextList.GetCurSel();

		if (currSel > -1)
		{
			GetDlgItem(IDC_SAVE)->EnableWindow(TRUE);

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
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch,
					 "UserSettingsException", use.what());

				// dont display a message, the setting may simply not be saved yet
			}

			m_freeTextContent.SetWindowText(newText.c_str());
            onChangeFreeText();
		}
		else
		{
			GetDlgItem(IDC_SAVE)->EnableWindow(FALSE);
		}
		
	}



	bool CFreeTextPage::findAndSelectMessageNameInList(CListBox& list, const std::string messageName)
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
	
	bool CFreeTextPage::findAndSelectStationMessage(const std::string& messageName)
	{
		bool ret = false;
		if(findAndSelectMessageNameInList(m_freeTextList, messageName))
		{
			//may need not to call the function, because the SetItemState may trigger this function
			OnSelchangeFreetextList();
			ret = true;
		}
		return ret;
	}

	HandledDlg::HandledDlg(UINT nIDTemplate, CWnd* pParentWnd):CDialog(nIDTemplate, pParentWnd),
		m_tab(NULL),
		m_resourceId(nIDTemplate)
	{

	}
	void HandledDlg::setHandleTab(CTisDialogCtrl* tab)
	{
		m_tab = tab;
	}

	UINT HandledDlg::getResourceId()
	{
		return m_resourceId;
	}

	CTisDialogCtrl* HandledDlg::getHandleTab()
	{
		return m_tab;
	}
} // TA_IRS_App





