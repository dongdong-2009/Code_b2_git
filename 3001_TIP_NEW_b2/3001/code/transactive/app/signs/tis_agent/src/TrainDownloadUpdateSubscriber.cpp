/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/signs/tis_agent/src/TrainDownloadUpdateSubscriber.cpp $
  * @author:  Robin Ashcroft
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * This is used to listen for download completion update messages (from the Train Agent)
  *
  */

#include "TrainDownloadUpdateSubscriber.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/TrainAgentComms_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/TTISManager.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "bus/trains/TrainAgentCorbaDef/idl/src/ITISTrainCommandsCorbaDef.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"

using TA_Base_Core::RunParams;
using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::MessageSubscriptionManager;
using TA_Base_Core::IEntityData;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::TrainAgentEntityData;
using TA_Base_Bus::TISAgentAccessFactory;



namespace TA_IRS_App
{

    
	TrainDownloadUpdateSubscriber::TrainDownloadUpdateSubscriber( unsigned long locationKey )
	{

		m_locationKey = locationKey;

		try
		{
			subscribeToNotifications();
		}
		catch ( const TA_Base_Core::DatabaseException& dbe )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );
		}
		catch ( const TA_Base_Core::DataException& de )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
		}

	}

	TrainDownloadUpdateSubscriber::~TrainDownloadUpdateSubscriber()
	{
		// unsubscribe to messages
		MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);
	}



	void TrainDownloadUpdateSubscriber::subscribeToNotifications()
	{
		//
		// Get the entity, subsystem and location key.
		// There should only be a single entity at a given location.
		//
  

		TA_Base_Core::IEntityDataList entityData =
			EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
							TrainAgentEntityData::getStaticType(), m_locationKey);


		if( entityData.size() != 1)
		{
			const char* msg = "There is no train agent at this location.";
			const char* fld = "Train Agent";
			TA_THROW(DataException(msg, DataException::MISSING_MANDATORY, fld));
		}

		unsigned long entityKey    = entityData[0]->getKey();
		unsigned long subsystemKey = entityData[0]->getSubsystem();

		delete entityData[0];

		MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
			TA_Base_Core::TrainAgentComms::TISMessage,
			this,
			entityKey,
			subsystemKey,
			m_locationKey);
	}


	void TrainDownloadUpdateSubscriber::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message )
	{
		// if the message is the right type
		if( std::string(message.messageTypeKey) ==
			TA_Base_Core::TrainAgentComms::TISMessage.getTypeKey() )
		{
			TA_Base_Bus::ITISTrainCommandsCorbaDef::TISNotificationEvent* tisNotification;

			//
			// Extract to the TIS Notification
			//
			if( ( message.messageState >>= tisNotification ) != 0 )
			{
				processTisUpdate(*tisNotification);
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
							"Could not extract TISNotificationEvent from CommsMessageCorbaDef");
				return;
			}
		}
	}


	void TrainDownloadUpdateSubscriber::processTisUpdate(const TA_Base_Bus::ITISTrainCommandsCorbaDef::TISNotificationEvent& tisNotification)
	{
		LOG_GENERIC(
			SourceInfo, DebugUtil::DebugInfo,
			"Tis Notification update received");

		
		switch(tisNotification.notification)
        {
            // if its a library download
            case TA_Base_Bus::ITISTrainCommandsCorbaDef::libraryDownloadNotification:
		    {
			    LOG_GENERIC(
			    SourceInfo, DebugUtil::DebugInfo,
			    "Received a library download completion message");

			    // Update the current download list info.
			    TTISManager::getInstance()->trainDownloadFinished( tisNotification.trainID,
                                                                   LIBRARY_DOWNLOAD,
                                                                   tisNotification.success,
                                                                   std::string(tisNotification.errorDetails) );

			    if( tisNotification.success )
			    {
				    // Update the data for this train - the download succeeded
				    TTISManager::getInstance()->updateTrainNextLibraryVersion( tisNotification.trainID,
                                                                               StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion() );
			    }
                break;
		    }
            // otherwise its a schedule download
            case TA_Base_Bus::ITISTrainCommandsCorbaDef::scheduleDownloadNotification:
		    {
			    LOG_GENERIC(
			    SourceInfo, DebugUtil::DebugInfo,
			    "Received a schedule download completion message");

			    // Update the current download list info
			    TTISManager::getInstance()->trainDownloadFinished( tisNotification.trainID,
                                                                   SCHEDULE_DOWNLOAD,
                                                                   tisNotification.success,
                                                                   std::string(tisNotification.errorDetails) );

			    if( tisNotification.success )
			    {
					TA_Base_Core::TimeScheduleVersion timeSchedule;
                    CORBA_CALL_RETURN( timeSchedule, TISAgentAccessFactory::getInstance().getOccSTISAgent(), getCurrentTrainTimeScheduleVersion, () );//limin
 				    // Update the data for this train - the download succeeded
				    TTISManager::getInstance()->updateTrainNextScheduleVersion( tisNotification.trainID, 
                                                                                timeSchedule.version);
			    }
                break;
		    }
            case TA_Base_Bus::ITISTrainCommandsCorbaDef::libraryVersionNotification:
            {
			    LOG_GENERIC(
			    SourceInfo, DebugUtil::DebugInfo,
			    "Received a library notification message");

                TTISManager::getInstance()->updateTrainLibraryVersions( tisNotification.trainID,
                                                                        tisNotification.libraryVersion,
                                                                        tisNotification.libraryVersion );
				TTISManager::getInstance()->updateTrainScheduleVersions( tisNotification.trainID, 
                                                                         tisNotification.scheduleVersion,
                                                                         tisNotification.scheduleVersion );
                break;
            }
            // dont know what it is. New Type?
            default:
		    {
			    LOG_GENERIC(
			    SourceInfo, DebugUtil::DebugError,
			    "Tis Notification update is of unknown type %d", tisNotification.notification);
		    }
        }
	}


} // TA_App

