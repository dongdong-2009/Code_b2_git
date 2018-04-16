/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/TTISManager.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Manages all Train Traveller Information System-related functionality
  *
  * Implements the ITTISManagerCorbaDef CORBA interface
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif // _MSC_VER


#include "TTISManager.h"

#include "app/signs/tis_agent/src/AgentModeMonitor.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "app/signs/tis_agent/src/StringHelper.h"
#include "app/signs/tis_agent/src/TISMessageDataDefinitionsHelper.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/trains/TrainAgentCorba/idl/src/ITrainExceptionsCorba.h" // CL-21314
#include "bus/trains/TrainCommonLibrary/src/TrainAccessDeniedException.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"
#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/trains/TrainCorbaProxy/src/TrainAgentProxyFactory.h"
#include "bus/generic_agent/src/GenericAgent.h"

#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/TTISEntityData.h"
#include "core/data_access_interface/entity_access/src/TrainAgentEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryTable.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/TISAlarms_MessageTypes.h"
#include "core/message/types/TISAudit_MessageTypes.h"
#include "core/message/types/TISBroadcastComms_MessageTypes.h"
#include "core/message/types/TISComms_MessageTypes.h"
#include "core/message/types/TISStateUpdate_MessageTypes.h"
#include "core/naming/src/NamedObject.h"
#include "core/utilities/src/RunParams.h"
#include "core/threads/src/ThreadMemberFunction.h"
#include "MessageLibraryParser.h"
#include "StringConverter.h"
#include "StringHelper.h"

#include <ace/OS.h>

#include <iomanip>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <boost/foreach.hpp>


using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::MessageSubscriptionManager;
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::LocationAccessFactory;
using TA_Base_Core::ILocation;
using TA_Base_Bus::TISAgentAccessFactory;


#define PRIORITY_AUDIT_MESSAGE_PARAMETER_NAME "Priority"
#define STARTTIME_AUDIT_MESSAGE_PARAMETER_NAME "Start time"
#define ENDTIME_AUDIT_MESSAGE_PARAMETER_NAME "End time"
#define OVERRIDE_AUDIT_MESSAGE_PARAMETER_NAME "Override"
#define REPEAT_INTERVAL_AUDIT_MESSAGE_PARAMETER_NAME "Repeat interval"
#define DISPLAY_ATTRIBUTE_AUDIT_MESSAGE_PARAMETER_NAME "Display attribute"
#define EVENT_PARAMETER_TRAIN_ID "TrainID"

namespace TA_IRS_App
{

    using TA_Base_Core::DebugUtil;

    //
    // Initialise statics.
    //

    TA_Base_Core::ReEntrantThreadLockable TTISManager::m_singletonLock;
    TTISManager* TTISManager::m_theClassInstance = NULL;


    TTISManager::TTISManager()
        : m_trainDataLock(),
          m_trainDownloadsLock(),
          m_locationKey( 0 ),
          m_locationName(),
          m_entityName(),
          m_entityKey( 0 ),
          m_subsystemKey( 0 ),  // TD9178 ~ added
          m_trainDownloadList(),
          m_trainDataVersions(),
          m_trainAgentSubscriber( NULL ),
          m_auditMessageSender( NULL ), //TES831 Events not logged!
          m_tisMessageSender( NULL ),
		  m_sessionID("")
    {
        FUNCTION_ENTRY( "TTISManager" );

        m_tisMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::TISComms::Context );

        m_tisBroadcastMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::TISBroadcastComms::Context );

        m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::TISAudit::Context );

        m_tisStateUpdateMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getStateUpdateMessageSender( TA_Base_Core::TISStateUpdate::Context );

        FUNCTION_EXIT;
    }


    TTISManager::~TTISManager()
    {
        FUNCTION_ENTRY( "~TTISManager" );

        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
            static_cast< TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>* >( this ) );

        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
            static_cast< TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>* >( this ) );

        deactivateServant();

        delete m_trainAgentSubscriber;
        m_trainAgentSubscriber = NULL;

        delete m_tisMessageSender;
        m_tisMessageSender = NULL;

        delete m_tisBroadcastMessageSender;
        m_tisBroadcastMessageSender = NULL;

        delete m_auditMessageSender;
        m_auditMessageSender = NULL;

        delete m_tisStateUpdateMessageSender;
        m_tisStateUpdateMessageSender = NULL;

        FUNCTION_EXIT;
    }


    TTISManager* TTISManager::getInstance()
    {
        FUNCTION_ENTRY( "getInstance" );

        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        // If guard acquired then make sure the singleton is still NULL.

        if ( NULL == m_theClassInstance )
        {
            // Create the one & only object.

            m_theClassInstance = new TTISManager();
        }

        FUNCTION_EXIT;
        return m_theClassInstance;
    }


    void TTISManager::initialise( TA_Base_Core::IEntityDataPtr EntityData )
    {
        FUNCTION_ENTRY( "initialise" );

        TA_IRS_Bus::TrainAgentProxyFactory::getInstance().agentInitialise( EntityData );

        m_trainTisCorbaProxy = TA_IRS_Bus::TrainAgentProxyFactory::getInstance().createTrainTisCorbaProxy();

        m_entityName = EntityData->getName();
        m_locationKey = EntityData->getLocation();
        m_locationName = EntityData->getLocationName();
        m_entityKey = EntityData->getKey();
        m_subsystemKey = EntityData->getSubsystem();
        m_entityTypeKey = EntityData->getTypeKey();

        try
        {
            m_locationType = TA_IRS_Bus::LocationCache::getInstance().getLocationTypeByKey( m_locationKey );
        }
        catch ( ... )
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                 "Error retrieving location type from database" );

            // do a name based check
            if ( m_locationName == "OCC" )
            {
                m_locationType = TA_Base_Core::ILocation::OCC;
            }
            else
            {
                m_locationType = TA_Base_Core::ILocation::STATION;
            }
        }

        // this is the OCC if this is the depot agent, and the depot if the occ agent
        if ( TA_Base_Core::ILocation::OCC == m_locationType )
        {
			std::vector<ILocation*> depotLocations = LocationAccessFactory::getInstance().getLocationsByType(ILocation::DPT);
			BOOST_FOREACH(ILocation* i, depotLocations)
			{
				m_otherAgentLocationNames.push_back(i->getName());
				delete i;
			}
        }
        else if ( TA_Base_Core::ILocation::DPT == m_locationType )
        {
			std::vector<ILocation*> occLocations = LocationAccessFactory::getInstance().getLocationsByType(ILocation::OCC);
			BOOST_FOREACH(ILocation* i, occLocations)
			{
				m_otherAgentLocationNames.push_back(i->getName());
				delete i;
			}
        }

        m_trainAgentSubscriber = new TrainAgentSubscriber( *this );

        getInitialTrainDataVersions();

        getInitialTrainDownloads();

        getInitialTrainDataVersionAlarms();

        activateServantWithName( m_entityName );

        FUNCTION_EXIT;
    }


    bool TTISManager::isValid()
    {
        FUNCTION_ENTRY( "isValid" );
        FUNCTION_EXIT;
        return true;
    }


    void TTISManager::update( const TA_Base_Core::ConfigUpdateDetails& configUpdateDetails )
    {
        FUNCTION_ENTRY( "update" );
        FUNCTION_EXIT;
    }


    void TTISManager::remove()
    {
        FUNCTION_ENTRY( "remove" );
        FUNCTION_EXIT;
    }


    void TTISManager::start()
    {
        FUNCTION_ENTRY( "start" );
        FUNCTION_EXIT;
    }


    void TTISManager::stop()
    {
        FUNCTION_ENTRY( "stop" );
        FUNCTION_EXIT;
    }


    void TTISManager::registerForStateUpdates()
    {
        FUNCTION_ENTRY( "TTISManager::registerForStateUpdates" );

        subscribeToNotifications( false );

        FUNCTION_EXIT;
    }


    void TTISManager::setToControlMode()
    {
        FUNCTION_ENTRY( "setToControlMode" );

        if( m_entityName != "" )
        {
            activateServantWithName( m_entityName );
        }

        //limin++--, for failover
        typedef TA_Base_Core::ThreadedMemberFunction< TTISManager > TTISManagerMemberFunction;
        TTISManagerMemberFunction& thrd = TTISManagerMemberFunction::alloc( *this );
        thrd.add( &TTISManager::setToControlModeInThread );
        thrd.start();
        //limin++--, for failover

        FUNCTION_EXIT;
    }


    //limin, for failover
    void TTISManager::setToControlModeInThread()
    {
        FUNCTION_ENTRY( "TTISManager::setToControlModeInThread" );

        subscribeToNotifications( true );

        // simply recheck train data versions
        verifyAllTrainDataVersions();

        FUNCTION_EXIT;
    }


    void TTISManager::setToMonitorMode()
    {
        FUNCTION_ENTRY( "setToMonitorMode" );
        deactivateServant();

        subscribeToNotifications( false );

        FUNCTION_EXIT;
    }


    unsigned short TTISManager::getFlexibleTimeOutValue()
    {
        FUNCTION_ENTRY( "getFlexibleTimeOutValue" );
        FUNCTION_EXIT;
        return  static_cast< unsigned short >( m_trainTisCorbaProxy->getMaximumWaitTime() );
    }


    TA_Base_Core::TrainDownloadList* TTISManager::getCurrentTrainDownloads()
    {
        FUNCTION_ENTRY( "getCurrentTrainDownloads" );

        TA_Base_Bus::GenericAgent::ensureControlMode();
        TA_Base_Core::TrainDownloadList* trainDownloadSeq = new TA_Base_Core::TrainDownloadList();

        // Set the CORBA sequence's length before populating it
        trainDownloadSeq->length( m_trainDownloadList.size() );

        unsigned long i;
        std::map< unsigned char, TA_Base_Core::TrainDownload >::iterator iter;
        for( iter = m_trainDownloadList.begin(), i = 0;
                ( m_trainDownloadList.end() != iter ) && ( i < trainDownloadSeq->length() );
                ++i, ++iter )
        {
            ( *trainDownloadSeq )[i] = iter->second;
        }

        FUNCTION_EXIT;
        return trainDownloadSeq;
    }


    TA_Base_Core::TrainDataVersionList* TTISManager::getTrainDataVersions()
    {
        FUNCTION_ENTRY( "getTrainDataVersions" );

        TA_Base_Bus::GenericAgent::ensureControlMode();
        std::vector<TA_Base_Core::TrainDataVersion> dataVersions =
            this->m_trainDataVersions.getTrainDataVersions();
        TA_Base_Core::TrainDataVersionList* trainDataVersionSeq;

        // Set the CORBA sequence's length before populating it
        trainDataVersionSeq = new TA_Base_Core::TrainDataVersionList();

        trainDataVersionSeq->length( dataVersions.size() );

        unsigned long i;
        for( i = 0 ; i < dataVersions.size() ; i++ )
        {
            ( *trainDataVersionSeq )[i].trainNumber = dataVersions[i].trainNumber;
            ( *trainDataVersionSeq )[i].predefinedLibraryVersion = dataVersions[i].predefinedLibraryVersion;
            ( *trainDataVersionSeq )[i].nextPredefinedLibraryVersion = dataVersions[i].nextPredefinedLibraryVersion;
        }

        FUNCTION_EXIT;
        return trainDataVersionSeq;
    }


    TA_Base_Core::TrainDataVersionAlarmList* TTISManager::getTrainDataAlarms()
    {
        FUNCTION_ENTRY( "getTrainDataAlarms" );

        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        TA_Base_Core::TrainDataVersionAlarmList* alarmList = new TA_Base_Core::TrainDataVersionAlarmList();

        alarmList->length( m_trainDataVersionAlarmList.size() );

        unsigned long i = 0;
        for ( std::map< unsigned char, TA_Base_Core::TrainDataVersionAlarms >::iterator iter = m_trainDataVersionAlarmList.begin();
                m_trainDataVersionAlarmList.end() != iter; ++iter, ++i )
        {
            ( *alarmList )[i] = iter->second;
        }

        FUNCTION_EXIT;
        return alarmList;
    }


    void TTISManager::submitPredefinedDisplayRequest( const TA_Base_Core::TrainList& trains,
            const TA_Base_Core::TTISPredefinedMessageParameters& displayParameters,
            long timestamp,
            const char* sessionId )
    {
        FUNCTION_ENTRY( "submitPredefinedDisplayRequest" );

        TA_Base_Bus::GenericAgent::ensureControlMode();
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Received client request to send TTIS Predefined display request" );
        try
        {
            m_trainTisCorbaProxy->displayStoredMessage( convertTrainList( trains ),
                    convertDisplayParameters( displayParameters ),
                    timestamp,
                    sessionId );

        }
        catch( const TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure( "Error contacting train agent" );
        }
        catch( const TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Core::AccessDeniedException( e.what() );
        }
        catch( const TA_IRS_Bus::InvalidParameterException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidParameterException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( e.what() );
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure( e.what() );
        }

        FUNCTION_EXIT;
    }


    void TTISManager::submitAdHocDisplayRequest( const TA_Base_Core::TrainList& trains,
            const TA_Base_Core::TTISFreeTextMessageParameters& displayParameters,
            long timestamp,
            const char* sessionId )
    {
        FUNCTION_ENTRY( "submitAdHocDisplayRequest" );

        TA_Base_Bus::GenericAgent::ensureControlMode();
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Received client request to send TTIS Adhoc display request" );


        try
        {
            m_trainTisCorbaProxy->displayFreeTextMessage( convertTrainList( trains ),
                    convertDisplayParameters( displayParameters ),
                    timestamp,
                    sessionId );

        }
        catch( const TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure( "Error contacting train agent" );
        }
        catch( const TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Core::AccessDeniedException( e.what() );
        }
        catch( const TA_IRS_Bus::InvalidParameterException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidParameterException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( e.what() );
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure( e.what() );
        }

        FUNCTION_EXIT;
    }


    void TTISManager::submitClearRequest( const TA_Base_Core::TrainList& trains,
                                          const TA_Base_Core::TTISMessageResetParameters& displayParameters,
                                          CORBA::Long timestamp,
                                          const char* sessionId )
    {
        FUNCTION_ENTRY( "submitClearRequest" );

        TA_Base_Bus::GenericAgent::ensureControlMode();
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Received client request to send TTIS Clear request to trains" );


        try
        {
            m_trainTisCorbaProxy->resetMessage( convertTrainList( trains ),
                                                convertDisplayParameters( displayParameters ),
                                                timestamp,
                                                sessionId );
        }
        catch( const TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure( "Error contacting train agent" );
        }
        catch( const TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Core::AccessDeniedException( e.what() );
        }
        catch( const TA_IRS_Bus::InvalidParameterException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidParameterException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( e.what() );
        }
        catch( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure( e.what() );
        }

        FUNCTION_EXIT;
    }


	void TTISManager::cancelDownloadMessageLibrary( const TA_Base_Core::TrainList& trains, const char* sessionId )
	{
		FUNCTION_ENTRY("cancelDownloadMessageLibrary");

		TA_Base_Bus::GenericAgent::ensureControlMode();

		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
			"Received client request to cancel the TTIS message library download" );
		m_sessionID = std::string( sessionId );

		try
		{
			m_trainTisCorbaProxy->cancelLibraryDownload( convertTrainList( trains ), sessionId );
		}
		catch( const TA_IRS_Bus::AgentCommunicationException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );

			FUNCTION_EXIT;
			throw TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure( "Error contacting train agent" );
		}
		catch( const TA_IRS_Bus::TrainAccessDeniedException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );

			FUNCTION_EXIT;
			throw TA_Base_Core::AccessDeniedException( e.what() );
		}
		catch( const TA_IRS_Bus::InvalidParameterException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidParameterException", e.what() );

			FUNCTION_EXIT;
			throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( e.what() );
		}

		FUNCTION_EXIT;
	}

    void TTISManager::downloadNextMessageLibrary( const TA_Base_Core::TrainList& trains,
            const char* sessionId )
    {
        FUNCTION_ENTRY( "downloadNextMessageLibrary" );

        TA_Base_Bus::GenericAgent::ensureControlMode();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Received client request to download next TTIS message library to a train" );
		
		m_sessionID = std::string( sessionId );
        unsigned long nextMessageLibraryVersion = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion();

		std::auto_ptr<TA_Base_Core::IPredefinedMessageLibrary> nextPredefinedMessageLibrary;

		// retrieve latest version from the database
		try
		{
			nextPredefinedMessageLibrary = std::auto_ptr<TA_Base_Core::IPredefinedMessageLibrary>(
				TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary( nextMessageLibraryVersion,
				TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_TTIS ) );
		}
		catch( TA_Base_Core::TransactiveException& e )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
			throw TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure( "Error loading message library from ISCS database" );
		}
		catch( ... )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "...", "While attempting to retrieve the current Predefined Message Library from the Database" );
			throw TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure( "Error loading message library from ISCS database" );
		}
	
		// Get the raw library file content
		TA_IRS_Bus::TtisTypes::LibraryDataType fileContent;
		nextPredefinedMessageLibrary->getLibraryFileContent( fileContent );

		// Erase the decomprised the iscs ttislibrary file
		std::string tmpStr = StringConverter::vectorCharToString(fileContent);
		fileContent.clear();
		const std::string mark = TtisLibraryParser::getComprisedFileSearchMark();
		std::string::size_type pos = tmpStr.find(mark);
		if(pos == std::string::npos)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "...", "Database stored ttis library file is brokendown" );			
			throw TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure( "Error loading message library from ISCS database" );
		}

		pos += mark.size();
		tmpStr.erase(0, pos);
		std::copy(tmpStr.begin(), tmpStr.end(), std::back_inserter(fileContent));

      

        try
        {
            m_trainTisCorbaProxy->libraryDownload( convertTrainList( trains ),
                                                   fileContent,
                                                   nextMessageLibraryVersion,
                                                   sessionId );

        }
        catch( const TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure( "Error contacting train agent" );
        }
        catch( const TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Core::AccessDeniedException( e.what() );
        }
        catch( const TA_IRS_Bus::InvalidParameterException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidParameterException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( e.what() );
        }

        FUNCTION_EXIT;
    }


    void TTISManager::upgradeMessageLibrary( const TA_Base_Core::TrainList& trains,
            const char* sessionId )
    {
        FUNCTION_ENTRY( "upgradeMessageLibrary" );

        TA_Base_Bus::GenericAgent::ensureControlMode();

        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                     "Received client request to upgrade the TTIS message library on a train" );
		m_sessionID = std::string( sessionId );

        try
        {
            m_trainTisCorbaProxy->libraryUpgrade( convertTrainList( trains ), sessionId );
        }
        catch( const TA_IRS_Bus::AgentCommunicationException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::AgentCommunicationException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure( "Error contacting train agent" );
        }
        catch( const TA_IRS_Bus::TrainAccessDeniedException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::TrainAccessDeniedException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Core::AccessDeniedException( e.what() );
        }
        catch( const TA_IRS_Bus::InvalidParameterException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_IRS_Bus::InvalidParameterException", e.what() );

            FUNCTION_EXIT;
            throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( e.what() );
        }

        FUNCTION_EXIT;
    }


    void TTISManager::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& message )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        if ( TA_Base_Core::TISComms::TisTrainDataVersionUpdate.getTypeKey() == message.messageTypeKey.in() )
        {
            TA_Base_Core::TrainDataVersion* versionChange = NULL;

            if ( message.messageState >>= versionChange )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                             "received version update: [train id=%d][current version=%d][next version=%d]",
                             versionChange->trainNumber, versionChange->predefinedLibraryVersion, versionChange->nextPredefinedLibraryVersion);
                m_trainDataVersions.updateTTISLibraryVersions( versionChange->trainNumber, versionChange->predefinedLibraryVersion, versionChange->nextPredefinedLibraryVersion );
            }
            else
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                             "Could not extract TrainDataVersion from CommsMessageCorbaDef" );
            }
        }


        if ( TA_Base_Core::TISComms::TisTrainDownloadUpdate.getTypeKey() == message.messageTypeKey.in() )
        {
            // this is an update from the peer agent
            // or a message that we have just sent to ourself.
            // so, if in control mode we know its a self message and it shouldnt be processed
            if ( true == AgentModeMonitor::getInstance().isInControlMode()  )
            {
                FUNCTION_EXIT;
                return;
            }

            TA_Base_Core::TrainDownloadStatus* downloadChange = NULL;

            if ( message.messageState >>= downloadChange )
            {
                TA_THREADGUARD( m_trainDownloadsLock );
                // find the item for this train
                std::map< unsigned char, TA_Base_Core::TrainDownload >::iterator findIter = m_trainDownloadList.find( downloadChange->trainNumber );

                if ( findIter == m_trainDownloadList.end() )
                {
                    // insert an item for this train
                    TA_Base_Core::TrainDownload downloadData;
                    downloadData.trainNumber = downloadChange->trainNumber;
                    downloadData.predefinedDownloadInProgress = false;
                    m_trainDownloadList.insert( std::map< unsigned char, TA_Base_Core::TrainDownload >::value_type( downloadChange->trainNumber, downloadData ) );
                }
                // take a reference to the new or existing item
                TA_Base_Core::TrainDownload& existingDownload = m_trainDownloadList[ downloadChange->trainNumber ];

                switch ( downloadChange->type )
                {
                    case TA_Base_Core::LibraryDownloadStart:
                        existingDownload.predefinedDownloadInProgress = downloadChange->success;
                        break;
                    case TA_Base_Core::LibraryDownloadFinish:
                        existingDownload.predefinedDownloadInProgress = false;
                        break;
                    case TA_Base_Core::LibraryUpgrade:
                        // no action
                        break;
                    default:
                        TA_ASSERT( false, "Invalid update type" );
                        break;
                }
            }
            else
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                             "Could not extract TrainDownloadStatus from CommsMessageCorbaDef" );
            }
        }

        FUNCTION_EXIT;
    }


    void TTISManager::receiveSpecialisedMessage( const TA_Base_Core::StateUpdateMessageCorbaDef& message )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        // only process in monitor mode
        if ( true == AgentModeMonitor::getInstance().isInControlMode() )
        {
            FUNCTION_EXIT;
            return;
        }

        if ( TA_Base_Core::TISStateUpdate::TisTrainDataVersionAlarm.getTypeKey() == message.messageTypeKey.in() )
        {
            TA_Base_Core::TrainDataVersionAlarms* alarmUpdate = NULL;

            if ( message.stateUpdateInfo >>= alarmUpdate )
            {
                TA_THREADGUARD( m_trainDataLock );

                m_trainDataVersionAlarmList[ alarmUpdate->trainNumber ] = *alarmUpdate;
            }
            else
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                             "Could not extract TrainDataVersionAlarms from StateUpdateMessageCorbaDef" );
            }
        }

        FUNCTION_EXIT;
    }


    void TTISManager::updateTrainLibraryVersions( TA_IRS_Bus::CommonTypes::TrainIdType trainId, unsigned long libraryVersion, unsigned short scheduleVersion )
    {
        FUNCTION_ENTRY( "updateTrainLibraryVersions" );

        // update the library and schedule versions with the new version from the train agent
        // this is an update from the train agent, it means that its from a wakeup or upgrade
        // so anything in the next version buffer is now gone

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Updating library %d for train: %d", libraryVersion, trainId );

        TA_Base_Core::TrainDataVersion data;

        try
        {
            data = m_trainDataVersions.getTrainDataVersion( trainId );
        }
        catch ( TA_IRS_Bus::TrainDoesNotExistException& )
        {
            data.trainNumber = trainId;
            data.predefinedLibraryVersion = 0;
            data.nextPredefinedLibraryVersion = 0;
        }

		//hongzhi, CL-21514, refine 40143 messages
		if((data.predefinedLibraryVersion != libraryVersion))
		{
			if(data.predefinedLibraryVersion != libraryVersion)
			{
				m_trainDataVersions.updateTTISLibraryVersions( trainId, libraryVersion, data.nextPredefinedLibraryVersion );
			}
			data = m_trainDataVersions.getTrainDataVersion( trainId );
			sendVersionUpdateMessage( data );
			// now check the versions dont mismatch between the master versions
			verifyTrainDataVersions( trainId );
		}

        FUNCTION_EXIT;
    }


    void TTISManager::trainDownloadStarted( TA_IRS_Bus::CommonTypes::TrainIdType trainId, ETrainLibraryType downloadType, unsigned long version )
    {
        FUNCTION_ENTRY( "trainDownloadStarted" );

        TA_THREADGUARD( m_trainDownloadsLock );

        std::map< unsigned char, TA_Base_Core::TrainDownload >::iterator findIter = m_trainDownloadList.find( trainId );

        if ( findIter == m_trainDownloadList.end() )
        {
            // insert an item for this train
            TA_Base_Core::TrainDownload downloadData;
            downloadData.trainNumber = trainId;
            downloadData.predefinedDownloadInProgress = false;
            m_trainDownloadList.insert( std::map< unsigned char, TA_Base_Core::TrainDownload >::value_type( trainId, downloadData ) );
        }

        // take a reference to the new or existing item
        TA_Base_Core::TrainDownload& existingDownload = m_trainDownloadList[ trainId ];

        TA_Base_Core::TrainDownloadStatus update;
        update.trainNumber = trainId;
        update.success = true;
        update.errorDetails = CORBA::string_dup( "In Progress" );

        switch ( downloadType )
        {
            case ITrainEventCallback::MessageLibrary:
                existingDownload.predefinedDownloadInProgress = true;
                update.type = TA_Base_Core::LibraryDownloadStart;
                break;
            default:
                TA_ASSERT( false, "Invalid library type" );
                break;
        }

        sendDownloadUpdateMessage( update, version );

        FUNCTION_EXIT;
    }


    void TTISManager::trainDownloadComplete( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
            ETrainLibraryType downloadType,
            bool success,
            std::string statusString,
            unsigned long nextVersion )
    {
        FUNCTION_ENTRY( "trainDownloadComplete" );

        TA_THREADGUARD( m_trainDownloadsLock );

        std::map< unsigned char, TA_Base_Core::TrainDownload >::iterator findIter = m_trainDownloadList.find( trainId );

        if ( findIter == m_trainDownloadList.end() )
        {
            // insert an item for this train
            TA_Base_Core::TrainDownload downloadData;
            downloadData.trainNumber = trainId;
            downloadData.predefinedDownloadInProgress = false;
            m_trainDownloadList.insert( std::map< unsigned char, TA_Base_Core::TrainDownload >::value_type( trainId, downloadData ) );
        }

        // take a reference to the new or existing item
        TA_Base_Core::TrainDownload& existingDownload = m_trainDownloadList[ trainId ];

        TA_Base_Core::TrainDownloadStatus update;
        update.trainNumber = trainId;
        update.success = success;
        update.errorDetails = CORBA::string_dup( statusString.c_str() );

        // check the current version on the train
        TA_Base_Core::TrainDataVersion data;

        try
        {
            data = m_trainDataVersions.getTrainDataVersion( trainId );
        }
        catch( TA_IRS_Bus::TrainDoesNotExistException& )
        {
            // download without a train wakeup first
            // we dont know what version is on the train
            // assume 0
            data.trainNumber = trainId;
            data.predefinedLibraryVersion = 0;
            data.nextPredefinedLibraryVersion = 0;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "hongzhi101: [train id=%d][download type=%d][success=%d][next version=%d][status=%s]",
                     trainId, downloadType, success, nextVersion, statusString.c_str() );

        switch ( downloadType )
        {
            case ITrainEventCallback::MessageLibrary:
                existingDownload.predefinedDownloadInProgress = false;
                update.type = TA_Base_Core::LibraryDownloadFinish;

                // now bump up the next version
                if ( true == success )
                {
                    m_trainDataVersions.updateTTISLibraryVersions( trainId, data.predefinedLibraryVersion, nextVersion );
                }

                break;
            default:
                TA_ASSERT( false, "Invalid library type" );
                break;
        }


        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "hongzhi100,update.type=%d", update.type );

        // send the message
        sendDownloadUpdateMessage( update, nextVersion );

		try
		{
	        // get the updated status
	        data = m_trainDataVersions.getTrainDataVersion( trainId );

	        // send the version to any observers
	        sendVersionUpdateMessage( data );
		}
        catch( TA_IRS_Bus::TrainDoesNotExistException& )
        {
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Train [%d] version data not existed");
        }

        FUNCTION_EXIT;
    }


    void TTISManager::trainUpgradeComplete( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                            ETrainLibraryType downloadType,
                                            bool success,
                                            std::string statusString,
                                            unsigned long currentVersion )
    {
        FUNCTION_ENTRY( "trainUpgradeComplete" );

        TA_Base_Core::TrainDownloadStatus update;
        update.trainNumber = trainId;
        update.success = success;
        update.errorDetails = CORBA::string_dup( statusString.c_str() );

        TA_Base_Core::TrainDataVersion data;

        try
        {
            data = m_trainDataVersions.getTrainDataVersion( trainId );
        }
        catch ( TA_IRS_Bus::TrainDoesNotExistException& )
        {
            // download without a train wakeup first
            // we dont know what version is on the train
            // assume 0
            data.trainNumber = trainId;
            data.predefinedLibraryVersion = 0;
            data.nextPredefinedLibraryVersion = 0;
        }

        switch ( downloadType )
        {
            case ITrainEventCallback::MessageLibrary:
                update.type = TA_Base_Core::LibraryUpgrade;

                // update the current and next version
                if ( true == success )
                {
                    m_trainDataVersions.updateTTISLibraryVersions( trainId, currentVersion, data.nextPredefinedLibraryVersion );
                }

                break;
            default:
                TA_ASSERT( false, "Invalid library type" );
                break;
        }

        // get the updated status
        data = m_trainDataVersions.getTrainDataVersion( trainId );

        // send the download update message
        sendDownloadUpdateMessage( update, currentVersion );

        // send the version to any observers
        sendVersionUpdateMessage( data );

        // now check the versions dont mismatch between the master versions
        verifyTrainDataVersions( trainId );

        FUNCTION_EXIT;
    }


    void TTISManager::predefinedMessageRequestComplete( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
            time_t timestamp,
            std::string sessionId,
            const TA_Base_Core::TTISPredefinedMessageParameters& displayParameters,
            bool success,
            std::string statusString )
    {
        FUNCTION_ENTRY( "predefinedMessageRequestComplete" );

        if ( false == AgentModeMonitor::getInstance().isInControlMode() )
        {
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "Predefined Request for train %d, time %d, session %s complete.",
                     trainId, timestamp, sessionId.c_str() );

        //
        // first the result must be sent to the client
        //
        TA_ASSERT( m_tisMessageSender != NULL, "Tis Message Sender is null" );

        TA_Base_Core::TTISDisplayResult resultMessage;
        resultMessage.trainId = trainId;
        resultMessage.sessionId = sessionId.c_str();
        resultMessage.timestamp = timestamp;
        resultMessage.originalCommand = TA_Base_Core::TisPredefinedMessageCommand;
        resultMessage.success = success;
        resultMessage.errorDetails = statusString.c_str();

        CORBA::Any data;
        data <<= resultMessage;

        m_tisMessageSender->sendCommsMessage( TA_Base_Core::TISComms::TrainDisplayResult,
                                              m_entityKey, data, m_subsystemKey, m_locationKey, NULL );

        // the following only happens if the request was a success
        if ( false == success )
        {
            FUNCTION_EXIT;
            return;
        }


        //
        // Second the display request must be logged
        //
        //
        // Third, the operation must be audited
        //
        TA_Base_Core::DescriptionParameters desc;
        std::ostringstream debugStream;

		// blank the event value
		TA_Base_Core::NameValuePair blank("Blank", " ");
		desc.push_back(&blank);

        // do all train details related parameters
		std::stringstream ss;
		ss << displayParameters.messageId;
		debugStream << "message ID = " << ss.str() << "|";
		TA_Base_Core::NameValuePair p1("message ID", ss.str());
		desc.push_back(&p1);

		ss.str("");
		ss << static_cast<unsigned short>(displayParameters.perRun);
		debugStream << "per-run = " << ss.str() << "|";
		TA_Base_Core::NameValuePair p2("per-run", ss.str());
		desc.push_back(&p2);

        std::ostringstream trainIDStream;
        trainIDStream << static_cast< unsigned long >(trainId);

        debugStream << "TrainID = " << trainIDStream.str();
        TA_Base_Core::NameValuePair p3( "TrainID", trainIDStream.str());
        desc.push_back( &p3 ); // ok


        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "TTISManager::predefinedMessageRequestComplete: %s",
                     debugStream.str().c_str() );

        timeb curtime;
		StringHelper::isConvertToTimeb( StringHelper::getNewTimestampString(), curtime );

        m_auditMessageSender->sendAuditMessage(
            TA_Base_Core::TISAudit::TTISPredefinedMessage,
            m_entityKey,
            desc,
            "", // Further description text
            sessionId,
            "", // alarm ID - not required
            "", // incident key - not required
            "", // event key - not required
            NULL, // data filter - not required
            curtime ); // logged time

        FUNCTION_EXIT;
    }


    void TTISManager::freeTextMessageRequestComplete( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
            time_t timestamp,
            std::string sessionId,
            const TA_Base_Core::TTISFreeTextMessageParameters& displayParameters,
            bool success,
            std::string statusString )
    {
        FUNCTION_ENTRY( "freeTextMessageRequestComplete" );

        if ( false == AgentModeMonitor::getInstance().isInControlMode() )
        {
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "Free Text Request for train %d, time %d, session %s complete.",
                     trainId, timestamp, sessionId.c_str() );
        //
        // first the result must be sent to the client
        //
        TA_ASSERT( m_tisMessageSender != NULL, "Tis Message Sender is null" );

        TA_Base_Core::TTISDisplayResult resultMessage;
        resultMessage.trainId = trainId;
        resultMessage.sessionId = sessionId.c_str();
        resultMessage.timestamp = timestamp;
        resultMessage.originalCommand = TA_Base_Core::TisFreeTextMessageCommand;
        resultMessage.success = success;
        resultMessage.errorDetails = statusString.c_str();

        CORBA::Any data;
        data <<= resultMessage;

        m_tisMessageSender->sendCommsMessage( TA_Base_Core::TISComms::TrainDisplayResult,
                                              m_entityKey, data, m_subsystemKey, m_locationKey, NULL );
        // the following only happens if the request was a success
        if ( false == success )
        {
            FUNCTION_EXIT;
            return;
        }

        //
        // Third, the operation must be audited
        //
        TA_Base_Core::DescriptionParameters desc;
		// blank the event value
		TA_Base_Core::NameValuePair blank("Blank", " ");
		desc.push_back(&blank);

        // do all train details related parameters
        std::ostringstream debugStream;
		std::string messageContent(displayParameters.message.in());
		debugStream << "MessageText = " << messageContent << "|";
		TA_Base_Core::NameValuePair p1("MessageText", messageContent);
		desc.push_back(&p1);

		std::stringstream ss;
		unsigned short perRun = displayParameters.perRun;
		ss << perRun;
		debugStream << "per-run = " << ss.str() << "|";
		TA_Base_Core::NameValuePair p2("per-run", ss.str());
		desc.push_back(&p2);

        std::ostringstream trainIDStream;
        trainIDStream << static_cast< unsigned long >( trainId );
        debugStream << "TrainID = " << trainIDStream.str();
		TA_Base_Core::NameValuePair p3("TrainID", trainIDStream.str());
        desc.push_back( &p3 );

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "TTISManager::freeTextMessageRequestComplete: %s",
                     debugStream.str().c_str() );

        timeb curtime;
		ftime(&curtime);
        m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::TTISFreeTextMessage, m_entityKey,
                                                desc,
                                                "", // Further description text
                                                sessionId,
                                                "", // alarm ID - not required
                                                "", // incident key - not required
                                                "", // event key - not required
                                                NULL, // data filter - not required
                                                curtime ); // logged time

        FUNCTION_EXIT;
    }


    void TTISManager::clearRequestComplete( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                            time_t timestamp,
                                            std::string sessionId,
                                            const TA_Base_Core::TTISMessageResetParameters& displayParameters,
                                            bool success,
                                            std::string statusString )
    {
        FUNCTION_ENTRY( "clearRequestComplete" );

        if ( false == AgentModeMonitor::getInstance().isInControlMode() )
        {
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "Clear Request for train %d, time %d, session %s complete.",
                     trainId, timestamp, sessionId.c_str() );

        //
        // first the result must be sent to the client
        //
        TA_ASSERT( m_tisMessageSender != NULL, "Tis Message Sender is null" );

        TA_Base_Core::TTISDisplayResult resultMessage;
        resultMessage.trainId = trainId;
        resultMessage.sessionId = sessionId.c_str();
        resultMessage.timestamp = timestamp;
        resultMessage.originalCommand = TA_Base_Core::TisClearCommand;
        resultMessage.success = success;
        resultMessage.errorDetails = statusString.c_str();

        CORBA::Any data;
        data <<= resultMessage;

        m_tisMessageSender->sendCommsMessage( TA_Base_Core::TISComms::TrainDisplayResult,
                                              m_entityKey, data, m_subsystemKey, m_locationKey, NULL );

        // the following only happens if the request was a success
        if ( false == success )
        {
            FUNCTION_EXIT;
            return;
        }


        //
        // Second the operation must be audited
        //

        TA_Base_Core::DescriptionParameters desc;

		// blank the event value
		TA_Base_Core::NameValuePair blank("Blank", " ");
		desc.push_back(&blank);

        // do all train details related parameters
        std::ostringstream trainIDStream;
        trainIDStream << static_cast< unsigned long >(trainId);

        TA_Base_Core::NameValuePair trainDP( "TrainID", trainIDStream.str().c_str() );
        desc.push_back( &trainDP );

        m_auditMessageSender->sendAuditMessage(
            TA_Base_Core::TISAudit::TTISResetMessage,
            m_entityKey,
            desc,
            "", // Further description text
            sessionId,
            "", // alarm ID - not required
            "", // incident key - not required
            "" ); // event key - not required

        FUNCTION_EXIT;
    }


    void TTISManager::verifyTrainDataVersions( TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "verifyTrainDataVersions" );

        // check agent in control
        if ( false == AgentModeMonitor::getInstance().isInControlMode() )
        {
            FUNCTION_EXIT;
            return;
        }

        // if this is not the OCC or depot, then downloads are not applicable here
        if ( ( TA_Base_Core::ILocation::OCC != m_locationType ) &&
                ( TA_Base_Core::ILocation::DPT != m_locationType ) )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            // check if this agent is responsible for the version management for this train
            bool isResponsibleForTrain = m_trainAgentSubscriber->isTrainUnderAgentAreaControl( trainId );

            TA_THREADGUARD( m_trainDataLock );

            bool changes = false;

            // get the current alarm state for the train
            std::map< unsigned char, TA_Base_Core::TrainDataVersionAlarms >::iterator findIter = m_trainDataVersionAlarmList.find( trainId );
            if ( findIter == m_trainDataVersionAlarmList.end() )
            {
                // insert an item for this train
                TA_Base_Core::TrainDataVersionAlarms newTrainAlarms;
                newTrainAlarms.trainNumber = trainId;
                newTrainAlarms.messageLibraryMistmatchAlarm = CORBA::string_dup( "" );
                newTrainAlarms.iscsLibraryVersion = 0;
                newTrainAlarms.trainLibraryVersion = 0;
                m_trainDataVersionAlarmList.insert( std::map< unsigned char, TA_Base_Core::TrainDataVersionAlarms >::value_type( trainId, newTrainAlarms ) );

                changes = true;
            }

            // take a reference to the entry in the map for simplicity
            // note not a copy
            TA_Base_Core::TrainDataVersionAlarms& trainAlarms = m_trainDataVersionAlarmList[ trainId ];

            // flags for final actions
            bool libraryAlarmShouldBeClosed = false;
            bool libraryAlarmShouldBeRaised = false;
            // get current ISCS library version
            unsigned long currentIscsLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion();
            // check if alarms are raised
            bool messageLibraryAlarmIsRaised = !std::string( trainAlarms.messageLibraryMistmatchAlarm.in() ).empty();


            TA_Base_Core::TrainDataVersion trainVersion;

            // if the agent is not supposed to be in control of the train, and there are any alarms active, close them
            if ( false == isResponsibleForTrain )
            {
                if ( true == messageLibraryAlarmIsRaised )
                {
                    libraryAlarmShouldBeClosed = true;
                }
            }
            // responsible for train
            // determine correct alarm state
            else
            {
                // get train data versions
                trainVersion = m_trainDataVersions.getTrainDataVersion( trainId );

                //
                // evaluate the library version alarm state
                //

                // check if library alarm should be raised
                bool shouldHaveLibraryAlarm = trainVersion.predefinedLibraryVersion != currentIscsLibraryVersion;

                if ( true == shouldHaveLibraryAlarm )
                {
                    // an alarm should exist
                    if ( false == messageLibraryAlarmIsRaised )
                    {
                        // if not, raise it
                        libraryAlarmShouldBeRaised = true;
                    }
                    // an alarm exists
                    else
                    {
                        // check if details match
                        if ( ( trainAlarms.iscsLibraryVersion != currentIscsLibraryVersion ) ||
                                ( trainAlarms.trainLibraryVersion != trainVersion.predefinedLibraryVersion ) )
                        {
                            // if not, close old, raise new
                            libraryAlarmShouldBeClosed = true;
                            libraryAlarmShouldBeRaised = true;
                        }
                    }
                }
                // if not, and one is raised
                else if ( true == messageLibraryAlarmIsRaised )
                {
                    // close it
                    libraryAlarmShouldBeClosed = true;
                }
            }

            //
            // do the final actions - raising and closing alarms
            //
            if ( true == libraryAlarmShouldBeClosed )
            {
                closeAlarm( trainAlarms.messageLibraryMistmatchAlarm.in() );
                trainAlarms.messageLibraryMistmatchAlarm = CORBA::string_dup( "" );
                trainAlarms.iscsLibraryVersion = 0;
                trainAlarms.trainLibraryVersion = 0;
                changes = true;
            }

            if ( true == libraryAlarmShouldBeRaised )
            {
                trainAlarms.messageLibraryMistmatchAlarm = CORBA::string_dup( raiseLibraryVersionAlarm( trainId, trainVersion.predefinedLibraryVersion, currentIscsLibraryVersion ).c_str() );
                trainAlarms.iscsLibraryVersion = currentIscsLibraryVersion;
                trainAlarms.trainLibraryVersion = trainVersion.predefinedLibraryVersion;
                changes = true;
            }

            // if changes were made
            if ( true == changes )
            {
                // send sync message to peer
                sendLibraryVersionAlarmUpdate( trainAlarms );
            }

        }
        catch( TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
        }
        // HuangQi add for CL-21314 memory leaking start on 2010-03-11
        catch ( TA_Base_Bus::ITrainExceptionsCorba::InvalidTrainException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITrainExceptionsCorba::InvalidTrainException", e.details._ptr );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknow", "Unknow exception catched" );
        }
        // HuangQi add for CL-21314 memory leaking end on 2010-03-11

        FUNCTION_EXIT;
    }


    void TTISManager::verifyAllTrainDataVersions()
    {
        FUNCTION_ENTRY( "verifyAllTrainDataVersions" );

        // check agent in control
        if ( false == AgentModeMonitor::getInstance().isInControlMode() )
        {
            FUNCTION_EXIT;
            return;
        }

        // if this is not the OCC or depot, then downloads are not applicable here
        if ( ( TA_Base_Core::ILocation::OCC != m_locationType ) &&
                ( TA_Base_Core::ILocation::DPT != m_locationType ) )
        {
            FUNCTION_EXIT;
            return;
        }

        TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap allTrains = m_trainAgentSubscriber->getTrainList();

        for ( TA_IRS_Bus::TrainInformationTypes::TrainDetailsMap::iterator iter = allTrains.begin(); iter != allTrains.end(); ++iter )
        {
            verifyTrainDataVersions( iter->second.trainId );
        }

        FUNCTION_EXIT;
    }


    void TTISManager::subscribeToNotifications( bool controlMode )
    {
        FUNCTION_ENTRY( "subscribeToNotifications" );

        // this agent will subscribe to two types of messages
        // the first is the download status updates,
        // this will be used by the monitor mode peer to update the current download list when the control agent sends an update to the GUI
        // This is only applicable for OCC and depot
        //
        // the second is the version update message.
        // this is a broadcast message sent out whenever a train version updates
        // it is also only used to update the internal list

        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
            static_cast< TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>* >( this ) );

        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(
            static_cast< TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>* >( this ) );


        if ( ( TA_Base_Core::ILocation::STATION != m_locationType ) &&
                ( false == controlMode ) )
        {
            // subscribe to 'this' agent
            MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::TISComms::TisTrainDownloadUpdate,
                static_cast< TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>* >( this ),
                m_entityKey,
                m_subsystemKey,
                m_locationKey );
        }

        MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
            TA_Base_Core::TISComms::TisTrainDataVersionUpdate,
            static_cast< TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>* >( this ),
            m_entityKey,
            m_subsystemKey,
            m_locationKey );

        if ( false == controlMode )
        {
            MessageSubscriptionManager::getInstance().subscribeToStateUpdateMessage( TA_Base_Core::TISStateUpdate::TisTrainDataVersionAlarm,
                    static_cast< TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>* >( this ),
                    TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ),
                    NULL );
        }

        FUNCTION_EXIT;
    }


    void TTISManager::getInitialTrainDataVersions()
    {
        FUNCTION_ENTRY( "getInitialTrainDataVersions" );

        // at startup its not possible to predict the mode we will be started in,
        // first, attempt to sync from the local peer (in case one exists)
        // if this fails, fall back to the OCC (if at depot) or depot (if at OCC)
        // a station agent can fall back to both the depot and OCC

        // build a list of location names to try to get version information from
        std::vector< std::string > locationAttemptOrder;

        // local agent
        locationAttemptOrder.push_back( m_locationName );

        // this is the OCC or depot Agent, so use the opposite agent
        if ( false == m_otherAgentLocationNames.empty() )
        {
			std::copy(m_otherAgentLocationNames.begin(),
				      m_otherAgentLocationNames.end(),
				      std::back_inserter(locationAttemptOrder));
        }
        // this is a station agent, so try both OCC and depot
        else
        {
            try
            {
				std::vector<ILocation*> occLocations = LocationAccessFactory::getInstance().getLocationsByType(ILocation::OCC);
				std::vector<ILocation*> depotLocations = LocationAccessFactory::getInstance().getLocationsByType(ILocation::DPT);
				BOOST_FOREACH(ILocation* i, occLocations)
				{
					locationAttemptOrder.push_back(i->getName());
					delete i;
				}
				BOOST_FOREACH(ILocation* i, depotLocations)
				{
					locationAttemptOrder.push_back(i->getName());
					delete i;
				}
            }
            catch ( const TA_Base_Core::DatabaseException& ex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException",
                                     ex.what() );
            }
            catch ( const TA_Base_Core::DataException& ex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException",
                                     ex.what() );
            }
        }


        for ( std::vector< std::string >::iterator iter = locationAttemptOrder.begin();
                iter != locationAttemptOrder.end(); ++iter )
        {
            try
            {
                TA_Base_Core::TrainDataVersionList_var versionList =
                    TISAgentAccessFactory::getInstance().getTTISAgentAtLocation( *iter )->getTrainDataVersions();

                for ( unsigned long i = 0; i < versionList->length(); i++ )
                {
                    this->m_trainDataVersions.updateTTISLibraryVersions( versionList[i].trainNumber,
                            versionList[i].predefinedLibraryVersion,
                            versionList[i].nextPredefinedLibraryVersion );
                }

                // exit loop on success
                break;
            }
            catch ( const TA_Base_Core::DatabaseException& ex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException",
                                     ex.what() );
            }
            catch ( const TA_Base_Core::DataException& ex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException",
                                     ex.what() );
            }
            catch ( const TA_Base_Core::ObjectResolutionException& ex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException",
                                     ex.what() );
            }
            catch( const CORBA::Exception& ex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception",
                                     TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ex ).c_str() );
            }
            catch ( ... )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "...", "Undefined exception "
                                     "when synchronising with control agent." );
            }
        }

        FUNCTION_EXIT;
    }


    void TTISManager::getInitialTrainDownloads()
    {
        FUNCTION_ENTRY( "getInitialTrainDownloads" );

        // if this is not the OCC or depot, then downloads are not applicable here
        if ( ( TA_Base_Core::ILocation::OCC != m_locationType ) &&
                ( TA_Base_Core::ILocation::DPT != m_locationType ) )
        {
            FUNCTION_EXIT;
            return;
        }

        // at startup its not possible to predict the mode we will be started in,
        // attempt to sync from the local peer (in case one exists)

        try
        {
            TA_Base_Core::TrainDownloadList_var trainDownloadList = TISAgentAccessFactory::getInstance().getTTISAgentAtLocation( m_locationName )->getCurrentTrainDownloads();

            for ( unsigned long i = 0; i < trainDownloadList->length(); ++i )
            {
                m_trainDownloadList[ trainDownloadList[i].trainNumber ] = trainDownloadList[i];
            }
        }
        catch ( const TA_Base_Core::DatabaseException& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException",
                                 ex.what() );
        }
        catch ( const TA_Base_Core::DataException& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException",
                                 ex.what() );
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException",
                                 ex.what() );
        }
        catch( const CORBA::Exception& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception",
                                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ex ).c_str() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "Undefined exception "
                                 "when synchronising with control agent." );
        }

        FUNCTION_EXIT;
    }


    void TTISManager::getInitialTrainDataVersionAlarms()
    {
        FUNCTION_ENTRY( "getInitialTrainDataVersionAlarms" );

        // if this is not the OCC or depot, then downloads are not applicable here
        if ( ( TA_Base_Core::ILocation::OCC != m_locationType ) &&
                ( TA_Base_Core::ILocation::DPT != m_locationType ) )
        {
            FUNCTION_EXIT;
            return;
        }

        TA_THREADGUARD( m_trainDataLock );

        // at startup its not possible to predict the mode we will be started in,
        // attempt to sync from the local peer (in case one exists)

        bool success = false;

        try
        {
            TA_Base_Core::TrainDataVersionAlarmList_var alarmList = TISAgentAccessFactory::getInstance().getTTISAgentAtLocation( m_locationName )->getTrainDataAlarms();

            for ( unsigned long i = 0; i < alarmList->length(); ++i )
            {
                m_trainDataVersionAlarmList[ alarmList[i].trainNumber ] = alarmList[i];
            }

            success = true;
        }
        catch ( const TA_Base_Core::DatabaseException& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException",
                                 ex.what() );
        }
        catch ( const TA_Base_Core::DataException& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException",
                                 ex.what() );
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::ObjectResolutionException",
                                 ex.what() );
        }
        catch( const CORBA::Exception& ex )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception",
                                 TA_Base_Core::CorbaUtil::getInstance().exceptionToString( ex ).c_str() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "Undefined exception "
                                 "when synchronising with control agent." );
        }

        // if there was no peer agent to sync from
        if ( false == success )
        {
            // then all alarms should be closed
            closeAllLibraryVersionAlarms();
        }

        FUNCTION_EXIT;
    }


    void TTISManager::sendVersionUpdateMessage( const TA_Base_Core::TrainDataVersion& trainData )
    {
        FUNCTION_ENTRY( "sendVersionUpdateMessage" );

        TA_ASSERT( m_tisMessageSender != NULL, "Tis Message Sender is null" );

        if ( false == AgentModeMonitor::getInstance().isInControlMode() )
        {
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "Sending TisTrainDataVersionUpdate message, [train id=%d][current version=%d][next verson=%d]",
                     trainData.trainNumber, trainData.predefinedLibraryVersion, trainData.nextPredefinedLibraryVersion);

        // put the data into the message
        CORBA::Any data;
        data <<= trainData;

        m_tisMessageSender->sendCommsMessage( TA_Base_Core::TISComms::TisTrainDataVersionUpdate, m_entityKey, data,  m_subsystemKey, m_locationKey );


        FUNCTION_EXIT;
    }


    void TTISManager::sendDownloadUpdateMessage( const TA_Base_Core::TrainDownloadStatus& downloadStatus, unsigned long version )
    {
        FUNCTION_ENTRY( "sendDownloadUpdateMessage" );

        TA_ASSERT( m_tisMessageSender != NULL, "Tis Message Sender is null" );

        if ( false == AgentModeMonitor::getInstance().isInControlMode() )
        {
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Sending TisTrainDownloadUpdate message." );

        CORBA::Any data;
        data <<= downloadStatus;

        m_tisMessageSender->sendCommsMessage( TA_Base_Core::TISComms::TisTrainDownloadUpdate,
                                              m_entityKey, data, m_subsystemKey, m_locationKey, NULL );


        // do all train details related parameters
        std::ostringstream trainIDStream;
		trainIDStream << static_cast<unsigned long>(downloadStatus.trainNumber);

        // audit the event
        TA_Base_Core::DescriptionParameters desc;
		// blank the event value
		TA_Base_Core::NameValuePair blank("Blank", " ");
		desc.push_back(&blank);

        TA_Base_Core::NameValuePair trainId( EVENT_PARAMETER_TRAIN_ID, trainIDStream.str());
        desc.push_back( &trainId );

        switch( downloadStatus.type )
        {
            case TA_Base_Core::LibraryDownloadStart:

                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Library Download Start" );

                m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::TTISLibraryDownload, m_entityKey,
                                                        desc,
                                                        "", // Further description text
                                                        m_sessionID,
                                                        "", // alarm ID - not required
                                                        "", // incident key - not required
                                                        "" ); // event key - not required

                break;

            case TA_Base_Core::LibraryDownloadFinish:

                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Library Download Finish" );
				// if cancelled by operator, submit TTISLibraryCancelDownload event
				if(strcmp("Cancelled", downloadStatus.errorDetails) == 0)
				{
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Library Download Finish, cancelled by operator" );
			        m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::TTISLibraryCancelDownload,
						                                    m_entityKey,
			                                                desc,
			                                                "", // Further description text
															m_sessionID,
			                                                "", // alarm ID - not required
			                                                "", // incident key - not required
			                                                "" ); // event key - not required
				}
                break;

            case TA_Base_Core::LibraryUpgrade:

                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Library Upgrade" );

                m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::TTISTrainLibraryUpgrade, m_entityKey,
                                                        desc,
                                                        "", // Further description text
                                                        m_sessionID,
                                                        "", // alarm ID - not required
                                                        "", // incident key - not required
                                                        "" ); // event key - not required

                break;

            default:
                break;
        }

        FUNCTION_EXIT;
    }


    void TTISManager::sendLibraryVersionAlarmUpdate( const TA_Base_Core::TrainDataVersionAlarms& update )
    {
        FUNCTION_ENTRY( "sendLibraryVersionAlarmUpdate" );

        TA_ASSERT( m_tisStateUpdateMessageSender != NULL, "Tis Message Sender is null" );

        if ( false == AgentModeMonitor::getInstance().isInControlMode() )
        {
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Sending TisTrainDataVersionAlarm message." );

        // put the data into the message
        CORBA::Any data;
        data <<= update;

        m_tisStateUpdateMessageSender->sendStateUpdateMessage( TA_Base_Core::TISStateUpdate::TisTrainDataVersionAlarm,
                m_entityKey,
                TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME ),
                data, NULL );

        FUNCTION_EXIT;
    }


    std::string TTISManager::raiseLibraryVersionAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId, unsigned long trainVersion, unsigned long iscsVersion )
    {
        FUNCTION_ENTRY( "raiseLibraryVersionAlarm" );
        FUNCTION_EXIT;
        return raiseVersionAlarm( TA_Base_Core::TISAlarms::TrainLibraryInconsistentWithISCS, trainId, trainVersion, iscsVersion );
    }


    std::string TTISManager::raiseVersionAlarm( const TA_Base_Core::MessageType& alarmType, TA_IRS_Bus::CommonTypes::TrainIdType trainId, unsigned long trainVersion, unsigned long iscsVersion )
    {
        FUNCTION_ENTRY( "raiseVersionAlarm" );

        TA_Base_Core::DescriptionParameters dp;

        std::ostringstream versionStream;
        versionStream << trainVersion;
        TA_Base_Core::NameValuePair nvp1( "OriginalVersion", versionStream.str() );
        dp.push_back( &nvp1 );

        std::ostringstream trainIDStream;
        try
        {
            // use PV/SERVICE string
            TA_IRS_Bus::TrainInformationTypes::TrainDetails details = m_trainAgentSubscriber->getTrainDetails( trainId );
            trainIDStream << details.serviceNumber;
        }
        catch( ... )
        {
            trainIDStream << std::setw( 2 ) << std::setfill( '0' ) << trainId << "/_";
        }

        TA_Base_Core::NameValuePair nvp2( "trainID", trainIDStream.str().c_str() );
        dp.push_back( &nvp2 );

        std::ostringstream iscsVersionStream;
        iscsVersionStream << iscsVersion;
        TA_Base_Core::NameValuePair nvp3( "ISCSVersion", iscsVersionStream.str() );
        dp.push_back( &nvp3 );

        FUNCTION_EXIT;
        return TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(
                   alarmType,
                   m_entityKey,
                   m_entityTypeKey,
                   dp,
                   m_entityName,
                   m_locationKey,
                   m_subsystemKey,
                   StatusMonitor::getInstance()->getAgentAssetName(),
                   TA_Base_Core::AlarmConstants::defaultTime );
    }


    void TTISManager::closeAllLibraryVersionAlarms()
    {
        FUNCTION_ENTRY( "closeAllLibraryVersionAlarms" );

        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation(
            TA_Base_Core::TISAlarms::TrainLibraryInconsistentWithISCS, m_entityKey, m_locationKey );

        FUNCTION_EXIT;
    }


    void TTISManager::closeAlarm( const std::string& alarmId )
    {
        FUNCTION_ENTRY( "closeAlarm" );

        TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation( alarmId, m_locationKey );

        FUNCTION_EXIT;
    }


    TA_IRS_Bus::CommonTypes::TrainIdList TTISManager::convertTrainList( const TA_Base_Core::TrainList& trainList )
    {
        FUNCTION_ENTRY( "convertTrainList" );

        TA_IRS_Bus::CommonTypes::TrainIdList result;

        for ( unsigned long i = 0; i < trainList.length(); ++i )
        {
            result.insert( trainList[i] );
        }

        FUNCTION_EXIT;
        return result;
    }

    TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters TTISManager::convertDisplayParameters( const TA_Base_Core::TTISPredefinedMessageParameters displayParameters )
    {
        FUNCTION_ENTRY( "convertDisplayParameters" );

        TA_IRS_Bus::TtisTypes::TtisPredefinedDisplayParameters result;

        result.pidList = convertTisPidListToTrain( displayParameters.pidList );
        result.libraryVersion = displayParameters.libraryVersion;
		result.messageId = displayParameters.messageId;
		result.perRun = displayParameters.perRun;

        FUNCTION_EXIT;
        return result;
    }


    TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters TTISManager::convertDisplayParameters( const TA_Base_Core::TTISFreeTextMessageParameters& displayParameters )
    {
        FUNCTION_ENTRY( "convertDisplayParameters" );

        TA_IRS_Bus::TtisTypes::TtisFreeTextDisplayParameters result;

        result.pidList = convertTisPidListToTrain( displayParameters.pidList );
        result.fontSize = convertTisLedFontSizeToTrain( displayParameters.fontSize );
        result.justification = convertTisJustificationToTrain( displayParameters.justification );
        result.intensity = convertTisIntensityToTrain( displayParameters.intensity );
        result.displayMode = convertTisDisplayModeToTrain( displayParameters.displayMode );
		result.perRun = displayParameters.perRun;
        result.message = displayParameters.message.in();

        FUNCTION_EXIT;
        return result;
    }


    TA_IRS_Bus::TtisTypes::TtisResetParameters TTISManager::convertDisplayParameters( const TA_Base_Core::TTISMessageResetParameters& displayParameters )
    {
        FUNCTION_ENTRY( "convertDisplayParameters" );

        TA_IRS_Bus::TtisTypes::TtisResetParameters result;

        result.pidList = convertTisPidListToTrain( displayParameters.pidList );
        result.messageType = convertTisClearTypeToTrain( displayParameters.messageType );

        FUNCTION_EXIT;
        return result;
    }


    TA_IRS_Bus::TtisTypes::TtisPidList TTISManager::convertTisPidListToTrain( const TA_Base_Core::TrainPIDList& pidList )
    {
        FUNCTION_ENTRY( "convertTisPidListToTrain" );

        TA_IRS_Bus::TtisTypes::TtisPidList result;

        for ( unsigned long i = 0; i < pidList.length(); ++i )
        {
            result.push_back( convertTisPidToTrain( pidList[i] ) );
        }

        FUNCTION_EXIT;
        return result;
    }


    TA_IRS_Bus::ProtocolTtisTypes::ETtisPid TTISManager::convertTisPidToTrain( TA_Base_Core::EPIDSelection pid )
    {
        FUNCTION_ENTRY( "convertTisPidToTrain" );

        switch ( pid )
        {
            case TA_Base_Core::ENTIRE_TRAIN:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidAllTrain;
                break;
            case TA_Base_Core::ALL_CAR1:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar1;
                break;
            case TA_Base_Core::ALL_CAR2:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar2;
                break;
            case TA_Base_Core::ALL_CAR3:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar3;
                break;
            case TA_Base_Core::CAR1_PID1:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar1Pid1;
                break;
            case TA_Base_Core::CAR1_PID2:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar1Pid2;
                break;
            case TA_Base_Core::CAR1_PID3:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar1Pid3;
                break;
            case TA_Base_Core::CAR1_PID4:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar1Pid4;
                break;
            case TA_Base_Core::CAR2_PID1:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar2Pid1;
                break;
            case TA_Base_Core::CAR2_PID2:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar2Pid2;
                break;
            case TA_Base_Core::CAR2_PID3:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar2Pid3;
                break;
            case TA_Base_Core::CAR2_PID4:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar2Pid4;
                break;
            case TA_Base_Core::CAR3_PID1:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar3Pid1;
                break;
            case TA_Base_Core::CAR3_PID2:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar3Pid2;
                break;
            case TA_Base_Core::CAR3_PID3:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar3Pid3;
                break;
            case TA_Base_Core::CAR3_PID4:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisPidCar3Pid4;
                break;
            default:

                FUNCTION_EXIT;
                throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( "Invalid PID selection" );
                break;
        }

        FUNCTION_EXIT;
        return TA_IRS_Bus::ProtocolTtisTypes::TtisPidAllTrain;
    }


    TA_IRS_Bus::ProtocolTtisTypes::ETtisResetMessageType TTISManager::convertTisClearTypeToTrain( TA_Base_Core::ETTISClearType messageType )
    {
        FUNCTION_ENTRY( "convertTisClearTypeToTrain" );

        //convert the enumeration from TISMessageDataDefinitions.idl to the C++ enumeration from TrainCommonLibrary
        //throw an exception if the conversion cant be performed

        switch ( messageType )
        {
            case TA_Base_Core::CLEAR_ALL:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisResetAll;
                break;
            case TA_Base_Core::CLEAR_EMERGENCY:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisResetEmergency;
                break;
            case TA_Base_Core::CLEAR_NORMAL:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisResetNormal;
                break;
            case TA_Base_Core::CLEAR_PRIORITY6:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisResetPriority6;
                break;
            default:

                FUNCTION_EXIT;
                throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( "Invalid PID messageType" );
                break;
        }

        FUNCTION_EXIT;
        return TA_IRS_Bus::ProtocolTtisTypes::TtisResetAll;

    }


    unsigned char TTISManager::convertTisLibrarySectionToTrain( TA_Base_Core::ELibrarySection librarySection )
    {
        FUNCTION_ENTRY( "convertTisLibrarySectionToTrain" );

        //convert the enumeration from TISMessageDataDefinitions.idl to the C++ enumeration from TrainCommonLibrary
        //throw an exception if the conversion cant be performed

        switch ( librarySection )
        {
            case TA_Base_Core::EMERGENCY_SECTION:
                FUNCTION_EXIT;
                return 0;
                break;
            case TA_Base_Core::NORMAL_SECTION:
                FUNCTION_EXIT;
                return 1;
                break;
            default:

                FUNCTION_EXIT;
                throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( "Invalid librarySection" );
                break;
        }

        FUNCTION_EXIT;
        return 0;

    }


    TA_IRS_Bus::ProtocolTtisTypes::ETtisPriority TTISManager::convertTisMessagePriorityToTrain( TA_Base_Core::ETTISMessagePriority priority )
    {
        FUNCTION_ENTRY( "convertTisMessagePriorityToTrain" );

        //convert the enumeration from TISMessageDataDefinitions.idl to the C++ enumeration from TrainCommonLibrary
        //throw an exception if the conversion cant be performed

        switch ( priority )
        {
            case TA_Base_Core::TTIS_PRIORITY_1:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityOne;
                break;
            case TA_Base_Core::TTIS_PRIORITY_2:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityTwo;
                break;
            case TA_Base_Core::TTIS_PRIORITY_3:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityThree;
                break;
            case TA_Base_Core::TTIS_PRIORITY_4:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityFour;
                break;
            case TA_Base_Core::TTIS_PRIORITY_5:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityFive;
                break;
            case TA_Base_Core::TTIS_PRIORITY_6:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPrioritySix;
                break;
            case TA_Base_Core::TTIS_PRIORITY_7:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPrioritySeven;
                break;
            case TA_Base_Core::TTIS_PRIORITY_8:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityEight;
                break;
            default:

                FUNCTION_EXIT;
                throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( "Invalid priority" );
                break;
        }

        FUNCTION_EXIT;
        return TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityOne;

    }


    TA_IRS_Bus::ProtocolTtisTypes::ETtisPriority TTISManager::convertTisPriorityToTrain( int priority )
    {
        FUNCTION_ENTRY( "convertTisPriorityToTrain" );

        //convert the integer to the C++ enumeration from TrainCommonLibrary
        //throw an exception if the conversion cant be performed

        switch ( priority )
        {
            case 1:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityOne;
                break;
            case 2:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityTwo;
                break;
            case 3:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityThree;
                break;
            case 4:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityFour;
                break;
            case 5:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityFive;
                break;
            case 6:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPrioritySix;
                break;
            case 7:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPrioritySeven;
                break;
            case 8:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityEight;
                break;
            default:

                FUNCTION_EXIT;
                throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( "Invalid priority" );
                break;
        }

        FUNCTION_EXIT;
        return TA_IRS_Bus::ProtocolTtisTypes::ETtisPriorityOne;
    }


    TA_IRS_Bus::TtisTypes::TimeInterval TTISManager::convertTisTimeIntervalToTrain( const std::string& startTime, const std::string& endTime )
    {
        FUNCTION_ENTRY( "convertTisTimeIntervalToTrain" );

        TA_IRS_Bus::TtisTypes::TimeInterval displayPeriod;
        std::string temp;

        // Get the start hour
        temp = startTime.substr( 0, 2 );
        displayPeriod.startHour = ( unsigned char )atoi( temp.c_str() );

        temp.empty();
        // get the start minutes
        temp = startTime.substr( 2, 2 );
        displayPeriod.startMin = ( unsigned char )atoi( temp.c_str() );

        temp.empty();
        // Get the end hour
        temp = endTime.substr( 0, 2 );
        displayPeriod.stopHour = ( unsigned char )atoi( temp.c_str() );

        temp.empty();
        // get the end minutes
        temp = endTime.substr( 2, 2 );
        displayPeriod.stopMin = ( unsigned char )atoi( temp.c_str() );

        FUNCTION_EXIT;
        return displayPeriod;
    }


    TA_IRS_Bus::ProtocolTtisTypes::ETtisFontSize TTISManager::convertTisLedFontSizeToTrain( TA_Base_Core::ETTISLEDFontSize fontSize )
    {
        FUNCTION_ENTRY( "convertTisLedFontSizeToTrain" );

        //convert the enumeration from TISMessageDataDefinitions.idl to the C++ enumeration from TrainCommonLibrary
        //throw an exception if the conversion cant be performed

        switch ( fontSize )
        {
            case TA_Base_Core::TTIS_FONT_SIZE_SMALL:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisFontSmall;
                break;
            case TA_Base_Core::TTIS_FONT_SIZE_LARGE:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisFontLarge;
                break;
            default:

                FUNCTION_EXIT;
                throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( "Invalid fontSize" );
                break;
        }

        FUNCTION_EXIT;
        return TA_IRS_Bus::ProtocolTtisTypes::TtisFontSmall;
    }


    TA_IRS_Bus::ProtocolTtisTypes::ETtisFontSize TTISManager::convertTisFontSizeToTrain( TA_Base_Core::ETTISLEDFontSize fontSize )
    {
        FUNCTION_ENTRY( "convertTisFontSizeToTrain" );

        //convert the enumeration from TISMessageDataDefinitions.idl to the C++ enumeration from TrainCommonLibrary
        //throw an exception if the conversion cant be performed

        switch ( fontSize )
        {
            case TA_Base_Core::FONT_SIZE_SMALL:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisFontSmall;
                break;
            case TA_Base_Core::FONT_SIZE_MEDIUM:
            case TA_Base_Core::FONT_SIZE_LARGE:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisFontLarge;
                break;
            default:
                FUNCTION_EXIT;
                throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( "Invalid fontSize" );
                break;
        }

        FUNCTION_EXIT;
        return TA_IRS_Bus::ProtocolTtisTypes::TtisFontSmall;
    }


    TA_IRS_Bus::ProtocolTtisTypes::ETtisJustification TTISManager::convertTisJustificationToTrain( TA_Base_Core::EJustification justifcation )
    {
        FUNCTION_ENTRY( "convertTisJustificationToTrain" );

        //convert the enumeration from TISMessageDataDefinitions.idl to the C++ enumeration from TrainCommonLibrary
        //throw an exception if the conversion cant be performed

        switch ( justifcation )
        {
            case TA_Base_Core::LEFT:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisJustificationLeft;
                break;
            case TA_Base_Core::CENTRED:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisJustificationCentre;
                break;
            case TA_Base_Core::RIGHT:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisJustificationRight;
                break;
            default:

                FUNCTION_EXIT;
                throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( "Invalid justifcation" );
                break;
        }

        FUNCTION_EXIT;
        return TA_IRS_Bus::ProtocolTtisTypes::TtisJustificationLeft;

    }


    TA_IRS_Bus::ProtocolTtisTypes::ETtisIntensity TTISManager::convertTisIntensityToTrain( TA_Base_Core::ETTISLEDIntensity intensity )
    {
        FUNCTION_ENTRY( "convertTisIntensityToTrain" );

        //convert the enumeration from TISMessageDataDefinitions.idl to the C++ enumeration from TrainCommonLibrary
        //throw an exception if the conversion cant be performed

        switch ( intensity )
        {
            case TA_Base_Core::NORMAL:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisIntensityNormal;
                break;
            case TA_Base_Core::HIGH:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisIntensityHigh;
                break;
            default:

                FUNCTION_EXIT;
                throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( "Invalid intensity" );
                break;
        }

        FUNCTION_EXIT;
        return TA_IRS_Bus::ProtocolTtisTypes::TtisIntensityNormal;

    }


    TA_IRS_Bus::ProtocolTtisTypes::ETtisDisplayMode TTISManager::convertTisDisplayModeToTrain( TA_Base_Core::EDisplayMode displayMode )
    {
        FUNCTION_ENTRY( "convertTisDisplayModeToTrain" );

        //convert the enumeration from TISMessageDataDefinitions.idl to the C++ enumeration from TrainCommonLibrary
        //throw an exception if the conversion cant be performed

        switch ( displayMode )
        {
            case TA_Base_Core::SCROLL_LEFT:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeScrollLeft;
                break;
            case TA_Base_Core::SCROLL_RIGHT:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeScrollRight;
                break;
            case TA_Base_Core::SCROLL_UP:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeScrollUp;
                break;
            case TA_Base_Core::SCROLL_DOWN:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeScrollDown;
                break;
            case TA_Base_Core::NORMAL_STATIONARY:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeInstantOn;
                break;
            case TA_Base_Core::BLINKING:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeBlinking;
                break;
            case TA_Base_Core::WIPING:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeWiping;
                break;
            case TA_Base_Core::SNOW:
                FUNCTION_EXIT;
                return TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeLikeSnow;
                break;
            default:

                FUNCTION_EXIT;
                throw TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter( "Invalid displayMode" );
                break;
        }

        FUNCTION_EXIT;
        return TA_IRS_Bus::ProtocolTtisTypes::TtisDisplayModeScrollLeft;
    }
}
