#if !defined(REAL_TIME_DATA_UPDATE_SUBSCRIBER_H)
#define REAL_TIME_DATA_UPDATE_SUBSCRIBER_H

/**
  * RealTimeDataUpdateSubscriber.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/history/history_viewer/src/RealTimeDataUpdateSubscriber.h $
  * @author:  Anita Lee
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * This class listens for DataPointStateUpdate events and updates the graph for any
  * state changes.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

using TA_Core::CommsMessageCorbaDef;

namespace TA_App
{
    //
    // Forward declaration
    //
    class CHistoryViewerDlg;

    //
    // RealTimeDataUpdateSubscriber
    //
    class RealTimeDataUpdateSubscriber : public TA_Core::SpecialisedMessageSubscriber<CommsMessageCorbaDef>
    {
    public:

        /**
		  * RealTimeDataUpdateSubscriber
		  * 
		  * Constructor.
		  *
		  */
        RealTimeDataUpdateSubscriber(CHistoryViewerDlg* parentDlg, unsigned long datapointKey);

        /**
		  * ~RealTimeDataUpdateSubscriber
		  * 
		  * Destructor.
		  *
		  */
        virtual ~RealTimeDataUpdateSubscriber();

        /**
		  * receiveSpecialisedMessage
		  * 
		  * Receives and processes the message.
		  *
		  * @param message The message
		  *
		  */
        virtual void receiveSpecialisedMessage(const CommsMessageCorbaDef& message);

        /**
		  * subscribeToMessage
		  * 
		  * Subscribes to the DataPointStateUpdate messages.
		  *
		  */
        void subscribeToMessage();

        /**
		  * unsubscribeToMessage
		  * 
		  * Unsubscribes to the DataPointStateUpdate messages.
		  *
		  */
        void unsubscribeToMessage();

    private:

        RealTimeDataUpdateSubscriber();
        RealTimeDataUpdateSubscriber(const RealTimeDataUpdateSubscriber& theSubscriber);
	    RealTimeDataUpdateSubscriber& operator=(const RealTimeDataUpdateSubscriber &);

    public:

        unsigned long getKey()
        {
            return m_datapointEntitykey;
        }

    private:

        CHistoryViewerDlg* m_parentProcess;
        unsigned long m_datapointEntitykey;
        unsigned long m_locationKey;
        bool m_isSubscribed;
    };
}



#endif // !defined(REAL_TIME_DATA_UPDATE_SUBSCRIBER_H)





