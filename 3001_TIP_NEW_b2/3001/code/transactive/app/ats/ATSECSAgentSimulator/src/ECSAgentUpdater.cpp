/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/ats/ATSECSAgentSimulator/src/ECSAgentUpdater.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/07/15 18:11:22 $
  * Last modified by:  $Author: qi.huang $
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
#include "bus/ats/ats_agent/IDL/src/IAtsCorbaDef.h"



using namespace TA_Base_Core;

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
    TA_IRS_Bus::IAtsEcsCorbaDef::AtsEcsMessageCorbaDef messageData;
    
    messageData.physicalTrainNumber = 99;
    messageData.atcTrainNumber = 99;

    messageData.localisation.scheduleNumber = 99;
    messageData.localisation.serviceNumber = 99;
    messageData.localisation.ecsZoneId = ECSZoneId;
    messageData.oa1Data.maxExternalTemperatureInCelsius = maxExternalTemperatureInCelsius;
    messageData.localisation.stallTimeInSeconds = stallTimeInSeconds;
    messageData.oa1Data.isInEmergency = isInEmergency;

    commsData <<= messageData;

    m_commsSender->sendCommsMessage( ATSAgentComms::ECSUpdate,   // Message Type
                                     m_entityKey,                // EntityKey
                                     commsData,                  // Message Data
                                     0,                          // SubsystemKey
                                     m_locationKey );                         // LocationKey
}
