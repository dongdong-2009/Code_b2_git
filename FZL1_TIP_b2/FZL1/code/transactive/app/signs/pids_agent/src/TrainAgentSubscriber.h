#ifndef TRAINAGENTSUBSCRIBER_H
#define TRAINAGENTSUBSCRIBER_H
/**
* The source code in this file is the property of
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/3002_TIP/3002/transactive/app/signs/pids_agent/src/StringHelper.h $
* @author:  Albertus Zakaria
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2009/10/01 16:36:38 $
* Last modified by:  $Author: grace.koh $
*
* A place holder for generic string functions
*
*/
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/types/TrainAgentComms_MessageTypes.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"

namespace TA_IRS_App
{
	class TrainAgentSubscriber:public TA_Base_Core::SpecialisedMessageSubscriber< TA_Base_Core::CommsMessageCorbaDef >
	{
	public:
		TrainAgentSubscriber(){};
		virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& commsm);
		void subscribeToMessages(unsigned long locationKey);
		virtual ~TrainAgentSubscriber();
	private:
		static const int SIGINFO_FEP_REGISTER_ADDRESS;
		
	};
}

#endif