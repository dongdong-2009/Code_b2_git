/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/stis_manager/src/FreeTextPage.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2011/03/29 11:49:08 $
  * Last modified by:  $Author: builder $
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

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164 



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

    // This is the biggest repeat interval
    static const unsigned short MAX_DISPLAY_TIME = 999;

    CFreeTextPage::CFreeTextPage(CWnd* pParent /*=NULL*/)
	    : CDialog(CFreeTextPage::IDD, pParent),
          m_messageSelectionListener(NULL),
          m_validMessage(false), m_userSettingsManager( TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID), STIS_MANAGER_GUI_APPTYPE)
    {
	    //{{AFX_DATA_INIT(CFreeTextPage)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT

		// TODO
		
    }

	CFreeTextPage::~CFreeTextPage()
	{
		// no longer accepting updates
		m_stisPredefinedMessages->deregisterCurrentVersionUser(this);

		m_stisPredefinedMessages->removeInstance(); // TD11310
	}

    void CFreeTextPage::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(CFreeTextPage)
	    DDX_Control(pDX, IDC_FREETEXT_LIST, m_freeTextList);
	    DDX_Control(pDX, IDC_PLASMA_BACKGROUND_COLOUR, m_plasmaBackgroundColour);
	    DDX_Control(pDX, IDC_DISPLAY_TIME, m_displayTime);
	    DDX_Control(pDX, IDC_PLASMA_FOREGROUND_COLOUR, m_plasmaForegroundColour);
	    DDX_Control(pDX, IDC_PLASMA_FONT_SIZE, m_plasmaFontSize);
	    DDX_Control(pDX, IDC_LED_FONT_SIZE, m_LEDFontSize);
		DDX_Control(pDX, IDC_LED_FONT_COLOR, m_LEDFontColor);
	    DDX_Control(pDX, IDC_STATION_FREE_TEXT, m_freeTextContent);
	    DDX_Control(pDX, IDC_SCROLL_SPEED, m_scrollSpeed);
	    DDX_Control(pDX, IDC_JUSTIFICATION, m_justification);
	    DDX_Control(pDX, IDC_DISPLAY_MODE, m_displayMode);
	    //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(CFreeTextPage, CDialog)
	    //{{AFX_MSG_MAP(CFreeTextPage)
	    ON_CBN_SELCHANGE(IDC_DISPLAY_MODE, OnSelchangeDisplayMode)
	    ON_BN_CLICKED(IDC_SPELL_CHECK, OnSpellCheck)
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


	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }
		
	void CFreeTextPage::init()
	{
		m_stisPredefinedMessages = STISPredefinedMessages::getInstance();  // TD11310
		
		m_displayMode.AddString("Scroll Left");
		m_displayMode.AddString("Scroll Right");
		m_displayMode.AddString("Scroll Up");
		m_displayMode.AddString("Scroll Down");
		m_displayMode.AddString("Instant On");
		m_displayMode.AddString("Blinking");
		m_displayMode.AddString("Wiping");
		m_displayMode.AddString("Snow");
		
		m_scrollSpeed.AddString("Slow");
		m_scrollSpeed.AddString("Medium");
		m_scrollSpeed.AddString("Fast");
		
		m_justification.AddString("Left");
		m_justification.AddString("Centre");
		m_justification.AddString("Right");
		
		m_LEDFontSize.AddString("Small");
		m_LEDFontSize.AddString("Medium");
		m_LEDFontSize.AddString("Large");

		//font color(new requirement)
		m_LEDFontColor.AddString("Red");
		m_LEDFontColor.AddString("Green");
		m_LEDFontColor.AddString("Orange");
		
		m_plasmaForegroundColour.AddString("Black");
		m_plasmaForegroundColour.AddString("White");
		m_plasmaForegroundColour.AddString("Red");
		m_plasmaForegroundColour.AddString("Green");
		m_plasmaForegroundColour.AddString("Blue");
		m_plasmaForegroundColour.AddString("Yellow");
		m_plasmaForegroundColour.AddString("Cyan");
		m_plasmaForegroundColour.AddString("Magenta");
		m_plasmaForegroundColour.AddString("Orange");
		m_plasmaForegroundColour.AddString("Brown");
		m_plasmaForegroundColour.AddString("Grey");
		m_plasmaForegroundColour.AddString("Light Blue");
		m_plasmaForegroundColour.AddString("Light Green");
		m_plasmaForegroundColour.AddString("Light Red");
		
		m_plasmaBackgroundColour.AddString("Black");
		m_plasmaBackgroundColour.AddString("White");
		m_plasmaBackgroundColour.AddString("Red");
		m_plasmaBackgroundColour.AddString("Green");
		m_plasmaBackgroundColour.AddString("Blue");
		m_plasmaBackgroundColour.AddString("Yellow");
		m_plasmaBackgroundColour.AddString("Cyan");
		m_plasmaBackgroundColour.AddString("Magenta");
		m_plasmaBackgroundColour.AddString("Orange");
		m_plasmaBackgroundColour.AddString("Brown");
		m_plasmaBackgroundColour.AddString("Grey");
		m_plasmaBackgroundColour.AddString("Light Blue");
		m_plasmaBackgroundColour.AddString("Light Green");
		m_plasmaBackgroundColour.AddString("Light Red");
		
		
		m_plasmaFontSize.AddString("24");
		m_plasmaFontSize.AddString("36");
		m_plasmaFontSize.AddString("42");
		m_plasmaFontSize.AddString("48");
		m_plasmaFontSize.AddString("60");
		m_plasmaFontSize.AddString("72");
		m_plasmaFontSize.AddString("96");
		m_plasmaFontSize.AddString("128");
		
		m_freeTextList.AddString("Ad Hoc Message 1");
		m_freeTextList.AddString("Ad Hoc Message 2");
		m_freeTextList.AddString("Ad Hoc Message 3");
		m_freeTextList.AddString("Ad Hoc Message 4");
		m_freeTextList.AddString("Ad Hoc Message 5");
		m_freeTextList.AddString("Ad Hoc Message 6");
		
		m_freeTextContent.SetLimitText(MAX_FREE_TEXT_CHARACTERS);
		
		// set the display time range (1 to 999 minutes)
		CTime lowerTime(1971, 1, 1, 0, 0, 1, 0);
		// set the repeat interval
		unsigned short minutes = MAX_DISPLAY_TIME / 60;
		unsigned short seconds = MAX_DISPLAY_TIME - (minutes * 60);
		CTime upperTime(1971, 1, 1, 0, minutes, seconds, 0);
		m_displayTime.SetRange(&lowerTime, &upperTime);
		
		// register with the pre-defined message singleton to be updated of a change
		m_stisPredefinedMessages->registerCurrentVersionUser(this);
		
		OnUpdateCurrentSTISVersion(0, 0);
	}


    void CFreeTextPage::OnDestroy() 
    {
	    CDialog::OnDestroy();
    }


    void CFreeTextPage::setDefaults()
    {
	    setDisplayAttributes(m_defaultDisplayAttributes);
        setPlasmaAttributes(m_defaultPlasmaAttributes);
        setLedAttributes(m_defaultLedAttributes);
    }


    FreeTextMessage CFreeTextPage::getMessage() const
    {
        CString messageText;
	    m_freeTextContent.GetWindowText(messageText);

        FreeTextMessage message;
        message.messageContent = messageText.GetBuffer(0);
        message.displayAttributes = getDisplayAttributes();
        message.plasmaAttributes = getPlasmaAttributes();
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
        TA_Base_Core::STISLedAttributes ledAttributes =
            m_stisPredefinedMessages->getDefaultLedDisplayAttributes();
        TA_Base_Core::STISPlasmaAttributes plasmaAttributes =
            m_stisPredefinedMessages->getDefaultPlasmaDisplayAttributes();

        // now convert them into the local structures
        // we will use the default display attributes for plasma where the
        // value exists in both the plasma and led attributes

		m_defaultDisplayAttributes.displayMode = plasmaAttributes.displayMode;
		m_defaultDisplayAttributes.scrollSpeed = plasmaAttributes.scrollSpeed;
		m_defaultDisplayAttributes.repeatInterval = plasmaAttributes.repeatInterval;
		m_defaultDisplayAttributes.displayTime = plasmaAttributes.displayTime;
		m_defaultDisplayAttributes.justification = plasmaAttributes.justification;

		m_defaultPlasmaAttributes.fontType = plasmaAttributes.fontType;
		m_defaultPlasmaAttributes.fontSize = plasmaAttributes.fontSize;
		m_defaultPlasmaAttributes.fontColour = plasmaAttributes.fontColour;
		m_defaultPlasmaAttributes.backgroundColour = plasmaAttributes.backgroundColour;

		m_defaultLedAttributes.fontSize = ledAttributes.fontSize;
		m_defaultLedAttributes.intensity = ledAttributes.intensity;
		m_defaultLedAttributes.fontColour = ledAttributes.fontColour;
		m_defaultLedAttributes.backgroundColour = ledAttributes.backgroundColour;

        // populate the values into the fields
        setDefaults();

        // set the default priority
        if (m_messageSelectionListener != NULL)
        {
            m_messageSelectionListener->adHocMessageSelected(false, m_validMessage, m_defaultDisplayAttributes.repeatInterval);
        }

        return 0;
    }


    TA_Base_Core::DisplayAttributes CFreeTextPage::getDisplayAttributes() const
    {
        TA_Base_Core::DisplayAttributes attributes;
        
        int selected = m_displayMode.GetCurSel();
        switch (selected)
        {
        case 1:
            attributes.displayMode = TA_Base_Core::SCROLL_RIGHT;
            break;
        case 2:
            attributes.displayMode = TA_Base_Core::SCROLL_UP;
            break;
        case 3:
            attributes.displayMode = TA_Base_Core::SCROLL_DOWN;
            break;
        case 4:
            attributes.displayMode = TA_Base_Core::INSTANT_ON;
            break;
        case 5:
            attributes.displayMode = TA_Base_Core::BLINKING;
            break;
        case 6:
            attributes.displayMode = TA_Base_Core::WIPING;
            break;
        case 7:
            attributes.displayMode = TA_Base_Core::SNOW;
            break;
        case 0:
        default:
            attributes.displayMode = TA_Base_Core::SCROLL_LEFT;
            break;
        }

        selected = m_justification.GetCurSel();
        switch (selected)
        {
        case 1:
            attributes.justification = TA_Base_Core::CENTRED;
            break;
        case 2:
            attributes.justification = TA_Base_Core::RIGHT;
            break;
        case 0:
        default:
            attributes.justification = TA_Base_Core::LEFT;
            break;
        }

        selected = m_scrollSpeed.GetCurSel();
        switch (selected)
	    {
        case 0:
            attributes.scrollSpeed = TA_Base_Core::SCROLL_SLOW;
            break;
        case 2:
            attributes.scrollSpeed = TA_Base_Core::SCROLL_FAST;
            break;
        case 1:
        default:
            attributes.scrollSpeed = TA_Base_Core::SCROLL_MEDIUM;
            break;
	    }


        CTime displayTime;
        m_displayTime.GetTime(displayTime);

        attributes.displayTime = ( displayTime.GetMinute() * 60 ) + ( displayTime.GetSecond() );

		//XF++: CL20325
//		if (FALSE == m_justification.IsWindowEnabled())
//		{
//			attributes.justification = TA_Base_Core::MOVING;
//		}
//		if (FALSE == m_scrollSpeed.IsWindowEnabled())
//		{
//			attributes.scrollSpeed = TA_Base_Core::SCROLL_STOP;
//		}
//		if (FALSE == m_displayTime.IsWindowEnabled())
//		{
//			attributes.displayTime = -1;
//		}		
		//++XF:CL20325

        return attributes;
    }


    void CFreeTextPage::setDisplayAttributes(TA_Base_Core::DisplayAttributes displayAttributes)
    {
        switch (displayAttributes.displayMode)
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

        switch (displayAttributes.justification)
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

        switch (displayAttributes.scrollSpeed)
	    {
        case TA_Base_Core::SCROLL_SLOW:
            m_scrollSpeed.SetCurSel(0);
            break;
        case TA_Base_Core::SCROLL_FAST:
            m_scrollSpeed.SetCurSel(2);
            break;
        case TA_Base_Core::SCROLL_MEDIUM:
        default:
            m_scrollSpeed.SetCurSel(1);
            break;
	    }


        if (displayAttributes.displayTime > MAX_DISPLAY_TIME)
        {
            displayAttributes.displayTime = MAX_DISPLAY_TIME;
        }

        // set the repeat interval
        unsigned short minutes = displayAttributes.displayTime / 60;
        unsigned short seconds = displayAttributes.displayTime % 60;

        CTime temp(1971, 1, 1, 0, minutes, seconds, 0);
        m_displayTime.SetTime(&temp);


        // disable/enable the justification window
        OnSelchangeDisplayMode();
    }


    TA_Base_Core::PlasmaAttributes CFreeTextPage::getPlasmaAttributes() const
    {
        TA_Base_Core::PlasmaAttributes plasmaAttributes;

        plasmaAttributes.fontType = TA_Base_Core::DEFAULT_PLASMA_FONT;

        int selected = m_plasmaForegroundColour.GetCurSel();
        switch (selected)
        {
        case 0:
            plasmaAttributes.fontColour = TA_Base_Core::PLASMA_BLACK;
            break;
		case 2:
            plasmaAttributes.fontColour = TA_Base_Core::PLASMA_RED;
            break;
		case 3:
            plasmaAttributes.fontColour = TA_Base_Core::PLASMA_GREEN;
            break;
		case 4:
            plasmaAttributes.fontColour = TA_Base_Core::PLASMA_BLUE;
            break;
		case 5:
            plasmaAttributes.fontColour = TA_Base_Core::PLASMA_YELLOW;
            break;
		case 6:
            plasmaAttributes.fontColour = TA_Base_Core::PLASMA_CYAN;
            break;
		case 7:
            plasmaAttributes.fontColour = TA_Base_Core::PLASMA_MAGENTA;
            break;
		case 8:
            plasmaAttributes.fontColour = TA_Base_Core::PLASMA_ORANGE;
            break;
		case 9:
            plasmaAttributes.fontColour = TA_Base_Core::PLASMA_BROWN;
            break;
		case 10:
            plasmaAttributes.fontColour = TA_Base_Core::PLASMA_GREY;
            break;
		case 11:
            plasmaAttributes.fontColour = TA_Base_Core::PLASMA_LIGHT_BLUE;
            break;
		case 12:
            plasmaAttributes.fontColour = TA_Base_Core::PLASMA_LIGHT_GREEN;
            break;
		case 13:
            plasmaAttributes.fontColour = TA_Base_Core::PLASMA_LIGHT_RED;
            break;
        case 1:
        default:
            plasmaAttributes.fontColour = TA_Base_Core::PLASMA_WHITE;
            break;   
        }


        selected = m_plasmaBackgroundColour.GetCurSel();
        switch (selected)
        {
        case 1:
            plasmaAttributes.backgroundColour = TA_Base_Core::PLASMA_WHITE;
            break;
		case 2:
            plasmaAttributes.backgroundColour = TA_Base_Core::PLASMA_RED;
            break;
		case 3:
            plasmaAttributes.backgroundColour = TA_Base_Core::PLASMA_GREEN;
            break;
		case 4:
            plasmaAttributes.backgroundColour = TA_Base_Core::PLASMA_BLUE;
            break;
		case 5:
            plasmaAttributes.backgroundColour = TA_Base_Core::PLASMA_YELLOW;
            break;
		case 6:
            plasmaAttributes.backgroundColour = TA_Base_Core::PLASMA_CYAN;
            break;
		case 7:
            plasmaAttributes.backgroundColour = TA_Base_Core::PLASMA_MAGENTA;
            break;
		case 8:
            plasmaAttributes.backgroundColour = TA_Base_Core::PLASMA_ORANGE;
            break;
		case 9:
            plasmaAttributes.backgroundColour = TA_Base_Core::PLASMA_BROWN;
            break;
		case 10:
            plasmaAttributes.backgroundColour = TA_Base_Core::PLASMA_GREY;
            break;
		case 11:
            plasmaAttributes.backgroundColour = TA_Base_Core::PLASMA_LIGHT_BLUE;
            break;
		case 12:
            plasmaAttributes.backgroundColour = TA_Base_Core::PLASMA_LIGHT_GREEN;
            break;
		case 13:
            plasmaAttributes.backgroundColour = TA_Base_Core::PLASMA_LIGHT_RED;
            break;
        case 0:
        default:
            plasmaAttributes.backgroundColour = TA_Base_Core::PLASMA_BLACK;
            break;
        }

        selected = m_plasmaFontSize.GetCurSel();
        switch(selected)
	    {
        case 0:
            plasmaAttributes.fontSize = TA_Base_Core::PLASMA_FONT_SIZE_24;
            break;
        case 2:
            plasmaAttributes.fontSize = TA_Base_Core::PLASMA_FONT_SIZE_42;
            break;
        case 3:
            plasmaAttributes.fontSize = TA_Base_Core::PLASMA_FONT_SIZE_48;
            break;
        case 4:
            plasmaAttributes.fontSize = TA_Base_Core::PLASMA_FONT_SIZE_60;
            break;
        case 5:
            plasmaAttributes.fontSize = TA_Base_Core::PLASMA_FONT_SIZE_72;
            break;
        case 6:
            plasmaAttributes.fontSize = TA_Base_Core::PLASMA_FONT_SIZE_96;
            break;
        case 7:
            plasmaAttributes.fontSize = TA_Base_Core::PLASMA_FONT_SIZE_128;
            break;
        case 1:
        default:
            plasmaAttributes.fontSize = TA_Base_Core::PLASMA_FONT_SIZE_36;
            break;
	    }

        return plasmaAttributes;
    }


    void CFreeTextPage::setPlasmaAttributes(TA_Base_Core::PlasmaAttributes plasmaAttributes)
    {
        switch (plasmaAttributes.fontColour)
        {
        case TA_Base_Core::PLASMA_BLACK:
            m_plasmaForegroundColour.SetCurSel(0);
            break;
		case TA_Base_Core::PLASMA_RED:
            m_plasmaForegroundColour.SetCurSel(2);
            break;
		case TA_Base_Core::PLASMA_GREEN:
            m_plasmaForegroundColour.SetCurSel(3);
            break;
		case TA_Base_Core::PLASMA_BLUE:
            m_plasmaForegroundColour.SetCurSel(4);
            break;
		case TA_Base_Core::PLASMA_YELLOW:
            m_plasmaForegroundColour.SetCurSel(5);
            break;
		case TA_Base_Core::PLASMA_CYAN:
            m_plasmaForegroundColour.SetCurSel(6);
            break;
		case TA_Base_Core::PLASMA_MAGENTA:
            m_plasmaForegroundColour.SetCurSel(7);
            break;
		case TA_Base_Core::PLASMA_ORANGE:
            m_plasmaForegroundColour.SetCurSel(8);
            break;
		case TA_Base_Core::PLASMA_BROWN:
            m_plasmaForegroundColour.SetCurSel(9);
            break;
		case TA_Base_Core::PLASMA_GREY:
            m_plasmaForegroundColour.SetCurSel(10);
            break;
		case TA_Base_Core::PLASMA_LIGHT_BLUE:
            m_plasmaForegroundColour.SetCurSel(11);
            break;
		case TA_Base_Core::PLASMA_LIGHT_GREEN:
            m_plasmaForegroundColour.SetCurSel(12);
            break;
		case TA_Base_Core::PLASMA_LIGHT_RED:
            m_plasmaForegroundColour.SetCurSel(13);
            break;
        case TA_Base_Core::PLASMA_WHITE:
        default:
            m_plasmaForegroundColour.SetCurSel(1);
            break;   
        }

        switch (plasmaAttributes.backgroundColour)
        {
        case TA_Base_Core::PLASMA_WHITE:
            m_plasmaBackgroundColour.SetCurSel(1);
            break;   
		case TA_Base_Core::PLASMA_RED:
            m_plasmaBackgroundColour.SetCurSel(2);
            break;
		case TA_Base_Core::PLASMA_GREEN:
            m_plasmaBackgroundColour.SetCurSel(3);
            break;
		case TA_Base_Core::PLASMA_BLUE:
            m_plasmaBackgroundColour.SetCurSel(4);
            break;
		case TA_Base_Core::PLASMA_YELLOW:
            m_plasmaBackgroundColour.SetCurSel(5);
            break;
		case TA_Base_Core::PLASMA_CYAN:
            m_plasmaBackgroundColour.SetCurSel(6);
            break;
		case TA_Base_Core::PLASMA_MAGENTA:
            m_plasmaBackgroundColour.SetCurSel(7);
            break;
		case TA_Base_Core::PLASMA_ORANGE:
            m_plasmaBackgroundColour.SetCurSel(8);
            break;
		case TA_Base_Core::PLASMA_BROWN:
            m_plasmaBackgroundColour.SetCurSel(9);
            break;
		case TA_Base_Core::PLASMA_GREY:
            m_plasmaBackgroundColour.SetCurSel(10);
            break;
		case TA_Base_Core::PLASMA_LIGHT_BLUE:
            m_plasmaBackgroundColour.SetCurSel(11);
            break;
		case TA_Base_Core::PLASMA_LIGHT_GREEN:
            m_plasmaBackgroundColour.SetCurSel(12);
            break;
		case TA_Base_Core::PLASMA_LIGHT_RED:
            m_plasmaBackgroundColour.SetCurSel(13);
            break;
        case TA_Base_Core::PLASMA_BLACK:
        default:
            m_plasmaBackgroundColour.SetCurSel(0);
            break;
        }

        switch(plasmaAttributes.fontSize)
	    {
        case TA_Base_Core::PLASMA_FONT_SIZE_24:
            m_plasmaFontSize.SetCurSel(0);
            break;
        case TA_Base_Core::PLASMA_FONT_SIZE_42:
            m_plasmaFontSize.SetCurSel(2);
            break;
        case TA_Base_Core::PLASMA_FONT_SIZE_48:
            m_plasmaFontSize.SetCurSel(3);
            break;
        case TA_Base_Core::PLASMA_FONT_SIZE_60:
            m_plasmaFontSize.SetCurSel(4);
            break;
        case TA_Base_Core::PLASMA_FONT_SIZE_72:
            m_plasmaFontSize.SetCurSel(5);
            break;
        case TA_Base_Core::PLASMA_FONT_SIZE_96:
            m_plasmaFontSize.SetCurSel(6);
            break;
        case TA_Base_Core::PLASMA_FONT_SIZE_128:
            m_plasmaFontSize.SetCurSel(7);
            break;
        case TA_Base_Core::PLASMA_FONT_SIZE_UNUSED:
        case TA_Base_Core::PLASMA_FONT_SIZE_36:
        default:
            m_plasmaFontSize.SetCurSel(1);
            break;
	    }
    }


    TA_Base_Core::LEDAttributes CFreeTextPage::getLedAttributes() const
    {
        TA_Base_Core::LEDAttributes attributes;

        attributes.intensity = TA_Base_Core::INTENSITY_MEDIUM;
		attributes.fontColour = TA_Base_Core::LED_RED;
		attributes.backgroundColour = TA_Base_Core::LED_BLACK;

        int selected = m_LEDFontSize.GetCurSel();
        switch (selected)
        {
        case 0:
            attributes.fontSize = TA_Base_Core::FONT_SIZE_SMALL;
            break;
        case 2:
            attributes.fontSize = TA_Base_Core::FONT_SIZE_LARGE;
            break;
        case 1:
        default:
            attributes.fontSize = TA_Base_Core::FONT_SIZE_MEDIUM;
            break;
        }

		//get selected color
		selected = m_LEDFontColor.GetCurSel();
		switch (selected)
		{
		case 1:
			attributes.fontColour = TA_Base_Core::LED_GREEN;
			break;
		case 2:
			attributes.fontColour = TA_Base_Core::LED_ORANGE;
			break;
		case 0:
		default:
			attributes.fontColour = TA_Base_Core::LED_RED;
			break;
		}

        return attributes;
    }


    void CFreeTextPage::setLedAttributes(TA_Base_Core::LEDAttributes ledAttributes)
    {
        switch (ledAttributes.fontSize)
        {
        case TA_Base_Core::FONT_SIZE_SMALL:
            m_LEDFontSize.SetCurSel(0);
            break;
        case TA_Base_Core::FONT_SIZE_LARGE:
            m_LEDFontSize.SetCurSel(2);
            break;
        case TA_Base_Core::FONT_SIZE_MEDIUM:
        default:
            m_LEDFontSize.SetCurSel(1);
            break;
        }

		//font color
		switch (ledAttributes.fontColour)
		{
		case TA_Base_Core::LED_GREEN:
			m_LEDFontColor.SetCurSel(1);
			break;
		case TA_Base_Core::LED_ORANGE:
			m_LEDFontColor.SetCurSel(2);
			break;
		case TA_Base_Core::LED_RED:
		default:
			m_LEDFontColor.SetCurSel(0);
			break;
		}
    }


    void CFreeTextPage::OnSelchangeDisplayMode() 
    {
        updateDisplayTimeState();

        int currSel = m_displayMode.GetCurSel();

	    // If this is a scrolling message, enable the scroll speed and disable the justification
	    if((currSel >=0) && (currSel <=3))
	    {
            // restore the selection and enable the list if the list is not enabled
            if (!m_scrollSpeed.IsWindowEnabled())
            {
                m_scrollSpeed.restoreSelection(0);
		        m_scrollSpeed.EnableWindow(TRUE);
                GetDlgItem(IDC_SCROLLSPEED_LABEL)->EnableWindow(TRUE);
            }

            // save the selection and disable the list if the list was enabled
            if (m_justification.IsWindowEnabled())
            {
                GetDlgItem(IDC_JUSTIFICATION_LABEL)->EnableWindow(FALSE);
		        m_justification.EnableWindow(FALSE);
                m_justification.saveSelection();
                m_justification.SetCurSel(-1);
            }
	    }
	    else
	    {
            // save the selection and disable the list if the list was enabled
            if (m_scrollSpeed.IsWindowEnabled())
            {
                m_scrollSpeed.saveSelection();
                m_scrollSpeed.SetCurSel(-1);
                m_scrollSpeed.EnableWindow(FALSE);
                GetDlgItem(IDC_SCROLLSPEED_LABEL)->EnableWindow(FALSE);
            }

            // restore the selection and enable the list if the list is not enabled
            if (!m_justification.IsWindowEnabled())
            {
                m_justification.restoreSelection(0);
		        m_justification.EnableWindow(TRUE);
                GetDlgItem(IDC_JUSTIFICATION_LABEL)->EnableWindow(TRUE);
            }
	    }
    }


    void CFreeTextPage::OnOK()
    {
    }


    void CFreeTextPage::OnCancel()
    {
    }

    
    void CFreeTextPage::OnSpellCheck() 
    {
	    // Spell check the text in the free text edit box

	    CSentrySpellDlg spellDlg(this);

	    spellDlg.SetKey(0x5B39C65E);
        SSCE_SetRegTreeName("HKEY_LOCAL_MACHINE\\Software\\Wintertree\\SSCE");
	    spellDlg.Check(m_freeTextContent);
	    
    }

	void CFreeTextPage::OnSave() 
	{
		// save the text in the message box to
		// the selected free text item

		// get the selected index
		int currSel = m_freeTextList.GetCurSel();

		CString text;
		m_freeTextContent.GetWindowText(text);

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

			auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISFreeTextMessageSaved, guiEntity->getKey(),
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
			LOG_EXCEPTION_CATCH(SourceInfo, "UserSettingsException", use.what() );

			// TD14164 ++
			/*AfxMessageBox("Error saving free-text message");*/
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg.showMsgBox(IDS_UE_070124);
			// ++ TD14164 

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


    void CFreeTextPage::timeTypeChanged(TimeControlManager::TimeType newTimeType)
    {
        m_timeType = newTimeType;
        updateDisplayTimeState();
    }


    void CFreeTextPage::updateDisplayTimeState()
    {
	    // If the operator has selected 'Instant On', 'Blinking' or 'Wiping' AND this is to be a timed message
	    // let them access the 'Display Time' parameter

        if (m_timeType == TimeControlManager::MESSAGE_CONTINUOUS)
        {
            m_displayTime.EnableWindow(FALSE);
            m_displayTime.SetFormat(_T(" "));
            GetDlgItem(IDC_DISPLAYTIME_LABEL)->EnableWindow(FALSE);
        }
        else
        {
            int currSel = m_displayMode.GetCurSel();

	        if((currSel >= 4) && (currSel <= 7  ))
	        {
		        m_displayTime.EnableWindow(TRUE);
                m_displayTime.SetFormat(_T("  mm:ss"));
                GetDlgItem(IDC_DISPLAYTIME_LABEL)->EnableWindow(TRUE);
	        }
	        else
	        {
                m_displayTime.EnableWindow(FALSE);
                m_displayTime.SetFormat(_T(" "));
                GetDlgItem(IDC_DISPLAYTIME_LABEL)->EnableWindow(FALSE);
	        }
        }
    }

	//TD 15349
	//zhou yuan++
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
	//++zhou yuan


} // TA_IRS_App





