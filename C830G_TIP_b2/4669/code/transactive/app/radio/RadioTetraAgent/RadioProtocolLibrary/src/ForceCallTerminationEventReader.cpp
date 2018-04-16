/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/ForceCallTerminationEventReader.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of ForceCallTermination Event Reader
  *
  */
#include "ForceCallTerminationEventReader.h"

namespace TA_IRS_App
{

    ForceCallTerminationEventReader::ForceCallTerminationEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_FORCECALLTERMINATION,FORCECALLTERMINATION_EVENT_LENGTH)
    {
    }

    ForceCallTerminationEventReader::~ForceCallTerminationEventReader()
    {
    }

    unsigned long ForceCallTerminationEventReader::getCallReference() const
    {
        return getParameterLong(CALL_REFERENCE_START);
    }

    const unsigned int ForceCallTerminationEventReader::CALL_REFERENCE_START              = 0;
    const unsigned int ForceCallTerminationEventReader::FORCECALLTERMINATION_EVENT_LENGTH = 4;

}


