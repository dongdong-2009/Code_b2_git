/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/radio/Radio_Agent_Monitor/src/RadioCommsSubscriber.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
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
