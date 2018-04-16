/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodChangeReference.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class encapsulates the Radio API SendSDS method.
  *
  */
///////////////////////////////////////////////////////////
//  RadioMethodChangeReference.cpp
//  Implementation of the Class RadioMethodChangeReference
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "RadioMethodChangeReference.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"
#include "app/radio/RadioTetraAgent/src/CallForwardingDetails.h"

namespace TA_IRS_App
{   
	const unsigned int CHANGE_REFERENCE_BASE_LENGTH     = 4;
    const unsigned int DETAILS_PARAMETER_START_POSITION = 4;
    const std::string ENABLE_FORWARDING                 = "K_SUPPLEMENTARY_SERVICES=+W";
    const std::string DISABLE_FORWARDING                = "K_SUPPLEMENTARY_SERVICES=-W";
    const std::string FORWARD_UNCONDITIONAL             = "K_FORWARD_UNCONDITIONAL=";
    const std::string FORWARD_NOT_REACHABLE             = "K_FORWARD_NOT_REACHABLE=";    
    const std::string TYPE                              = "K_TYPE";
    const std::string PATCH_ID                          = "K_PATCH_ID";
    const std::string OWNER_DISPATCHER                  = "K_OWNER_DISPATCHER";
    const std::string PATCH_HANG_TIME                   = "K_PATCH_HANG_TIMER";
    const std::string MEMBERS                           = "K_MEMBERS";
    const std::string SEARCH_MODE                       = "K_SEARCH_MODE";
    const std::string LEFT_VOLUME	                    = "K_VOLUME_L";
    const std::string RIGHT_VOLUME	                    = "K_VOLUME_R";

    // specific key values
    const std::string PATCH_TYPE                        = "P";
    const std::string FULL_SEARCH                       = "FULL";
    const std::string NORMAL_SEARCH                     = "NORMAL";

    
    const std::string PATCH_HANG_TIME_VALUE = "0";

    const std::string KEY_SEPARATOR     = "\n";
    const std::string VECTOR_SEPARATOR  = ";";

	const std::string DETAILS_SEPARATOR = ",";

    RadioMethodChangeReference::RadioMethodChangeReference(RadioSessionReference sessionRef, RadioCallReference callRef) 
                       : RadioMethodCallReference(CHANGE_REFERENCE_BASE_LENGTH,METHOD_CHANGE_REFERENCE
                                                  ,sessionRef,callRef)
    {
    }



    RadioMethodChangeReference::~RadioMethodChangeReference()
    {
    }


    void RadioMethodChangeReference::setForwardingDetails(const CallForwardingDetails& details)
    {
		FUNCTION_ENTRY("setForwardingDetails");
        // Determine if forwarding has been enabled or disabled
        if (TA_IRS_App::CallForwardingDetails::NeverForward != details.forwardAction)
        {
            // If enabled, start with the enable string
            std::string detailsString = ENABLE_FORWARDING + KEY_SEPARATOR;

            // Then add the specification for the TYPE of forwarding
			detailsString += FORWARD_NOT_REACHABLE;
            if (TA_IRS_App::CallForwardingDetails::ForwardNotReachable == details.forwardAction)
            {
				detailsString += std::string(details.destinationTSI) + VECTOR_SEPARATOR + std::string(details.destinationTSI);
            }
			else
			{
				detailsString += "NULL" + VECTOR_SEPARATOR + "NULL";
			}


            detailsString += KEY_SEPARATOR + FORWARD_UNCONDITIONAL;
            if (TA_IRS_App::CallForwardingDetails::ForwardAlways == details.forwardAction)
            {
				detailsString += std::string(details.destinationTSI) + VECTOR_SEPARATOR + std::string(details.destinationTSI);
            }
			else
			{
				detailsString += "NULL" + VECTOR_SEPARATOR + "NULL";
			}

            // And then finish off the details.
            detailsString += KEY_SEPARATOR;

            // Resize the method
            setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());

            // Add the string
            setParameterLongString(DETAILS_PARAMETER_START_POSITION,detailsString);
        }
        else
        {
            // Forwarding disabled, so only using the static text. Before adding it, need to resize the message.
            setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + DISABLE_FORWARDING.size() + KEY_SEPARATOR.size());
            setParameterLongString(DETAILS_PARAMETER_START_POSITION,DISABLE_FORWARDING + KEY_SEPARATOR);
        }
		FUNCTION_EXIT;
    }

    void RadioMethodChangeReference::setPatchDetails(const std::string& controllingITSI,
        const std::vector<std::string>& patchList,const std::string& patchName)
    {
		FUNCTION_ENTRY("setPatchDetails");
        // Adding a patch requires setting a number of specific variables.
        std::string detailsString = TYPE + "=" + PATCH_TYPE + KEY_SEPARATOR;
        detailsString += PATCH_ID + "=" + patchName + KEY_SEPARATOR;
        detailsString += OWNER_DISPATCHER + "=" + controllingITSI + KEY_SEPARATOR;
        detailsString += PATCH_HANG_TIME + "=" + PATCH_HANG_TIME_VALUE + KEY_SEPARATOR;

        // And now the members - first add the key and the "=" sign.
        detailsString += MEMBERS + "=";
        
        // Now loop through each member on the patch list and add it.
        for (unsigned int i = 0;i < patchList.size();i++)
        {
            detailsString += "+" + patchList[i];
            if (i+1 != patchList.size())
            {
                detailsString += DETAILS_SEPARATOR;
            }
        }

        // and the vectoral item ends with a new line
        detailsString += KEY_SEPARATOR;

        setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
        setParameterLongString(DETAILS_PARAMETER_START_POSITION,detailsString);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
            "setPatchDetails() - parameter string: \"%s\"", detailsString.c_str());        

		FUNCTION_EXIT;
    }

	void RadioMethodChangeReference::setGroupMembers(const std::string& memberList)
    {
		FUNCTION_ENTRY("setGroupMembers");
        // Adding a patch requires setting a number of specific variables.
        std::string detailsString = MEMBERS + "=" + memberList + KEY_SEPARATOR;
        setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
        setParameterLongString(DETAILS_PARAMETER_START_POSITION,detailsString);
    
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
             "setGroupMembers() - parameter string: \"%s\"", detailsString.c_str());
		FUNCTION_EXIT;
    }


    void RadioMethodChangeReference::setSearchType(const RadioMethodChangeReference::ESearchType searchType)
    {
		FUNCTION_ENTRY("setSearchType");
        // Setting the search mode is a relativley simple call. K_SEARCH_MODE=full or normal depending on the
        // type desired.
        std::string detailsString = SEARCH_MODE + "=" + ((searchType == FULL) ? FULL_SEARCH : NORMAL_SEARCH) + KEY_SEPARATOR;
        setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
        setParameterLongString(DETAILS_PARAMETER_START_POSITION,detailsString);
		FUNCTION_EXIT;
    }

	void RadioMethodChangeReference::setAudioVolume(int leftVolume,int rightVolume)
	{
		FUNCTION_ENTRY("setAudioVolume");
		char  lVolumeText[30] = {0};
		char  rVolumeText[30] = {0};

		//itoa(leftVolume,lVolumeText,10);
		//itoa(rightVolume,rVolumeText,10);
		sprintf(lVolumeText,"%d",leftVolume);
		sprintf(rVolumeText,"%d",rightVolume);

        std::string detailsString = LEFT_VOLUME + "=" + lVolumeText + KEY_SEPARATOR +
			                        RIGHT_VOLUME + "=" + rVolumeText + KEY_SEPARATOR;

        setMessageSize(CHANGE_REFERENCE_BASE_LENGTH + 4 + detailsString.size());
        setParameterLongString(DETAILS_PARAMETER_START_POSITION,detailsString);

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, 
             "setAudioVolume() - parameter string: \"%s\"", detailsString.c_str());
		FUNCTION_EXIT;
	}
}

