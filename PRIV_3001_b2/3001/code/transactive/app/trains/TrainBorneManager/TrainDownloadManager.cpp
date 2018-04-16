/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP/3001/transactive/app/signs/ttis_manager/src/TrainDownloadManager.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/12/29 14:12:57 $
  * Last modified by:  $Author: grace.koh $
  *
  * This is used to make download and upgrade requests. Also to retry those
  * requests if necessary.
  *
  * It manages each type of download/upgrade request, and has instances of
  * TrainLibraryDownloads, TrainLibraryUpgrades, TrainScheduleDownloads, TrainScheduleUpgrades
  *
  * It is a thread that periodically retries requests. It will update the window given in the
  * constructor with WM_UPDATE_DOWNLOADLIST messages.
  * wParam is a TrainDownloadStatus*, it must be deleted when used)
  *
  * Downloads started from other TTIS manager GUIs will also be received from
  * the TIS agent and shown in the list.
  */

#include "stdafx.h"

#include "TTISCommonConfiguration.h"
#include "TrainDownloadManager.h"

#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/TTISEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/TISBroadcastComms_MessageTypes.h"
#include "core/message/types/TISComms_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "app/trains/TrainBorne_PIDS/src/RunThreadedFunc.h"
#include "boost/bind.hpp"

namespace TA_IRS_App
{

    using TA_Base_Core::ThreadGuard;
    using TA_Base_Core::DebugUtil;
    using TA_Base_Core::DatabaseException;
    using TA_Base_Core::DataException;
    using TA_Base_Core::MessageSubscriptionManager;
    using TA_Base_Core::IEntityData;
    using TA_Base_Core::EntityAccessFactory;
    using TA_Base_Bus::TISAgentAccessFactory;


    TrainDownloadManager::TrainDownloadManager(CWnd* window)
        : m_libraryDownloads(window),
          m_libraryUpgrades(window)
    {
        FUNCTION_ENTRY( "TrainDownloadManager" );

        TA_ASSERT(window != NULL, "Window to update is null");

		runThreadedFunc(boost::bind(&TrainDownloadManager::init, this));

        FUNCTION_EXIT;
    }


    TrainDownloadManager::~TrainDownloadManager()
    {
        FUNCTION_ENTRY( "~TrainDownloadManager" );

        // unsubscribe to messages
        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

        FUNCTION_EXIT;
    }


	void TrainDownloadManager::init()
	{
        // subscribe for messages
        subscribeToMessages();

        // get the initial download lists
        buildDownloadList();
	}

    void TrainDownloadManager::startLibraryDownload(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList, bool retry)
    {
        FUNCTION_ENTRY( "startLibraryDownload" );

        m_libraryDownloads.startDownload(trainList, retry);

        FUNCTION_EXIT;
    }

	void TrainDownloadManager::cancelDownload(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList)
	{
		FUNCTION_ENTRY("cancelDownload");

		m_libraryDownloads.requestCancelDownload(trainList);

		FUNCTION_EXIT;
	}

    void TrainDownloadManager::startLibraryUpgrade(const TA_IRS_Bus::CommonTypes::TrainIdList& trainList, bool retry)
    {
        FUNCTION_ENTRY( "startLibraryUpgrade" );

        m_libraryUpgrades.startDownload(trainList, retry);

        FUNCTION_EXIT;
    }   

    void TrainDownloadManager::start()
    {
        FUNCTION_ENTRY( "start" );

        // start each download type
        m_libraryDownloads.start();
        m_libraryUpgrades.start();
    
        FUNCTION_EXIT;
    }


    void TrainDownloadManager::terminateAndWait()
    {
        FUNCTION_ENTRY( "terminateAndWait" );

        // stop each download type
        m_libraryDownloads.terminateAndWait();
        m_libraryUpgrades.terminateAndWait();
      
        FUNCTION_EXIT;
    }


    void TrainDownloadManager::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Received download update message");
        if( std::string(message.messageTypeKey) ==
        TA_Base_Core::TISComms::TisTrainDownloadUpdate.getTypeKey() )
        {
            TA_Base_Core::TrainDownloadStatus* downloadChange;

            if( ( message.messageState >>= downloadChange ) != 0 )
            {
                processDownloadChange(*downloadChange);
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                            "Could not extract TrainDownloadStatus from CommsMessageCorbaDef");

                FUNCTION_EXIT;
                return;
            }
        }

        FUNCTION_EXIT;
    }


    void TrainDownloadManager::processDownloadChange(const TA_Base_Core::TrainDownloadStatus& downloadChange)
    {
        FUNCTION_ENTRY( "processDownloadChange" );

        // update the appropriate object
        switch (downloadChange.type)
        {
        case TA_Base_Core::LibraryDownloadStart:
            m_libraryDownloads.processDownloadStart(downloadChange.trainNumber);
            break;
        case TA_Base_Core::LibraryDownloadFinish:
            m_libraryDownloads.processDownloadFinish(downloadChange.trainNumber,
                                                     downloadChange.success,
                                                     std::string(downloadChange.errorDetails));
            break;
      
        case TA_Base_Core::LibraryUpgrade:
            m_libraryUpgrades.processDownloadFinish(downloadChange.trainNumber,
                                                    downloadChange.success,
                                                    std::string(downloadChange.errorDetails));
            break;
     
        }

        FUNCTION_EXIT;
    }


    void TrainDownloadManager::subscribeToMessages()
    {
        FUNCTION_ENTRY( "subscribeToMessages" );

        //
        // Get the entity, subsystem and location key.
        // There should only be a single entity at a given location.
        //
        try
        {
            unsigned int locationKey = TTISCommonConfiguration::getInstance().getLocationKey();

            TA_Base_Core::IEntityDataList entityData =
                EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation(
                    TA_Base_Core::TTISEntityData::getStaticType(), locationKey);


            if( entityData.size() != 1)
            {
                const char* msg = "There is no TIS agent at this location.";
                const char* fld = "TIS Agent";
                TA_THROW(DataException(msg, DataException::MISSING_MANDATORY, fld));
            }

            unsigned long entityKey    = entityData[0]->getKey();
            unsigned long subsystemKey = entityData[0]->getSubsystem();

            delete entityData[0];

            MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::TISComms::TisTrainDownloadUpdate,
                this,
                entityKey,
                subsystemKey,
                locationKey);
        }
        catch ( const TA_Base_Core::DatabaseException& dbe )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );
        }
        catch ( const TA_Base_Core::DataException& de )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
        }

        FUNCTION_EXIT;
    }


    void TrainDownloadManager::buildDownloadList()
    {
        FUNCTION_ENTRY( "buildDownloadList" );

        try
        {
            // get the downloads from the agent
            //TA_Base_Core::TrainDownloadList_var downloadList = TISAgentAccessFactory::getInstance().getTTISAgent()->getCurrentTrainDownloads();
			TA_Base_Core::TrainDownloadList_var downloadList;

			//libo++
			try
			{
				CORBA_CALL_RETURN(downloadList,TISAgentAccessFactory::getInstance().getTTISAgent(),getCurrentTrainDownloads,());
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(downloadList,TISAgentAccessFactory::getInstance().getTTISAgent(),getCurrentTrainDownloads,());
			}
			//++libo

            // go through the sequence, and send updates
            for ( unsigned int i = 0; i < downloadList->length(); i++)
            {
                // if there is a library download in progress
                if ( downloadList[i].predefinedDownloadInProgress == true )
                {
                    TA_Base_Core::TrainDownloadStatus update;
                    update.trainNumber = downloadList[i].trainNumber;
                    update.type = TA_Base_Core::LibraryDownloadStart;
                    update.success = true;

                    // notify the window to update and the download thread
                    processDownloadChange(update);
                }

                // if there is a schedule download in progress
                if ( downloadList[i].timeScheduleDownloadInProgress == true )
                {
                    TA_Base_Core::TrainDownloadStatus update;
                    update.trainNumber = downloadList[i].trainNumber;
                    update.type = TA_Base_Core::ScheduleDownloadStart;
                    update.success = true;

                    // notify the window to update and the download thread
                    processDownloadChange(update);
                }
            }
        }
        catch ( const TA_Base_Core::DatabaseException& dbe )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );
        }
        catch ( const TA_Base_Core::DataException& de )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::ObjectResolutionException", ore.what() );
        }
        catch( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while retrieving download state: " );
                exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );
        }

        FUNCTION_EXIT;
    }


} // TA_IRS_App
