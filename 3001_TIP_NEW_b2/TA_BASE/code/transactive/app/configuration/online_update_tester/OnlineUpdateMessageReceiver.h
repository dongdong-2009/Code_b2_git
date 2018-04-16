/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/configuration/online_update_tester/OnlineUpdateMessageReceiver.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/src/ConfigUpdateMessageSender.h"
#include "core/message/IDL/src/ConfigUpdateMessageCorbaDef.h"


class OnlineUpdateMessageReceiver : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::ConfigUpdateMessageCorbaDef>
{
	public:

        OnlineUpdateMessageReceiver();

        virtual void receiveSpecialisedMessage(const TA_Base_Core::ConfigUpdateMessageCorbaDef& message);


};
