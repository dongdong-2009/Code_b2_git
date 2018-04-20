/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/DeleteReferenceEventReader.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
///////////////////////////////////////////////////////////
//
//  DeleteReferenceEventReader.cpp
//  Implementation of the Class DeleteReferenceEventReader
//  Generated by Enterprise Architect
//  Created on:      10-Nov-2003 16:38:14
//  Original author: 
//  
///////////////////////////////////////////////////////////
//  Modification history:
//  
//
///////////////////////////////////////////////////////////

#include "DeleteReferenceEventReader.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_IRS_App
{

    const unsigned int DeleteReferenceEventReader::DELETE_REFERENCE_EVENT_LENGTH            =  8;
    const unsigned int DeleteReferenceEventReader::DELETE_REFERENCE_TYPE_START              =  0;
    const unsigned int DeleteReferenceEventReader::REFERENCE_TYPE_PARAMETER_POSITION        =  4;


    DeleteReferenceEventReader::DeleteReferenceEventReader(RadioEvent& event)
						    : RadioEventReader(event,EVENT_DELETE_REFERENCE,DELETE_REFERENCE_EVENT_LENGTH)
    {
    }

    DeleteReferenceEventReader::~DeleteReferenceEventReader()
    {
    }


    const DeleteReferenceEventReader::RadioReference DeleteReferenceEventReader::getReference() const
    {
	    return getParameterLong(REFERENCE_TYPE_PARAMETER_POSITION);
    }

    const DeleteReferenceEventReader::EReferenceType DeleteReferenceEventReader::getReferenceType() const
    {
        // Extract the reference type - it's 4 bytes long, but only the first byte has useful information
        const char typeChar = getParameterChar(REFERENCE_TYPE_PARAMETER_POSITION);

        switch (typeChar)
        {
            case 'C':
                return CALL_REFERENCE;
            case 'D':
                return SDS_REFERENCE;
            case 'S':
                return SUBSCRIBER_REFERENCE;
            case 'W':
                return DATA_REFERENCE;
            case 'A':
                return AUDIO_REFERENCE;
            case 'Z':
                return CAD_REFERENCE;
            case 'V':
                return SEARCH_REFERENCE;
            case 'M':
                return MONITOR_REFERENCE;
            default:
                // Error
                TA_ASSERT(false,"Unknown reference type returned from system");
        }
        // is impossible to get here because of the default clause, but the
        // compiler prefers a return value if we've said this function will
        // return one
        return CALL_REFERENCE;
    }
}


