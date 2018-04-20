/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/history/history_viewer/src/HistoryCommsSubscriber.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// HistoryCommsSubscriber.h: interface for the CHistoryCommsSubscriber class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(HISTORY_COMMS_SUBSCRIBER_H)
#define HISTORY_COMMS_SUBSCRIBER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <time.h>
#include "SpecialisedMessageSubscriber.h"
#include "CommsMessageCorbaDef.h"


//struct HistoryUpdate
//{
//	unsigned long historyConfigPkey;
//	time_t updateTime;
//};

using namespace TA_Core;


namespace TA_App
{
    class HistoryCommsSubscriber : public SpecialisedMessageSubscriber<CommsMessageCorbaDef>
    {
    public:

        HistoryCommsSubscriber() {}

        virtual ~HistoryCommsSubscriber() {}


        /**
        * receiveMessage
        */
        virtual void receiveSpecialisedMessage(const CommsMessageCorbaDef& message); 


        // This exception is thrown when there is problems with events
        // A string should be supplied describing the failure.

        class ExEvent 
        {
        public:
	        //Constructor - str should describe nature of failure
	        ExEvent(const std::string& str) : m_str(str){}
	        
	        //Destructor
	        virtual ~ExEvent(){}
			        
	        //Return the string describing the failure
	        std::string GetDesc() const {return m_str;} 

        private:
	        std::string m_str;
        };
    };
}



#endif // !defined(HISTORY_COMMS_SUBSCRIBER_H)





