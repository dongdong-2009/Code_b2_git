/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/T253_PSCADA_TIP/TA_BASE/transactive/app/configuration/online_update_tester/OnlineUpdateMessageReceiver.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/01/23 17:37:20 $
  * Last modified by:  $Author: builder $
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
