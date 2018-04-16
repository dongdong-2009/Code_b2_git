/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL13_TIP/GZL13/transactive/app/signs/pids_agent/src/PIDSManager.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/10/01 16:36:38 $
  * Last modified by:  $Author: grace.koh $
  *
  * Manages all Station Traveller Information System-related functionality
  *
  * Implements the IPIDSManagerCorbaDef CORBA interface
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif // _MSC_VER
#include "app/signs/pids_agent/src/AgentModeMonitor.h"
#include "app/signs/pids_agent/src/Exceptions.h"
#include "app/signs/pids_agent/src/PIDSAdHocDisplayRequest.h"
#include "app/signs/pids_agent/src/PIDSClearRequest.h"
#include "app/signs/pids_agent/src/PIDSManager.h"
#include "app/signs/pids_agent/src/StringHelper.h"
#include "app/signs/pids_agent/src/PIDSCommunicationsHandler.h"
#include "app/signs/pids_agent/src/globaldef.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/PIDSEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/database/src/SimpleDb.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/PIDSAlarms_MessageTypes.h"
#include "core/message/types/PIDSAudit_MessageTypes.h"
#include "core/message/types/PIDSComms_MessageTypes.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/threads/src/Thread.h"
#include "core/message/types/ATSAgentComms_MessageTypes.h"
#include <ace/OS.h>

#include <boost/tokenizer.hpp>

#include <iomanip>


#define STATUS_DP_NAME "eiiPIDSC-Status"
#define LOCKSTATUS_DP_NAME "diiPIDSC-LockStatus"

#define LOCATION_AUDIT_MESSAGE_PARAMETER_NAME "Location"
// TD 9983
#define PIDCONTROL_LOCATION_AUDIT_MESSAGE_PARAMETER_NAME "location"
// TD 9983
#define MESSAGE_TAG_AUDIT_MESSAGE_PARAMETER_NAME "ID"
#define ADHOC_MESSAGE_AUDIT_MESSAGE_PARAMETER_NAME "Message"
#define LIBRARY_VERSION_AUDIT_MESSAGE_PARAMETER_NAME "Version"
#define DESTINATION_AUDIT_MESSAGE_PARAMETER_NAME "Destination"
#define TYPE_AUDIT_MESSAGE_PARAMETER_NAME "type"
#define MESSAGE_AUDIT_MESSAGE_PARAMETER_NAME "Message"
#define RAPIDSTAG_AUDIT_MESSAGE_PARAMETER_NAME "RaPids Tag"
#define ENABLE_STATUS_AUDIT_MESSAGE_PARAMETER_NAME "Enabled/Disabled"
#define STATUS_AUDIT_MESSAGE_PARAMETER_NAME "on/off"
#define RAPIDSDESTINATION_AUDIT_MESSAGE_PARAMETER_NAME "RaPids destination"
#define APPROVAL_STATUS_AUDIT_MESSAGE_PARAMETER_NAME "Approved/Rejected"
// TD 9983
#define PID_AUDIT_MESSAGE_PARAMETER_NAME "PID"
// TD 9983
#define PRIORITY_AUDIT_MESSAGE_PARAMETER_NAME "Priority"
#define STARTTIME_AUDIT_MESSAGE_PARAMETER_NAME "Start time"
#define ENDTIME_AUDIT_MESSAGE_PARAMETER_NAME "End time"
#define OVERRIDE_AUDIT_MESSAGE_PARAMETER_NAME "Override"
#define VETTED_AUDIT_MESSAGE_PARAMETER_NAME "Vetted"
#define REPEAT_INTERVAL_AUDIT_MESSAGE_PARAMETER_NAME "Repeat interval"
#define DISPLAY_ATTRIBUTE_AUDIT_MESSAGE_PARAMETER_NAME "Display attribute"
// CL-20439
#define VIRTUAL_DATAPOINT_ADDRESS_NAME "VIRTUAL"
// CL-20439

namespace TA_IRS_App
{
    //
    // Initialise statics.
    //

    TA_Base_Core::ReEntrantThreadLockable PIDSManager::m_singletonLock;
    PIDSManager* PIDSManager::m_theClassInstance = NULL;


    //
    // getInstance
    //
    PIDSManager* PIDSManager::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        // If guard acquired then make sure the singleton is still NULL.

        if ( NULL == m_theClassInstance )
        {
            // Create the one & only object.
            m_theClassInstance = new PIDSManager();
        }

        return m_theClassInstance;
    }

    PIDSManager::PIDSManager() :
        m_auditMessageSender( NULL ),
        m_rightsLibrary( NULL ),
        m_PidsMessageSender( NULL ),
        m_raisedAtsForwardingFailureAlarm( false ), //TD15198 azenitha
        m_nonUniqueAlarmHelper( TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper() )
    {

        TA_Base_Bus::RightsLibraryFactory   rlf;
        m_rightsLibrary = rlf.buildRightsLibrary();
        TA_ASSERT( m_rightsLibrary != NULL, "Rights library not created." );

        m_auditMessageSender =
            TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::PIDSAudit::Context );

        TA_ASSERT( m_auditMessageSender != NULL, "Could not create message publication manager" );


        m_PidsMessageSender = TA_Base_Core::MessagePublicationManager::getInstance()
                              .getCommsMessageSender( TA_Base_Core::PIDSComms::Context );

    }

    PIDSManager::~PIDSManager()
    {
        FUNCTION_ENTRY( "~PIDSManager" );

        if ( m_auditMessageSender )
        {
            delete m_auditMessageSender;
            m_auditMessageSender = NULL;
        }

        TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

        FUNCTION_EXIT;
    }

    void PIDSManager::submitSigRequest(const ::TA_Base_Bus::IPIDSManagerCorbaDef::SigData& data)
    {
        FUNCTION_ENTRY( "submitSigRequest" );
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Received sig request from SigAgent" );
		std::auto_ptr<PIDSSigDisplayRequest> pidsSigRequest(new PIDSSigDisplayRequest());
		pidsSigRequest->setData(data);
		PIDSCommunicationsHandler::getInstance()->sendMessage( pidsSigRequest.get(), true, SIGINFO_FEP_REGISTER_ADDRESS );
        FUNCTION_EXIT;
    }

    ::CORBA::Short PIDSManager::submitAdhocDisplayRequest( const ::TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList& destinationList,
            const ::TA_Base_Bus::IPIDSManagerCorbaDef::PidsMessage& messageContent,
            ::CORBA::Boolean msgType,
            ::CORBA::Boolean showType,
            ::CORBA::Long trainNo,
            const ::TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime& startTime,
            const ::TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime& endTime,
            ::CORBA::Long interval,
            ::CORBA::Boolean isCyclic )
    {
        FUNCTION_ENTRY( "submitAdHocDisplayRequest" );
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Received adhoc display request from GUI" );
        std::ostringstream dump;
        dump << "location,downtrack,uptrack,concourse:";
        for ( int i = 0; i < destinationList.length(); i++ )
        {
            dump << destinationList[i].station << " " << destinationList[i].downTrack << " " << destinationList[i].upTrack << " " << destinationList[i].concourse << ", ";
        }
        dump << "content=";
        for ( int i = 0; i < messageContent.length(); i++ )
        {
            dump << messageContent[i];
        }
        dump << ", messageType=" << msgType << ", showType=" << showType << ", trainNo=" << trainNo;
        dump << ", startTime=" << startTime.hour << ":" << startTime.minute << ":" << startTime.second;
        dump << ", endTime=" << endTime.hour << ":" << endTime.minute << ":" << endTime.second;
        dump << ", isCyclic=" << isCyclic << ", interval=" << interval;
        LOGLARGESTRING( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, dump.str().c_str() );
        TA_Base_Bus::GenericAgent::ensureControlMode();

        // Create the new display request object
        PIDSAdHocDisplayRequest* displayRequest = new PIDSAdHocDisplayRequest();

        // Set the data members
        displayRequest->setData( destinationList, messageContent, msgType, showType, trainNo,
                                 startTime, endTime, interval, isCyclic );

        // Send the message to the PIDS server
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending adhoc display request message" );
        short nRet = PIDSCommunicationsHandler::getInstance()->sendMessage( displayRequest );
        // Add it to the display request log
        displayRequest->log( true );
        delete displayRequest;
        return nRet;
        FUNCTION_EXIT;
    }

    ::CORBA::Short PIDSManager::submitAdhocDisplayRequestWithMulityByteMsgContent( const ::TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList& destinationList,
            const ::TA_Base_Bus::IPIDSManagerCorbaDef::PidsMessage& messageContent,
            ::CORBA::Boolean msgType,
            ::CORBA::Boolean showType,
            ::CORBA::Boolean isTrain,
            const ::TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime& startTime,
            const ::TA_Base_Bus::IPIDSManagerCorbaDef::MessageTime& endTime,
            ::CORBA::Long interval,
            ::CORBA::Boolean isCyclic )
    {
		return 0;
    }


    ::CORBA::Short PIDSManager::submitClearRequest( const ::TA_Base_Bus::IPIDSManagerCorbaDef::PidsDestinationList& destinationList,
            ::CORBA::Long trainNo )
    {
        FUNCTION_ENTRY( "submitClearRequest" );
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Received clear request from GUI" );
        std::ostringstream dump;
        dump << "location,downtrack,uptrack,concourse:";
        for ( int i = 0; i < destinationList.length(); i++ )
        {
            dump << destinationList[i].station << " " << destinationList[i].downTrack << " " << destinationList[i].upTrack << " " << destinationList[i].concourse << ", ";
        }
        dump << ", trainNo=" << trainNo;
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s", dump.str().c_str() );
        TA_Base_Bus::GenericAgent::ensureControlMode();

        // Create the new display request object
        PIDSClearRequest* clearRequest = new PIDSClearRequest();

        // Set the data members
        clearRequest->setData( destinationList, trainNo );

        // Send the message to the PIDS server
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending clear request message" );
        short nRet = PIDSCommunicationsHandler::getInstance()->sendMessage( clearRequest );
        //LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Clear message sent successfully." );

        delete clearRequest;

        return nRet;
        FUNCTION_EXIT;
    }

    std::string PIDSManager::getNewTimestampString()
    {
        std::stringstream timeStamp;

        time_t currentTime;
        tm localTime;

        currentTime = time( NULL );
        ACE_OS::localtime_r( &currentTime, &localTime );

        // the timestamp is in the format
        // YYYYMMDDHHmmSS

        // YYYY
        timeStamp << ( localTime.tm_year + 1900 );

        // MM
        // tm_mon is months since january, so 0 = january
        timeStamp << std::setw( 2 ) << std::setfill( '0' ) << ( localTime.tm_mon + 1 );

        // DD
        // tm_mday is the day of the month (1-31)
        timeStamp << std::setw( 2 ) << std::setfill( '0' ) << localTime.tm_mday;

        // HH
        // tm_hour is the hours past the day (0-23)
        timeStamp << std::setw( 2 ) << std::setfill( '0' ) << localTime.tm_hour;

        // MM
        // tm_min is the minutes past the hour (0-59)
        timeStamp << std::setw( 2 ) << std::setfill( '0' ) << localTime.tm_min;

        // SS
        // tm_sec is the seconds past the minute (0-60) (61 for leap seconds)
        timeStamp << std::setw( 2 ) << std::setfill( '0' ) << localTime.tm_sec;

        return timeStamp.str();
    }

    //++zhou yuan

    bool PIDSManager::isValid()
    {
        FUNCTION_ENTRY( "isValid" );

        FUNCTION_EXIT;
        return true;
    }

    void PIDSManager::start()
    {
        FUNCTION_ENTRY( "start" );
        // always runs - it will just sit on the semaphore until it receives a message
//        m_statusRequestQueue.start();

        FUNCTION_EXIT;

    }

    void PIDSManager::stop()
    {
        FUNCTION_ENTRY( "stop" );

//      m_statusRequestQueue.terminateAndWait();

        // Disconnect from the PIDS server
//		m_communicationsHandler.disconnectFromServer();

        deactivateServant();

        // for each of the data points created for this agent, deactive
        // the data point's servant

        // Dont deactivate the PID datapoints if this is the OCC agent
        // the PIDs are not real datapoints and are used to establish the address
        // of the station PIDs when sending display requests
        /*		if( m_locationType == TA_Base_Core::ILocation::OCC)
        		{
        			std::vector< TA_IRS_App::DataPointDetails* >::iterator dpIter;
        			for ( dpIter = m_pidVirtualDataPointList.begin();
        				  dpIter != m_pidVirtualDataPointList.end();
        				  dpIter++ )
        			{
        				// Deactivate the DataPoint's servant.  The GenericAgent
        				// will delete the object from memory space
        				delete (*dpIter);
        			}
        		}
        */
        FUNCTION_EXIT;
    }


    void PIDSManager::update( const TA_Base_Core::ConfigUpdateDetails& configUpdateDetails )
    {
        FUNCTION_ENTRY( "update" );

        FUNCTION_EXIT;
    }


    void PIDSManager::remove()
    {
        FUNCTION_ENTRY( "remove" );

        FUNCTION_EXIT;
    }


    void PIDSManager::setToControlMode()
    {
        FUNCTION_ENTRY( "setToControlMode" );

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "PIDSManager switching to Control Mode" );

        // Dont set the PID datapoints to Control mode if this is the OCC agent
        // the PIDs are not real datapoints and are used to establish the address
        // of the station PIDs when sending display requests
        /*		if( m_locationType != TA_Base_Core::ILocation::OCC)
        		{
        			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Setting datapoints to Control Mode");
        			// set the data point to control mode now
        			std::vector< TA_IRS_App::DataPointDetails* >::iterator dpIter;
        			for ( dpIter = m_pidVirtualDataPointList.begin();
        				  dpIter != m_pidVirtualDataPointList.end();
        				  dpIter++ )
        			{
        				std::stringstream dpMessage;
        				dpMessage << "Set datapoint ";
        				dpMessage << (*dpIter)->dataPoint->getDataPointName();
        				dpMessage << " to Control mode";

        				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, dpMessage.str().c_str() );
        				const_cast<DataPoint*>((*dpIter)->dataPoint)->setToControlMode();
        			}
        		}
        */
        // check and process if any virtual DataPoints have been changed
        //processDataPointStateManuallyUpdate();

//		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "PIDSManager asking the Communications Handler to connect to the PIDS Server");

        // Attempt to connect to the PIDS server
        // If we're not at the depot
        /*		if( m_locationType != TA_Base_Core::ILocation::DPT )
        		{
        			m_communicationsHandler.connectToServer( m_serverIPAddress, m_serverPort );
        		}
        */
        if ( m_entityName.compare( "" ) )
            activateServantWithName( m_entityName );

        //chenlei-+ moved this procedure into a new thread
//		PIDSFailoverThread* failoverloadThread = new PIDSFailoverThread;
//		failoverloadThread->start();
        // Start the thread that polls the hardware
        //StatusMonitor::getInstance()->setDataPointsToControlMode();

        //StatusMonitor::getInstance()->setStationLibrariesSynchronised(true);
        //StatusMonitor::getInstance()->setTrainLibrariesSynchronised(true);

        FUNCTION_EXIT;
    }

    void PIDSManager::setToMonitorMode()
    {
        FUNCTION_ENTRY( "setToMonitorMode" );

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "PIDSManager switching to Monitor Mode" );

        // Dont set the PID datapoints to Monitor mode if this is the OCC agent
        // the PIDs are not real datapoints and are used to establish the address
        // of the station PIDs when sending display requests
        /*		if( m_locationType != TA_Base_Core::ILocation::OCC )
        		{
        			std::vector< TA_IRS_App::DataPointDetails* >::iterator dpIter;
        			for ( dpIter = m_pidVirtualDataPointList.begin();
        				  dpIter != m_pidVirtualDataPointList.end();
        				  dpIter++ )
        			{
        				try
        				{
        					const_cast<DataPoint*>((*dpIter)->dataPoint)->setToMonitorMode();
        				}
        				catch( ... )
        				{
        					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "The PIDSManager caught an exception while attempting to switch a datapoint to Monitor Mode");
        				}
        			}
        		}

        StatusMonitor::getInstance()->setDataPointsToMonitorMode();
        */
//		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "PIDSManager Asking the Communications Handler to disconnect from the PIDS Server");

//		m_communicationsHandler.disconnectFromServer();

        // only need to deactivate the servant - dont need to delete it
        deactivateServant();

        FUNCTION_EXIT;
    }

    void PIDSManager::setEntityName( std::string entityName )
    {
        m_entityName = entityName;
    }

	/*
    void PIDSManager::subscribeAtsPidUpdateMsg( unsigned long locationKey )
    {
        //subscribe to AtsPidsUpdate message
        m_tainAgentSubscriber.subscribeToMessages( locationKey );
    }
	*/

    unsigned long PIDSManager::getNewSequenceNumber()
    {
        FUNCTION_ENTRY( "getNewSequenceNumber" );

        TA_Base_Core::ThreadGuard guard( m_sequenceNumberLock );

        // Ensure that the sequence number is in the acceptable range
        if ( m_sequenceNumber < 9999 )
        {
            m_sequenceNumber++;
        }
        else
        {
            // Start from the beginning again
            m_sequenceNumber = 1;
        }

        FUNCTION_EXIT;
        return m_sequenceNumber;
    }

    void PIDSManager::activate()
    {
        FUNCTION_ENTRY( "activate" );

        if ( m_entityName != "" )
        {
            activateServantWithName( m_entityName );
        }

        // TODO - throw an exception otherwise
        FUNCTION_EXIT;

    }

    std::string PIDSManager::getOperatorNameFromSession( const std::string& currentSessionID )
    {
        FUNCTION_ENTRY( "getOperatorNameFromSession" );

        std::string returnName( "Not available" );

        TA_Base_Core::ISession* operatorSession = NULL;

        try
        {
            operatorSession = TA_Base_Core::SessionAccessFactory::getInstance().getSession( currentSessionID );

            if ( NULL != operatorSession )
            {
                returnName = operatorSession->getOperatorName();

                delete operatorSession;
            }
        }
        catch ( const TA_Base_Core::DatabaseException& dbe )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", dbe.what() );
        }
        catch ( const TA_Base_Core::DataException& de )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", de.what() );
        }

        return returnName;

        FUNCTION_EXIT;
    }

    void PIDSManager::updateATSTrainInformation()
    {
        FUNCTION_ENTRY( "updateATSTrainInformation" );

        FUNCTION_EXIT;
    }

    std::string PIDSManager::getLocationName()
    {
        return m_locationName;
    }

    TA_Base_Core::ILocation::ELocationType PIDSManager::getLocationType()
    {
        return m_locationType;
    }

    unsigned long PIDSManager::getLocationKey()
    {
        return m_locationKey;
    }

    void PIDSManager::submitAuditMessage( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& desc, const char* sessionID )
    {
        m_auditMessageSender->sendAuditMessage( type,
                                                m_entityKey,
                                                desc,
                                                "", // Further description text
                                                sessionID,
                                                "", // alarm ID - not required
                                                "", // incident key - not required
                                                "" ); // event key - not required
    }

    void PIDSManager::submitAlarm( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& dp )
    {
        FUNCTION_ENTRY( "submitAlarm" );
        /*
                    if( type.getTypeKey() == TA_Base_Core::PIDSAlarms::PIDSServerCommFailure.getTypeKey() )
        		{
        			TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(type,
        				m_entityKey,
        				m_entityTypeKey,
        				dp,
        				m_entityName,
        				m_locationKey,
        				m_subsystemKey,
        				StatusMonitor::getInstance()->getAgentAssetName(),
        				TA_Base_Core::AlarmConstants::defaultTime );
        		}
        		// ++ TD9998

        		//TD10007
        		//zhou yuan++
        		else
        		{
                    // TES907 ~ added if statement

        			closeAlarm(type);

        			TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset(type,
        				m_entityKey,
        				m_entityTypeKey,
        				dp,
        				m_entityName,
        				m_locationKey,
        				m_subsystemKey,
        				StatusMonitor::getInstance()->getAgentAssetName(),
        				TA_Base_Core::AlarmConstants::defaultTime );
        		}
        		//++zhou yuan
                    FUNCTION_EXIT;
        */
    }

    void PIDSManager::closeAlarm( const TA_Base_Core::MessageType& type )
    {
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation( type, m_entityKey, m_locationKey );
    }

    // TD 16367
    void PIDSManager::sendPIDStatusChangedAuditMessage( const std::string& pid, const bool isOn, const std::string& sessionID )
    {
        if ( m_auditMessageSender == NULL )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Could not broadcast message to indicate incoming RAPIDS message" );
            return;
        }

        // Audit message content is:
        // The PID [PID] at Station [location] has been switched [on/off]
        TA_Base_Core::DescriptionParameters desc;
        TA_Base_Core::NameValuePair pidPair( PID_AUDIT_MESSAGE_PARAMETER_NAME, pid );
        // TD 9983
        TA_Base_Core::NameValuePair locationPair( PIDCONTROL_LOCATION_AUDIT_MESSAGE_PARAMETER_NAME, m_locationName );
        // TD 9983
        TA_Base_Core::NameValuePair statusPair( STATUS_AUDIT_MESSAGE_PARAMETER_NAME, isOn ? "on" : "off" );

        desc.push_back( &pidPair );
        desc.push_back( &locationPair );
        desc.push_back( &statusPair );

        /*
        m_auditMessageSender->sendAuditMessage( TA_Base_Core::PIDSAudit::PIDSPidStatusChanged, m_entityKey,
        	desc,
        	"", // Further description text
        	sessionID,
        	"", // alarm ID - not required
        	"", // incident key - not required
        	""); // event key - not required
        */
        /*		m_auditMessageSender->sendAuditMessageWithAsset( TA_Base_Core::PIDSAudit::PIDSPidStatusChanged, m_entityKey,
        			desc,
        			"", // Further description text
        			sessionID,
        			"", // alarm ID - not required
        			"", // incident key - not required
        			"",
        			StatusMonitor::getInstance()->getAgentAssetName()); // event key - not required
        */
    }

    unsigned long PIDSManager::getEntityKey()
    {
        return m_entityKey;
    }

    unsigned long PIDSManager::getEntityKeyType()
    {
        return m_entityTypeKey;
    }

    std::string PIDSManager::getEntityName()
    {
        return m_entityName;
    }

    unsigned long PIDSManager::getSubsystemKey()
    {
        return m_subsystemKey;
    }

    void PIDSFailoverThread::run()
    {
        /*		StatusMonitor::getInstance()->setDataPointsToControlMode();

        		StatusMonitor::getInstance()->setStationLibrariesSynchronised(true);
        		StatusMonitor::getInstance()->setTrainLibrariesSynchronised(true);
        */
    };

}
