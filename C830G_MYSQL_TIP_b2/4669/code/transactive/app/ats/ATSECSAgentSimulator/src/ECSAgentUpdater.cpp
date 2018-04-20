/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSECSAgentSimulator/src/ECSAgentUpdater.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  */

// ECSAgentUpdater.cpp: implementation of the ECSAgentUpdater class.
//
//////////////////////////////////////////////////////////////////////

#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER

#include <sstream>
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "app/ats/ATSECSAgentSimulator/src/ECSAgentUpdater.h"
#include "bus/ats/AtsAgentCorbaDef/src/IAtsEcsCorbaDef.h"



using namespace TA_Core;

ECSAgentUpdater* ECSAgentUpdater::m_me = NULL;
ReEntrantThreadLockable ECSAgentUpdater::m_lock;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// ExceptionChecked
ECSAgentUpdater::ECSAgentUpdater()
: m_commsSender(NULL),
  m_locationKey(0)
{
    FUNCTION_ENTRY("Constructor");

    TA_ASSERT( RunParams::getInstance().isSet( RPARAM_LOCATIONKEY ), 
           "Location Key was not set as run param" );

    std::string locationKeyAsString = RunParams::getInstance().get(RPARAM_LOCATIONKEY);

    m_locationKey = atol(locationKeyAsString.c_str());

    m_commsSender = MessagePublicationManager::getInstance().getCommsMessageSender
                    ( ATSAgentComms::Context );
    
    
    FUNCTION_EXIT;
}

// ExceptionChecked
ECSAgentUpdater::~ECSAgentUpdater()
{
    FUNCTION_ENTRY("Destructor");

    if (m_commsSender)
    {
        delete m_commsSender;
    }

    FUNCTION_EXIT;
}

// ExceptionChecked
ECSAgentUpdater* ECSAgentUpdater::getInstance()
{
    if ( 0 == m_me )
	{
		// Double checking to prevent multiple threads
		// creating multiple instances.

        // Any lock is okay at this stage.
		ThreadGuard guard( m_lock );

		if ( 0 == m_me )
		{
			m_me = new ECSAgentUpdater();
    	}
	}

	return m_me;
}

// ExceptionChecked
void ECSAgentUpdater::removeInstance( )
{
    
	// 
	// Guard this to prevent multiple threads atempting
	// to delete/create simultaneously
	//
	ThreadGuard guard( m_lock );
	if ( m_me != NULL )
	{
		delete m_me;
		m_me = NULL;
	}

}

void ECSAgentUpdater::setEntityKey (unsigned long data)
{
	ThreadGuard guard( m_lock );
    m_entityKey = data;
}

void ECSAgentUpdater::sendUpdate(   unsigned long ECSZoneId, 
                                    unsigned long maxExternalTemperatureInCelsius, 
                                    unsigned long stallTimeInSeconds,
                                    bool          isInEmergency )
{
	ThreadGuard guard( m_lock );
    CORBA::Any commsData;
    TA_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef messageData;
    
    messageData.physicalTrainNumber = 99;
    messageData.atcTrainNumber = 99;
    messageData.serviceNumber = 99;
    messageData.scheduleNumber = 99;

    messageData.ecsZoneId = ECSZoneId;
    messageData.maxExternalTemperatureInCelsius = maxExternalTemperatureInCelsius;
    messageData.stallTimeInSeconds = stallTimeInSeconds;
    messageData.isInEmergency = isInEmergency;

    commsData <<= messageData;

    m_commsSender->sendCommsMessage( ATSAgentComms::ECSUpdate,   // Message Type
                                     m_entityKey,                // EntityKey
                                     commsData,                  // Message Data
                                     0,                          // SubsystemKey
                                     m_locationKey );                         // LocationKey
}
