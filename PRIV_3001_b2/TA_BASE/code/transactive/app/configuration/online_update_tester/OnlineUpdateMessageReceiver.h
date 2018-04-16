/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/configuration/online_update_tester/OnlineUpdateMessageReceiver.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
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
