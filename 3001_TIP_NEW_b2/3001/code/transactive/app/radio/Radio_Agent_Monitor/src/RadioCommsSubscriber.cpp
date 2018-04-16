/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/radio/Radio_Agent_Monitor/src/RadioCommsSubscriber.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#include <ctime>
#include "RadioCommsSubscriber.h"
#include "core\message\types\RadioComms_MessageTypes.h"
#include "app\radio\RadioAgentCorbaDef\src\IRadioCorbaDef.h"
#include "DebugUtil.h"

namespace TA_App
{
	RadioCommsSubscriber::~RadioCommsSubscriber()
	{

	}


	RadioCommsSubscriber::RadioCommsSubscriber( )
	{
		// nothing !
	}

	
	/**
	* receiveMessage
	*
	* You need to override this pure virtual method in your derived class 
	*
	* @param    message	A pointer to a GenericMessage object
	*/
	void RadioCommsSubscriber::receiveSpecialisedMessage(const TA_Core::CommsMessageCorbaDef& message)
	{
		// The data structure you want
		TA_App::IRadioCorbaDef::CallStatusEvent* callStatus = NULL;

		std::string messageTypeKey(message.messageTypeKey);
		// Extract to the messageState
		if ( ( message.messageState >>= callStatus ) != 0)
		{
			// process messageState now
			unsigned long key = message.assocEntityKey;

			// Report the call ID and new state
			std::cout << "Radio status update message for call " << callStatus->callID 
					  << " with new status of " << callStatus->callStatus << std::endl;

		}
		else
        {
            LOG(SourceInfo, TA_Core::DebugUtil::GenericLog,  TA_Core::DebugUtil::DebugInfo, "Event does not contain dataPointEvent");
    		return;
        }
	}


}; // TA_Core

