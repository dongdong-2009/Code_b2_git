/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/JoinEventReader.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of Join Event Reader
  *
  */
#include "JoinEventReader.h"

namespace TA_IRS_App
{

    JoinEventReader::JoinEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_JOIN,JOIN_EVENT_LENGTH)
    {
    }

    JoinEventReader::~JoinEventReader()
    {
    }

    unsigned long JoinEventReader::getCallReference() const
    {
        return getParameterLong(CALL_REFERENCE_START);
    }

    const unsigned int JoinEventReader::CALL_REFERENCE_START = 0;
    const unsigned int JoinEventReader::JOIN_EVENT_LENGTH    = 4;

}

