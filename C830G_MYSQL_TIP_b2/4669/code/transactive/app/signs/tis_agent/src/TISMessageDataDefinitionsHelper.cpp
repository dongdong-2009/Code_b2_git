 /**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/TISMessageDataDefinitionsHelper.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
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

std::string TISMessageDataDefinitionsHelper::convertDisplayAttributeToString(const TA_Base_Core::DisplayAttributes &displayAttribute)
{
    FUNCTION_ENTRY( "convertDisplayAttributeToString" );
    stringstream returnValue;
    
    returnValue << "Display Mode " << convertEDisplayModeToString(displayAttribute.displayMode) << ", ";

    returnValue << "Justification " << convertEJustificationToString(displayAttribute.justification) << ", ";

    returnValue << "Scroll Speed " << convertEScrollSpeedToString(displayAttribute.scrollSpeed) << ", ";

    returnValue << "Display Time " << displayAttribute.displayTime << " Seconds";

    FUNCTION_EXIT;
    return returnValue.str();
}

std::string TISMessageDataDefinitionsHelper::convertPlasmaAttributeToString(const TA_Base_Core::PlasmaAttributes &plasmaAttribute)
{
    FUNCTION_ENTRY( "convertPlasmaAttributeToString" );
    stringstream returnValue;

    returnValue << "Font Size " << convertEPlasmaFontSizeToString(plasmaAttribute.fontSize) << ", ";

    returnValue << "Font Colour " << convertEPlasmaColourToString(plasmaAttribute.fontColour) << ", ";

    returnValue << "Background Colour " << convertEPlasmaColourToString(plasmaAttribute.backgroundColour);

    FUNCTION_EXIT;
    return returnValue.str();
}

std::string TISMessageDataDefinitionsHelper::convertLEDAttributesToString(const TA_Base_Core::LEDAttributes &ledAttribute)
{
    FUNCTION_ENTRY( "convertLEDAttributesToString" );
    stringstream returnValue;

    returnValue << "Font Size " << convertELEDFontSizeToString(ledAttribute.fontSize);

    FUNCTION_EXIT;
    return returnValue.str();
}

std::string TISMessageDataDefinitionsHelper::convertSTISLedAttributesToString(const TA_Base_Core::STISLedAttributes &stisLedAttribute)
{
    FUNCTION_ENTRY( "convertSTISLedAttributesToString" );
    stringstream returnValue;

    returnValue << convertEDisplayModeToString(stisLedAttribute.displayMode) << ", ";

    returnValue << convertEJustificationToString(stisLedAttribute.justification) << ", ";

    returnValue << convertEScrollSpeedToString(stisLedAttribute.scrollSpeed) << ", ";

    returnValue << convertELEDFontSizeToString(stisLedAttribute.fontSize) << ", ";

    returnValue << convertELEDIntensityToString(stisLedAttribute.intensity) << ", ";
    
    returnValue << convertELEDColourToString(stisLedAttribute.fontColour) << ", ";

    returnValue << convertELEDColourToString(stisLedAttribute.backgroundColour) << ", ";

    returnValue << stisLedAttribute.validityPeriod << ", ";
    returnValue << stisLedAttribute.repeatInterval << ", ";
    returnValue << stisLedAttribute.displayTime;
    
    FUNCTION_EXIT;
    return returnValue.str();
}

std::string TISMessageDataDefinitionsHelper::convertTTISLedAttributesToString(const TA_Base_Core::TTISLedAttributes &ttisLedAttribute)
{
    FUNCTION_ENTRY( "convertTTISLedAttributesToString" );
    stringstream returnValue;

    returnValue << convertEDisplayModeToString(ttisLedAttribute.displayMode) << ", ";

    returnValue << convertEJustificationToString(ttisLedAttribute.justification) << ", ";

    returnValue << convertEScrollSpeedToString(ttisLedAttribute.scrollSpeed) << ", ";

    returnValue << convertELEDFontSizeToString(ttisLedAttribute.latinFontSize) << ", ";

    returnValue << convertETTISLEDIntensityToString(ttisLedAttribute.intensity) << ", ";

    returnValue << ttisLedAttribute.validityPeriod << ", ";
    returnValue << ttisLedAttribute.repeatInterval << ", ";
    returnValue << ttisLedAttribute.displayTime;

    FUNCTION_EXIT;
    return returnValue.str();
}

std::string TISMessageDataDefinitionsHelper::convertSTISPlasmaAttributesToString(const TA_Base_Core::STISPlasmaAttributes &stisPlasmaAttribute)
{
    FUNCTION_ENTRY( "convertSTISPlasmaAttributesToString" );
    stringstream returnValue;
  
    returnValue << convertEDisplayModeToString(stisPlasmaAttribute.displayMode) << ", ";

    returnValue << convertEJustificationToString(stisPlasmaAttribute.justification) << ", ";

    returnValue << convertEScrollSpeedToString(stisPlasmaAttribute.scrollSpeed) << ", ";

    returnValue << convertEPlasmaFontSizeToString(stisPlasmaAttribute.fontSize) << ", ";
    
    returnValue << convertEPlasmaColourToString(stisPlasmaAttribute.fontColour) << ", ";

    returnValue << convertEFontTypeToString(stisPlasmaAttribute.fontType) << ", ";

    returnValue << convertEPlasmaColourToString(stisPlasmaAttribute.backgroundColour) << ", ";

    returnValue << stisPlasmaAttribute.validityPeriod << ", ";
    returnValue << stisPlasmaAttribute.repeatInterval << ", ";
    returnValue << stisPlasmaAttribute.displayTime;
        
    FUNCTION_EXIT;
    return returnValue.str();
}


std::string TISMessageDataDefinitionsHelper::convertELEDIntensityToString(const TA_Base_Core::ELEDIntensity &intensity)
{
    FUNCTION_ENTRY( "convertELEDIntensityToString" );
    switch (intensity)
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

std::string TISMessageDataDefinitionsHelper::convertELEDFontSizeToString(const TA_Base_Core::ELEDFontSize &fontSize)
{
    FUNCTION_ENTRY( "convertELEDFontSizeToString" );
    switch (fontSize)
    {
    case TA_Base_Core::UNUSED_LED_FONT_SIZE:
        FUNCTION_EXIT;
        return "UNUSED_LED_FONT_SIZE";
    case TA_Base_Core::FONT_SIZE_SMALL:
        FUNCTION_EXIT;
        return "Small";
    case TA_Base_Core::FONT_SIZE_MEDIUM:
        FUNCTION_EXIT;
        return "Medium";
    case TA_Base_Core::FONT_SIZE_LARGE:
        FUNCTION_EXIT;
        return "Large";
    default:
        FUNCTION_EXIT;
        return "UNKNOWN";
    }
}

std::string TISMessageDataDefinitionsHelper::convertETTISLEDFontSizeToString(const TA_Base_Core::ETTISLEDFontSize &fontSize)
{
    switch (fontSize)
    {
    case TA_Base_Core::TTIS_FONT_SIZE_SMALL:
        return "Small";
    case TA_Base_Core::TTIS_FONT_SIZE_LARGE:
        return "Large";
    default:
        return "UNKNOWN";
    }
}
std::string TISMessageDataDefinitionsHelper::convertEFontTypeToString(const TA_Base_Core::EFontType &fontType)
{
    FUNCTION_ENTRY( "convertEFontTypeToString" );
    switch (fontType)
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

std::string TISMessageDataDefinitionsHelper::convertEJustificationToString(const TA_Base_Core::EJustification &justification)
{
    FUNCTION_ENTRY( "convertEJustificationToString" );
    switch (justification)
    {
    case TA_Base_Core::LEFT:
        FUNCTION_EXIT;
        return "Left";
    case TA_Base_Core::CENTRED:
        FUNCTION_EXIT;
        return "Centred";
    case TA_Base_Core::RIGHT:
        FUNCTION_EXIT;
        return "Right";
    default:
        FUNCTION_EXIT;
        return "UNKNOWN";
    }
}
 
std::string TISMessageDataDefinitionsHelper::convertEScrollSpeedToString(const TA_Base_Core::EScrollSpeed &scrollSpeed)
{
    FUNCTION_ENTRY( "convertEScrollSpeedToString" );
    switch (scrollSpeed)
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

std::string TISMessageDataDefinitionsHelper::convertEPlasmaFontSizeToString(const TA_Base_Core::EPlasmaFontSize & fontSize)
{
    FUNCTION_ENTRY( "convertEPlasmaFontSizeToString" );
    switch (fontSize)
    {
    case TA_Base_Core::PLASMA_FONT_SIZE_UNUSED:
        FUNCTION_EXIT;
        return "PLASMA_FONT_SIZE_UNUSED";
    case TA_Base_Core::PLASMA_FONT_SIZE_24:
        FUNCTION_EXIT;
        return "24";
    case TA_Base_Core::PLASMA_FONT_SIZE_36:
        FUNCTION_EXIT;
        return "36";
    case TA_Base_Core::PLASMA_FONT_SIZE_42:
        FUNCTION_EXIT;
        return "42";
    case TA_Base_Core::PLASMA_FONT_SIZE_48:
        FUNCTION_EXIT;
        return "48";
    case TA_Base_Core::PLASMA_FONT_SIZE_60:
        FUNCTION_EXIT;
        return "60";
    case TA_Base_Core::PLASMA_FONT_SIZE_72:
        FUNCTION_EXIT;
        return "72";
    case TA_Base_Core::PLASMA_FONT_SIZE_96:
        FUNCTION_EXIT;
        return "96";
    case TA_Base_Core::PLASMA_FONT_SIZE_128:
        FUNCTION_EXIT;
        return "128";
    default:
        FUNCTION_EXIT;
        return "UNKNOWN";
    }
}

std::string  TISMessageDataDefinitionsHelper::convertEDisplayModeToString(const TA_Base_Core::EDisplayMode &displayMode)
{
    FUNCTION_ENTRY( "convertEDisplayModeToString" );
    switch (displayMode)
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
    case TA_Base_Core::INSTANT_ON:
        FUNCTION_EXIT;
        return "Instant On";
    case TA_Base_Core::BLINKING:
        FUNCTION_EXIT;
        return "Blinking";
    case TA_Base_Core::WIPING:
        FUNCTION_EXIT;
        return "Wiping";
    case TA_Base_Core::SNOW:
        FUNCTION_EXIT;
        return "Snow";
    default:
        FUNCTION_EXIT;
        return "UNKNOWN";
    }        
}

std::string TISMessageDataDefinitionsHelper::convertEPlasmaColourToString(const TA_Base_Core::EPlasmaColour &fontColor)
{
    FUNCTION_ENTRY( "convertEPlasmaColourToString" );
    switch (fontColor)
    {
    case TA_Base_Core::PLASMA_BLACK:
        FUNCTION_EXIT;
        return "Black";
    case TA_Base_Core::PLASMA_WHITE:
        FUNCTION_EXIT;
        return "White";
    case TA_Base_Core::PLASMA_RED:
        FUNCTION_EXIT;
        return "Red";
    case TA_Base_Core::PLASMA_GREEN:
        FUNCTION_EXIT;
        return "Green";
    case TA_Base_Core::PLASMA_BLUE:
        FUNCTION_EXIT;
        return "Blue";
    case TA_Base_Core::PLASMA_YELLOW:
        FUNCTION_EXIT;
        return "Yellow";
    case TA_Base_Core::PLASMA_CYAN:
        FUNCTION_EXIT;
        return "Cyan";
    case TA_Base_Core::PLASMA_MAGENTA:
        FUNCTION_EXIT;
        return "Magenta";
    case TA_Base_Core::PLASMA_ORANGE:
        FUNCTION_EXIT;
        return "Orange";
    case TA_Base_Core::PLASMA_BROWN:
        FUNCTION_EXIT;
        return "Brown";
    case TA_Base_Core::PLASMA_GREY:
        FUNCTION_EXIT;
        return "Grey";
    case TA_Base_Core::PLASMA_LIGHT_BLUE:
        FUNCTION_EXIT;
        return "Light blue";//TD17905 AVteam
    case TA_Base_Core::PLASMA_LIGHT_GREEN:
        FUNCTION_EXIT;
        return "Light green";//TD17905 AVteam
    case TA_Base_Core::PLASMA_LIGHT_RED:
        FUNCTION_EXIT;
        return "Light red";//TD17905 AVteam
    case TA_Base_Core::PLASMA_COLOUR_RESERVED_E:
        FUNCTION_EXIT;
        return "PLASMA_COLOUR_RESERVED_E";
    case TA_Base_Core::PLASMA_COLOUR_RESERVED_F:
        FUNCTION_EXIT;
        return "PLASMA_COLOUR_RESERVED_F";
    default:
        FUNCTION_EXIT;
        return "UNKNOWN";
    }        
}

std::string TISMessageDataDefinitionsHelper::convertELEDColourToString(const TA_Base_Core::ELEDColour &fontColor)
{
    FUNCTION_ENTRY( "convertELEDColourToString" );

    switch (fontColor)
    {
    case TA_Base_Core::UNUSED_LED_COLOUR:
        FUNCTION_EXIT;
        return "UNUSED_LED_COLOUR";
    case TA_Base_Core::LED_BLACK:
        FUNCTION_EXIT;
        return "Black";
    case TA_Base_Core::LED_RED:
        FUNCTION_EXIT;
        return "Red";
    default:

        FUNCTION_EXIT;
        return "UNKNOWN";
    }
}

std::string TISMessageDataDefinitionsHelper::convertETTISLEDIntensityToString(const TA_Base_Core::ETTISLEDIntensity &intensity)
{
    FUNCTION_ENTRY( "convertETTISLEDIntensityToString" );
    switch (intensity)
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

std::string TISMessageDataDefinitionsHelper::convertTimeStringToFormattedTimeString(const std::string &timeString)
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
	if (timeString.length() == DATE_TIME_LENGTH) // must be of format YYYYMMDDHHMMSS
	{
	    string sYear = timeString.substr(YEAR_POS, YEAR_LENGTH);
	    string sMonth = timeString.substr(MONTH_POS, MONTH_LENGTH);
	    string sDay = timeString.substr(DAY_POS, DAY_LENGTH);
	    string sHour = timeString.substr(HOUR_POS, HOUR_LENGTH);
	    string sMinute = timeString.substr(MINUTE_POS, MINUTE_LENGTH);
	    string sSecond = timeString.substr(SECOND_POS, SECOND_LENGTH);

	    long year, month, day, hour, min, sec;
        if ((StringHelper::isNumeric(sYear, year) == false) ||
		    (StringHelper::isNumeric(sMonth, month) == false) ||
		    (StringHelper::isNumeric(sDay, day) == false) ||
		    (StringHelper::isNumeric(sHour, hour) == false) ||
		    (StringHelper::isNumeric(sMinute, min) == false) ||
		    (StringHelper::isNumeric(sSecond, sec) == false))
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
    else if (timeString.length() == TIME_LENGTH)
    {
        string sHour = timeString.substr(HOUR_POS - 8, HOUR_LENGTH);
        string sMinute = timeString.substr(MINUTE_POS - 8, MINUTE_LENGTH);
        
        long hour, min;
		if ((StringHelper::isNumeric(sHour, hour) == false) ||
		    (StringHelper::isNumeric(sMinute, min) == false))
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
std::string TISMessageDataDefinitionsHelper::convertTrainPIDListToString(const TA_Base_Core::TrainPIDList& pidList)
{
    FUNCTION_ENTRY( "convertTrainPIDListToString" );

    std::ostringstream pidListStream;
    for( unsigned int i = 0 ; i < pidList.length() ; ++i )
    {
        if( i != 0 )
        {
            pidListStream << ", ";
        }
        pidListStream << convertTrainPIDToString( pidList[i] );
    }

    FUNCTION_EXIT;
    return pidListStream.str();
}
std::string TISMessageDataDefinitionsHelper::convertTrainPIDToString(TA_Base_Core::EPIDSelection pid)
{
    FUNCTION_ENTRY( "convertTrainPIDToString" );

    switch ( pid )
    {
    case TA_Base_Core::ENTIRE_TRAIN:
        FUNCTION_EXIT;
        return "ENTIRE_TRAIN";
        break;
    case TA_Base_Core::ALL_CAR1:
        FUNCTION_EXIT;
        return "All Car 1";
        break;
    case TA_Base_Core::ALL_CAR2:
        FUNCTION_EXIT;
        return "All Car 2";
        break;
    case TA_Base_Core::ALL_CAR3:
        FUNCTION_EXIT;
        return "All Car 3";
        break;
    case TA_Base_Core::CAR1_PID1:
        FUNCTION_EXIT;
        return "Car 1 PID 1";
        break;
    case TA_Base_Core::CAR1_PID2:
        FUNCTION_EXIT;
        return "Car 1 PID 2";
        break;
    case TA_Base_Core::CAR1_PID3:
        FUNCTION_EXIT;
        return "Car 1 PID 3";
        break;
    case TA_Base_Core::CAR1_PID4:
        FUNCTION_EXIT;
        return "Car 1 PID 4";
        break;
    case TA_Base_Core::CAR2_PID1:
        FUNCTION_EXIT;
        return "Car 2 PID 1";
        break;
    case TA_Base_Core::CAR2_PID2:
        FUNCTION_EXIT;
        return "Car 2 PID 2";
        break;
    case TA_Base_Core::CAR2_PID3:
        FUNCTION_EXIT;
        return "Car 2 PID 3";
        break;
    case TA_Base_Core::CAR2_PID4:
        FUNCTION_EXIT;
        return "Car 2 PID 4";
        break;
    case TA_Base_Core::CAR3_PID1:
        FUNCTION_EXIT;
        return "Car 3 PID 1";
        break;
    case TA_Base_Core::CAR3_PID2:
        FUNCTION_EXIT;
        return "Car 3 PID 2";
        break;
    case TA_Base_Core::CAR3_PID3:
        FUNCTION_EXIT;
        return "Car 3 PID 3";
        break;
    case TA_Base_Core::CAR3_PID4:
        FUNCTION_EXIT;
        return "Car 3 PID 4";
        break;
    }

    FUNCTION_EXIT;
    return "";
}
std::string TISMessageDataDefinitionsHelper::convertETTISMessagePriorityToString(TA_Base_Core::ETTISMessagePriority priority)
{
    FUNCTION_ENTRY( "convertETTISMessagePriorityToString" );

    switch ( priority )
    {
    case TA_Base_Core::TTIS_PRIORITY_1:
        FUNCTION_EXIT;
        return "1";
        break;
    case TA_Base_Core::TTIS_PRIORITY_2:
        FUNCTION_EXIT;
        return "2";
        break;
    case TA_Base_Core::TTIS_PRIORITY_3:
        FUNCTION_EXIT;
        return "3";
        break;
    case TA_Base_Core::TTIS_PRIORITY_4:
        FUNCTION_EXIT;
        return "4";
        break;
    case TA_Base_Core::TTIS_PRIORITY_5:
        FUNCTION_EXIT;
        return "5";
        break;
    case TA_Base_Core::TTIS_PRIORITY_6:
        FUNCTION_EXIT;
        return "6";
        break;
    case TA_Base_Core::TTIS_PRIORITY_7:
        FUNCTION_EXIT;
        return "7";
        break;
    case TA_Base_Core::TTIS_PRIORITY_8:
        FUNCTION_EXIT;
        return "8";
        break;
    }

    FUNCTION_EXIT;
    return "";
}


std::string TISMessageDataDefinitionsHelper::convertETTISClearTypeToString(TA_Base_Core::ETTISClearType clearType)
{
    FUNCTION_ENTRY( "convertETTISClearTypeToString" );

    switch ( clearType )
    {
    case TA_Base_Core::CLEAR_ALL:
        FUNCTION_EXIT;
        return "CLEAR_ALL";
        break;
    case TA_Base_Core::CLEAR_EMERGENCY:
        FUNCTION_EXIT;
        return "CLEAR_EMERGENCY";
        break;
    case TA_Base_Core::CLEAR_NORMAL:
        FUNCTION_EXIT;
        return "CLEAR_NORMAL";
        break;
    case TA_Base_Core::CLEAR_PRIORITY6:
        FUNCTION_EXIT;
        return "CLEAR_PRIORITY6";
        break;
    }

    FUNCTION_EXIT;
    return "";
}


