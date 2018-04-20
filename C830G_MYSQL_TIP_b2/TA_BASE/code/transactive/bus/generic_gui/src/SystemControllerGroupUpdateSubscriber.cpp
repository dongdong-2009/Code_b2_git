/**
  * SystemControllerGroupUpdateSubscriber.h
  *
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/bus/generic_gui/src/SystemControllerGroupUpdateSubscriber.cpp $
  * @author:  Anita Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * This class implements represents a subscriber that listens specifically for 
  * SystemControllerComms::SystemControllerGroupOffline and SystemControllerComms::SystemControllerGroupOnline
  * events. When an event is received and it corresponds to a group specified by the command line parameter,
  * --central-server-group (eg. --central-server-group=OCC) this object will notify the observer of
  * whether the system is in degraded or normal mode.
  */

#include "bus/generic_gui/src/SystemControllerGroupUpdateSubscriber.h"
#include "bus/generic_gui/src/ISystemStateUpdateObserver.h"

#include "core/message/types/SystemControllerComms_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

using namespace TA_Base_Core;

namespace TA_Base_Bus
{
    const std::string SystemControllerGroupUpdateSubscriber::RPARAM_CENTRAL_SERVER_GROUP("CentralServerGroup");

	//TD16358++
	const std::string SystemControllerGroupUpdateSubscriber::GROUP_ONLINE = "GroupOnline";
	//++TD16358

    //
    // Constructor
    //
    SystemControllerGroupUpdateSubscriber::SystemControllerGroupUpdateSubscriber(TA_Base_Bus::ISystemStateUpdateObserver& systemStateUpdateObserver)
        : m_systemStateUpdateObserver(systemStateUpdateObserver),
          m_sCGroupOnline(true),//++TD16358
          m_centralServerGroupName(""),
		  m_systemControllerName("")
    {
        FUNCTION_ENTRY("SystemControllerGroupUpdateSubscriber");

        // Retrieve the group name of the central server. When the system controller sends a message that this
        // group if offline, we know the central server is down, hence the system is in degraded mode.
        m_centralServerGroupName = RunParams::getInstance().get(RPARAM_CENTRAL_SERVER_GROUP.c_str());

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                            "[TD16358] central server group name is %s",m_centralServerGroupName.c_str());
		
        if(m_centralServerGroupName.empty())
        {
            // An error is logged here, but an exception is not thrown because this won't cause the application
            // to be in a bad state, but it just won't be notified when the system is in degraded mode.
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "CentralServerGroup Run Param has not been set.");
        }

		//TD16358++
		std::string systemControllerName = RunParams::getInstance().get(RPARAM_SYSTEMCONTROLLERNAME);

		if (systemControllerName.empty())
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
				"[TD16358]No system controller found in runtime parameter");
			//TA_THROW( SessionException(errorMsg.c_str()) );
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
				"[TD16358]System controller %s found in runtime parameter",systemControllerName.c_str());
			m_systemControllerName = systemControllerName;
		}
		//++TD16358

        FUNCTION_EXIT;
    }


    //
    // Destructor
    //
    SystemControllerGroupUpdateSubscriber::~SystemControllerGroupUpdateSubscriber()
    {
        FUNCTION_ENTRY("~SystemControllerGroupUpdateSubscriber");

        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

        FUNCTION_EXIT;
    }


    //
    // receiveSpecialisedMessage
    //
	//TD16358++
    void SystemControllerGroupUpdateSubscriber::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
    {
        FUNCTION_ENTRY("receiveSpecialisedMessage");
        
         std::string mesgTypeKey = message.messageTypeKey;

		 LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                            "[TD16358] SystemControllerGroupUpdateSubscriber receiveSpecialisedMessage %s",mesgTypeKey.c_str());

		 // if we got an offline message and we're already know that, then just ignore, otherwise notify
        if (0 == mesgTypeKey.compare(TA_Base_Core::SystemControllerComms::SystemControllerGroupOnline.getTypeKey()))
        {
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                            "[TD16358] SystemControllerGroupUpdateSubscriber recieve group online message");

            if (!m_sCGroupOnline)
            {
                char const* groupString = "";
                if((message.messageState >>= groupString) != 0)
                {
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                            "[TD16358] groupstring %s", groupString);

					
					if((message.messageState >>= groupString) == 0)
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "[TD16358]Could not retrieve the group string for SystemControllerGroupOnline message");
						groupString = "Unknown";
					}		
					
					m_sCGroupOnline = true;
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
						"Received a SystemControllerGroupOnline message");
					std::string strGroupString(groupString);
					m_centralServerGroupName = strGroupString;
            
                    // Notify the observer the system has recovered
                    m_systemStateUpdateObserver.notifySystemStateNormal();
                    
                }
            }
        }
        
         // if we got an online message and we're already know that, then just ignore, otherwise notify
        if (0 == mesgTypeKey.compare(TA_Base_Core::SystemControllerComms::SystemControllerGroupOffline.getTypeKey()))
        {  
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                            "[TD16358] SystemControllerGroupUpdateSubscriber recieve group offline message");

            if (m_sCGroupOnline)
            {
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                            "[TD16358] Passed system degraded");

                char const* groupString = "";
                if((message.messageState >>= groupString) != 0)
                {
					LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                            "[TD16358] groupstring %s", groupString);

					if((message.messageState >>= groupString) == 0)
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "[TD16358]Could not retrieve the group string for SystemControllerGroupOffline message");
						groupString = "Unknown";
					}

                    m_sCGroupOnline = false;
                    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                        "Received a SystemControllerGroupOnline message");

					std::string strGroupString(groupString);
					m_centralServerGroupName = strGroupString;
                
                    // Notify the observer the system has entered degraded mode
                    m_systemStateUpdateObserver.notifySystemStateDegraded();   
                }
            }
        }   
        FUNCTION_EXIT;
    }


    void SystemControllerGroupUpdateSubscriber::startNotifications()
    {
        // TODO: Need to get the system controller name somehow. Could set this as a RunParam from the Control Station.
        // (See SessionManager::locateSystemController() in Control Station)
        std::string systemControllerName = RunParams::getInstance().get(RPARAM_SYSTEMCONTROLLERNAME);
		bool wasSuccessfulInitialisation = false;
        
        try
        {

			//TD16358++
			TA_Base_Core::IEntityData* systemControllerEntity = EntityAccessFactory::getInstance().getEntity(m_systemControllerName);
			//++TD16358

			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                            "[TD16358] SystemControllerGroupUpdateSubscriber start notification typekey %s, entitykey %d, subsystemkey %d, locationkey %d",
								TA_Base_Core::SystemControllerComms::SystemControllerGroupOnline.getTypeKey().c_str(),
								systemControllerEntity->getKey(),
								systemControllerEntity->getSubsystem(),
								systemControllerEntity->getLocation());
        
            // now subscribe to the system controller online and offline comms messages
            MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::SystemControllerComms::SystemControllerGroupOnline, this, systemControllerEntity->getKey(), systemControllerEntity->getSubsystem(), systemControllerEntity->getLocation(), NULL);
        
            MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::SystemControllerComms::SystemControllerGroupOffline, this, systemControllerEntity->getKey(), systemControllerEntity->getSubsystem(), systemControllerEntity->getLocation(), NULL);
        
            delete systemControllerEntity;
            systemControllerEntity = NULL;

			wasSuccessfulInitialisation = true;
        }
        catch(const TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not subscribe to SystemControllerComms messages");
        }
        catch(const TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not subscribe to SystemControllerComms messages");
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not subscribe to SystemControllerComms messages");
        }

		if(wasSuccessfulInitialisation)
		{
			std::string param = TA_Base_Core::RunParams::getInstance().get(RPARAM_GROUPSOFFLINE);
			if( (!param.empty()) && (param.compare(GROUP_ONLINE) != 0) )
			{
				m_sCGroupOnline = false;
			}
		}

		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                            "[TD16358] SystemControllerGroupUpdateSubscriber start notification started without errors");
    }
    
}   // TA_Base_App


