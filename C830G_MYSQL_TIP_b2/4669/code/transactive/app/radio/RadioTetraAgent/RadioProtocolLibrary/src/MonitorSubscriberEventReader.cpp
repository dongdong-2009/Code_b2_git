/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/MonitorSubscriberEventReader.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of MonitorSubscriber Event Reader
  *
  */
#include "MonitorSubscriberEventReader.h"

namespace TA_IRS_App
{

    MonitorSubscriberEventReader::MonitorSubscriberEventReader(RadioEvent& event)
           : RadioEventReader(event,EVENT_MONITORSUBSCRIBER,MONITORSUBSCRIBER_EVENT_LENGTH)
    {
    }

    MonitorSubscriberEventReader::~MonitorSubscriberEventReader()
    {
    }

    long MonitorSubscriberEventReader::getMonitorReference() const
    {
        return getParameterLong(MONITOR_REFERENCE_START);
    }


    std::string MonitorSubscriberEventReader::getSubscriberNumber() const
    {
        return getParameterShortString(SUBSCRIBER_NUMBER_START);
    }

    unsigned char MonitorSubscriberEventReader::getRejectReason() const
    {
        return getParameterChar(REJECT_REASON_START);
    }

    const unsigned int MonitorSubscriberEventReader::SUBSCRIBER_NUMBER_START           =  0;
    const unsigned int MonitorSubscriberEventReader::MONITOR_REFERENCE_START           =  32;
    const unsigned int MonitorSubscriberEventReader::REJECT_REASON_START               =  36;
    const unsigned int MonitorSubscriberEventReader::MONITORSUBSCRIBER_EVENT_LENGTH    =  37;

}


