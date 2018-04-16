// PIDSAdhocMessagePage.cpp : implementation file
//

#include "stdafx.h"
#include "PIDSAdhocMessagePage.h"
#include "DisplayAttributeManager.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/utilities/src/DebugUtil.h"
#include <boost/foreach.hpp>

// PIDSAdhocMessagePage dialog
namespace TA_IRS_App
{
    const CString PIDSAdhocMessagePage::FREE_TEXT_VALID_CHARACTERS =
        "0123456789"
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        " !\"#$%&'()*+,-./" //ASCII code 32 to 47
        ":;<=>?\x03\x08\x16\x18"; //ASCII code 58 to 63

    const UINT PIDSAdhocMessagePage::MAX_FREE_TEXT_CHARACTERS = 130;

    const PIDSAdhocMessagePage::DefaultDisplayAttributes PIDSAdhocMessagePage::m_defaultAttributes;
    PIDSAdhocMessagePage::UintToStringMap PIDSAdhocMessagePage::m_fontSizeToNameMap;
    PIDSAdhocMessagePage::UintToStringMap PIDSAdhocMessagePage::m_displayModeToNameMap;
    PIDSAdhocMessagePage::UintToStringMap PIDSAdhocMessagePage::m_justificationToNameMap;
    PIDSAdhocMessagePage::UintToStringMap PIDSAdhocMessagePage::m_intensityToNameMap;

    void PIDSAdhocMessagePage::initAllMaps()
    {
		m_fontSizeToNameMap[TA_Base_Core::TTIS_FONT_SIZE_SMALL] = "Small";
		m_fontSizeToNameMap[TA_Base_Core::TTIS_FONT_SIZE_LARGE] = "Large";

		m_displayModeToNameMap[TA_Base_Core::SCROLL_LEFT] = "Scroll Left";
        m_displayModeToNameMap[TA_Base_Core::SCROLL_RIGHT] = "Scroll Right";
        m_displayModeToNameMap[TA_Base_Core::SCROLL_UP] = "Scroll Up";
        m_displayModeToNameMap[TA_Base_Core::SCROLL_DOWN] = "Scroll Down" ;
        m_displayModeToNameMap[TA_Base_Core::NORMAL_STATIONARY] = "Normal/Stationary";
        m_displayModeToNameMap[TA_Base_Core::BLINKING] = "Blinking";
        m_displayModeToNameMap[TA_Base_Core::WIPING] = "Wiping";

		m_justificationToNameMap[TA_Base_Core::LEFT] = "Left";
        m_justificationToNameMap[TA_Base_Core::CENTRED] = "Centre";
        m_justificationToNameMap[TA_Base_Core::RIGHT] = "Right";

		m_intensityToNameMap[TA_Base_Core::NORMAL] = "Normal";
        m_intensityToNameMap[TA_Base_Core::HIGH] = "High";
    }

    PIDSAdhocMessagePage::PIDSAdhocMessagePage( CWnd* pParent /*=NULL*/ )
            : CDialog( PIDSAdhocMessagePage::IDD, pParent ),
              m_messageSelectionListener( NULL )
    {
        initAllMaps();
    }


    void PIDSAdhocMessagePage::DoDataExchange( CDataExchange* pDX )
    {
        CDialog::DoDataExchange( pDX );
        //{{AFX_DATA_MAP(PIDSAdhocMessagePage)
        DDX_Control( pDX, IDC_TRAIN_FREE_TEXT, m_messageContent );
        DDX_Control( pDX, IDC_DISPLAY_MODE, m_displayMode );
        DDX_Control( pDX, IDC_FONT_SIZE, m_fontSize );
        DDX_Control( pDX, IDC_JUSTIFICATION, m_justification );
        DDX_Control( pDX, IDC_INTENSITY, m_intensity );
        //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP( PIDSAdhocMessagePage, CDialog )
        //{{AFX_MSG_MAP(PIDSAdhocMessagePage)
        ON_WM_SHOWWINDOW()
        ON_EN_CHANGE( IDC_TRAIN_FREE_TEXT, OnChangeTrainFreeText )
		ON_CBN_SELCHANGE(IDC_DISPLAY_MODE, OnDisplayModeChange)
        ON_WM_DESTROY()
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // PIDSAdhocMessagePage message handlers

    void PIDSAdhocMessagePage::OnChangeTrainFreeText()
    {
        notifyParentWindowOfMessage();
    }

	void PIDSAdhocMessagePage::OnDisplayModeChange()
	{
		TA_Base_Core::EDisplayMode displayMode = getDisplayMode();
		// if scroll mode, disable justification
		if(	displayMode == TA_Base_Core::SCROLL_LEFT
			|| displayMode == TA_Base_Core::SCROLL_RIGHT
			|| displayMode == TA_Base_Core::SCROLL_UP
			|| displayMode == TA_Base_Core::SCROLL_DOWN )
		{
			m_justification.EnableWindow(false);
		}
		else
		{
			m_justification.EnableWindow(true);
		}
	}

    void PIDSAdhocMessagePage::OnDestroy()
    {
        setMessageSelectionListener( NULL );
        CDialog::OnDestroy();
    }

    void PIDSAdhocMessagePage::notifyParentWindowOfMessage()
    {
        if ( m_messageSelectionListener != NULL )
        {
            DisplayAttributeManager* displayAttributeManager = m_messageSelectionListener->getDisplayAttributeManager();
            if ( displayAttributeManager != NULL )
            {
                m_messageSelectionListener->messageSelectionChanged( hasValidMessage() );
            }
        }
    }

    BOOL PIDSAdhocMessagePage::OnInitDialog()
    {
        CDialog::OnInitDialog();
        return TRUE;
    }

    void PIDSAdhocMessagePage::Init()
    {
		BOOST_FOREACH(UintToStringMap::value_type& i, m_displayModeToNameMap)
		{
			m_displayMode.AddString(i.second.c_str());
		}
		BOOST_FOREACH(UintToStringMap::value_type& i, m_justificationToNameMap)
		{
			m_justification.AddString(i.second.c_str());
		}
		BOOST_FOREACH(UintToStringMap::value_type& i, m_intensityToNameMap)
		{
			m_intensity.AddString(i.second.c_str());
		}
		BOOST_FOREACH(UintToStringMap::value_type& i, m_fontSizeToNameMap)
		{
			m_fontSize.AddString(i.second.c_str());
		}
        m_messageContent.setAllowedCharacters( FREE_TEXT_VALID_CHARACTERS );
        m_messageContent.SetLimitText( MAX_FREE_TEXT_CHARACTERS );
        SetDefaults();
    }

    void PIDSAdhocMessagePage::windowShown()
    {
        TA_ASSERT( m_messageSelectionListener != NULL, "Not set m_messageSelectionListener")
        DisplayAttributeManager* displayAttributeManager = m_messageSelectionListener->getDisplayAttributeManager();
        TA_ASSERT( displayAttributeManager != NULL, "None DisplayAttributeManager instance" );
		displayAttributeManager->setPerrunBtnCheck();
        m_messageSelectionListener->messageSelectionChanged( hasValidMessage() );
    }

    void PIDSAdhocMessagePage::setMessageSelectionListener( IMessageSelectionListener* messageSelectionListener )
    {
        m_messageSelectionListener = messageSelectionListener;
    }

    void PIDSAdhocMessagePage::SetDefaults()
    {
        setDisplayMode( m_defaultAttributes.displayMode );
        setJustification( m_defaultAttributes.justification );
        setIntensity( m_defaultAttributes.intensity );
        setFontSize( m_defaultAttributes.latinFontSize );
    }




    /**
      * hasValidMessage
      *
      * test if a message has been entered.
      *
      * @return true if a valid message has been entered.
      */
    bool PIDSAdhocMessagePage::hasValidMessage() const
    {
        CString message;
        m_messageContent.GetWindowText( message );
		return !message.IsEmpty();
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
    PIDSAdhocMessagePage::AdHocMessage PIDSAdhocMessagePage::getMessage() const
    {
        FUNCTION_ENTRY( "getMessage" );

        TA_ASSERT( hasValidMessage(),
                   "getMessage called when no valid message present" );

        AdHocMessage message;

        // get the message content
        CString messageText;
        m_messageContent.GetWindowText( messageText );
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

        LOG_FUNCTION_EXIT( SourceInfo, "PIDSAdhocMessagePage::getMessage" );
        return message;
    }


    TA_Base_Core::ETTISLEDFontSize PIDSAdhocMessagePage::getFontSize() const
    {
        int selected = m_fontSize.GetCurSel();
        switch ( selected )
        {
            case 1:
                return TA_Base_Core::TTIS_FONT_SIZE_LARGE;
            case 0:
            default:
                return TA_Base_Core::TTIS_FONT_SIZE_SMALL;
        }
    }


    void PIDSAdhocMessagePage::setFontSize( TA_Base_Core::ETTISLEDFontSize fontSize )
    {
        switch ( fontSize )
        {
            case TA_Base_Core::FONT_SIZE_LARGE:
                m_fontSize.SetCurSel( 1 );
                break;
            case TA_Base_Core::FONT_SIZE_SMALL:
            default:
                m_fontSize.SetCurSel( 0 );
                break;
        }
    }


    TA_Base_Core::EJustification PIDSAdhocMessagePage::getJustification() const
    {
        int selected = m_justification.GetCurSel();
        switch ( selected )
        {
            case 1:
                return TA_Base_Core::CENTRED;
            case 2:
                return TA_Base_Core::RIGHT;
            case 0:
            default:
                return TA_Base_Core::LEFT;
        }
    }


    void PIDSAdhocMessagePage::setJustification( TA_Base_Core::EJustification justification )
    {
        switch ( justification )
        {
            case TA_Base_Core::CENTRED:
                m_justification.SetCurSel( 1 );
                break;
            case TA_Base_Core::RIGHT:
                m_justification.SetCurSel( 2 );
                break;
            case TA_Base_Core::LEFT:
            default:
                m_justification.SetCurSel( 0 );
                break;
        }
    }


    TA_Base_Core::ETTISLEDIntensity PIDSAdhocMessagePage::getIntensity() const
    {
        int selected = m_intensity.GetCurSel();
        switch ( selected )
        {
            case 1:
                return TA_Base_Core::HIGH;
            case 0:
            default:
                return TA_Base_Core::NORMAL;
        }
    }


    void PIDSAdhocMessagePage::setIntensity( TA_Base_Core::ETTISLEDIntensity intensity )
    {
        switch ( intensity )
        {
            case TA_Base_Core::HIGH:
                m_intensity.SetCurSel( 1 );
                break;
            case TA_Base_Core::NORMAL:
            default:
                m_intensity.SetCurSel( 0 );
                break;
        }
    }


    TA_Base_Core::EDisplayMode PIDSAdhocMessagePage::getDisplayMode() const
    {
        int selected = m_displayMode.GetCurSel();
        switch ( selected )
        {
            case 1:
                return TA_Base_Core::SCROLL_RIGHT;
            case 2:
                return TA_Base_Core::SCROLL_UP;
            case 3:
                return TA_Base_Core::SCROLL_DOWN;
            case 4:
                return TA_Base_Core::NORMAL_STATIONARY;
            case 5:
                return TA_Base_Core::BLINKING;
            case 6:
                return TA_Base_Core::WIPING;
            case 7:
                return TA_Base_Core::SNOW;
            case 0:
            default:
                return TA_Base_Core::SCROLL_LEFT;
        }
    }


    void PIDSAdhocMessagePage::setDisplayMode( TA_Base_Core::EDisplayMode displayMode )
    {
        switch ( displayMode )
        {
            case TA_Base_Core::SCROLL_RIGHT:
                m_displayMode.SetCurSel( 1 );
                break;
            case TA_Base_Core::SCROLL_UP:
                m_displayMode.SetCurSel( 2 );
                break;
            case TA_Base_Core::SCROLL_DOWN:
                m_displayMode.SetCurSel( 3 );
                break;
            case TA_Base_Core::NORMAL_STATIONARY:
                m_displayMode.SetCurSel( 4 );
                break;
            case TA_Base_Core::BLINKING:
                m_displayMode.SetCurSel( 5 );
                break;
            case TA_Base_Core::WIPING:
                m_displayMode.SetCurSel( 6 );
                break;
            case TA_Base_Core::SNOW:
                m_displayMode.SetCurSel( 7 );
                break;
            case TA_Base_Core::SCROLL_LEFT:
            default:
                m_displayMode.SetCurSel( 0 );
                break;
        }
		OnDisplayModeChange();
    }



    BOOL PIDSAdhocMessagePage::PreTranslateMessage( MSG* pMsg )
    {
        switch ( pMsg->message )
        {
            case WM_KEYDOWN:
				if(pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
				{
					return TRUE;
				}
				break;
            default:
				break;
        }
        return CDialog::PreTranslateMessage( pMsg );
    }
}

// PIDSAdhocMessagePage message handlers
