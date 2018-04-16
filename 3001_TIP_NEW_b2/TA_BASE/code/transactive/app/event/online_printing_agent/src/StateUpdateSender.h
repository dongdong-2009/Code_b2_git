#if !defined(STATEUPDATESENDER_H_INCLUDED)
#define STATEUPDATESENDER_H_INCLUDED

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/online_printing_agent/src/StateUpdateSender.h $
  * @author:   Bart Golab
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * Sends Online Printing Agent state update messages.
  * 
  */

namespace TA_Base_App
{
	class StateUpdateSender
	{
    // Operations
	public:
		StateUpdateSender();
		virtual ~StateUpdateSender();

        /**
         * sendEventProcessedUpdate
         *
         * Sends an Online Printing Agent state update for last processed event.
         */ 
		void sendEventProcessedUpdate(EventSequenceNumber lastProcessedEvent);

        /**
         * requestStateUpdate
         *
         * Sends an Online Printing Agent state update request.
         */ 
		void requestStateUpdate();


    // Attributes
	private:
		TA_Base_Core::StateUpdateMessageSender* m_messageSender;
	};
}

#endif // !defined(STATEUPDATESENDER_H_INCLUDED)
