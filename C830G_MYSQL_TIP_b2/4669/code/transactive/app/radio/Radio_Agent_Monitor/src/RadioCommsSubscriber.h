/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/Radio_Agent_Monitor/src/RadioCommsSubscriber.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(RADIO_COMMS_SUBSCRIBER_H)
#define RADIO_COMMS_SUBSCRIBER_H

#include "SpecialisedMessageSubscriber.h"
#include "CommsMessageCorbaDef.h"
#include "Timer.h"

namespace TA_App
{
	class RadioCommsSubscriber : public TA_Core::SpecialisedMessageSubscriber<TA_Core::CommsMessageCorbaDef>
	{
	public:

		RadioCommsSubscriber( );

		virtual ~RadioCommsSubscriber();

		/**
		* receiveMessage
		*
		* You need to override this pure virtual method in your derived class 
		*
		* @param    message	A pointer to a GenericMessage object
		*/
		virtual void receiveSpecialisedMessage(const TA_Core::CommsMessageCorbaDef& message);

	private:
}; // class RadioCommsSubscriber


}; // TA_App

#endif 
