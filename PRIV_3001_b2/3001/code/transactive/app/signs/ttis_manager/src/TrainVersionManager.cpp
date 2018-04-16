/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/ttis_manager/src/TrainVersionManager.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * This is used to listen for train library and
  * time schedule version updates.
  *
  * Update messages are sent to the given window, they are:
  * WM_UPDATE_TRAINVERSION
  * wParam is a TrainLibraryAndTimeScheduleVersion*, it must be deleted when used
  *
  */

#include "stdafx.h"

#include "resource.h"

#include "TTISCommonConfiguration.h"
#include "TrainVersionManager.h"

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


using TA_Base_Core::ThreadGuard;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::MessageSubscriptionManager;
using TA_Base_Core::IEntityData;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Bus::TISAgentAccessFactory;

namespace TA_IRS_App
{

    TrainVersionManager::TrainVersionManager(CWnd* window)
        : m_windowToUpdate(window),
          m_trainVersions()
    {
        FUNCTION_ENTRY( "TrainVersionManager" );

        TA_ASSERT(m_windowToUpdate != NULL, "Window to update is null");

        // subscribe for messages
        subscribeToMessages();

        // get the initial download lists and train versions
        buildVersionList();

        FUNCTION_EXIT;
    }


    TrainVersionManager::~TrainVersionManager()
    {
        FUNCTION_ENTRY( "~TrainVersionManager" );

        // unsubscribe to messages
        MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

        FUNCTION_EXIT;
    }


    TrainVersionManager::TrainLibraryAndTimeScheduleVersion TrainVersionManager::getVersionForTrain(TA_IRS_Bus::CommonTypes::TrainIdType trainNumber)
    {
        FUNCTION_ENTRY( "getVersionForTrain" );

        // get the threadlock
        ThreadGuard guard(m_versionLock);

        // find the version for the train
        TrainVersionMap::iterator iter = m_trainVersions.find(trainNumber);

        // if it was found
        if ( iter != m_trainVersions.end() )
        {
            LOG_GENERIC (SourceInfo, DebugUtil::DebugDebug,
                        "Found version information for train %d", trainNumber);
            // return the version information
            FUNCTION_EXIT;
            return iter->second;
        }
        else
        {
            LOG_GENERIC (SourceInfo, DebugUtil::DebugDebug,
                        "Version information for train %d not found. Returning 0.", trainNumber);

            // not found. The verison is unknown
            // return an empty version.
            TrainLibraryAndTimeScheduleVersion version;
            version.trainNumber = trainNumber;
            version.predefinedLibraryVersion = 0;
            version.nextPredefinedLibraryVersion = 0;
            version.trainTimeScheduleVersion = 0;
            version.nextTrainTimeScheduleVersion = 0;

            FUNCTION_EXIT;
            return version;
        }
    }


    void TrainVersionManager::subscribeToMessages()
    {
        FUNCTION_ENTRY( "subscribeToMessages" );

        //
        // Subscribe to OCC and Depot TisAgent
        // There should only be a single entity at each location.
        //

        // subscribe to the global system wide version update messages for trains
        try
        {
            unsigned int locationKey = TTISCommonConfiguration::getInstance().getLocationKey();
			
            TA_Base_Core::IEntityDataList entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( TA_Base_Core::TTISEntityData::getStaticType(),
				locationKey );
			
			
            if ( entityData.size() != 1 )
            {
                const char* msg = "There is no TIS agent at this location.";
                const char* fld = "TIS Agent";
                TA_THROW( DataException( msg, DataException::MISSING_MANDATORY, fld ) );
            }
			
            unsigned long entityKey = entityData[0]->getKey();
            unsigned long subsystemKey = entityData[0]->getSubsystem();
			
            delete entityData[0];
			
            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( TA_Base_Core::TISComms::TisTrainDataVersionUpdate,
				this, entityKey, subsystemKey,
				locationKey );
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


    void TrainVersionManager::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        if( std::string(message.messageTypeKey) ==
            TA_Base_Core::TISComms::TisTrainDataVersionUpdate.getTypeKey() )
        {			
            TA_Base_Core::TrainDataVersion* versionChange;

            if( ( message.messageState >>= versionChange ) != 0 )
            {
                // get the threadlock
                ThreadGuard guard(m_versionLock);

                processVersionChange(*versionChange);
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Could not extract TrainDataVersion from CommsMessageCorbaDef");

                FUNCTION_EXIT;
                return;
            }
        }

        FUNCTION_EXIT;
    }


    void TrainVersionManager::buildVersionList()
    {
        FUNCTION_ENTRY( "buildVersionList" );

        try
        {
            // get the versions from the agent
            /*TA_Base_Core::TrainDataVersionList_var versionList =
                TISAgentAccessFactory::getInstance().getTTISAgent()->getTrainDataVersions();*/
			//libo++
			TA_Base_Core::TrainDataVersionList_var versionList;
			try
			{
				CORBA_CALL_RETURN(versionList,TISAgentAccessFactory::getInstance().getTTISAgent(),getTrainDataVersions,());
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(versionList,TISAgentAccessFactory::getInstance().getTTISAgent(),getTrainDataVersions,());
			}
			//++libo


            // now start processing. get the threadlock
            ThreadGuard guard(m_versionLock);

            // empty the map - it should be empty anyway
            m_trainVersions.clear();

            // go through the sequence, and send version updates
            // add the current data to the internal list
            for ( unsigned int i = 0; i < versionList->length(); i++)
            {
                processVersionChange(versionList[i]);
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


    void TrainVersionManager::processVersionChange(const TA_Base_Core::TrainDataVersion& versionChange)
    {
        FUNCTION_ENTRY( "processVersionChange" );

        // decouple from the corba object
        TrainLibraryAndTimeScheduleVersion* version = new TrainLibraryAndTimeScheduleVersion();
        version->trainNumber = versionChange.trainNumber;
        version->predefinedLibraryVersion = versionChange.predefinedLibraryVersion;
        version->nextPredefinedLibraryVersion = versionChange.nextPredefinedLibraryVersion;
        version->trainTimeScheduleVersion = versionChange.trainTimeScheduleVersion;
        version->nextTrainTimeScheduleVersion = versionChange.nextTrainTimeScheduleVersion;

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "Processing version update for train %d: Versions: %d %d %d %d",
                    version->trainNumber,
                    version->predefinedLibraryVersion,
                    version->nextPredefinedLibraryVersion,
                    version->trainTimeScheduleVersion,
                    version->nextTrainTimeScheduleVersion );

        // add or update the versions in the list
        m_trainVersions[versionChange.trainNumber] = *version;

        int ret = m_windowToUpdate->PostMessage( WM_UPDATE_TRAINVERSION,
                                                 reinterpret_cast<WPARAM>(version) );
        if (ret == 0)
        {
            LOG_GENERIC(
            SourceInfo, DebugUtil::DebugError,
            "Error posting train WM_UPDATE_TRAINVERSION message.");

            // clean up
            delete version;
        }

        FUNCTION_EXIT;
    }


} // TA_IRS_App

