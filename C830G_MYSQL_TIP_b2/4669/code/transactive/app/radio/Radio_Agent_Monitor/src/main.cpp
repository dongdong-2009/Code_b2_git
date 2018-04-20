/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/radio/Radio_Agent_Monitor/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */
// RadioAgentMonitor.cpp : Defines the entry point for the console application.
//
// Simply subscribes for Radio Agent messages and displays them!
//
#include "stdafx.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\message\src\MessageSubscriptionManager.h"
//#include "AbstractMessageSubscriber.h"
#include "RadioCommsSubscriber.h"
#include "core\message\types\RadioComms_MessageTypes.h"
#include "NamingService_MessageTypes.h"
#include "Thread.h"
#include "core\corba\src\CorbaUtil.h"
#include "RunParams.h"
#include "Timer.h"


int main(int argc, char* argv[])
{
    //
	// CORBA Initialisation
	//
	// TA_MSG::MessageSubscriptionManager::getInstance();
	TA_Core::CorbaUtil::getInstance().initialise();
	TA_Core::CorbaUtil::getInstance().activate();

	//TA_Core::RunParams::getInstance().set(RPARAM_NAMESERV, "corbaloc::jupiter:6666/NameService" );
	TA_Core::RunParams::getInstance().set(RPARAM_NOTIFYHOSTS, "ophelia,bunsen" );
	TA_Core::DebugUtil::getInstance().setLevel( TA_Core::DebugUtil::DebugDebug );

	//
	// Create subscriber and listen for messages
	//
	TA_App::RadioCommsSubscriber   subscriber;
	TA_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( TA_Core::RadioComms::RadioCallStatus, &subscriber, NULL, 0, 0, NULL );

	// Sleep the main thread for a while
	while (true)
	{
		TA_Core::Thread::sleep(3000);
	}

	// Clean up the Event Consumers
	TA_Core::MessageSubscriptionManager::getInstance().cleanUp();

    return 0;
}
