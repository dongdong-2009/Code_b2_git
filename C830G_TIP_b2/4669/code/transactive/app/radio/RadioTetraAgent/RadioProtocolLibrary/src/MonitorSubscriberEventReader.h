/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/RadioTetraAgent/RadioProtocolLibrary/src/MonitorSubscriberEventReader.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of MonitorSubscriber Event Reader
  *
  */


#if !defined(MonitorSubscriberEventReader_INCLUDED_)
#define MonitorSubscriberEventReader_INCLUDED_

#include "RadioEventReader.h"


namespace TA_IRS_App
{
    class MonitorSubscriberEventReader : public RadioEventReader
    {
        public:

            MonitorSubscriberEventReader(RadioEvent & event);
            virtual ~MonitorSubscriberEventReader();

            long getMonitorReference() const;
            std::string getSubscriberNumber() const;
            unsigned char getRejectReason() const;

        private:
            static const unsigned int MONITORSUBSCRIBER_EVENT_LENGTH;
            static const unsigned int SUBSCRIBER_NUMBER_START;
            static const unsigned int MONITOR_REFERENCE_START;
            static const unsigned int REJECT_REASON_START;
    };
};

#endif // !defined(MonitorSubscriberEventReader_INCLUDED_)
