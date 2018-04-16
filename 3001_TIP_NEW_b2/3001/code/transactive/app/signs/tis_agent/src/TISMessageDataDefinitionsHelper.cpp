/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/TISMessageDataDefinitionsHelper.cpp $
 * @author:  Robin Ashcroft
 * @version: $Revision: #4 $
 *
 * Last modification: $DateTime: 2012/03/28 15:52:59 $
 * Last modified by:  $Author: huang.wenbo $
 *
 * This is used to listen for download completion update messages (from the Train Agent)
 *
 */

#include "app/signs/tis_agent/src/StringHelper.h"
#include "app/signs/tis_agent/src/TISMessageDataDefinitionsHelper.h"

#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "core/utilities/src/DebugUtil.h"

#include <iomanip>
#include <iostream>
#include <sstream>


using namespace std;

std::string TISMessageDataDefinitionsHelper::convertDisplayAttributeToString( const TA_Base_Core::DisplayAttributes &displayAttribute )
{
    FUNCTION_ENTRY( "convertDisplayAttributeToString" );
    stringstream returnValue;

    returnValue << "Display Mode " << convertEDisplayModeToString( displayAttribute.displayMode ) << ", ";

    returnValue << "Justification " << convertEJustificationToString( displayAttribute.justification ) << ", ";

    returnValue << "Scroll Speed " << convertEScrollSpeedToString( displayAttribute.scrollSpeed ) << ", ";

    returnValue << "Display Time " << displayAttribute.displayTime << " Seconds";

    FUNCTION_EXIT;
    return returnValue.str();
}

std::string TISMessageDataDefinitionsHelper::convertLCDFontSizeToString( const TA_Base_Core::ELcdFontSize lcdFontSize )
{

    FUNCTION_ENTRY( "convertLCDFontSizeToString" );
    switch ( lcdFontSize )
    {
        case TA_Base_Core::LCD_SMALL_FONT:
            FUNCTION_EXIT;
            return "LCD Small Font";
        case TA_Base_Core::LCD_MEDIUM_FONT:
            FUNCTION_EXIT;
            return "LCD Medium Font";
        case TA_Base_Core::LCD_LARGE_FONT:
            FUNCTION_EXIT;
            return "LCD Large Font";
        default:
            FUNCTION_EXIT;
            return "UNKNOWN";
    }
}

std::string TISMessageDataDefinitionsHelper::convertLCDFontColourToString( const TA_Base_Core::ELcdFontColour lcdFontColour )
{
    FUNCTION_ENTRY( "convertLCDFontColourToString" );
    switch ( lcdFontColour )
    {
        case TA_Base_Core::BLACK:
            FUNCTION_EXIT;
            return "Black";
        case TA_Base_Core::WHITE:
            FUNCTION_EXIT;
            return "White";
        case TA_Base_Core::RED:
            FUNCTION_EXIT;
            return "Red";
        case TA_Base_Core::GREEN:
            FUNCTION_EXIT;
            return "Green";
        case TA_Base_Core::BLUE:
            FUNCTION_EXIT;
            return "Blue";
        case TA_Base_Core::YELLOW:
            FUNCTION_EXIT;
            return "Yellow";
        case TA_Base_Core::CYAN:
            FUNCTION_EXIT;
            return "Cyan";
        case TA_Base_Core::MAGENTA:
            FUNCTION_EXIT;
            return "Magenta";
        case TA_Base_Core::ORANGE:
            FUNCTION_EXIT;
            return "Orange";
        default:
            FUNCTION_EXIT;
            return "UNKNOWN";
    }
}

std::string TISMessageDataDefinitionsHelper::convertLCDAttributeToString( const TA_Base_Core::LCDAttributesSTIS &lcdAttribute )
{
    FUNCTION_ENTRY( "convertLCDAttributeToString" );
    stringstream returnValue;
    returnValue << "Font Size " << convertLCDFontSizeToString( lcdAttribute.fontSize ) << ", ";
    returnValue << "Font Colour " << convertLCDFontColourToString( lcdAttribute.fontColour );
    FUNCTION_EXIT;
    return returnValue.str();

}


std::string TISMessageDataDefinitionsHelper::convertLEDAttributesToString( const TA_Base_Core::LEDAttributes &ledAttribute )
{
    FUNCTION_ENTRY( "convertLEDAttributesToString" );
    stringstream returnValue;
    returnValue << "Font Size " << convertELEDFontSizeToString( ledAttribute.fontSize ) << ", ";
    returnValue << "Font Colour" << convertELEDColourToString( ledAttribute.fontColour );
    FUNCTION_EXIT;
    return returnValue.str();
}

/*
std::string TISMessageDataDefinitionsHelper::convertSTISLedAttributesToString( const TA_Base_Core::STISLedAttributes &stisLedAttribute )
{
    FUNCTION_ENTRY( "convertSTISLedAttributesToString" );
    stringstream returnValue;

    returnValue << convertEDisplayModeToString( stisLedAttribute.displayMode ) << ", ";

    returnValue << convertEJustificationToString( stisLedAttribute.justification ) << ", ";

    returnValue << convertEScrollSpeedToString( stisLedAttribute.scrollSpeed ) << ", ";

    returnValue << convertELEDFontSizeToString( stisLedAttribute.fontSize ) << ", ";

    returnValue << convertELEDIntensityToString( stisLedAttribute.intensity ) << ", ";

    returnValue << convertELEDColourToString( stisLedAttribute.fontColour ) << ", ";

    returnValue << convertELEDColourToString( stisLedAttribute.backgroundColour ) << ", ";

    returnValue << stisLedAttribute.validityPeriod << ", ";
    returnValue << stisLedAttribute.repeatInterval << ", ";
    returnValue << stisLedAttribute.displayTime;

    FUNCTION_EXIT;
    return returnValue.str();
}
*/


std::string TISMessageDataDefinitionsHelper::convertTTISLedAttributesToString( const TA_Base_Core::TTISLedAttributes &ttisLedAttribute )
{
    FUNCTION_ENTRY( "convertTTISLedAttributesToString" );
    stringstream returnValue;

    returnValue << convertEDisplayModeToString( ttisLedAttribute.displayMode ) << ", ";

    returnValue << convertEJustificationToString( ttisLedAttribute.justification ) << ", ";

    returnValue << convertETTISLEDFontSizeToString( ttisLedAttribute.latinFontSize ) << ", ";

    returnValue << convertETTISLEDIntensityToString( ttisLedAttribute.intensity ) << ", ";

    returnValue << ttisLedAttribute.perRun;

    FUNCTION_EXIT;
    return returnValue.str();
}

/*
std::string TISMessageDataDefinitionsHelper::convertSTISPlasmaAttributesToString( const TA_Base_Core::STISPlasmaAttributes &stisPlasmaAttribute )
{
    FUNCTION_ENTRY( "convertSTISPlasmaAttributesToString" );
    stringstream returnValue;

    returnValue << convertEDisplayModeToString( stisPlasmaAttribute.displayMode ) << ", ";

    returnValue << convertEJustificationToString( stisPlasmaAttribute.justification ) << ", ";

    returnValue << convertEScrollSpeedToString( stisPlasmaAttribute.scrollSpeed ) << ", ";

    returnValue << convertEPlasmaFontSizeToString( stisPlasmaAttribute.fontSize ) << ", ";

    returnValue << convertEPlasmaColourToString( stisPlasmaAttribute.fontColour ) << ", ";

    returnValue << convertEFontTypeToString( stisPlasmaAttribute.fontType ) << ", ";

    returnValue << convertEPlasmaColourToString( stisPlasmaAttribute.backgroundColour ) << ", ";

    returnValue << stisPlasmaAttribute.validityPeriod << ", ";
    returnValue << stisPlasmaAttribute.repeatInterval << ", ";
    returnValue << stisPlasmaAttribute.displayTime;

    FUNCTION_EXIT;
    return returnValue.str();
}
*/


std::string TISMessageDataDefinitionsHelper::convertELEDIntensityToString( const TA_Base_Core::ELEDIntensity intensity )
{
    FUNCTION_ENTRY( "convertELEDIntensityToString" );
    switch ( intensity )
    {
        case TA_Base_Core::INTENSITY_LOW:
            FUNCTION_EXIT;
            return "Low";
        case TA_Base_Core::INTENSITY_MEDIUM:
            FUNCTION_EXIT;
            return "Medium";
        case TA_Base_Core::INTENSITY_HIGH:
            FUNCTION_EXIT;
            return "High";
        default:
            FUNCTION_EXIT;
            return "UNKNOWN";
    }
}

std::string TISMessageDataDefinitionsHelper::convertELEDFontSizeToString( const TA_Base_Core::ELEDFontSize fontSize )
{
    FUNCTION_ENTRY( "convertELEDFontSizeToString" );
    switch ( fontSize )
    {
        case TA_Base_Core::FONT_SIZE_SMALL:
            FUNCTION_EXIT;
            return "LED Small Font";
        case TA_Base_Core::FONT_SIZE_MEDIUM:
            FUNCTION_EXIT;
            return "LED Medium Font";
        case TA_Base_Core::FONT_SIZE_LARGE:
            FUNCTION_EXIT;
            return "LED Large Font";
        default:
            FUNCTION_EXIT;
            return "UNKNOWN";
    }
}

std::string TISMessageDataDefinitionsHelper::convertETTISLEDFontSizeToString( const TA_Base_Core::ETTISLEDFontSize fontSize )
{
    switch ( fontSize )
    {
        case TA_Base_Core::TTIS_FONT_SIZE_SMALL:
            return "Small";
        case TA_Base_Core::TTIS_FONT_SIZE_LARGE:
            return "Large";
        default:
            return "UNKNOWN";
    }
}
std::string TISMessageDataDefinitionsHelper::convertEFontTypeToString( const TA_Base_Core::EFontType fontType )
{
    FUNCTION_ENTRY( "convertEFontTypeToString" );
    switch ( fontType )
    {
        case TA_Base_Core::DEFAULT_PLASMA_FONT:
            FUNCTION_EXIT;
            return "DEFAULT_PLASMA_FONT";
        case TA_Base_Core::TIMES_NEW_ROMAN:
            FUNCTION_EXIT;
            return "TIMES_NEW_ROMAN";
        case TA_Base_Core::ARIAL:
            FUNCTION_EXIT;
            return "ARIAL";
        case TA_Base_Core::COURIER_NEW:
            FUNCTION_EXIT;
            return "COURIER_NEW";
        case TA_Base_Core::PLASMA_FONT_RESERVED4:
            FUNCTION_EXIT;
            return "PLASMA_FONT_RESERVED4";
        case TA_Base_Core::PLASMA_FONT_RESERVED5:
            FUNCTION_EXIT;
            return "PLASMA_FONT_RESERVED5";
        case TA_Base_Core::PLASMA_FONT_RESERVED6:
            FUNCTION_EXIT;
            return "PLASMA_FONT_RESERVED6";
        case TA_Base_Core::PLASMA_FONT_RESERVED7:
            FUNCTION_EXIT;
            return "PLASMA_FONT_RESERVED7";
        case TA_Base_Core::PLASMA_FONT_RESERVED8:
            FUNCTION_EXIT;
            return "PLASMA_FONT_RESERVED8";
        case TA_Base_Core::PLASMA_FONT_RESERVED9:
            FUNCTION_EXIT;
            return "PLASMA_FONT_RESERVED9";
        default:
            FUNCTION_EXIT;
            return "UNKNOWN";
    }
}

std::string TISMessageDataDefinitionsHelper::convertEJustificationToString( const TA_Base_Core::EJustification justification )
{
    FUNCTION_ENTRY( "convertEJustificationToString" );
    switch ( justification )
    {
        case TA_Base_Core::LEFT:
            FUNCTION_EXIT;
            return "Left";
        case TA_Base_Core::CENTRED:
            FUNCTION_EXIT;
            return "Centre";
        case TA_Base_Core::RIGHT:
            FUNCTION_EXIT;
            return "Right";
        default:
            FUNCTION_EXIT;
            return "UNKNOWN";
    }
}

std::string TISMessageDataDefinitionsHelper::convertEScrollSpeedToString( const TA_Base_Core::EScrollSpeed scrollSpeed )
{
    FUNCTION_ENTRY( "convertEScrollSpeedToString" );
    switch ( scrollSpeed )
    {
        case TA_Base_Core::SCROLL_SLOW:
            FUNCTION_EXIT;
            return "Slow";
        case TA_Base_Core::SCROLL_MEDIUM:
            FUNCTION_EXIT;
            return "Medium";
        case TA_Base_Core::SCROLL_FAST:
            FUNCTION_EXIT;
            return "Fast";
        default:
            FUNCTION_EXIT;
            return "UNKNOWN";
    }
}



std::string  TISMessageDataDefinitionsHelper::convertEDisplayModeToString( const TA_Base_Core::EDisplayMode displayMode )
{
    FUNCTION_ENTRY( "convertEDisplayModeToString" );
    switch ( displayMode )
    {
        case TA_Base_Core::SCROLL_LEFT:
            FUNCTION_EXIT;
            return "Scroll Left";
        case TA_Base_Core::SCROLL_RIGHT:
            FUNCTION_EXIT;
            return "Scroll Right";
        case TA_Base_Core::SCROLL_UP:
            FUNCTION_EXIT;
            return "Scroll Up";
        case TA_Base_Core::SCROLL_DOWN:
            FUNCTION_EXIT;
            return "Scroll Down";
        case TA_Base_Core::NORMAL_STATIONARY:
            FUNCTION_EXIT;
            return "Normal Stationary";
        case TA_Base_Core::BLINKING:
            FUNCTION_EXIT;
            return "Blinking";
        case TA_Base_Core::WIPING:
            FUNCTION_EXIT;
            return "Wiping";
        case TA_Base_Core::SNOW:
            FUNCTION_EXIT;
            return "Snow";
        case TA_Base_Core::INSTANT_ON_STIS:
            FUNCTION_EXIT;
            return "Instant On";
        case TA_Base_Core::SCROLL_LEFT_STIS:
            FUNCTION_EXIT;
            return "Scroll Left";
        case TA_Base_Core::SCROLL_RIGHT_STIS:
            FUNCTION_EXIT;
            return "Scroll Right";
        case TA_Base_Core::ROLL_UP_STIS:
            FUNCTION_EXIT;
            return "Roll Up";
        case TA_Base_Core::ROLL_DOWN_STIS:
            FUNCTION_EXIT;
            return "Roll Down";
        case TA_Base_Core::WIPE_LEFT_STIS:
            FUNCTION_EXIT;
            return "Wipe Left";
        case TA_Base_Core::WIPE_RIGHT_STIS:
            FUNCTION_EXIT;
            return "Wipe Right";
        case TA_Base_Core::LEFT_MULTIPLE_WIPING_STIS:
            FUNCTION_EXIT;
            return "Left Multiple Wipe";
        case TA_Base_Core::RIGHT_MULTIPLE_WIPING_STIS:
            FUNCTION_EXIT;
            return "Right Multiple Wipe";
        case TA_Base_Core::CURTAIN_OPEN_STIS:
            FUNCTION_EXIT;
            return "Curtain Open";
        case TA_Base_Core::CURTAIN_CLOSE_STIS:
            FUNCTION_EXIT;
            return "Curtain Close";
        case TA_Base_Core::RANDOM_ORDER_STIS:
            FUNCTION_EXIT;
            return "Random Order";
        case TA_Base_Core::CENTER_SPREAD_STIS:
            FUNCTION_EXIT;
            return "Centre Spread";
        case TA_Base_Core::TWO_ENDS_SPREAD_STIS:
            FUNCTION_EXIT;
            return "Two Ends Spread";
        case TA_Base_Core::FLASHING_STIS:
            FUNCTION_EXIT;
            return "Flashing";
        case TA_Base_Core::BLINKING_STIS:
            FUNCTION_EXIT;
            return "Blinking";
        case TA_Base_Core::COLOR_CHANGE_STIS:
            FUNCTION_EXIT;
            return "Colour Change";
        case TA_Base_Core::DROPPING_STIS:
            FUNCTION_EXIT;
            return "Dropping";
        default:
            FUNCTION_EXIT;
            return "UNKNOWN";
    }
}



std::string TISMessageDataDefinitionsHelper::convertELEDColourToString( const TA_Base_Core::ELEDColour fontColor )
{
    FUNCTION_ENTRY( "convertELEDColourToString" );

    switch ( fontColor )
    {
        case TA_Base_Core::LED_RED:
            FUNCTION_EXIT;
            return "Red";
        case TA_Base_Core::LED_GREEN:
            FUNCTION_EXIT;
            return "Green";
        case TA_Base_Core::LED_ORANGE:
            FUNCTION_EXIT;
            return "Orange";
        default:
            FUNCTION_EXIT;
            return "UNKNOWN";
    }
}

std::string TISMessageDataDefinitionsHelper::convertETTISLEDIntensityToString( const TA_Base_Core::ETTISLEDIntensity intensity )
{
    FUNCTION_ENTRY( "convertETTISLEDIntensityToString" );
    switch ( intensity )
    {
        case TA_Base_Core::NORMAL:
            FUNCTION_EXIT;
            return "Normal";
        case TA_Base_Core::HIGH:
            FUNCTION_EXIT;
            return "High";
        default:
            FUNCTION_EXIT;
            return "UNKNOWN";
    }
}


std::string TISMessageDataDefinitionsHelper::convertTimeStringToFormattedTimeString( const std::string& timeString )
{
    FUNCTION_ENTRY( "convertTimeStringToFormattedTimeString" );
    const int DATE_TIME_LENGTH = 14;
    const int TIME_LENGTH = 4;
    const int YEAR_LENGTH   = 4;
    const int MONTH_LENGTH  = 2;
    const int DAY_LENGTH    = 2;
    const int HOUR_LENGTH   = 2;
    const int MINUTE_LENGTH = 2;
    const int SECOND_LENGTH = 2;

    const int YEAR_POS   = 0;
    const int MONTH_POS  = 4;
    const int DAY_POS    = 6;
    const int HOUR_POS   = 8;
    const int MINUTE_POS = 10;
    const int SECOND_POS = 12;

    std::stringstream formattedTimeString;

    //timeString = YYYYMMDDHHMMSS, STIS date_time format
    if ( timeString.length() == DATE_TIME_LENGTH ) // must be of format YYYYMMDDHHMMSS
    {
        string sYear = timeString.substr( YEAR_POS, YEAR_LENGTH );
        string sMonth = timeString.substr( MONTH_POS, MONTH_LENGTH );
        string sDay = timeString.substr( DAY_POS, DAY_LENGTH );
        string sHour = timeString.substr( HOUR_POS, HOUR_LENGTH );
        string sMinute = timeString.substr( MINUTE_POS, MINUTE_LENGTH );
        string sSecond = timeString.substr( SECOND_POS, SECOND_LENGTH );

        long year, month, day, hour, min, sec;
        if ( ( StringHelper::isNumeric( sYear, year ) == false ) ||
                ( StringHelper::isNumeric( sMonth, month ) == false ) ||
                ( StringHelper::isNumeric( sDay, day ) == false ) ||
                ( StringHelper::isNumeric( sHour, hour ) == false ) ||
                ( StringHelper::isNumeric( sMinute, min ) == false ) ||
                ( StringHelper::isNumeric( sSecond, sec ) == false ) )
        {
            FUNCTION_EXIT;
            return "";
        }

        formattedTimeString << sDay << "/" << sMonth << "/" << sYear
        << " " << sHour << ":" << sMinute << ":" << sSecond;

        FUNCTION_EXIT;
        return formattedTimeString.str();
    }
    //timeString = HHMM, TTIS time format
    else if ( timeString.length() == TIME_LENGTH )
    {
        string sHour = timeString.substr( HOUR_POS - 8, HOUR_LENGTH );
        string sMinute = timeString.substr( MINUTE_POS - 8, MINUTE_LENGTH );

        long hour, min;
        if ( ( StringHelper::isNumeric( sHour, hour ) == false ) ||
                ( StringHelper::isNumeric( sMinute, min ) == false ) )
        {
            FUNCTION_EXIT;
            return "";
        }

        formattedTimeString << sHour << ":" << sMinute;

        FUNCTION_EXIT;
        return formattedTimeString.str();
    }

    FUNCTION_EXIT;
    return "";
}
std::string TISMessageDataDefinitionsHelper::convertTrainPIDListToString( const TA_Base_Core::TrainPIDList& pidList )
{
    FUNCTION_ENTRY( "convertTrainPIDListToString" );

    std::ostringstream pidListStream;
    for ( unsigned long i = 0 ; i < pidList.length() ; ++i )
    {
        if ( i != 0 )
        {
            pidListStream << ", ";
        }
        pidListStream << convertTrainPIDToString( pidList[i] );
    }

    FUNCTION_EXIT;
    return pidListStream.str();
}
std::string TISMessageDataDefinitionsHelper::convertTrainPIDToString( TA_Base_Core::EPIDSelection pid )
{
    FUNCTION_ENTRY( "convertTrainPIDToString" );
	if( TA_Base_Core::ENTIRE_TRAIN == pid ) // 955 can only control all train
	{
        FUNCTION_EXIT;
        return "ENTIRE_TRAIN";
	}
    FUNCTION_EXIT;
    return "";
}
std::string TISMessageDataDefinitionsHelper::convertETTISMessagePriorityToString( TA_Base_Core::ETTISMessagePriority priority )
{
    FUNCTION_ENTRY( "convertETTISMessagePriorityToString" );

    switch ( priority )
    {
        case TA_Base_Core::TTIS_PRIORITY_1:
            FUNCTION_EXIT;
            return "1";
        case TA_Base_Core::TTIS_PRIORITY_2:
            FUNCTION_EXIT;
            return "2";
        case TA_Base_Core::TTIS_PRIORITY_3:
            FUNCTION_EXIT;
            return "3";
        case TA_Base_Core::TTIS_PRIORITY_4:
            FUNCTION_EXIT;
            return "4";
        case TA_Base_Core::TTIS_PRIORITY_5:
            FUNCTION_EXIT;
            return "5";
        case TA_Base_Core::TTIS_PRIORITY_6:
            FUNCTION_EXIT;
            return "6";
        case TA_Base_Core::TTIS_PRIORITY_7:
            FUNCTION_EXIT;
            return "7";
        case TA_Base_Core::TTIS_PRIORITY_8:
            FUNCTION_EXIT;
            return "8";
    }
    FUNCTION_EXIT;
    return "";
}


std::string TISMessageDataDefinitionsHelper::convertETTISClearTypeToString( TA_Base_Core::ETTISClearType clearType )
{
    FUNCTION_ENTRY( "convertETTISClearTypeToString" );

    switch ( clearType )
    {
        case TA_Base_Core::CLEAR_ALL:
            FUNCTION_EXIT;
            return "CLEAR_ALL";
        case TA_Base_Core::CLEAR_EMERGENCY:
            FUNCTION_EXIT;
            return "CLEAR_EMERGENCY";
        case TA_Base_Core::CLEAR_NORMAL:
            FUNCTION_EXIT;
            return "CLEAR_NORMAL";
        case TA_Base_Core::CLEAR_PRIORITY6:
            FUNCTION_EXIT;
            return "CLEAR_PRIORITY6";
    }
    FUNCTION_EXIT;
    return "";
}


char TISMessageDataDefinitionsHelper::convertDisplayModeToChar( TA_Base_Core::EDisplayMode displayMode )
{
    char defaultVal = '0';
    int tmp = displayMode - TA_Base_Core::INSTANT_ON_STIS;
    stringstream ss;
    if ( tmp < 0 || tmp > 17 )
    {
        return defaultVal;
    }
    else if ( tmp <= 15 )
    {
        ss << std::hex << std::uppercase << tmp;
        ss >> defaultVal;
    }
    else if ( tmp == 16 )
    {
        defaultVal = 'G';
    }
    else
    {
        defaultVal = 'H';
    }
    return defaultVal;
}

char TISMessageDataDefinitionsHelper::convertScrollSpeedToChar( TA_Base_Core::EScrollSpeed scrollSpeed )
{
    char defaultVal = '8';
    switch ( scrollSpeed )
    {
        case TA_Base_Core::SCROLL_SLOW:
            defaultVal = '0';
            break;
        case TA_Base_Core::SCROLL_MEDIUM:
            defaultVal = '8';
            break;
        case TA_Base_Core::SCROLL_FAST:
            defaultVal = 'F';
            break;
    }
    return defaultVal;
}

char TISMessageDataDefinitionsHelper::convertLcdFontSizeToChar( TA_Base_Core::ELcdFontSize lcdFontSize )
{
    char defaultVal = '2';
    switch ( lcdFontSize )
    {
        case TA_Base_Core::LCD_SMALL_FONT:
            defaultVal = '1';
            break;
        case TA_Base_Core::LCD_MEDIUM_FONT:
            defaultVal = '2';
            break;
        case TA_Base_Core::LCD_LARGE_FONT:
            defaultVal = '3';
            break;
    }
    return defaultVal;
}

char TISMessageDataDefinitionsHelper::convertLedFontSizeToChar( TA_Base_Core::ELEDFontSize ledFontSize )
{
    char defaultVal = '2';
    switch ( ledFontSize )
    {
        case TA_Base_Core::FONT_SIZE_SMALL:
            defaultVal = '1';
            break;
        case TA_Base_Core::FONT_SIZE_MEDIUM:
            defaultVal = '2';
            break;
        case TA_Base_Core::FONT_SIZE_LARGE:
            defaultVal = '3';
            break;
    }
    return defaultVal;
}

char TISMessageDataDefinitionsHelper::convertLedFontColourToChar( TA_Base_Core::ELEDFontColour ledFontColour )
{
    char defaultVal = '2';
    switch ( ledFontColour )
    {
        case TA_Base_Core::LED_RED:
            defaultVal = '2';
            break;
        case TA_Base_Core::LED_GREEN:
            defaultVal = '3';
            break;
        case TA_Base_Core::LED_ORANGE:
            defaultVal = '4';
            break;
    }
    return defaultVal;
}

TA_Base_Core::EDisplayMode TISMessageDataDefinitionsHelper::convertCharToDisplayMode( char displayModeCh )
{
    TA_Base_Core::EDisplayMode defaultVal = TA_Base_Core::INSTANT_ON_STIS;
    if ( '0' <= displayModeCh && displayModeCh <= '9' )
    {
		defaultVal = static_cast<TA_Base_Core::EDisplayMode>(defaultVal + ( displayModeCh - '0' ));
    }
    else if ( 'A' <= displayModeCh && displayModeCh <= 'H' )
    {
        defaultVal = static_cast<TA_Base_Core::EDisplayMode>(defaultVal + ( 10 + ( displayModeCh - 'A' )));
    }
    return defaultVal;
}

TA_Base_Core::EScrollSpeed TISMessageDataDefinitionsHelper::convertCharToScrollSpeed( char scrollSpeedCh )
{
    TA_Base_Core::EScrollSpeed defaultVal = TA_Base_Core::SCROLL_MEDIUM;
    switch ( scrollSpeedCh )
    {
        case '0':
            defaultVal = TA_Base_Core::SCROLL_SLOW;
            break;
        case '8':
            break;
        case 'F':
            defaultVal = TA_Base_Core::SCROLL_FAST;
            break;
    }
    return defaultVal;
}

TA_Base_Core::ELcdFontSize TISMessageDataDefinitionsHelper::convertCharToLcdFontSize( char lcdFontSizeCh )
{
    TA_Base_Core::ELcdFontSize defaultVal = TA_Base_Core::LCD_MEDIUM_FONT;
    switch ( lcdFontSizeCh )
    {
        case '1':
            defaultVal = TA_Base_Core::LCD_SMALL_FONT;
            break;
        case '2':
            break;
        case '3':
            defaultVal = TA_Base_Core::LCD_LARGE_FONT;
            break;
    }
    return defaultVal;
}

TA_Base_Core::ELEDFontSize TISMessageDataDefinitionsHelper::convertCharToLedFontSize( char ledFontSizeCh )
{
    TA_Base_Core::ELEDFontSize defaultVal = TA_Base_Core::FONT_SIZE_MEDIUM;
    switch ( ledFontSizeCh )
    {
        case '1':
            defaultVal = TA_Base_Core::FONT_SIZE_SMALL;
            break;
        case '2':
            break;
        case '3':
            defaultVal = TA_Base_Core::FONT_SIZE_LARGE;
            break;
    }
    return defaultVal;
}

TA_Base_Core::ELEDFontColour TISMessageDataDefinitionsHelper::convertCharToLedFontColour( char ledFontColourCh )
{
    TA_Base_Core::ELEDFontColour defaultVal = TA_Base_Core::LED_RED;
    switch ( ledFontColourCh )
    {
        case '2':
            break;
        case '3':
            defaultVal = TA_Base_Core::LED_GREEN;
            break;
        case '4':
            defaultVal = TA_Base_Core::LED_ORANGE;
            break;
    }
    return defaultVal;
}