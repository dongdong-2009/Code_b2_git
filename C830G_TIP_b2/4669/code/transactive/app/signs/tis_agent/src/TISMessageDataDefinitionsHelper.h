#ifndef TISMESSAGEDATADEFINITIONSHELPER_H_INCLUDED
#define TISMESSAGEDATADEFINITIONSHELPER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/TISMessageDataDefinitionsHelper.h $
  * @author:   James Mao
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"

#include <string>

class TISMessageDataDefinitionsHelper
{
public:
    /**
      * convertDisplayAttributeToString
      * 
      * helper function to convert DisplayAttributes into a string
      *
      * @return DisplayAttributes as a string
      *
      * @param : const TA_IRS_Core::DisplayAttributes & displayAttribute
      */
    static std::string convertDisplayAttributeToString(const TA_Base_Core::DisplayAttributes &displayAttribute);

	/**
	  * convertPlasmaAttributeToString
	  * 
	  * <description>
	  * 
	  * @return static std::string 
	  * @param : const TA_Base_Core::PlasmaAttributes &plasmaAttribute
	  * 
	  * @exception <exceptions> Optional
	  */
    static std::string convertPlasmaAttributeToString(const TA_Base_Core::PlasmaAttributes &plasmaAttribute);

	/**
	  * convertLEDAttributesToString
	  * 
	  * <description>
	  * 
	  * @return static std::string 
	  * @param : const TA_Base_Core::LEDAttributes &ledAttribute
	  * 
	  * @exception <exceptions> Optional
	  */
    static std::string convertLEDAttributesToString(const TA_Base_Core::LEDAttributes &ledAttribute);

	/**
	  * convertSTISLedAttributesToString
      * 
      * helper function to convert STISLedAttributes into a string
      *
      * @return STISLedAttributes as a string
      *
	  * @param : const TA_IRS_Core::STISLedAttributes &STISLedAttribute
	  */
    static std::string convertSTISLedAttributesToString(const TA_Base_Core::STISLedAttributes &stisLedAttribute);

	/**
	  * convertSTISPlasmaAttributesToString
      * 
      * helper function to convert STISPlasmaAttributes into a string
      *
      * @return STISPlasmaAttributes as a string
      *
	  * @param : const TA_IRS_Core::STISPlasmaAttributes &stisPlasmaAttribute
	  */
    static std::string convertSTISPlasmaAttributesToString(const TA_Base_Core::STISPlasmaAttributes &stisPlasmaAttribute);

	/**
	  * convertTTISLedAttributesToString
      * 
      * helper function to convert TTISLedAttributes into a string
      *
      * @return TTISLedAttributes as a string
      *
	  * @param : const TA_IRS_Core::TTISLedAttributes &ttisLedAttribute
	  */
    static std::string convertTTISLedAttributesToString(const TA_Base_Core::TTISLedAttributes &ttisLedAttribute);

	/**
	  *  convertDisplayModeToString
	  * 
	  * <description>
	  * 
	  * @return static std::string 
	  * @param : const TA_Base_Core::EDisplayMode
	  * 
	  * @exception <exceptions> Optional
	  */
    static std::string  convertEDisplayModeToString(const TA_Base_Core::EDisplayMode &displayMode);

	/**
	  * convertEScrollSpeedToString
	  * 
	  * <description>
	  * 
	  * @return static std::string 
	  * @param : const TA_Base_Core::EScrollSpeed
	  * 
	  * @exception <exceptions> Optional
	  */
    static std::string convertEScrollSpeedToString(const TA_Base_Core::EScrollSpeed &scrollSpeed);

    /**
	  * convertEJustificationToString
	  * 
	  * <description>
	  * 
	  * @return static std::string 
	  * @param : const TA_Base_Core::EJustification
	  * 
	  * @exception <exceptions> Optional
	  */
    static std::string convertEJustificationToString(const TA_Base_Core::EJustification &justification);

	/**
	  * convertlatinFontSizeToString
	  * 
	  * <description>
	  * 
	  * @return static std::string 
	  * @param : const TA_Base_Core::latinFontSize
	  * 
	  * @exception <exceptions> Optional
	  */
    static std::string convertELEDFontSizeToString(const TA_Base_Core::ELEDFontSize &fontSize);

	/**
    /**
      * convertlatinFontSizeToString
      *
      * <description>
      *
      * @return static std::string
      * @param : const TA_Base_Core::latinFontSize
      *
      * @exception <exceptions> Optional
      */
    static std::string convertETTISLEDFontSizeToString(const TA_Base_Core::ETTISLEDFontSize &fontSize);


    /**
	  * convertIntensityToString
	  * 
	  * <description>
	  * 
	  * @return static std::string 
	  * @param : const TA_Base_Core::intensity
	  * 
	  * @exception <exceptions> Optional
	  */
    static std::string convertELEDIntensityToString(const TA_Base_Core::ELEDIntensity &intensity);

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
    static std::string convertEFontTypeToString(const TA_Base_Core::EFontType &fontType);

	/**
	  * convertEPlasmaFontSizeToString
	  * 
	  * <description>
	  * 
	  * @return static std::string 
	  * @param : const TA_Base_Core::EPlasmaFontSize & fontSize
	  * 
	  * @exception <exceptions> Optional
	  */
    static std::string convertEPlasmaFontSizeToString(const TA_Base_Core::EPlasmaFontSize &fontSize);

	/**
	  * convertEPlasmaColourToString
	  * 
	  * <description>
	  * 
	  * @return static std::string 
	  * @param : const TA_Base_Core::EPlasmaColour &fontColor
	  * 
	  * @exception <exceptions> Optional
	  */
    static std::string convertEPlasmaColourToString(const TA_Base_Core::EPlasmaColour &fontColor);

	/**
	  * convertELEDColourToString
	  * 
	  * <description>
	  * 
	  * @return static std::string 
	  * @param : const TA_Base_Core::ELEDColour &fontColor
	  * 
	  * @exception <exceptions> Optional
	  */
    static std::string convertELEDColourToString(const TA_Base_Core::ELEDColour &fontColor);
        
	/**
	  * convertETTISLEDIntensityToString
	  * 
	  * <description>
	  * 
	  * @return static std::string 
	  * @param : const TA_Base_Core::ETTISLEDIntensity &intensity
	  * 
	  * @exception <exceptions> Optional
	  */
    static std::string convertETTISLEDIntensityToString(const TA_Base_Core::ETTISLEDIntensity &intensity);

	/**
	  * convertTimeStringToFormattedTimeString
	  * 
	  * <description>
	  * 
	  * @return static std::string 
	  * @param : const std::string timeString
	  * 
	  * @exception <exceptions> Optional
	  */
    static std::string convertTimeStringToFormattedTimeString(const std::string &timeString);


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
};

#endif
