#include "stdafx.h"
#include "resource.h"
#include "DisplayAttributesManager.h"
//only use the two file, no need to place it to base library
#include "app/signs/tis_agent/src/StringHelper.cpp"
#include "app/signs/tis_agent/src/TISMessageDataDefinitionsHelper.cpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_IRS_App
{
    // This is the max repeat interval, 959 seconds
    const unsigned int DisplayAttributesManager::MAX_DISPLAY_TIME = 959;

    DisplayAttributesManager::DisplayAttributesManager(
        CallBackComboBox& displayMode,
        CStatic& displayTimeLabel,
        CDateTimeCtrl& displayTime,
        CComboBox& scrollSpeed,
        CComboBox& justification,
        CComboBox& lcdFontColour,
        CComboBox& lcdFontSize,
        CComboBox& ledFontColour,
        CComboBox& ledFontSize,
		CStatic& justificationLabel,
		CStatic& scrollSpeedLabel):
            m_displayMode( displayMode ),
            m_displayTimeLabel( displayTimeLabel ),
            m_displayTime( displayTime ),
            m_scrollSpeed( scrollSpeed ),
            m_justification( justification ),
            m_lcdFontColour( lcdFontColour ),
            m_lcdFontSize( lcdFontSize ),
            m_ledFontColor( ledFontColour ),
            m_ledFontSize( ledFontSize ),
			m_justificationLabel( justificationLabel ),
			m_scrollSpeedLabel( scrollSpeedLabel )
    {
        m_displayMode.setCallback( this );
    }

    DisplayAttributesManager::~DisplayAttributesManager()
    {
        m_displayMode.setCallback( NULL );
    }


    void DisplayAttributesManager::init()
    {
		TISMessageDataDefinitionsHelper helper;

		//display mode
		for(int displayMode = TA_Base_Core::INSTANT_ON_STIS;
			displayMode <= TA_Base_Core::DROPPING_STIS;
			++displayMode)
		{
			m_displayMode.AddString(helper.convertEDisplayModeToString(static_cast<TA_Base_Core::EDisplayMode>(displayMode)).c_str());
		}

		//scroll speed
		for(int scrollSpeed = TA_Base_Core::SCROLL_SLOW;
			scrollSpeed <= TA_Base_Core::SCROLL_FAST;
			++scrollSpeed)
		{
			m_scrollSpeed.AddString(helper.convertEScrollSpeedToString(static_cast<TA_Base_Core::EScrollSpeed>(scrollSpeed)).c_str());
		}

		//justification
		for(int justification = TA_Base_Core::LEFT;
			justification <=  TA_Base_Core::RIGHT;
			++justification)
		{
			m_justification.AddString(helper.convertEJustificationToString(static_cast<TA_Base_Core::EJustification>(justification)).c_str());
		}

		//led font size
		for(int ledFontSize = TA_Base_Core::FONT_SIZE_SMALL;
			ledFontSize <= TA_Base_Core::FONT_SIZE_LARGE;
			++ledFontSize)
		{
			m_ledFontSize.AddString(helper.convertELEDFontSizeToString(static_cast<TA_Base_Core::ELEDFontSize>(ledFontSize)).c_str());
		}

		//led font colour
		for(int ledFontColor = TA_Base_Core::LED_RED;
			ledFontColor <= TA_Base_Core::LED_ORANGE;
			++ledFontColor)
		{
			m_ledFontColor.AddString(helper.convertELEDColourToString(static_cast<TA_Base_Core::ELEDFontColour>(ledFontColor)).c_str());
		}

		//lcd font size
		for(int lcdFontSize = TA_Base_Core::LCD_SMALL_FONT;
			lcdFontSize <= TA_Base_Core::LCD_LARGE_FONT;
			++lcdFontSize)
		{
			m_lcdFontSize.AddString(helper.convertLCDFontSizeToString(static_cast<TA_Base_Core::ELcdFontSize>(lcdFontSize)).c_str());
		}

		//lcd font colour
		for(int lcdFontColor = TA_Base_Core::BLACK;
			lcdFontColor <= TA_Base_Core::ORANGE;
			++lcdFontColor)
		{
			m_lcdFontColour.AddString(helper.convertLCDFontColourToString(static_cast<TA_Base_Core::ELcdFontColour>(lcdFontColor)).c_str());
		}
        // set the display time range (1 to 959 secs)
        CTime lowerTime( 1971, 1, 1, 0, 0, 1, 0 );
        // set the repeat interval
        unsigned short minutes = MAX_DISPLAY_TIME / 60;
        unsigned short seconds = MAX_DISPLAY_TIME - ( minutes * 60 );
        CTime upperTime( 1971, 1, 1, 0, minutes, seconds, 0 );
        m_displayTime.SetRange( &lowerTime, &upperTime );
        m_displayTime.SetFormat( "mm:ss" );
    }

    void DisplayAttributesManager::timeTypeChanged( TimeControlManager::TimeType newTimeType )
    {
        m_currentTimeType = newTimeType;
        TA_Base_Core::EDisplayMode displayMode = getDisplayMode();
		bool isScrollMode = (TA_Base_Core::SCROLL_LEFT_STIS == displayMode
			|| TA_Base_Core::SCROLL_RIGHT_STIS == displayMode
			|| TA_Base_Core::ROLL_UP_STIS == displayMode
			|| TA_Base_Core::ROLL_DOWN_STIS == displayMode);

		if (TimeControlManager::MESSAGE_CYCLIC == m_currentTimeType)
		{
			if(isScrollMode)			
			{
				disableDisplayTime();				
			}
			else
			{
				enableDisplayTime();
			}
		}
        else
        {
            disableDisplayTime();
        }
    }

    void DisplayAttributesManager::enableDisplayTime()
    {
        m_displayTime.SetFormat( " mm:ss" );
        m_displayTime.EnableWindow( TRUE );
        m_displayTimeLabel.EnableWindow( TRUE );
    }

    void DisplayAttributesManager::disableDisplayTime()
    {
        m_displayTime.SetFormat( " " );
        m_displayTime.EnableWindow( FALSE );
        m_displayTimeLabel.EnableWindow( FALSE );
    }

    TA_Base_Core::EDisplayMode DisplayAttributesManager::getDisplayMode()
    {
        int selected = m_displayMode.GetCurSel();
        TA_Base_Core::EDisplayMode displayMode = TA_Base_Core::INSTANT_ON_STIS;
        if ( selected > -1 )
        {
			return static_cast<TA_Base_Core::EDisplayMode>( selected + TA_Base_Core::INSTANT_ON_STIS );
        }
        return displayMode;
    }

    void DisplayAttributesManager::setDisplayMode( TA_Base_Core::EDisplayMode displayMode )
    {
		if (( displayMode < TA_Base_Core::INSTANT_ON_STIS ) || (displayMode > TA_Base_Core::DROPPING_STIS))
        {
            m_displayMode.SetCurSel( 0 );
        }
        else
        {
            m_displayMode.SetCurSel( displayMode - TA_Base_Core::INSTANT_ON_STIS );
        }
        selectedChanged( &m_displayMode );
    }

    //displaytime
    unsigned int DisplayAttributesManager::getDisplayTime()
    {
        CTime displayTime;
        m_displayTime.GetTime( displayTime );
        return ( displayTime.GetMinute() * 60 ) + ( displayTime.GetSecond() );
    }

    void DisplayAttributesManager::setDisplayTime( unsigned int displayTime )
    {
        if ( displayTime > MAX_DISPLAY_TIME )
        {
            displayTime = MAX_DISPLAY_TIME;
        }

        // set the repeat interval
        unsigned short minutes = displayTime / 60;
        unsigned short seconds = displayTime % 60;

        CTime temp( 1971, 1, 1, 0, minutes, seconds, 0 );
        m_displayTime.SetTime( &temp );
    }

    //scroll speed
    TA_Base_Core::EScrollSpeed DisplayAttributesManager::getScrollSpeed()
    {
        TA_Base_Core::EScrollSpeed scrollSpeed = TA_Base_Core::SCROLL_MEDIUM;
        int selected = m_scrollSpeed.GetCurSel();
		if( selected > -1 )
		{
			scrollSpeed = static_cast<TA_Base_Core::EScrollSpeed>(selected);
		}
        return scrollSpeed;
    }

    void DisplayAttributesManager::setScrollSpeed( TA_Base_Core::EScrollSpeed scrollSpeed )
    {
		if((scrollSpeed < TA_Base_Core::SCROLL_SLOW) || (scrollSpeed > TA_Base_Core::SCROLL_FAST))
		{
			m_scrollSpeed.SetCurSel(TA_Base_Core::SCROLL_MEDIUM);
		}
		m_scrollSpeed.SetCurSel(scrollSpeed);
    }

    //justification
    TA_Base_Core::EJustification DisplayAttributesManager::getJustification()
    {
		TA_Base_Core::EJustification justification = TA_Base_Core::CENTRED;
        int selected = m_justification.GetCurSel();
        if ( selected > - 1)
		{
			justification = static_cast<TA_Base_Core::EJustification>(selected);
		}
        return justification;
    }

    void DisplayAttributesManager::setJustification( TA_Base_Core::EJustification justification )
    {
		if ((justification < TA_Base_Core::LEFT) || (justification > TA_Base_Core::RIGHT))
		{
			m_justification.SetCurSel(TA_Base_Core::CENTRED);
		}
		else
		{
			m_justification.SetCurSel(justification);
		}
    }

    //lcd attributes
    TA_Base_Core::ELcdFontColour DisplayAttributesManager::getLcdFontColour()
    {
        TA_Base_Core::ELcdFontColour fontColour = TA_Base_Core::WHITE; //white is the default colour
        int selected = m_lcdFontColour.GetCurSel();
        if ( selected > -1)
        {
            fontColour = static_cast<TA_Base_Core::ELcdFontColour>( selected );
        }
        return fontColour;
    }

    void DisplayAttributesManager::setLcdFontColour( TA_Base_Core::ELcdFontColour lcdFontColour )
    {
		if ((lcdFontColour < TA_Base_Core::BLACK) || (lcdFontColour > TA_Base_Core::ORANGE))
		{
			m_lcdFontColour.SetCurSel(TA_Base_Core::WHITE);
		}
		else
		{
			m_lcdFontColour.SetCurSel(lcdFontColour);
		}
    }

    TA_Base_Core::ELcdFontSize DisplayAttributesManager::getLcdFontSize()
    {
        TA_Base_Core::ELcdFontSize fontSize = TA_Base_Core::LCD_MEDIUM_FONT;
        int selected = m_lcdFontSize.GetCurSel();
		if( selected > -1 )
		{
			fontSize = static_cast<TA_Base_Core::ELcdFontSize>( selected );	
		}
        return fontSize;
    }

    void DisplayAttributesManager::setLcdFontSize( TA_Base_Core::ELcdFontSize lcdFontSize )
    {
		if ((lcdFontSize < TA_Base_Core::LCD_SMALL_FONT) || (lcdFontSize > TA_Base_Core::LCD_LARGE_FONT))
		{
			m_lcdFontSize.SetCurSel(TA_Base_Core::LCD_MEDIUM_FONT);
		}
		else
		{
			m_lcdFontSize.SetCurSel(lcdFontSize);
		}
    }

    //led attributes
    TA_Base_Core::ELEDColour DisplayAttributesManager::getLedFontColour()
    {
        TA_Base_Core::ELEDColour fontColour = TA_Base_Core::LED_RED;
        int selected = m_ledFontColor.GetCurSel();
		if ( selected > -1 )
		{
			fontColour = static_cast<TA_Base_Core::ELEDColour>( selected );
		}
		return fontColour;
    }

    void DisplayAttributesManager::setLedFontColour( TA_Base_Core::ELEDColour ledFontColour )
    {	
		if(ledFontColour < TA_Base_Core::LED_RED || ledFontColour > TA_Base_Core::LED_ORANGE)
		{
			m_ledFontColor.SetCurSel(TA_Base_Core::LED_RED); // set default color red
		}		
		else
		{
            m_ledFontColor.SetCurSel( ledFontColour);
		}
    }

    TA_Base_Core::ELEDFontSize DisplayAttributesManager::getLedFontSize()
    {

        TA_Base_Core::ELEDFontSize fontSize = TA_Base_Core::FONT_SIZE_MEDIUM;
        int selected = m_ledFontSize.GetCurSel();
		if ( selected > -1 )
		{
			fontSize = static_cast<TA_Base_Core::ELEDFontSize>( selected );
		}
		return fontSize;
    }

    void DisplayAttributesManager::setLedFontSize( TA_Base_Core::ELEDFontSize ledFontSize )
    {
		if((ledFontSize < TA_Base_Core::FONT_SIZE_SMALL) || (ledFontSize > TA_Base_Core::FONT_SIZE_LARGE))
		{
			m_ledFontSize.SetCurSel(TA_Base_Core::FONT_SIZE_MEDIUM);
		}
		else
		{
            m_ledFontSize.SetCurSel( ledFontSize);
		}
    }


    void DisplayAttributesManager::selectedChanged( CallBackComboBox* control )
    {
        if ( &m_displayMode == control )
        {
            TA_Base_Core::EDisplayMode displayMode = getDisplayMode();
			bool isScrollMode = (TA_Base_Core::SCROLL_LEFT_STIS == displayMode
				|| TA_Base_Core::SCROLL_RIGHT_STIS == displayMode
				|| TA_Base_Core::ROLL_UP_STIS == displayMode
				|| TA_Base_Core::ROLL_DOWN_STIS == displayMode);
			// Change display time
			if (TimeControlManager::MESSAGE_CYCLIC == m_currentTimeType)
			{
				// Any scroll mode, disable display time, else enable it
				if(isScrollMode)
				{
					disableDisplayTime();
				}
				else
				{
					enableDisplayTime();
				}				
			}
	        else
	        {
	            disableDisplayTime();
	        }

			if(isScrollMode)
			{
				// Any scroll mode, disable justification
                enableJustification(false);
				// Any scroll mode, enable scroll speed
				enableScrollSpeed(true);
			}		
			else
			{
				// Enable justification
				enableJustification(true);
				// Disable scroll speed
				enableScrollSpeed(false);
			}

        }
    }

	void DisplayAttributesManager::enableJustification(bool isEnable)
	{
		m_justificationLabel.EnableWindow(isEnable);
        m_justification.EnableWindow(isEnable);
	}
	void DisplayAttributesManager::enableScrollSpeed(bool isEnable)
	{
		m_scrollSpeedLabel.EnableWindow(isEnable);
        m_scrollSpeed.EnableWindow(isEnable);
	}

} // TA_IRS_App





