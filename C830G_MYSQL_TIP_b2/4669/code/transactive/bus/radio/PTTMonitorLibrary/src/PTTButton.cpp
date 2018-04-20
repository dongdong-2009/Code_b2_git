/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/bus/radio/PTTMonitorLibrary/src/PTTButton.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class manages the PTT activation, notifying the radio
  * of requests and advising the client of actions.
  *
  */

///////////////////////////////////////////////////////////
//  PTTButton.cpp
//  Implementation of the Class PTTButton
//  Created on:      17-Feb-2004 10:10:32 AM
///////////////////////////////////////////////////////////

#include "PTTButton.h"
#include "PTTButtonException.h"
#include "RS232MonitorThread.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core\data_access_interface\entity_access\src\RadioEntityData.h"
#include "core\data_access_interface\entity_access\src\IRadioSessionEntityData.h"
#include "core\data_access_interface\entity_access\src\RadioSessionEntityData.h"
#include "core\message\types\RadioComms_MessageTypes.h"
#include "core\message\src\MessageSubscriptionManager.h"
#include "bus\radio\RadioAgentCorbaDef_4669\idl\src\IRadioCorbaDef.h"
#include "core\data_access_interface\entity_access\src\ConsoleAccessFactory.h"

namespace TA_IRS_Bus
{

	/**
	 * Constructor
	 * @param notify    Interface used to notify user of change
	 * 
	 */
	PTTButton::PTTButton(TA_IRS_Bus::IPTTMonitoring& notify) 
			  : m_rs232Monitor(NULL),
				m_callback(notify)
	{
 		FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;

	}


	PTTButton::~PTTButton()
	{
 		FUNCTION_ENTRY("Destructor");

		// Free the RS232 monitor
		if (m_rs232Monitor!=NULL)
		{
			m_rs232Monitor->terminateAndWait();
			delete m_rs232Monitor;
			m_rs232Monitor = NULL;
		}

		FUNCTION_EXIT;
	}

	/**
	 * Will need to connect to the RS232 port and monitor radio session messages (for
	 * DemandTX events).  It will also need to access the RadioSession remote object
	 * in order to send the PTT requests.
	 * @param sessionId
	 * 
	 */
	void PTTButton::startMonitoring(long callID, std::string sessionId, std::string comPort)
	{
 		FUNCTION_ENTRY("startMonitoring");

		TA_ASSERT(m_rs232Monitor==NULL,"RS232 monitoring is already in progress.");

		m_sessionID = sessionId;

		// Determine the physical location of this component
		TA_ASSERT(!sessionId.empty(), "SessionId parameter is not set");
		TA_Base_Core::IConsole* console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(sessionId);
		unsigned long locationKey = console->getLocation();
		unsigned long consoleKey = console->getKey();
		delete console;

		// Set radio session object
		m_radioSession.setEntityName(getRadioSessionEntityName(locationKey,consoleKey),false);

		// Start listening for PTT notifications - Queued->Granted notifications
		startRadioPTTSubscription(locationKey);

		// Select the call - to apply demand/cease tx to
		// Note: Radio Manager will also use this but we only have one
		// button therefore they can't both use it at the same time!
		try
		{
			//m_radioSession->selectCall(callID,m_sessionID.c_str());
		}
		catch (...)
		{
			TA_THROW(PTTButtonException("Unable to select call."));
		}

		// Start PTT monitoring
		m_rs232Monitor = new TA_IRS_Bus::RS232MonitorThread(*this,comPort);
		TA_ASSERT(m_rs232Monitor!=NULL,"RS232 monitoring not constructed.");
		m_rs232Monitor->start();

		// Ensure port is connected ok


		FUNCTION_EXIT;
	}


	/**
	 * release RS232 and RadioSession object, stop subscription to RadioSession events.
	 * 
	 */
	void PTTButton::endMonitoring()
	{
 		FUNCTION_ENTRY("endMonitoring");
		TA_ASSERT(m_rs232Monitor!=NULL,"RS232 monitoring not in progress.");

		endRadioPTTSubscription();

		m_rs232Monitor->terminateAndWait();
		delete m_rs232Monitor;
		m_rs232Monitor = NULL;

		FUNCTION_EXIT;
	}

	bool PTTButton::isMonitoring()
	{
		return (m_rs232Monitor!=NULL);
	}


	/**
	 * onPortReady  This method is called when the communications port is ready and
	 * monitoring
	 */
	void PTTButton::onPortReady()
	{
 		FUNCTION_ENTRY("onPortReady");
		m_callback.onPTTMonitoring();
		FUNCTION_EXIT;
	}

	/**
	 * onPortBusy  This method is called when the communications port is busy and
	 * cannot be connected to.
	 */
	void PTTButton::onPortBusy()
	{
 		FUNCTION_ENTRY("onPortBusy");
		m_callback.onPTTBusy();
		FUNCTION_EXIT;
	}

	/**
	 * onCTSEnable  This method is called when the CTS changes from disabled to
	 * enabled
	 */
	void PTTButton::onCTSEnable()
	{
 		FUNCTION_ENTRY("onCTSEnable");

		// Advise client of the activation
		m_callback.onPTTActivated();

		// Demand TX from radio session
		short priority = 3;
		try
		{
			CORBA_CALL( m_radioSession,
			            demandTX,
			            ( priority, m_sessionID.c_str(), m_queuedID ) );
		}
		catch (...)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError,
				"Error requesting demandTX from radio");
		}

		// Advise client of radio confirmation
		if (m_queuedID==0)
		{
			m_callback.onPTTGranted();
		}
		else
		{
			m_callback.onPTTQueued();
		}

		FUNCTION_EXIT;
	}


	/**
	 * onCTSDisable  This method is called when the CTS changes from enabled to
	 * disabled
	 */
	void PTTButton::onCTSDisable()
	{
 		FUNCTION_ENTRY("onCTSDisable");
		
		// Advise client of button release
		m_callback.onPTTReleased();

		// Send request to radio
		try
		{
			CORBA_CALL( m_radioSession,
			            ceaseTX,
			            ( m_sessionID.c_str() ) );
		}
		catch (...)
		{
			// Can only log error as no one to notify
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugError,
				"Error requesting ceaseTX from radio");

		}

		// Advise client of radio confirmation
		m_callback.onPTTCease();

		FUNCTION_EXIT;
	}




    void PTTButton::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
    {
		FUNCTION_ENTRY("receiveSpecialisedMessage");

		// The data structure you want
		TA_Base_Bus::IRadioCorbaDef::PTTStatusNotification* pttStatus = NULL;

		// Extract to the messageState
		if ( ( message.messageState >>= pttStatus ) != 0)
		{
			// Check if we are expecting a queued message.
			if (m_queuedID!=0)
			{
				if ((pttStatus->transactionID==m_queuedID)&&
					(pttStatus->state==TA_Base_Bus::IRadioCorbaDef::PTT_GRANTED))
				{
					m_queuedID = 0;
					m_callback.onPTTGranted();
				}
			}
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, "Event does not contain PTTStatusNotification");
		}

		FUNCTION_EXIT;
	}







	//******************
	// Private Methods *
	//******************


	std::string PTTButton::getRadioSessionEntityName(unsigned long locationKey, unsigned long consoleKey)
	{
		FUNCTION_ENTRY("getRadioSessionEntityName");

		std::vector<TA_Base_Core::IEntityData*> entityList;
		TA_Base_Core::IRadioSessionEntityData*  radioSessionEntity;
		std::string   radioSessionEntityName;
		bool entityFound = false;

		// Should be just one radio entity at each location
		entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(TA_Base_Core::RadioSessionEntityData::getStaticType(),locationKey);
		TA_ASSERT(entityList.size()>0,"No radio session entities found for this location.");
		while (!entityList.empty())
		{
			radioSessionEntity = dynamic_cast<TA_Base_Core::IRadioSessionEntityData*>(entityList[0]);
			TA_ASSERT(radioSessionEntity,"Invalid entity type returned from RadioSession search.");
			if (radioSessionEntity->getAssociatedConsoleKey()==consoleKey)
			{
				if (entityFound)
				{
					TA_ASSERT(false,"Multiple radio sessions associated with this console.");
				}
				else
				{
					radioSessionEntityName = radioSessionEntity->getName();
					entityFound = true;
				}
			}

			entityList.erase(entityList.begin());

			delete radioSessionEntity;
		}
		TA_ASSERT(entityFound,"Radio session not found for this console.");

		FUNCTION_EXIT;
		return radioSessionEntityName;
	}


	void PTTButton::startRadioPTTSubscription(unsigned long locationKey)
	{
		FUNCTION_ENTRY("startRadioPTTSubscription");

		// Get the radio entity
		TA_Base_Core::IEntityDataList entityList = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
			TA_Base_Core::RadioEntityData::getStaticType(), locationKey );
		TA_ASSERT(entityList.size()==1,"Unique radio entity not be found.");
		unsigned long radioEntityKey = entityList[0]->getKey();
		delete entityList[0];

		// Subscribe
		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( 
			TA_Base_Core::RadioComms::RadioPTTNotification, this, radioEntityKey, 0, locationKey, NULL );
		FUNCTION_EXIT;
	}

	void PTTButton::endRadioPTTSubscription()
	{
		FUNCTION_ENTRY("endRadioPTTSubscription");
		TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
		FUNCTION_EXIT;
	}

}; // Namespace



