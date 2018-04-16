/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source : $File$
  * @author  A. Parker
  * @version $Revision$
  * Last modification : $DateTime$
  * Last modified by : $Author$
  *
  *
  */
#if defined(_MSC_VER)
#pragma warning(disable : 4786 4146 4018 4250 4290)
#endif // defined _MSC_VER


#include <ace/OS.h>
#include <time.h>
#include <iostream>
#include <sstream>

#include "core/exceptions/src/RightsException.h"
#include "core/naming/src/Naming.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include "bus/generic_agent/src/GenericAgent.h"
#include "bus/pa/cachedmaps/src/PaStationDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaTrainDvaMessageCachedMap.h"
#include "bus/pa/cachedmaps/src/PaZoneCachedMap.h"
#include "bus/pa/cachedmaps/src/PaZoneGroupCachedMap.h"
#include "bus/pa/cachedmaps/src/CachedMaps.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/trains/TrainCorbaProxy/src/TrainInformationCorbaProxy.h"

#include "app/pa/PAAgent/src/TriggeredDVABroadcast.h"
#include "app/pa/PAAgent/src/BroadcastWorker.h"
#include "app/pa/PAAgent/src/CachedConfig.h"
#include "app/pa/PAAgent/src/DVARecordingBroadcast.h"
#include "app/pa/PAAgent/src/MasterDVABroadcast.h"
#include "app/pa/PAAgent/src/MasterLiveBroadcast.h"
#include "app/pa/PAAgent/src/PasHelpers.h"
#include "app/pa/PAAgent/src/StationDVABroadcast.h"
#include "app/pa/PAAgent/src/StationLiveBroadcast.h"
#include "app/pa/PAAgent/src/StationMusicBroadcast.h"
#include "app/pa/PAAgent/src/DVABroadcastStart.h"
#include "app/pa/PAAgent/src/DvaPlaybackStart.h"
#include "app/pa/PAAgent/src/LiveBroadcastStart.h"
#include "app/pa/PAAgent/src/DVABroadcastStop.h"
#include "app/pa/PAAgent/src/DvaPlaybackStop.h"
#include "app/pa/PAAgent/src/LiveBroadcastStop.h"
#include "app/pa/PAAgent/src/PASConnection.h"
#include "app/pa/PAAgent/src/ScheduleBroadcastWorker.h"
#include "app/pa/PAAgent/src/TriggeredBroadcastWorker.h"
#include "app/pa/PAAgent/src/BroadcastManager.h"
#include "app/pa/PAAgent/src/ZoneMonitoringStart.h"//added by lixiaoxia
#include "app/pa/PAAgent/src/ZoneMonitoringStop.h"//added by lixiaoxia
#include "app/pa/PAAgent/src/BackgroundMusicStart.h"
#include "app/pa/PAAgent/src/BackgroundMusicStop.h"

using namespace TA_Base_Core;
using namespace TA_Base_Bus;


namespace TA_IRS_App
{


    const std::string BroadcastManager::ATS_AGENT_SESSION_ID = "AtsAgentSessionId";
    const std::string BroadcastManager::MANAGER_LAUNCHING_BROADCAST_ID = "HardwareConnectionTest";


    BroadcastManager::BroadcastManager()
    : m_broadcastWorker( new BroadcastWorker() ),
      m_pScheduleBroadcastWorker( new ScheduleBroadcastWorker() ),
      m_pTriggeredBroadcastWorker( new TriggeredBroadcastWorker() ),
      m_rightsLibrary(0),
      m_resourceId(0),
      m_allBroadcasts( AbstractBroadcastCircularList::getInstance() ),
      m_MusicType( TA_Base_Bus::IPAAgentCorbaDef::NoMusic )
    {
        FUNCTION_ENTRY( "BroadcastManager" );

        if ( 0 != m_broadcastWorker )
        {
            m_broadcastWorker->start();
        }

        FUNCTION_EXIT;
    }

    BroadcastManager::~BroadcastManager()
    {
        FUNCTION_ENTRY( "~BroadcastManager" );

        try
        {
            if ( 0 != m_pScheduleBroadcastWorker )
            {
                delete m_pScheduleBroadcastWorker;
                m_pScheduleBroadcastWorker = 0;
            }

            if ( 0 != m_pTriggeredBroadcastWorker )
            {
                delete m_pTriggeredBroadcastWorker;
                m_pTriggeredBroadcastWorker = 0;
            }

            if ( 0 != m_broadcastWorker)
            {
                if (m_broadcastWorker->getCurrentState() == Thread::THREAD_STATE_RUNNING)
                {
                    m_broadcastWorker->terminateAndWait();
                }
                delete m_broadcastWorker;
                m_broadcastWorker = 0;
            }

            // Cleanup time for all broadcasts
            while (!m_allBroadcasts->isEmpty())
            {
                AbstractBroadcast* broadcast = m_allBroadcasts->getNext();
                m_allBroadcasts->remove( broadcast );
                delete broadcast;
            }
            m_allBroadcasts = NULL;

            if (m_rightsLibrary != NULL)
            {
                delete m_rightsLibrary;
                m_rightsLibrary = NULL;
            }
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Exception caught in destructor");
        }

        FUNCTION_EXIT;
    }

	void BroadcastManager::enable()
	{
        if ( 0 != m_broadcastWorker )
        {
	        m_broadcastWorker->enable();
        }

        if ( 0 != m_pScheduleBroadcastWorker )
        {
            m_pScheduleBroadcastWorker->setToControl();
        }

        if ( 0 != m_pTriggeredBroadcastWorker )
        {
            m_pTriggeredBroadcastWorker->setToControl();
        }
	}

	void BroadcastManager::disable()
	{
        if ( 0 != m_broadcastWorker )
        {
            m_broadcastWorker->disable();
        }

        if ( 0 != m_pScheduleBroadcastWorker )
        {
            m_pScheduleBroadcastWorker->setToMonitor();
        }

        if ( 0 != m_pTriggeredBroadcastWorker )
        {
            m_pTriggeredBroadcastWorker->setToMonitor();
        }
	}

    // IDL Interface functions
    // exception checked
    std::string BroadcastManager::getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::EBroadcastType broadcastType,
                                                    const std::string& extraInformation /* = "" */)
        // TA_THROW CorruptInternalStateDataException
    {
        FUNCTION_ENTRY( "getNewBroadcastId" );

        ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();
        static std::string lastNowString = "";
        static unsigned long extraInfoForUniqueness = 0;

        std::string broadcastTypeString;

        switch (broadcastType)
        {
        case TA_Base_Bus::IPAAgentCorbaDef::StationMusic :
            if (CachedConfig::getInstance()->getIsDepot())
            {
                broadcastTypeString = "DepotMusic at ";
            }
            else
            {
                broadcastTypeString = "StationMusic at ";
            }
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::StationLive :
            broadcastTypeString = "StationLive " + extraInformation + " at ";
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::StationDva :
            broadcastTypeString = "StationDva " + extraInformation + " at ";
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::TrainLive :
            broadcastTypeString = "TrainLive at ";
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::TrainDva :
            broadcastTypeString = "TrainDva at ";
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::StationRecording :
            broadcastTypeString = "StationRecording at ";
            break;
        case TA_Base_Bus::IPAAgentCorbaDef::OccPlayback :
            broadcastTypeString = "OccPlayback at ";
            break;
		//added by lixiaoxia
		case TA_Base_Bus::IPAAgentCorbaDef::ZoneMonitoring :
			broadcastTypeString = "ZoneMonitoring at";
			break;
        default :
            TA_THROW(CorruptInternalStateDataException("Unexpected EBroadcastType"));

        }

        // Build time stamp

        time_t now = time( NULL );
        tm nowTm;
        ACE_OS::localtime_r(&now, &nowTm);

        char nowString[80] = {0};
        sprintf(nowString,
                "%02d:%02d:%02d",
                (nowTm.tm_hour),
                (nowTm.tm_min),
                (nowTm.tm_sec));


        std::string broadcastId;

        // Since we only have a resolution of 1 sec, we need to check if the request
        // was done within 1 second of the previous broadcast by comparing the new nowString
        // with the previous.  We just need to make sure that the broadcastIds are all
        // unique.

        if (lastNowString == nowString)
        {
            // Ensure broadcastIds are different by updating the suffix
            ++extraInfoForUniqueness;

            char suffix[10] = {0};
            sprintf(suffix, " (%d)", extraInfoForUniqueness);

            broadcastId = broadcastTypeString + nowString + suffix;
        }
        else
        {
            // Reset for the next time
            extraInfoForUniqueness = 0;

            broadcastId = broadcastTypeString + nowString;
        }

        lastNowString = nowString;

        FUNCTION_EXIT;
        return broadcastId;
    }


    // exception checked
    char* BroadcastManager::broadcastStationDva( const ::TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages, 
        const ::TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, ::CORBA::UShort dwellInSecs, ::CORBA::Boolean hasChime, 
        ::CORBA::Boolean isSynchronisedWithTis, const char* groupName, const char* sessionId )
    // raises (BadParameterException, TA_Base_Core::AccessDeniedException, BroadcastLimitReached)
    {
        FUNCTION_ENTRY( "broadcastStationDva" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "broadcastStationDva" );

        TA_Base_Bus::GenericAgent::ensureControlMode();

        ThreadGuard guard( m_lock );

        // this is to build a broadcast ID with more information
        std::ostringstream extraInformation;
        extraInformation << groupName << " (";
        try
        {
            for ( int i = 0; i < messages.length(); ++i )
            {
                extraInformation << TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageIdFromKey(messages[i]);

                if ( i != (messages.length() - 1) )
                {
                    // avoid adding comma for the last item
                    extraInformation << ",";
                }
            }
        }
        catch(TA_Base_Core::CachedMappingNotFoundException& e)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what());

            throw TA_Base_Core::BadParameterException("The specified message does not exist in the database");
        }
        extraInformation << ")";

        std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationDva, extraInformation.str());

        // this will validate the parameters like the zones and messages
        // as part of the audit message submission

        PasHelpers::getInstance()->submitPaBroadcastStationDvaRequestAuditMessage( hasChime, NULL, zones, messages, sessionId );

        CORBA::UShort sourceId = 0;

        TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig newBroadcastConfigP;
        TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig theSpecifics;

        // Note: string members will make a deep copy from a const char*

        newBroadcastConfigP.broadcastType      = TA_Base_Bus::IPAAgentCorbaDef::StationDva;
        newBroadcastConfigP.broadcastId        = broadcastId.c_str();
        newBroadcastConfigP.isEventTriggered   = false;
        newBroadcastConfigP.submitTime         = time( 0 );
        newBroadcastConfigP.isStop             = false;

        theSpecifics.dwellInSecs                = dwellInSecs;
        theSpecifics.hasChime                   = hasChime;
        theSpecifics.isSynchronisedWithTis      = isSynchronisedWithTis;
        theSpecifics.messages                   = messages;
        theSpecifics.sourceId                   = static_cast<CORBA::Octet>(sourceId);
        theSpecifics.zones                      = zones;
        // No audit message needed for local playback
        theSpecifics.isLocalPlayback            = false;
        theSpecifics.priorityGroup = PasHelpers::getInstance()->getPaPriorityIdBySessionId( sessionId );
        theSpecifics.stCyclicParams.usCyclicTimes = 0;

        // Initialise the union inside BroadcastConfig
        newBroadcastConfigP.theBroadcastConfigSpecifics.theStationDvaConfig(theSpecifics);

        DVABroadcastStart newBroadcast( broadcastId );

        try
        {
            // Can throw TA_Base_Core::BadParameterException

            // update parameters
            newBroadcast.updatePriorityBySessionId( sessionId );

            newBroadcast.updateCylicTimes( 0 );
            
            Keys lstZoneKeys;
            PasHelpers::getInstance()->coverageToZoneKeys( zones, lstZoneKeys );
            newBroadcast.updateStationZoneIdsByZoneKeys( lstZoneKeys );

            newBroadcast.updateDVAMsgIdByDVAMsgKey( messages[0] );

            if ( newBroadcast.isParameterInvalid() )
            {
                std::string strBadParams = newBroadcast.getInvalidParameter();

                throw (TA_Base_Core::BadParameterException(strBadParams.c_str()));
            }
        }
        catch(TA_Base_Core::BadParameterException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "BadParameterException", ex.reason.in());

            throw;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

            throw (TA_Base_Core::BadParameterException("Unknown exception"));;
        }

        try
        {
            PASConnection::getInstance()->writeToHardwareImmediately( &newBroadcast );
        }
        catch (...)
        {
            // Throw to client
        	throw;
        }

        FUNCTION_EXIT;
        return CORBA::string_dup(broadcastId.c_str());
    }


    // exception checked
    char* BroadcastManager::broadcastStationScheduleDva( const ::TA_Base_Bus::IPAAgentCorbaDef::MessageKeySeq& messages, 
        const ::TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, ::CORBA::UShort dwellInSecs, 
        ::CORBA::Boolean hasChime, ::CORBA::Boolean isSynchronisedWithTis, const char* groupName, 
        const ::TA_Base_Bus::IPAAgentCorbaDef::StationCyclicParams& stCyclicParams, const char* sessionId )
        // raises (BadParameterException, TA_Base_Core::AccessDeniedException, BroadcastLimitReached)
    {
        FUNCTION_ENTRY( "broadcastStationScheduleDva" );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "broadcastStationDva" );

        TA_Base_Bus::GenericAgent::ensureControlMode();

        ThreadGuard guard( m_lock );

        // this is to build a broadcast ID with more information
        std::ostringstream extraInformation;
        extraInformation << groupName << " (";
        try
        {
            for ( int i = 0; i < messages.length(); ++i )
            {
                extraInformation << TA_IRS_Bus::CachedMaps::getInstance()->getStationDvaMessageIdFromKey( messages[i] );

                if ( i != (messages.length() - 1) )
                {
                    // avoid adding comma for the last item
                    extraInformation << ",";
                }
            }
        }
        catch ( TA_Base_Core::CachedMappingNotFoundException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::CachedMappingNotFoundException", e.what() );

            throw TA_Base_Core::BadParameterException( "The specified message does not exist in the database" );
        }
        extraInformation << ")";

        std::string broadcastId = getNewBroadcastId( TA_Base_Bus::IPAAgentCorbaDef::StationDva, extraInformation.str() );

        // this will validate the parameters like the zones and messages
        // as part of the audit message submission

        PasHelpers::getInstance()->submitPaBroadcastStationDvaRequestAuditMessage( hasChime, &stCyclicParams, zones, messages, sessionId );

        CORBA::UShort sourceId = 0;

        TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig newBroadcastConfigP;
        TA_Base_Bus::IPAAgentCorbaDef::StationDvaConfig theSpecifics;

        // Note: string members will make a deep copy from a const char*

        newBroadcastConfigP.broadcastType = TA_Base_Bus::IPAAgentCorbaDef::StationDva;
        newBroadcastConfigP.broadcastId = broadcastId.c_str();
        newBroadcastConfigP.isEventTriggered = false;
        newBroadcastConfigP.submitTime = time( 0 );
        newBroadcastConfigP.isStop = false;

        theSpecifics.dwellInSecs = dwellInSecs;
        theSpecifics.hasChime = hasChime;
        theSpecifics.isSynchronisedWithTis = isSynchronisedWithTis;
        theSpecifics.messages = messages;
        theSpecifics.sourceId = static_cast<CORBA::Octet>(sourceId);
        theSpecifics.zones = zones;
        // No audit message needed for local playback
        theSpecifics.isLocalPlayback = false;
        theSpecifics.priorityGroup = PasHelpers::getInstance()->getPaPriorityIdBySessionId( sessionId );
        theSpecifics.stCyclicParams = stCyclicParams;

        // Initialise the union inside BroadcastConfig
        newBroadcastConfigP.theBroadcastConfigSpecifics.theStationDvaConfig( theSpecifics );

		if (( stCyclicParams.startTime == 0 ) && ( stCyclicParams.stopTime == 0 ))
		{
			DVABroadcastStart newBroadcast(broadcastId);

			try
			{
				// Can throw TA_Base_Core::BadParameterException

				// update parameters
				newBroadcast.updatePriorityBySessionId(sessionId);

				newBroadcast.updateCylicTimes(stCyclicParams.usCyclicTimes);

				Keys lstZoneKeys;
				PasHelpers::getInstance()->coverageToZoneKeys(zones, lstZoneKeys);
				newBroadcast.updateStationZoneIdsByZoneKeys(lstZoneKeys);

				newBroadcast.updateDVAMsgIdByDVAMsgKey(messages[0]);

				if (newBroadcast.isParameterInvalid())
				{
					std::string strBadParams = newBroadcast.getInvalidParameter();

					throw (TA_Base_Core::BadParameterException(strBadParams.c_str()));
				}
			}
			catch (TA_Base_Core::BadParameterException& ex)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "BadParameterException", ex.reason.in());

				throw;
			}
			catch (...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown reason");

				throw (TA_Base_Core::BadParameterException("Unknown exception"));;
			}

			try
			{
				PASConnection::getInstance()->writeToHardwareImmediately(&newBroadcast);
			}
			catch (...)
			{
				// Throw to client
				throw;
			}
		}
		else
		{
			m_pScheduleBroadcastWorker->addNewScheduleBroadcast(newBroadcastConfigP);
		}

        return CORBA::string_dup( broadcastId.c_str() );;
    }

    void BroadcastManager::agentLinkTest()
    {
        FUNCTION_ENTRY( "agentLinkTest" );

		TA_Base_Bus::GenericAgent::ensureControlMode();
        // empty method for PA Manager link test only

        FUNCTION_EXIT;
    }

    bool BroadcastManager::hardwareLinkTest(const char* broadcastId, const char* sessionId)
    {
        FUNCTION_ENTRY( "hardwareLinkTest" );

        // test hardware connection availability
        ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();
        return false;
    }

    // exception checked
    char* BroadcastManager::broadcastStationLive( ::CORBA::Boolean hasChime, ::CORBA::UShort sourceId, const ::TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const char* groupName, const char* sessionId )
    // raises (BadParameterException, TA_Base_Core::AccessDeniedException, BroadcastLimitReached)
    {
        FUNCTION_ENTRY( "broadcastStationLive" );

        LOGOUT("broadcastStationLive");
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "broadcastStationLive" );

        ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();
        // this is to build a bvroadcast ID with more information
        std::string extraInformation(groupName);
        std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationLive, extraInformation);

        // this will validate the parameters like the zones and messages
        // as part of the audit message submission
        PasHelpers::getInstance()->submitPaBroadcastStationLiveRequestAuditMessage( zones, sessionId );


        // Check the broadcast limit after sending the audit messages for tracability
//         if (m_allBroadcasts->getSize() >= CachedConfig::getInstance()->getMaxBroadcasts())
//         {
//             LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
//                 "Request failed due to broadcast limit (%s)",
//                 CachedConfig::getInstance()->getMaxBroadcasts() );
// 
//             PasHelpers::getInstance()->throwBroadcastLimitReachedException(broadcastId);
//         }


        TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig newBroadcastConfigP;
        TA_Base_Bus::IPAAgentCorbaDef::StationLiveConfig theSpecifics;

        // Note: string members will make a deep copy from a const char*

        newBroadcastConfigP.broadcastType      = TA_Base_Bus::IPAAgentCorbaDef::StationLive;
        newBroadcastConfigP.broadcastId        = broadcastId.c_str();
        newBroadcastConfigP.isEventTriggered   = false;
        newBroadcastConfigP.submitTime         = time( NULL );
        newBroadcastConfigP.isStop             = false;
        theSpecifics.sourceId                  = sourceId;
        theSpecifics.zones                     = zones;

        newBroadcastConfigP.theBroadcastConfigSpecifics.theStationLiveConfig(theSpecifics);

        LiveBroadcastStart newBroadcast( broadcastId );

        try
        {
            // update parameters
            newBroadcast.updatePriorityBySessionId( sessionId );

            Keys lstZoneKeys;
            PasHelpers::getInstance()->coverageToZoneKeys( zones, lstZoneKeys );
            newBroadcast.updateStationZoneIdsByZoneKeys( lstZoneKeys );

            if ( newBroadcast.isParameterInvalid() )
            {
                std::string strBadParams = newBroadcast.getInvalidParameter();

                throw (TA_Base_Core::BadParameterException(strBadParams.c_str()));
            }
        }
	    catch(TA_Base_Core::BadParameterException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "BadParameterException", ex.reason.in());

            // Pass on to the client
            throw;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

            throw (TA_Base_Core::BadParameterException("Unknown exception"));;
        }

        try
        {
            PASConnection::getInstance()->writeToHardwareImmediately( &newBroadcast );
        }
        catch (...)
        {
            // Throw to client
            throw;
        }

        // Notify all PA Managers
//         TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData updateData;

        // Note: string members will make a deep copy from a const char*
//         updateData.fromBroadcastId = const_cast<const char*>("");
//         updateData.toBroadcastId   = broadcastId.c_str();
//         updateData.broadcastType   = TA_Base_Bus::IPAAgentCorbaDef::StationLive;
//         updateData.sessionId       = sessionId;

        // Send state update message to Manager
//         PasHelpers::getInstance()->sendCurrentBroadcastsUpdate(updateData);
        // Syn to monitor Agent
//         PasHelpers::getInstance()->sendPAAgentNewBroadcastUpdate(*newBroadcastConfigP);

        FUNCTION_EXIT;
        return CORBA::string_dup(broadcastId.c_str());
    }

    TA_Base_Bus::IPAAgentCorbaDef::EMusicType BroadcastManager::getMusicStatus(const char* sessionId)
    {
        FUNCTION_ENTRY( "getMusicStatus" );

		TA_Base_Bus::GenericAgent::ensureControlMode();

        FUNCTION_EXIT;
        return m_MusicType;
    }

	void BroadcastManager::broadcastStationMusic(TA_Base_Bus::IPAAgentCorbaDef::EMusicType musicType, const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const char* sessionId)
	{
		FUNCTION_ENTRY( "broadcastStationMusic" );

		LOGOUT("broadcastStationMusic");

		ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();

        // Allow operator to redo music broadcast.
        //if ( m_MusicType == musicType )
        //{
        //	throw BadParameterException( "Can't set to same music type" );
        //}

		std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationMusic);

		if (CachedConfig::getInstance()->getIsMaster())
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Background music broadcasts are not possible from OCC" );

			FUNCTION_EXIT;
			throw BadParameterException("Background music broadcasts are not possible from OCC");
		}

		PasHelpers::getInstance()->submitPaBroadcastStationMusicRequestAuditMessage( musicType, sessionId );

		ModbusPacket* newBackgroundMusic = NULL;

		switch (musicType)
		{
		case TA_Base_Bus::IPAAgentCorbaDef::OccMusic:			
		case TA_Base_Bus::IPAAgentCorbaDef::LocalMusic:
            newBackgroundMusic = new BackgroundMusicStart(broadcastId);
			break;

		case TA_Base_Bus::IPAAgentCorbaDef::NoMusic:
			newBackgroundMusic = new BackgroundMusicStop(broadcastId);
			break;

		default:

			LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Invalid music type specified in request" );

			FUNCTION_EXIT;
			throw BadParameterException("Invalid music type specified in request");
		}

		try
		{
			// update parameters
			newBackgroundMusic->updatePriorityBySessionId( sessionId );

			Keys lstZoneKeys;
			PasHelpers::getInstance()->coverageToZoneKeys(zones, lstZoneKeys);
			newBackgroundMusic->updateStationZoneIdsByZoneKeys(lstZoneKeys);

			if ( newBackgroundMusic->isParameterInvalid() )
			{
				std::string strBadParams = newBackgroundMusic->getInvalidParameter();

				throw (TA_Base_Core::BadParameterException(strBadParams.c_str()));
			}
		}
		catch(TA_Base_Core::BadParameterException& ex)
		{
			delete newBackgroundMusic;
			newBackgroundMusic = NULL;

            LOG_EXCEPTION_CATCH(SourceInfo, "BadParameterException", ex.reason.in());
			throw;
		}
		catch(...)
		{
			delete newBackgroundMusic;
			newBackgroundMusic = NULL;
			LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

			throw;
		}

		try
		{
			PASConnection::getInstance()->writeToHardwareImmediately( newBackgroundMusic );
		}
		catch (...)
		{
			delete newBackgroundMusic;
			newBackgroundMusic = NULL;

			// Throw to client
			throw;
		}
        
		delete newBackgroundMusic;
        newBackgroundMusic = NULL;
		
		switch (musicType)
		{
		case TA_Base_Bus::IPAAgentCorbaDef::OccMusic:
		case TA_Base_Bus::IPAAgentCorbaDef::LocalMusic:
		case TA_Base_Bus::IPAAgentCorbaDef::NoMusic:
			m_MusicType = musicType;
			break;

		default:
			// impossible to reach here thanks to previous switch statement
			// which thrown an exception
			;
		}

		FUNCTION_EXIT;
	}

    char* BroadcastManager::recordAdhocMessage( ::CORBA::UShort sourceId, ::CORBA::ULong messageKey, ::CORBA::UShort usMsgType, const char* sessionId )
    // raises (TA_Base_Core::BadParameterException, TA_Base_Core::AccessDeniedException, BroadcastLimitReached)
    {
        FUNCTION_ENTRY( "recordAdhocMessage" );

        LOGOUT("recordAdhocMessage");

        ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();
        std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationRecording);

        PasHelpers::getInstance()->submitPaRecordAdhocRequestAuditMessage( sourceId, messageKey, 0, sessionId );

        // Check the broadcast limit after sending the audit messages for tracability
        if (m_allBroadcasts->getSize() >= CachedConfig::getInstance()->getMaxBroadcasts())
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                "Request failed due to broadcast limit (%d)",
                CachedConfig::getInstance()->getMaxBroadcasts() );

            // submit audit message
            TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;
            zones.length(0);

            PasHelpers::getInstance()->throwBroadcastLimitReachedException();
        }


        TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* newBroadcastConfigP = new TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig();
        TA_Base_Bus::IPAAgentCorbaDef::StationRecordingConfig theSpecifics;

        // Note: string members will make a deep copy from a const char*

        newBroadcastConfigP->broadcastType      = TA_Base_Bus::IPAAgentCorbaDef::StationRecording;
        newBroadcastConfigP->broadcastId        = broadcastId.c_str();
        newBroadcastConfigP->isEventTriggered   = false;
        newBroadcastConfigP->submitTime         = time( NULL );

        theSpecifics.messageKey                 = messageKey;
        theSpecifics.sourceId                   = sourceId;

        newBroadcastConfigP->theBroadcastConfigSpecifics.theStationRecordingConfig(theSpecifics);


        AbstractBroadcast* newBroadcast = NULL;
        try
        {
            // Can throw TA_Base_Core::BadParameterException

            newBroadcast = new DVARecordingBroadcast( newBroadcastConfigP, sessionId);

            newBroadcast->validateBroadcastConfig();
        }
        catch(TA_Base_Core::BadParameterException& ex)
        {
            delete newBroadcast;
            newBroadcast = NULL;

            LOG_EXCEPTION_CATCH(SourceInfo, "BadParameterException", ex.reason.in());
            // Pass on to the client

            FUNCTION_EXIT;
            throw;
        }
        catch(...)
        {
            delete newBroadcast;
            newBroadcast = NULL;

            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

            // Pass on to the client

            FUNCTION_EXIT;
            throw;
        }

        m_allBroadcasts->insert(newBroadcast);

        // Notify all PA Managers
        //TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData updateData;

        //// Note: string members will make a deep copy from a const char*
        //updateData.fromBroadcastId = const_cast<const char*>("");
        //updateData.toBroadcastId   = newBroadcastConfigP->broadcastId;
        //updateData.broadcastType   = newBroadcastConfigP->broadcastType;
        //updateData.sessionId       = sessionId;

        //PasHelpers::getInstance()->sendCurrentBroadcastsUpdate(updateData);
        //PasHelpers::getInstance()->sendPAAgentNewBroadcastUpdate(*newBroadcastConfigP);

        FUNCTION_EXIT;
        return CORBA::string_dup(newBroadcastConfigP->broadcastId);
    }


    void BroadcastManager::retryStationBroadcast
        ( const char* broadcastId,
          const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
          const char* sessionId )
    //raises (BroadcastIdNotFound, BadParameterException, TA_Base_Core::AccessDeniedException)
    {
        FUNCTION_ENTRY( "retryStationBroadcast" );

        LOGOUT("retryStationBroadcast");

        ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();
        AbstractBroadcast* broadcast = m_allBroadcasts->getBroadcast(broadcastId);
        if (broadcast == NULL)
        {
            PasHelpers::getInstance()->throwBroadcastIdNotFoundException(
                "Request failed due to invalid BroadcastId",
                broadcastId );
        }

        AbstractStationBroadcast* stationBroadcast = dynamic_cast<AbstractStationBroadcast*>(broadcast);
        if (stationBroadcast == NULL)
        {
            std::ostringstream msg;
            msg << "The specified broadcastId " << broadcastId << " is not a station broadcast";

            FUNCTION_EXIT;
            throw BadParameterException(msg.str().c_str());
        }

        PasHelpers::getInstance()->submitPaRetryStationBroadcastRequestAuditMessage( stationBroadcast->getBroadcastType(), zones, sessionId );

        // Queue actual execution in BroadcastWorker thread
        stationBroadcast->retry(zones, sessionId);

        FUNCTION_EXIT;
    }


    void BroadcastManager::terminateBroadcast( const char* broadcastId, const char* sessionId )
    //raises (BroadcastIdNotFound, TA_Base_Core::AccessDeniedException)
    {
        FUNCTION_ENTRY( "terminateBroadcast" );

        LOGOUT("terminateBroadcast");

        ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();
        AbstractBroadcast* broadcast = m_allBroadcasts->getBroadcast(broadcastId);
        if (broadcast == NULL)
        {
            PasHelpers::getInstance()->throwBroadcastIdNotFoundException(
                "Failed to terminate a non-existent broadcast ",
                broadcastId );
        }


        // TES 735 - dont display event for playback
        // Merge the conditions to make the playback aborting just return without
        // submitting message. Otherwise, submit terminating message.
        if ( broadcast->getBroadcastType() == TA_Base_Bus::IPAAgentCorbaDef::StationDva )
        {
            TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* config = broadcast->getBroadcastConfigCopy();

            if (! config->theBroadcastConfigSpecifics.theStationDvaConfig().isLocalPlayback)
            {
            }

            delete config;
        }
        else
        {
        }

        if (broadcast->getBroadcastState() != TA_Base_Bus::IPAAgentCorbaDef::Termination &&
            broadcast->getBroadcastState() != TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval)
        {
            // Queue actual termination in BroadcastWorker thread
            broadcast->operatorAbort();
        }

        FUNCTION_EXIT;
    }


    void BroadcastManager::removeBroadcast ( const char* broadcastId, const char* sessionId )
    //raises (BroadcastIdNotFound, BadParameterException, TA_Base_Core::AccessDeniedException)
    {
        FUNCTION_ENTRY( "removeBroadcast" );

        LOGOUT("removeBroadcast");

        ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();
        AbstractBroadcast* broadcast = m_allBroadcasts->getBroadcast(broadcastId);
        if (broadcast == NULL)
        {
            PasHelpers::getInstance()->throwBroadcastIdNotFoundException(
                "Failed to remove a non-existent broadcast",
                broadcastId );
        }

        if ( (broadcast->getBroadcastState() == TA_Base_Bus::IPAAgentCorbaDef::PendingRemoval) ||
             (broadcast->getBroadcastState() == TA_Base_Bus::IPAAgentCorbaDef::Termination))
        {
            // TES 735 - no audit for local playback
            if ( broadcast->getBroadcastType() == TA_Base_Bus::IPAAgentCorbaDef::StationDva )
            {
                TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* config = broadcast->getBroadcastConfigCopy();

                if (! config->theBroadcastConfigSpecifics.theStationDvaConfig().isLocalPlayback)
                {
                    PasHelpers::getInstance()->submitPaRemoveBroadcastAuditMessage(broadcastId, sessionId);
                }

                delete config;
            }
            else
            {
                PasHelpers::getInstance()->submitPaRemoveBroadcastAuditMessage(broadcastId, sessionId);
            }

            // Flag the table for deletion when the BroadcastWorker is ready
            broadcast->setIsTaggedForRemoval(true);
        }
        else
        {
            FUNCTION_EXIT;
            throw BadParameterException("Broadcast must be terminated first");
        }

        FUNCTION_EXIT;
    }


    ::CORBA::Boolean BroadcastManager::setAdhocType( ::CORBA::ULong messageKey, ::CORBA::UShort messageType, const char* sessionId )
    //raises (BadParameterException, FepcCommsFailure, DatabaseCommsFailure, TA_Base_Core::AccessDeniedException);
    {
        FUNCTION_ENTRY("setAdhocType");

        FUNCTION_EXIT;
        return true;
    }


    ::CORBA::Boolean BroadcastManager::setAdhocLabel( ::CORBA::ULong messageKey, const char* label, const char* sessionId )
    //raises (BadParameterException, DatabaseCommsFailure, TA_Base_Core::AccessDeniedException);
    {
        FUNCTION_ENTRY( "setAdhocLabel" );

        FUNCTION_EXIT;
        return true;
    }


    TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq* BroadcastManager::getBroadcastIdTypes()
    {
        FUNCTION_ENTRY( "getBroadcastIdTypes" );

        LOGOUT("GetBroadcastIdsgetBroadcastIdTypes");

        ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();
        TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq* broadcastIdTypes = new TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq();

        m_allBroadcasts->getAllBroadcastIdTypes(*broadcastIdTypes);

        FUNCTION_EXIT;
        return broadcastIdTypes;
    }


    void BroadcastManager::changeBroadcastId
        ( const char* fromBroadcastId,
          const char* toBroadcastId,
          const char* sessionId )
    // raises (BroadcastIdNotFound, BadParameterException, TA_Base_Core::AccessDeniedException)
    {
        FUNCTION_ENTRY( "changeBroadcastId" );

        LOGOUT("changeBroadcastId");

        ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();

        FUNCTION_EXIT;
    }


    TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* BroadcastManager::getBroadcastConfig ( const char* broadcastId )
    // raises (BroadcastIdNotFound)
    {
        FUNCTION_ENTRY( "getBroadcastConfig" );

        LOGOUT("getBroadcastConfig");

        ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();
        AbstractBroadcast* broadcast = m_allBroadcasts->getBroadcast(broadcastId);
        if (broadcast == NULL)
        {
            PasHelpers::getInstance()->throwBroadcastIdNotFoundException("Failed to return the broadcast config for a non-existent broadcast", broadcastId);
        }

        // Return the deep copy
        FUNCTION_EXIT;
        return broadcast->getBroadcastConfigCopy();
    }


    TA_Base_Bus::IPAAgentCorbaDef::ProgressReport* BroadcastManager::getProgressReport ( const char* broadcastId )
    // raises (BroadcastIdNotFound)
    {
        FUNCTION_ENTRY( "getProgressReport" );

        LOGOUT("getProgressReport");

        ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();
        AbstractBroadcast* broadcast = m_allBroadcasts->getBroadcast(broadcastId);
        if (broadcast == NULL)
        {
            PasHelpers::getInstance()->throwBroadcastIdNotFoundException("Failed to return the broadcast config for a non-existent broadcast", broadcastId);
        }

        // Return the deep copy
        FUNCTION_EXIT;
        return broadcast->getProgressReportCopy();
    }

    
    TA_Base_Bus::IPAAgentCorbaDef::StationDvaMessageRecords* BroadcastManager::getStationDvaMessageRecords()
    {
        FUNCTION_ENTRY( "getStationDvaMessageRecords" );

        LOGOUT("getStationDvaMessageRecords");

        ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();
        TA_Base_Bus::IPAAgentCorbaDef::StationDvaMessageRecords* newData = NULL;

        TA_IRS_Bus::PaStationDvaMessageCachedMap& theMap = TA_IRS_Bus::CachedMaps::getInstance()->getPaStationDvaMessageCachedMap();
        std::list<TA_IRS_Bus::StationDvaMessageRecord> allRecords = theMap.getAllRecords();
        std::list<TA_IRS_Bus::StationDvaMessageRecord>::iterator it = allRecords.begin();

        newData = new TA_Base_Bus::IPAAgentCorbaDef::DvaMessagesUpdateData();
        newData->length(allRecords.size());
        for (int index=0;    it!=allRecords.end();    ++it,++index)
        {
            (*newData)[index].m_pKey                = (*it).ulKey;
            (*newData)[index].m_id                  = (*it).usMsgId;
            (*newData)[index].m_locationKey         = (*it).ulLocationKey;
            (*newData)[index].m_label               = (*it).strMsgName.c_str();
            (*newData)[index].m_type                = (*it).usMsgType;
        }

        // There should be no exceptions thrown above
        FUNCTION_EXIT;
        return newData;
    }


    TA_Base_Bus::IPAAgentCorbaDef::TrainDvaMessageRecords* BroadcastManager::getTrainDvaMessageRecords()
    {
        FUNCTION_ENTRY( "getTrainDvaMessageRecords" );

        LOGOUT("getTrainDvaMessageRecords");
        ThreadGuard guard( m_lock );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "IDL interface getTrainDvaMessageRecords is called but should not used any more" );
        // There should be no exceptions thrown above
        FUNCTION_EXIT;
        return new TA_Base_Bus::IPAAgentCorbaDef::TrainDvaMessageRecords();
    }


    TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords* BroadcastManager::getDvaVersionRecords()
    {
        FUNCTION_ENTRY( "getDvaVersionRecords" );

        LOGOUT("getDvaVersionRecords");

        ThreadGuard guard( m_lock );

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "IDL interface getDvaVersionRecords is called but should not used any more" );
        FUNCTION_EXIT;
        return new TA_Base_Bus::IPAAgentCorbaDef::DvaVersionRecords();
    }

    
    void BroadcastManager::broadcastEventTriggeredStationDva( CORBA::ULong messageKey,
                                                              const char* messageVersionNumber,
                                                              CORBA::Boolean hasChime,
                                                              CORBA::Boolean isSynchronisedWithTis,
                                                              CORBA::ULong durationInSecs,
                                                              CORBA::UShort periodInSecs,
                                                              const char* sessionId,
                                                              const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones)
    //raises (BadParameterException, TA_Base_Core::AccessDeniedException, BroadcastLimitReached)
    {
        FUNCTION_ENTRY( "broadcastEventTriggeredStationDva" );

        ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();

        std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationDva);

        PasHelpers::getInstance()->submitPaBroadcastEventTriggeredStationDvaRequestAuditMessage(
            broadcastId,
            messageKey,
            messageVersionNumber,
            hasChime,
            isSynchronisedWithTis,
            durationInSecs,
            periodInSecs,
            zones,
            sessionId );

        broadcastEventTriggeredStationDvaInternal(
            broadcastId,
            messageKey,
            zones,
            0,
            0,
            SIG_TRIGGER_BROADCAST );

        FUNCTION_EXIT;
    }


    void BroadcastManager::broadcastEventTriggeredStationDvaInternal(
        const std::string& broadcastId, unsigned long messageKey, const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones,
        unsigned long ulCyclicTimes, unsigned long ulDelaySeconds, TRIGGER_EVENT_TYPE eEventType )
    //raises (BadParameterException, TA_Base_Core::AccessDeniedException, BroadcastLimitReached)
    {
        FUNCTION_ENTRY( "broadcastEventTriggeredStationDvaInternal" );

        // Get current time and plus by delay time
        time_t tmStartTime = time(0) + ulDelaySeconds;

        // Change corba zones to zone list
        Keys vecZoneKeys;
        PasHelpers::getInstance()->coverageToZoneKeys( zones, vecZoneKeys );

        if ( SIG_TRIGGER_BROADCAST == eEventType )
        {
            m_pTriggeredBroadcastWorker->addSigTriggeredBroadcast( tmStartTime, broadcastId, messageKey, vecZoneKeys, ulCyclicTimes );
        }
        else if ( FAS_TRIGGER_BROADCAST == eEventType )
        {
            m_pTriggeredBroadcastWorker->executeFasTriggeredBroadcast( broadcastId, messageKey, vecZoneKeys, ulCyclicTimes );
        }
        else
        {
        }

        FUNCTION_EXIT;
    }


    void BroadcastManager::broadcastEventTriggeredStationDvaForStation( CORBA::ULong messageKey,
                                                                        const char* messageVersionNumber,
                                                                        CORBA::Boolean hasChime,
                                                                        CORBA::Boolean isSynchronisedWithTis,
                                                                        CORBA::ULong durationInSecs,
                                                                        CORBA::UShort periodInSecs,
                                                                        const char* sessionId )
    //raises (BadParameterException, TA_Base_Core::AccessDeniedException, BroadcastLimitReached)
    {
        FUNCTION_ENTRY( "broadcastEventTriggeredStationDvaForStation" );

        LOGOUT("broadcastEventTriggeredStationDvaForStation");

        ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();

        std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationDva);

        TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;

        // Find all valid PA Zones for this location
        TA_IRS_Bus::PaZoneRecords records =
            TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneRecordsFromLocationKey(CachedConfig::getInstance()->getAgentLocationKey());

        zones.length(records.size());

        TA_IRS_Bus::PaZoneRecordsIt it = records.begin();
        int index=0;
        int removedVirtualZones=0;
        for (; it != records.end(); ++it, ++index)
        {
            if ( (*it).m_stnEquipmentEntityName.empty() &&
                 (*it).m_occEquipmentEntityName.empty() )
            {
                // These zones are ignored because they are for paging console outputs.
                ++removedVirtualZones;
                continue;
            }
            zones[index] = (*it).m_pKey;
        }

        // Resize if zones removed;
        if (removedVirtualZones)
        {
            zones.length(records.size()-removedVirtualZones);
        }

        PasHelpers::getInstance()->submitPaBroadcastEventTriggeredStationDvaForStationRequestAuditMessage(
            broadcastId,
            messageKey,
            messageVersionNumber,
            hasChime,
            isSynchronisedWithTis,
            durationInSecs,
            periodInSecs,
            CachedConfig::getInstance()->getAgentLocationKey(),
            sessionId );

        broadcastEventTriggeredStationDvaInternal(
            broadcastId,
            messageKey,
            zones,
            0,
            0,
            SIG_TRIGGER_BROADCAST );

        FUNCTION_EXIT;
    }


    void BroadcastManager::broadcastEventTriggeredStationDvaForArea( CORBA::ULong messageKey,
                                                                     const char* messageVersionNumber,
                                                                     CORBA::Boolean hasChime,
                                                                     CORBA::Boolean isSynchronisedWithTis,
                                                                     CORBA::ULong durationInSecs,
                                                                     CORBA::UShort periodInSecs,
                                                                     const char* sessionId,
                                                                     const char* zoneArea )
    //raises (BadParameterException, TA_Base_Core::AccessDeniedException, BroadcastLimitReached)
    {
        FUNCTION_ENTRY( "broadcastEventTriggeredStationDvaForArea" );

        LOGOUT("broadcastEventTriggeredStationDvaForArea");

        ThreadGuard guard( m_lock );

		TA_Base_Bus::GenericAgent::ensureControlMode();
        std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationDva);

        TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;

        // Find all valid PA Zones for the given area
        TA_IRS_Bus::PaZoneRecords records = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneRecordsFromArea(zoneArea);

        zones.length(records.size());

        TA_IRS_Bus::PaZoneRecordsIt it = records.begin();
        int index=0;
        for (; it != records.end(); ++it, ++index)
        {
            zones[index] = (*it).m_pKey;
        }

        PasHelpers::getInstance()->submitPaBroadcastEventTriggeredStationDvaForAreaRequestAuditMessage(
            broadcastId,
            messageKey,
            messageVersionNumber,
            hasChime,
            isSynchronisedWithTis,
            durationInSecs,
            periodInSecs,
            zoneArea,
            sessionId );

        broadcastEventTriggeredStationDvaInternal(
            broadcastId,
            messageKey,
            zones,
            0,
            0,
            SIG_TRIGGER_BROADCAST );

        FUNCTION_EXIT;
    }


    void BroadcastManager::broadcastEventTriggeredStationDvaForGroup( CORBA::ULong messageKey,
                                                                      const char* messageVersionNumber,
                                                                      CORBA::Boolean hasChime,
                                                                      CORBA::Boolean isSynchronisedWithTis,
                                                                      CORBA::ULong durationInSecs,
                                                                      CORBA::UShort periodInSecs,
                                                                      const char* sessionId,
                                                                      const char* zoneGroupLabel )
    //raises (BadParameterException, TA_Base_Core::AccessDeniedException, BroadcastLimitReached)
    {
        FUNCTION_ENTRY( "broadcastEventTriggeredStationDvaForGroup" );

        LOGOUT("broadcastEventTriggeredStationDvaForGroup");

        ThreadGuard guard( m_lock );

        TA_Base_Bus::GenericAgent::ensureControlMode();

        std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationDva);

        TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;

        // Find all PA Zones for the given group
        try
        {
            std::list<unsigned long> zoneKeys =
                TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneGroupCachedMap().getZonesAssociatedWithGroup(
                    CachedConfig::getInstance()->getAgentLocationKey(),
                    zoneGroupLabel );

            zones.length(zoneKeys.size());

            std::list<unsigned long>::iterator it = zoneKeys.begin();
            int index=0;
            for (; it != zoneKeys.end(); ++it, ++index)
            {
                zones[index] = *it;
            }
        }
        catch (...)
        {
            throw BadParameterException("Event triggered Station broadcast failed - zone group details could not be retrieved");
        }

        PasHelpers::getInstance()->submitPaBroadcastEventTriggeredStationDvaForGroupRequestAuditMessage(
            broadcastId,
            messageKey,
            messageVersionNumber,
            hasChime,
            isSynchronisedWithTis,
            durationInSecs,
            periodInSecs,
            zoneGroupLabel,
            sessionId );

        broadcastEventTriggeredStationDvaInternal(
            broadcastId,
            messageKey,
            zones,
            0,
            0,
            SIG_TRIGGER_BROADCAST );

        FUNCTION_EXIT;
    }


    void BroadcastManager::throwAccessDeniedExceptionIfCannotPerformAction(unsigned long action, const std::string& sessionId, const std::string& broadcastId)
        //throw(TA_Base_Core::AccessDeniedException)
    {
        FUNCTION_ENTRY( "throwAccessDeniedExceptionIfCannotPerformAction" );

        if (!isRightsCheckingAvailable())
        {
            PasHelpers::getInstance()->
                throwAccessDeniedException("Failed to determine rights for this action", broadcastId);
        }

        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;
        bool canPerformAction = false;

        try
        {
            canPerformAction = m_rightsLibrary->isActionPermittedOnResource(
                sessionId,
                m_resourceId,
                action,
                reason,
                decisionModule );
        }
        catch( ... )
        {
            PasHelpers::getInstance()->
                throwAccessDeniedException("Failed to determine rights for this action", broadcastId);
        }

        if (!canPerformAction)
        {
            PasHelpers::getInstance()->
                throwAccessDeniedException("Action is not permitted", broadcastId);
        }

        FUNCTION_EXIT;
    }


    TA_Base_Bus::IPAAgentCorbaDef::CompleteBroadcastStateSeq* BroadcastManager::getCompleteBroadcastStateSeqCopy()
    {
        FUNCTION_ENTRY( "getCompleteBroadcastStateSeqCopy" );

		TA_Base_Bus::GenericAgent::ensureControlMode();
        TA_Base_Bus::IPAAgentCorbaDef::CompleteBroadcastStateSeq* allBroadcasts =
            new TA_Base_Bus::IPAAgentCorbaDef::CompleteBroadcastStateSeq;

        TA_Base_Bus::IPAAgentCorbaDef::BroadcastIdTypeSeq broadcastIdTypeSeq;
        m_allBroadcasts->getAllBroadcastIdTypes(broadcastIdTypeSeq);

        allBroadcasts->length(broadcastIdTypeSeq.length());
        int numberOfsomehowRemovedBroadcasts = 0;
        for (int index = 0; index < broadcastIdTypeSeq.length(); ++index)
        {
            AbstractBroadcast* broadcast = m_allBroadcasts->getBroadcast(static_cast<const char*>(broadcastIdTypeSeq[index].broadcastId));

            if ( broadcast == NULL)
            {
                ++numberOfsomehowRemovedBroadcasts;
                continue;
            }

            TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* broadcastConfig =
                broadcast->getBroadcastConfigCopy();

            TA_Base_Bus::IPAAgentCorbaDef::ProgressReport*  progressReport =
                broadcast->getProgressReportCopy();

            TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameterSeq*  broadcastParameters =
                broadcast->getBroadcastParametersCopy();

            (*allBroadcasts)[index].broadcastId =
                broadcast->getBroadcastId().c_str();
            (*allBroadcasts)[index].theBroadcastConfig = *broadcastConfig;
            (*allBroadcasts)[index].theProgressReport = *progressReport;
            (*allBroadcasts)[index].broadcastParameters = *broadcastParameters;

            delete broadcastConfig;
            delete progressReport;
            delete broadcastParameters;
        }

        // resize for lost broadcasts

        allBroadcasts->length(broadcastIdTypeSeq.length() - numberOfsomehowRemovedBroadcasts);

        FUNCTION_EXIT;
        return allBroadcasts;
    }


    void BroadcastManager::processPAAgentNewBroadcastUpdate( TA_Base_Bus::IPAAgentCorbaDef::BroadcastConfig* updateData )
        // throw CorruptInternalStateDataException
    {
        FUNCTION_ENTRY( "processPAAgentNewBroadcastUpdate" );

        if ( NULL == updateData )
        {
            TA_THROW(CorruptInternalStateDataException("updateData is NULL"));
        }

        if ( updateData->broadcastType == TA_Base_Bus::IPAAgentCorbaDef::StationDva)
        {
            if ( 0 != m_pScheduleBroadcastWorker )
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Creating schedule broadcast: %d.", updateData->theBroadcastConfigSpecifics.theStationDvaConfig().pKey );
                m_pScheduleBroadcastWorker->addNewScheduleBroadcast( *updateData );
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error null pointer of ScheduleBroadcastWorker" );
            }
        }
        else
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
                "NewBroadcastUpdate message processing failed: Unknown broadcastType" );

            return;
        }

        FUNCTION_EXIT;
    }


    void BroadcastManager::processPAAgentNewBroadcastUpdate( TA_Base_Bus::IPAAgentCorbaDef::TriggeredBroadcastConfig* updateData )
        // throw CorruptInternalStateDataException
    {
        FUNCTION_ENTRY( "processPAAgentNewBroadcastUpdate" );

        if ( NULL == updateData )
        {
            TA_THROW(CorruptInternalStateDataException("updateData is NULL"));
        }

        Keys lstZoneKeys;
        PasHelpers::getInstance()->coverageToZoneKeys( updateData->lstZoneKeys, lstZoneKeys );
        std::string strStdBroadcastId = updateData->strBroadcastId._ptr;

        if ( 0 != m_pTriggeredBroadcastWorker )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Sync sig triggered broadcast:: %s.", strStdBroadcastId.c_str() );
            m_pTriggeredBroadcastWorker->addSigTriggeredBroadcast( updateData->tmStartTime, 
                strStdBroadcastId, updateData->ulDvaMsgKey, lstZoneKeys, updateData->ulCyclicTimes );
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error null pointer of TriggeredBroadcastWorker" );
        }

        FUNCTION_EXIT;
    }


    void BroadcastManager::processPAAgentRemovedBroadcastUpdate( unsigned long ulScheduleKey )
        // throw CorruptInternalStateDataException
    {
        FUNCTION_ENTRY( "processPAAgentRemovedBroadcastUpdate" );

        if ( 0 != ulScheduleKey )
        {
            if ( 0 != m_pScheduleBroadcastWorker )
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Removing schedule broadcast: %d.", ulScheduleKey );
                m_pScheduleBroadcastWorker->delScheduleBroadcast( ulScheduleKey, false );
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error null pointer of ScheduleBroadcastWorker" );
            }
        }
        else
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
                "RemovedBroadcastUpdate message processing failed: BroadcastId=%d unknown", ulScheduleKey  );
        }

        FUNCTION_EXIT;
    }


    void BroadcastManager::processPAAgentRenamedBroadcastUpdate( const TA_Base_Bus::IPAAgentCorbaDef::CurrentBroadcastsUpdateData* updateData )
        // throw CorruptInternalStateDataException
    {
        FUNCTION_ENTRY( "processPAAgentRenamedBroadcastUpdate" );

        if ( !updateData )
        {
            TA_THROW(CorruptInternalStateDataException("updateData is NULL"));
        }

        FUNCTION_EXIT;
    }


    void BroadcastManager::processPAAgentBroadcastParameterUpdate( const TA_Base_Bus::IPAAgentCorbaDef::BroadcastParameter* updateData )
        // throw CorruptInternalStateDataException
    {
        FUNCTION_ENTRY( "processPAAgentBroadcastParameterUpdate" );

        if ( !updateData )
        {
            TA_THROW(CorruptInternalStateDataException("updateData is NULL"));
        }

        AbstractBroadcast* broadcast = m_allBroadcasts->getBroadcast(static_cast<const char*>(updateData->broadcastId));
        if (broadcast)
        {
            // Logging included
            broadcast->setBroadcastParameter(*updateData);
        }
        else
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
                "BroadcastParameterUpdate message processing failed: BroadcastId=%s unknown", static_cast<const char*>(updateData->broadcastId) );
        }

        FUNCTION_EXIT;
    }


    void BroadcastManager::processPAAgentBroadcastProgressUpdate( const TA_Base_Bus::IPAAgentCorbaDef::ProgressReport* updateData )
        // throw CorruptInternalStateDataException
    {
        FUNCTION_ENTRY( "processPAAgentBroadcastProgressUpdate" );

        if ( !updateData )
        {
            TA_THROW(CorruptInternalStateDataException("updateData is NULL"));
        }

        AbstractBroadcast* broadcast = m_allBroadcasts->getBroadcast(static_cast<const char*>(updateData->broadcastId));
        if (broadcast)
        {
            broadcast->setProgressReport(*updateData);
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "BroadcastProgressUpdate message processed: BroadcastId=%s", static_cast<const char*>(updateData->broadcastId) );

        }
        else
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn,
                "BroadcastProgressUpdate message processing failed: BroadcastId=%s unknown", static_cast<const char*>(updateData->broadcastId) );
        }

        FUNCTION_EXIT;
    }


    void BroadcastManager::processPAAgentEntireAgentStateUpdate( TA_Base_Bus::IPAAgentCorbaDef::CompleteBroadcastStateSeq* updateData )
        // throw CorruptInternalStateDataException
    {
        FUNCTION_ENTRY( "processPAAgentEntireAgentStateUpdate" );

        if ( !updateData )
        {
            TA_THROW(CorruptInternalStateDataException("updateData is NULL"));
        }

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            "Processing EntireAgentStateUpdate message" );


        for (int index = 0; index < updateData->length(); ++index)
        {
            processPAAgentNewBroadcastUpdate( &((*updateData)[index].theBroadcastConfig));
            processPAAgentBroadcastProgressUpdate( &((*updateData)[index].theProgressReport));

            for (int parameterIndex = 0; parameterIndex < (*updateData)[index].broadcastParameters.length(); ++parameterIndex)
            {
                processPAAgentBroadcastParameterUpdate( &((*updateData)[index].broadcastParameters[parameterIndex]));
            }
        }

        FUNCTION_EXIT;
    }


    bool BroadcastManager::isRightsCheckingAvailable()
    {
        FUNCTION_ENTRY( "isRightsCheckingAvailable" );

        if (m_rightsLibrary)
        {
            FUNCTION_EXIT;
            return true;
        }

        TA_Base_Bus::RightsLibraryFactory rightsLibraryFactory;
        IResource* resource = NULL;

        try
        {
            if ( TA_Base_Core::RunParams::getInstance().isSet("DebugAlwaysTrueRightsLib") )
            {
                m_rightsLibrary = rightsLibraryFactory.buildRightsLibrary(TA_Base_Bus::ALWAYS_TRUE_RL);
            }
            else
            {
                m_rightsLibrary = rightsLibraryFactory.buildRightsLibrary();
            }

            // We must figure out the entity key as we do not yet have access to the ConfigurationEditorGUI
            // object to retrieve the key.

            resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(
                CachedConfig::getInstance()->getAgentKey() );
            m_resourceId = resource->getKey();

            delete resource;
            resource = NULL;
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database Error", "Could not initialise ready to check rights.");
            if (m_rightsLibrary != NULL)
            {
                delete m_rightsLibrary;
                m_rightsLibrary = NULL;
            }
            if (resource != NULL)
            {
                delete resource;
                resource = NULL;
            }

            FUNCTION_EXIT;
            return false;
        }

        FUNCTION_EXIT;
        return true;
    }

	void BroadcastManager::setModeChangingFlag(bool flag)
	{
        if ( 0 != m_broadcastWorker )
        {
		    m_broadcastWorker->setModeChangingFlag(flag);
        }
	}

	//wuzhongyi CL-19136
	void BroadcastManager::forceTerminateAllExistPA()
	{
		FUNCTION_ENTRY("forceTerminateAllExistPA");
		TA_Base_Bus::GenericAgent::ensureControlMode();
		try
		{
		}
		catch(TA_Base_Core::PasWriteErrorException& err)
		{
			TA_THROW(err);
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown exception when forceTerminateAllExistPA()");			
			throw;
		}
		
		FUNCTION_EXIT;
	}

    //begin-added by lixiaoxia
	void BroadcastManager::enableZoneMonitoring( CORBA::Boolean bEnable, const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const char* sessionId )
	{
		FUNCTION_ENTRY("enableZoneMonitoring");

		LOGOUT("enableZoneMonitoring");
		ThreadGuard guard( m_lock );
		TA_Base_Bus::GenericAgent::ensureControlMode();

		std::string extraInformation = "";
		std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::ZoneMonitoring, extraInformation);

		if(bEnable)
		{
			// this will validate the parameters like the zones and session id
			// as part of the audit message submission
			PasHelpers::getInstance()->submitPaBroadcastMonitoringRequestAuditMessage( zones, sessionId );

		    ZoneMonitoringStart newZoneMonitoringStart(broadcastId);
			try
			{
				// update parameters
				newZoneMonitoringStart.updatePriorityBySessionId( sessionId );

				Keys lstZoneKeys;
				PasHelpers::getInstance()->coverageToZoneKeys(zones, lstZoneKeys);
				newZoneMonitoringStart.updateStationZoneIdsByZoneKeys(lstZoneKeys);

				if ( newZoneMonitoringStart.isParameterInvalid() )
				{
					std::string strBadParams = newZoneMonitoringStart.getInvalidParameter();

					throw (TA_Base_Core::BadParameterException(strBadParams.c_str()));
				}
            }
            catch( TA_Base_Core::BadParameterException& expWhat )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::BadParameterException", expWhat.reason.in() );
                throw;
            }
			catch(...)
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
				throw;
			}

			try
			{
				PASConnection::getInstance()->writeToHardwareImmediately( &newZoneMonitoringStart );
			}
			catch (...)
			{
				// Throw to client
				throw;
			}
			

		}
		else
		{
			// this will validate the parameters like session id
			// as part of the audit message submission
			PasHelpers::getInstance()->submitPaStopBroadcastMonitoringRequestAuditMessage( sessionId );

		    ZoneMonitoringStop newZoneMonitoringStop( broadcastId );
			try
			{
				// update parameters
				newZoneMonitoringStop.updatePriorityBySessionId( sessionId );

				if ( newZoneMonitoringStop.isParameterInvalid() )
				{
					std::string strBadParams = newZoneMonitoringStop.getInvalidParameter();

					throw (TA_Base_Core::BadParameterException(strBadParams.c_str()));
				}
			}
			catch( TA_Base_Core::BadParameterException& expWhat )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::BadParameterException", expWhat.reason.in() );
				throw;
			}
			catch(...)
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
				throw;
			}

			try
			{
				PASConnection::getInstance()->writeToHardwareImmediately( &newZoneMonitoringStop );
			}
			catch (...)
			{
				// Throw to client
				throw;
			}
		}

		FUNCTION_EXIT;
	}
	//end-added by lixiaoxia
    
    void BroadcastManager::stopBroadcastStationDva( const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const char* sessionId )
    {
		FUNCTION_ENTRY("stopBroadcastStationDva");

        LOGOUT("stopBroadcastStationDva");
        ThreadGuard guard( m_lock );
        TA_Base_Bus::GenericAgent::ensureControlMode();

        std::string extraInformation = "";
        std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationDva, extraInformation);

		// this will validate the parameters like the zones and session id
		// as part of the audit message submission

		PasHelpers::getInstance()->submitPaStopBroadcastStationDvaRequestAuditMessage( zones, sessionId );

        DVABroadcastStop newBroadcast( broadcastId );

        try
        {
            // update parameters
            newBroadcast.updatePriorityBySessionId( sessionId );

            Keys lstZoneKeys;
            PasHelpers::getInstance()->coverageToZoneKeys( zones, lstZoneKeys );
            newBroadcast.updateStationZoneIdsByZoneKeys( lstZoneKeys );

            if ( newBroadcast.isParameterInvalid() )
            {
                std::string strBadParams = newBroadcast.getInvalidParameter();

                throw (TA_Base_Core::BadParameterException(strBadParams.c_str()));
            }
        }
        catch( TA_Base_Core::BadParameterException& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::BadParameterException", expWhat.reason.in() );
            throw;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );
            throw;
        }

        try
        {
            PASConnection::getInstance()->writeToHardwareImmediately( &newBroadcast );
        }
        catch (...)
        {
            // Throw to client
            throw;
        }

		FUNCTION_EXIT;
    }
    
    void BroadcastManager::stopBroadcastStationLive( const TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage& zones, const char* sessionId )
    {
		FUNCTION_ENTRY("stopBroadcastStationLive");

        LOGOUT("stopBroadcastStationLive");
        ThreadGuard guard( m_lock );
		TA_Base_Bus::GenericAgent::ensureControlMode();

        // this is to build a broadcast ID with more information
        std::string extraInformation = "";
        std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationLive, extraInformation);

		// this will validate the parameters like the zones and messages
		// as part of the audit message submission
		PasHelpers::getInstance()->submitPaStopBroadcastStationLiveRequestAuditMessage(	zones, sessionId );

        LiveBroadcastStop newBroadcast( broadcastId );

        try
        {
            // update parameters
            newBroadcast.updatePriorityBySessionId( sessionId );

            Keys lstZoneKeys;
            PasHelpers::getInstance()->coverageToZoneKeys( zones, lstZoneKeys );
            newBroadcast.updateStationZoneIdsByZoneKeys( lstZoneKeys );

            if ( newBroadcast.isParameterInvalid() )
            {
                std::string strBadParams = newBroadcast.getInvalidParameter();

                throw (TA_Base_Core::BadParameterException(strBadParams.c_str()));
            }
        }
        catch( TA_Base_Core::BadParameterException& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::BadParameterException", expWhat.reason.in() );
            throw;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

            // Pass on to the client
            throw;
        }

        try
        {
            PASConnection::getInstance()->writeToHardwareImmediately( &newBroadcast );
        }
        catch (...)
        {
            // Throw to client
            throw;
        }

		FUNCTION_EXIT;
    }
    
    char* BroadcastManager::playbackStationDva( CORBA::ULong messageKey, ::CORBA::UShort sourceId, const char* sessionId )
    {
		FUNCTION_ENTRY("startDVAMessagePlayback");

        LOGOUT("startDVAMessagePlayback");
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "startDVAMessagePlayback" );
        ThreadGuard guard( m_lock );
		TA_Base_Bus::GenericAgent::ensureControlMode();

        std::string extraInformation = "";
        std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::OccPlayback, extraInformation);

        DvaPlaybackStart newBroadcast( broadcastId );

        try
        {
            // update parameters
            newBroadcast.updatePriorityBySessionId( sessionId );

            newBroadcast.updateDVAMsgIdByDVAMsgKey( messageKey );

            if ( newBroadcast.isParameterInvalid() )
            {
                std::string strBadParams = newBroadcast.getInvalidParameter();

                throw (TA_Base_Core::BadParameterException(strBadParams.c_str()));
            }
        }
        catch( TA_Base_Core::BadParameterException& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::BadParameterException", expWhat.reason.in() );
            throw;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown reason" );

            throw (TA_Base_Core::BadParameterException("Unknown exception"));;
        }

        try
        {
            PASConnection::getInstance()->writeToHardwareImmediately( &newBroadcast );
        }
        catch (...)
        {
            // Throw to client
            throw;
        }

		FUNCTION_EXIT;
        return CORBA::string_dup(broadcastId.c_str());
    }
    
    // Not support by hardware yet
    void BroadcastManager::stopPlaybackStationDva( const char* sessionId )
    {
		FUNCTION_ENTRY("stopPlaybackStationDva");

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "stopPlaybackStationDva" );
        ThreadGuard guard( m_lock );
		TA_Base_Bus::GenericAgent::ensureControlMode();

		FUNCTION_EXIT;
    }

    TA_Base_Bus::IPAAgentCorbaDef::EBackupKeyState BroadcastManager::getBackupKeyStatus( const char* backupName )
    {
		FUNCTION_ENTRY("getBackupKeyStatus");

        LOGOUT("getBackupKeyStatus");
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getBackupKeyStatus" );
        ThreadGuard guard( m_lock );
		TA_Base_Bus::GenericAgent::ensureControlMode();

        TA_Base_Bus::IPAAgentCorbaDef::EBackupKeyState eBackupMode = TA_Base_Bus::IPAAgentCorbaDef::BackupUnlock;

		FUNCTION_EXIT;
        return eBackupMode;
    }

    void BroadcastManager::broadcastSigTriggeredStationDva( CORBA::ULong ulEventType, CORBA::ULong ulSrcStationLocationKey, 
        CORBA::ULong ulDestStationLocationKey, CORBA::ULong ulDestPlatformId, CORBA::Boolean bIsLastTrain, 
        CORBA::Boolean bTrainInfoValid, TA_Base_Core::DateTime dtStartTime )
    {
        FUNCTION_ENTRY("broadcastSigTriggeredStationDva");

        LOGOUT("broadcastSigTriggeredStationDva");
        ThreadGuard guard( m_lock );
        TA_Base_Bus::GenericAgent::ensureControlMode();

        unsigned long ulValidDestStationKey = 0u;        // Unallocated if invalid
        bool bValidIsLastTrain = false;       // Not the last train if invalid

        if ( bTrainInfoValid )
        {
            ulValidDestStationKey = ulDestStationLocationKey;
            bValidIsLastTrain = bIsLastTrain; 
        }

        std::stringstream msg;
        msg << "Processing Sig info"
            << " EventType=" << ulEventType
            << " StationId=" << ulSrcStationLocationKey
            << " PlatformId=" << ulDestPlatformId
            << " Valid=" << bTrainInfoValid
            << " DestinationStationId=" << ulDestStationLocationKey
            << " LastTrain=" << bIsLastTrain;

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, msg.str().c_str());

        if (!bTrainInfoValid)
        {
            msg << "Sig info is not valid"
                << " EffectiveDestinationStationId=" << ulValidDestStationKey
                << " EffectiveLastTrain=" << bValidIsLastTrain;

            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugWarn, msg.str().c_str());
        }

        TA_IRS_Bus::PaAtsTriggeredBroadcastRecord record;
        record.m_pKey = TA_Base_Core::DatabaseKey::getInvalidKey();
        try
        {
            record = TA_IRS_Bus::CachedMaps::getInstance()->getAtsTriggeredBroadcastRecordFromDetail(
                ulEventType, ulSrcStationLocationKey, ulValidDestStationKey, ulDestPlatformId, bValidIsLastTrain );
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown exception" );
        }

        if (record.m_pKey == TA_Base_Core::DatabaseKey::getInvalidKey())
        {
            // Not found - ignore
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
                "Event triggered broadcast not configured - ignored");
            return;
        }

        if (!record.m_isEnabled)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
                "Event triggered broadcast has been disabled - ignored");
            return;
        }

        TA_IRS_Bus::PaZoneGroupRecord groupRecord;

        try
        {
            groupRecord = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneGroupRecordFromKey(record.m_paZoneGroupKey);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException& )
        {
            // log the error
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                "Event triggered Station broadcast failed - invalid zone group key %d",
                record.m_paZoneGroupKey );
            return;
        }

        // Verify validity of zone group
        if ( !groupRecord.m_isEventGroup )
        {
            // log the error
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                "Event triggered Station broadcast failed - invalid zone group key %d",
                record.m_paZoneGroupKey );
            return;
        }

        std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationDva);

        TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;
        // Find all PA Zones for the given group
        try
        {
            std::list<unsigned long> zoneKeys =
                TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneGroupCachedMap().getZonesAssociatedWithGroup(
                CachedConfig::getInstance()->getAgentLocationKey(),
                groupRecord.m_label );

            zones.length(zoneKeys.size());

            std::list<unsigned long>::iterator it = zoneKeys.begin();
            int index=0;
            for (; it != zoneKeys.end(); ++it, ++index)
            {
                zones[index] = *it;
            }
        }
        catch (...)
        {
            throw BadParameterException("Event triggered Station broadcast failed - zone group details could not be retrieved");
        }

        const std::string sessonId = "Sig Triggered broadcast session";
        PasHelpers::getInstance()->submitPaBroadcastEventTriggeredStationDvaForGroupRequestAuditMessage(
            broadcastId, record.m_paDvaMessageKey, "0", false, false, 0, 0, groupRecord.m_label, sessonId );

        broadcastEventTriggeredStationDvaInternal( broadcastId, record.m_paDvaMessageKey, zones, 0, 0, SIG_TRIGGER_BROADCAST );

        FUNCTION_EXIT;
    }

    void BroadcastManager::broadcastFasTriggeredStationDva( CORBA::ULong ulAlarmEntityKey, CORBA::ULong ulSrcStationLocationKey )
    {
        FUNCTION_ENTRY("broadcastFasTriggeredStationDva");

        LOGOUT("broadcastFasTriggeredStationDva");
        ThreadGuard guard( m_lock );
        TA_Base_Bus::GenericAgent::ensureControlMode();

        std::stringstream msg;
        msg << "Processing Fas info" << " AlarmEntityKey=" << ulAlarmEntityKey << " StationId=" << ulSrcStationLocationKey;
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, msg.str().c_str());

        TA_IRS_Bus::PaFasTriggeredBroadcastRecord record;
        record.m_pKey = TA_Base_Core::DatabaseKey::getInvalidKey();
        try
        {
            record = TA_IRS_Bus::CachedMaps::getInstance()->getFasTriggeredBroadcastRecordFromDetail( ulAlarmEntityKey );
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown exception", "Unknown exception" );
        }

        if (record.m_pKey == TA_Base_Core::DatabaseKey::getInvalidKey())
        {
            // Not found - ignore
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
                "Event triggered broadcast not configured - ignored");
            return;
        }

        if (!record.m_isEnabled)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog,  TA_Base_Core::DebugUtil::DebugInfo, 
                "Event triggered broadcast has been disabled - ignored");
            return;
        }

        TA_IRS_Bus::PaZoneGroupRecord groupRecord;
        try
        {
            groupRecord = TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneGroupRecordFromKey(record.m_paZoneGroupKey);
        }
        catch(TA_Base_Core::CachedMappingNotFoundException& )
        {
            // log the error
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                "Event triggered Station broadcast failed - invalid zone group key %d",
                record.m_paZoneGroupKey );
            return;
        }

        // Verify validity of zone group
        if ( !groupRecord.m_isEventGroup )
        {
            // log the error
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                "Event triggered Station broadcast failed - invalid zone group key %d",
                record.m_paZoneGroupKey );
            return;
        }

        std::string broadcastId = getNewBroadcastId(TA_Base_Bus::IPAAgentCorbaDef::StationDva);

        TA_Base_Bus::IPAAgentCorbaDef::ZoneCoverage zones;
        // Find all PA Zones for the given group
        try
        {
            std::list<unsigned long> zoneKeys =
                TA_IRS_Bus::CachedMaps::getInstance()->getPaZoneGroupCachedMap().getZonesAssociatedWithGroup(
                CachedConfig::getInstance()->getAgentLocationKey(),
                groupRecord.m_label );

            zones.length(zoneKeys.size());

            std::list<unsigned long>::iterator it = zoneKeys.begin();
            int index=0;
            for (; it != zoneKeys.end(); ++it, ++index)
            {
                zones[index] = *it;
            }
        }
        catch (...)
        {
            throw BadParameterException("Event triggered Station broadcast failed - zone group details could not be retrieved");
        }

        const std::string sessonId = "Fas Triggered broadcast session";
        PasHelpers::getInstance()->submitPaBroadcastEventTriggeredStationDvaForGroupRequestAuditMessage(
            broadcastId, record.m_paDvaMessageKey, "0", false, false, 0, 0, groupRecord.m_label, sessonId );

        broadcastEventTriggeredStationDvaInternal( broadcastId, record.m_paDvaMessageKey, zones, record.m_paCyclicTimes, 0, FAS_TRIGGER_BROADCAST );

        FUNCTION_EXIT;
    }

    void BroadcastManager::removeScheduleBroadcast( CORBA::ULong ulScheduleKey, const char* sessionId )
    {
        FUNCTION_ENTRY("removeScheduleBroadcast");

        LOGOUT("removeScheduleBroadcast");
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "removeScheduleBroadcast" );
        ThreadGuard guard( m_lock );
        TA_Base_Bus::GenericAgent::ensureControlMode();

        if ( 0 != m_pScheduleBroadcastWorker )
        {
            m_pScheduleBroadcastWorker->delScheduleBroadcast( ulScheduleKey, false, true );
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "null pointer of ScheduleBroadcastWorker" );
        }

        FUNCTION_EXIT;
    }

    void BroadcastManager::restoreBroadcast( const char* broadcastId, const char* sessionId )
    {
        FUNCTION_ENTRY( "removeScheduleBroadcast" );

        FUNCTION_EXIT;
    }

} // namespace TA_IRS_App

