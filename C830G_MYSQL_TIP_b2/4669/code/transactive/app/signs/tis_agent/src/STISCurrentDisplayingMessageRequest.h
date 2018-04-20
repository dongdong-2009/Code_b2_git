#ifndef STISCURRENTDISPLAYINGMESSAGEREQUEST_H_INCLUDED
#define STISCURRENTDISPLAYINGMESSAGEREQUEST_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/tis_agent/src/STISCurrentDisplayingMessageRequest.h $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

#include "app/signs/tis_agent/src/STISMessage.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"

#include <string>

namespace TA_IRS_App
{
	class STISCurrentDisplayingMessageRequest : public STISMessage
	{
	public:

		STISCurrentDisplayingMessageRequest();

		~STISCurrentDisplayingMessageRequest();

		virtual std::vector<unsigned char> createProtocolMessage();

		virtual void log(bool sendSuccess);

		void setData( const std::string& station, int pidAddress );

		// Processes the response message and stores the message in its member variable
		void processResponse();

		TA_Base_Core::CurrentDisplayingMessage getMessage();

	private:

		std::string stationID;   // 3-letter acronym of the station

		int m_PID;  // Address of the PID 

		TA_Base_Core::CurrentDisplayingMessage m_currentDisplayingMessage;

		bool m_responseProcessed;

        static const int DATA_LENGTH_START;
        static const int DATA_LENGTH_NUMBYTE;
        static const int REPORT_STATION_NUMBYTE;
        static const int REPORT_PID_NUMBYTE;
        static const int NUM_MESSAGE_LIST_NUMBYTE;
        static const int MESSAGE_TAG_NUMBYTE;
        static const int START_TIME_NUMBYTE;
        static const int END_TIME_NUMBYTE;
        static const int PRIORITY_NUMBYTE;
        static const int MESSAGE_LENGTH_NUMBYTE;
		//TD 9014
		//zhou yuan++
        static const int MINIMUM_LENGTH;
		static const int MINIMUM_LENGTH_WITH_MESSAGE;
		static const std::string EMPTY_MESSAGE;
		//++zhou yuan
	};

}

#endif
