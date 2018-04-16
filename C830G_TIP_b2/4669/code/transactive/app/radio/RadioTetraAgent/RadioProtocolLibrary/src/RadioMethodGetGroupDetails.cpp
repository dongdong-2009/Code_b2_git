/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/RadioMethodGetGroupDetails.cpp $
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
//  RadioMethodGetGroupDetails.cpp
//  Implementation of the Class RadioMethodGetGroupDetails
//  Created on:      29-Nov-2003 09:57:09 AM
///////////////////////////////////////////////////////////

#include "RadioMethodGetGroupDetails.h"
#include "RadioProtocol.h"
#include "core/utilities/src/TAAssert.h"
#include "app/radio/RadioTetraAgent/src/CallForwardingDetails.h"

namespace TA_IRS_App
{
    const unsigned int RadioMethodGetGroupDetails::GET_GROUP_DETAILS_BASE_LENGTH     = 8;
    const unsigned int RadioMethodGetGroupDetails::SUBSCRIBER_REFERENCE_PARAMETER_POSITION = 0;
    const unsigned int RadioMethodGetGroupDetails::ROW_COUNT_PARAMETER_POSITION = 4;
    const unsigned int RadioMethodGetGroupDetails::FIELDS_PARAMETER_START_POSITION = 8;

    const unsigned int RadioMethodGetGroupDetails::MAX_ROWS_PER_EVENT = 5;

    RadioMethodGetGroupDetails::RadioMethodGetGroupDetails(RadioSessionReference sessionRef,unsigned long subscriberRef) 
                       : RadioMethod(GET_GROUP_DETAILS_BASE_LENGTH,METHOD_GET_GROUP_DETAILS,
                       sessionRef)
    {
        setParameterLong(SUBSCRIBER_REFERENCE_PARAMETER_POSITION,subscriberRef);
        setParameterLong(ROW_COUNT_PARAMETER_POSITION,MAX_ROWS_PER_EVENT);
    }

    void RadioMethodGetGroupDetails::setFields(std::vector<std::string> fields)
    {
        std::string fieldsString;
        for (unsigned int i = 0;i < fields.size();i++)
        {
            fieldsString += fields[i];

            if (i != (fields.size() - 1))
            {
                fieldsString += ",";
            }
        }
		// RvH 20050128 pre-Radio iFAT
		// OTE TCPServer/HLR bug means that it isn't so happy about the presence
		// of trailing newlines, nor the absence of a terminating null character
		// fieldsString += "\n";
		fieldsString += "\0";

        setMessageSize(GET_GROUP_DETAILS_BASE_LENGTH+fieldsString.size()+4);
        setParameterLongString(FIELDS_PARAMETER_START_POSITION,fieldsString);
    }



    RadioMethodGetGroupDetails::~RadioMethodGetGroupDetails()
    {
    }
}

