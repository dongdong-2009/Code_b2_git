#pragma once
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "CallBackComboBox.h"
#include "TimeControlManager.h"
#include "ITimeTypeListener.h"

namespace TA_IRS_App
{	
	class DisplayAttributes
	{
	public:
		DisplayAttributes()
		{
			m_displayAttributes.displayMode = TA_Base_Core::INSTANT_ON_STIS;
			m_displayAttributes.displayTime = 1;
			m_displayAttributes.scrollSpeed = TA_Base_Core::SCROLL_MEDIUM;
			m_displayAttributes.justification = TA_Base_Core::CENTRED;
			m_displayAttributes.repeatInterval = 1;
		}
		operator TA_Base_Core::DisplayAttributes() {return m_displayAttributes;}
	private:
		TA_Base_Core::DisplayAttributes m_displayAttributes;
	};

	class LCDAttributesSTIS
	{
	public:
		LCDAttributesSTIS()
		{
			m_LcdAttributes.fontColour = TA_Base_Core::WHITE;
			m_LcdAttributes.fontSize = TA_Base_Core::LCD_MEDIUM_FONT;
		}
		operator TA_Base_Core::LCDAttributesSTIS() {return m_LcdAttributes;}
	private:
		TA_Base_Core::LCDAttributesSTIS m_LcdAttributes;
	};

	class LEDAttributes
	{
	public:
		LEDAttributes()
		{
			m_LedAttributes.fontColour = TA_Base_Core::LED_RED;
			m_LedAttributes.fontSize = TA_Base_Core::FONT_SIZE_MEDIUM;
		}
		operator TA_Base_Core::LEDAttributes() {return m_LedAttributes;}
	private:
		TA_Base_Core::LEDAttributes m_LedAttributes;
	};

    class DisplayAttributesManager : public IComboBoxListener,
                                     public ITimeTypeListener
    {
        public:
            DisplayAttributesManager(
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
				CStatic& scrollSpeedLabel);
            virtual ~DisplayAttributesManager();

            virtual void init();
            virtual void timeTypeChanged( TimeControlManager::TimeType newTimeType );
            virtual void selectedChanged( CallBackComboBox* control );
            //displaymode
            TA_Base_Core::EDisplayMode getDisplayMode();
            void setDisplayMode( TA_Base_Core::EDisplayMode displayMode );

            //displaytime
            unsigned int getDisplayTime();
            void setDisplayTime( unsigned int displayTime );

            //scroll speed
            TA_Base_Core::EScrollSpeed getScrollSpeed();
            void setScrollSpeed( TA_Base_Core::EScrollSpeed scrollSpeed );

            //justification
            TA_Base_Core::EJustification getJustification();
            void setJustification( TA_Base_Core::EJustification justification );

            //lcd attributes
            TA_Base_Core::ELcdFontColour getLcdFontColour();
            void setLcdFontColour( TA_Base_Core::ELcdFontColour lcdFontColour );
            TA_Base_Core::ELcdFontSize getLcdFontSize();
            void setLcdFontSize( TA_Base_Core::ELcdFontSize lcdFontSize );

            //led attributes
            TA_Base_Core::ELEDColour getLedFontColour();
            void setLedFontColour( TA_Base_Core::ELEDColour ledFontColour );

            TA_Base_Core::ELEDFontSize getLedFontSize();
            void setLedFontSize( TA_Base_Core::ELEDFontSize ledFontSize );

            void enableDisplayTime();
            void disableDisplayTime();

			void enableJustification(bool isEnable);
			void enableScrollSpeed(bool isEnable);
        protected:
            CComboBox&    m_lcdFontColour;
            CComboBox&       m_lcdFontSize;
            CStatic&       m_displayTimeLabel;
            CDateTimeCtrl& m_displayTime;
            CComboBox&     m_ledFontSize;
            CComboBox&     m_ledFontColor;
            CComboBox&     m_scrollSpeed;
            CComboBox&     m_justification;
            CallBackComboBox&     m_displayMode;
			CStatic&       m_justificationLabel;
			CStatic&       m_scrollSpeedLabel;
            TimeControlManager::TimeType m_currentTimeType;
            static const unsigned int MAX_DISPLAY_TIME;
    };

    class TSDisplayAttributesManager : public DisplayAttributesManager
    {
        public:
            TSDisplayAttributesManager(
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
				CStatic& scrollSpeedLabel)
				:DisplayAttributesManager( displayMode,
                                           displayTimeLabel,
                                           displayTime,
                                           scrollSpeed,
                                           justification,
                                           lcdFontColour,
                                           lcdFontSize,
                                           ledFontColour,
                                           ledFontSize,
										   justificationLabel,
										   scrollSpeedLabel)
			{
			}

            virtual void timeTypeChanged( TimeControlManager::TimeType newTimeType )
			{
				return;
			}

            virtual void selectedChanged( CallBackComboBox* control )
			{
                TA_Base_Core::EDisplayMode displayMode = getDisplayMode();
				bool isScrollMode = (TA_Base_Core::SCROLL_LEFT_STIS == displayMode
					|| TA_Base_Core::SCROLL_RIGHT_STIS == displayMode
					|| TA_Base_Core::ROLL_UP_STIS == displayMode
					|| TA_Base_Core::ROLL_DOWN_STIS == displayMode);
				if(isScrollMode)
				{
					// Any scroll mode, disable display time
					disableDisplayTime();
					// Any scroll mode, disable justification
					enableJustification(false);
					// Any scroll mode, enable scroll speed
					enableScrollSpeed(true);
				}
				else
				{
					// Enable display time
					enableDisplayTime();
					// Enable justification
					enableJustification(true);
					// Disable scroll speed
					enableScrollSpeed(false);
				}	           
		 }
    };
}