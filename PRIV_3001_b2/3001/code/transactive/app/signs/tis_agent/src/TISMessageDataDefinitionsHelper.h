#ifndef TISMESSAGEDATADEFINITIONSHELPER_H_INCLUDED
#define TISMESSAGEDATADEFINITIONSHELPER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/TISMessageDataDefinitionsHelper.h $
  * @author:   James Mao
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  */

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"

#include <string>

class TISMessageDataDefinitionsHelper
{
public:
    static std::string convertDisplayAttributeToString(const TA_Base_Core::DisplayAttributes& displayAttribute);
	static std::string convertEDisplayModeToString(const TA_Base_Core::EDisplayMode displayMode);
	//scroll speed
    static std::string convertEScrollSpeedToString(const TA_Base_Core::EScrollSpeed scrollSpeed);

	//jusctification
    static std::string convertEJustificationToString(const TA_Base_Core::EJustification justification);

	//lcd attribute convert
	static std::string convertLCDFontSizeToString( const TA_Base_Core::ELcdFontSize lcdFontSize);
	static std::string convertLCDFontColourToString( const TA_Base_Core::ELcdFontColour lcdFontColour);
	static std::string convertLCDAttributeToString(const TA_Base_Core::LCDAttributesSTIS& lcdAttribute);

	//led attribute convert
    static std::string convertLEDAttributesToString(const TA_Base_Core::LEDAttributes& ledAttribute);
    static std::string convertELEDFontSizeToString(const TA_Base_Core::ELEDFontSize fontSize);
    static std::string convertELEDColourToString(const TA_Base_Core::ELEDColour fontColor);

	//used by ttis
    static std::string convertTTISLedAttributesToString(const TA_Base_Core::TTISLedAttributes& ttisLedAttribute);
    static std::string convertETTISLEDFontSizeToString(const TA_Base_Core::ETTISLEDFontSize fontSize);
    static std::string convertELEDIntensityToString(const TA_Base_Core::ELEDIntensity intensity);
    static std::string convertETTISLEDIntensityToString(const TA_Base_Core::ETTISLEDIntensity intensity);

	/**
	  * convertEFontTypeToString
	  * 
	  * <description>
	  * 
	  * @return static std::string 
	  * @param : const TA_Base_Core::EFontType
	  * 
	  * @exception <exceptions> Optional
	  */
    static std::string convertEFontTypeToString(const TA_Base_Core::EFontType fontType);

	/**
	  * convertTimeStringToFormattedTimeString
	  * 
	  * <description>
	  * 
	  * @return static std::string 
	  * @param : const std::string& timeString
	  * 
	  * @exception <exceptions> Optional
	  */
    static std::string convertTimeStringToFormattedTimeString(const std::string& timeString);


    /**
     * Converts a list of PIDs to a printable string for logging/auditing
     *
     * @return a printable string
     *
     * @param pidList    The PID list
     */
    static std::string convertTrainPIDListToString(const TA_Base_Core::TrainPIDList& pidList);


    /**
     * Converts a PID to a printable string for logging/auditing
     *
     * @return a printable string
     *
     * @param pid    The PID
     */
    static std::string convertTrainPIDToString(TA_Base_Core::EPIDSelection pid);


    /**
     * Converts a priority to a printable string for logging/auditing
     *
     * @return a printable string
     *
     * @param priority    The priority
     */
    static std::string convertETTISMessagePriorityToString(TA_Base_Core::ETTISMessagePriority priority);


    /**
     * Converts a PID to a printable string for logging/auditing
     *
     * @return a printable string
     *
     * @param clearType    The clear type
     */
    static std::string convertETTISClearTypeToString(TA_Base_Core::ETTISClearType clearType);


	// Convert display attributes to protocol value
	static char convertDisplayModeToChar(TA_Base_Core::EDisplayMode displayMode);
	static char convertScrollSpeedToChar(TA_Base_Core::EScrollSpeed scrollSpeed);
	static char convertLcdFontSizeToChar(TA_Base_Core::ELcdFontSize lcdFontSize);
	static char convertLedFontSizeToChar(TA_Base_Core::ELEDFontSize ledFontSize);
	static char convertLedFontColourToChar(TA_Base_Core::ELEDFontColour ledFontColour);


	static TA_Base_Core::EDisplayMode convertCharToDisplayMode(char displayModeCh);
	static TA_Base_Core::EScrollSpeed convertCharToScrollSpeed(char scrollSpeedCh);
	static TA_Base_Core::ELcdFontSize convertCharToLcdFontSize(char lcdFontSizeCh);
	static TA_Base_Core::ELEDFontSize convertCharToLedFontSize(char ledFontSizeCh);
	static TA_Base_Core::ELEDFontColour convertCharToLedFontColour(char ledFontColourCh);
};

#endif
