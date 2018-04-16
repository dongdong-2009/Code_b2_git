/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/MonitorCallEventReader.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of MonitorCall Event Reader
  *
  */
#include "MonitorCallEventReader.h"

namespace TA_IRS_App
{

    MonitorCallEventReader::MonitorCallEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_MONITORCALL,MONITORCALL_EVENT_LENGTH)
    {
    }

    MonitorCallEventReader::~MonitorCallEventReader()
    {
    }

    long MonitorCallEventReader::getCallReference() const
    {
        return getParameterLong(CALL_REFERENCE_START);
    }

    const unsigned int MonitorCallEventReader::CALL_REFERENCE_START  = 0;
    const unsigned int MonitorCallEventReader::MONITORCALL_EVENT_LENGTH = 4;

}

