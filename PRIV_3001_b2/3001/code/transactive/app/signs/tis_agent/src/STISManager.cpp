/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_agent/src/STISManager.cpp $
  * @author:   Robin Ashcroft
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  * Manages all Station Traveller Information System-related functionality
  *
  * Implements the ISTISManagerCorbaDef CORBA interface
  *
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#endif // _MSC_VER

#include "app/signs/tis_agent/src/AgentModeMonitor.h"
#include "app/signs/tis_agent/src/Exceptions.h"
#include "app/signs/tis_agent/src/STISAdHocDisplayRequest.h"
#include "app/signs/tis_agent/src/STISClearRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/STISOCCStatusRequest.h"
#include "app/signs/tis_agent/src/STISPIDControlRequest.h"
#include "app/signs/tis_agent/src/STISPredefinedDisplayRequest.h"
#include "app/signs/tis_agent/src/STISStationStatusRequest.h"
#include "app/signs/tis_agent/src/STISAllStationsStatusRequest.h"
#include "app/signs/tis_agent/src/STISUpgradePredefinedMessageLibraryRequest.h"
#include "app/signs/tis_agent/src/StatusMonitor.h"
#include "app/signs/tis_agent/src/StringHelper.h"
#include "app/signs/tis_agent/src/TISMessageDataDefinitionsHelper.h"
#include "app/signs/tis_agent/src/TTISManager.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/STISEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/SessionAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryTable.h"
#include "core/data_access_interface/tis_agent/src/StisTimeScheduleAccessFactory.h"
#include "core/data_access_interface/src/GlobalParameterAccessFactory.h"
#include "core/database/src/SimpleDb.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/CommsMessageSender.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/TISAlarms_MessageTypes.h"
#include "core/message/types/TISAudit_MessageTypes.h"
#include "core/message/types/TISComms_MessageTypes.h"
#include "core/threads/src/Thread.h"
#include "core/sftp/src/SFTPManager.h"
#include "core/sftp/src/SFTPException.h"
#include "bus/generic_agent/src/GenericAgent.h"
#include "app/signs/tis_agent/src/ILocationStore.h"
#include "app/signs/tis_agent/src/CCTOCCStatusRequest.h"
#include "app/signs/tis_agent/src/WriteLibraryToLocalDB.h"
#include "app/signs/tis_agent/src/StringConverter.h"
#include "app/signs/tis_agent/src/StringHelper.h"
#include "app/signs/tis_agent/src/GlobalConstant.h"
#include "app/signs/tis_agent/src/LibrarySyncFailedManager.h"

#include <string>
#include <ace/OS.h>
#include <boost/tokenizer.hpp>
#include <iomanip>
#include <sstream>

#define LCD_STATUS_DP_LIKE_NAME "LCD-Status"
#define LED_STATUS_DP_LIKE_NAME "LED-Status"

namespace TA_IRS_App
{

    using TA_Base_Bus::DataPoint;
    using TA_Base_Bus::DataPointState;
    using TA_Base_Bus::DpValue;
    using TA_Base_Core::DebugUtil;
	
    //
    // Initialise statics.
    //

    TA_Base_Core::ReEntrantThreadLockable STISManager::m_singletonLock;
    STISManager* STISManager::m_theClassInstance = NULL;


    //
    // getInstance
    //
    STISManager* STISManager::getInstance()
    {
        TA_Base_Core::ThreadGuard guard( m_singletonLock );

        // If guard acquired then make sure the singleton is still NULL.

        if ( NULL == m_theClassInstance )
        {
            // Create the one & only object.
            m_theClassInstance = new STISManager();
        }

        return m_theClassInstance;
    }

    STISManager::STISManager() :
        m_stationMessageLibrarySynchroniser( NULL ),
        m_trainMessageLibrarySynchroniser( NULL ),
        m_sequenceNumber( 0 ),
        m_messageLibraryManager( NULL ),
        m_auditMessageSender( NULL ),
        m_rightsLibrary( NULL ),
        m_stationLibrarySynchronisationInProgress( false ),
        m_trainLibrarySynchronisationInProgress( false ),
        m_stationLibraryUpgradeInProgress( false ),
        m_trainLibraryUpgradeInProgress( false ),
        m_tisMessageSender( NULL ),
		m_stisDBSyn( NULL ),
		m_ttisDBSyn( NULL ),
		m_stationMessageLibraryUpgrader(NULL),
		m_trainMessageLibraryUpgrader(NULL),
		m_librarySynFailedManager(NULL),
		m_isPaTisSyncBroadcastIgnoreVersion(true)
    {
        TA_Base_Bus::RightsLibraryFactory   rlf;
        m_rightsLibrary = rlf.buildRightsLibrary();

        m_auditMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::TISAudit::Context );
        m_tisMessageSender = TA_Base_Core::MessagePublicationManager::getInstance().getCommsMessageSender( TA_Base_Core::TISComms::Context );

		m_stisDBSyn = new StisLibraryLocalDBWriter();
		m_ttisDBSyn = new TtisLibraryLocalDBWriter();

		try
		{
			std::auto_ptr<IGlobalParameter> param(TA_Base_Core::GlobalParameterAccessFactory::getInstance().getGlobalParameterByNameAndSubsystem("PaTisSynBroadcastIgnoreVersion", 17));
			m_isPaTisSyncBroadcastIgnoreVersion = (param->getValue() != "0");
		}
		catch ( ... )
		{
			
		}
    }

    STISManager::~STISManager()
    {
        FUNCTION_ENTRY( "~STISManager" );
        if( m_auditMessageSender )
        {
            delete m_auditMessageSender;
            m_auditMessageSender = NULL;
        }

		if( m_stisDBSyn )
		{
			delete m_stisDBSyn;
			m_stisDBSyn = NULL;
		}

		if( m_ttisDBSyn )
		{
			delete m_ttisDBSyn;
			m_ttisDBSyn = NULL;
		}
		TA_IRS_Core::StisTimeScheduleAccessFactory::getInstance().removeInstance();
        TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

        FUNCTION_EXIT;
    }


	void STISManager::submitTimeScheduleDownloadRequest(const char* sessionID)
	{
		FUNCTION_ENTRY("submitTimeScheduleDownloadRequest");
		TA_Base_Bus::GenericAgent::ensureControlMode();
		if(isConnectedToSTISServer())
		{
			std::vector<TA_IRS_Core::StisTimeScheduleData*> timeSchedules;
			try
			{
				timeSchedules = TA_IRS_Core::StisTimeScheduleAccessFactory::getInstance().getStisTimeSchedules();
			}
			catch ( TA_Base_Core::DatabaseException& )
			{
				TA_IRS_Core::StisTimeScheduleAccessFactory::removeInstance();
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", "");
				throw TA_Base_Core::DatabaseErrorException();
			}
			catch ( TA_Base_Core::DataException& )
			{
				TA_IRS_Core::StisTimeScheduleAccessFactory::removeInstance();
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", "");
				throw TA_Base_Core::DatabaseErrorException();
			}
			TA_IRS_Core::StisTimeScheduleAccessFactory::removeInstance();

			std::stringstream ss;
			try
			{
				int i = 0;
				for( std::vector<TA_IRS_Core::StisTimeScheduleData*>::iterator iter = timeSchedules.begin();
					iter != timeSchedules.end();
					++iter)
				{
					TA_IRS_Core::StisTimeScheduleData* data = (*iter);
					if(true == data->isRecordEmtpy())
					{
						continue;
					}
					// id from 1
					ss << std::setw(2) << std::setfill('0') << (++i) << ";";
					// starttime and endtime
					ss << data->getStartTime() << ";" << data->getEndTime() << ";";
					// priority
					ss << data->getPriority() << ";";
					// display mode
					ss << TISMessageDataDefinitionsHelper::convertDisplayModeToChar(data->getDisplayMode()) << ';';
					// scroll speed
					ss << TISMessageDataDefinitionsHelper::convertScrollSpeedToChar(data->getScrollSpeed()) << ';';
					// repeat interval
					ss << std::setw(3) << std::setfill('0') << data->getRepeatInterval() << ";";
					// lcd font size and colour
					ss << TISMessageDataDefinitionsHelper::convertLcdFontSizeToChar(data->getLcdFontSize()) << ';';
					ss << data->getLcdFontColour() << ';';
					// led font size and colour
					ss << TISMessageDataDefinitionsHelper::convertLedFontSizeToChar(data->getLedFontSize()) << ';';
					ss << TISMessageDataDefinitionsHelper::convertLedFontColourToChar(data->getLedFontColour()) << ';';
					// display time
					ss << std::setw(3) << std::setfill('0') << data->getDisplayTime() << ";";
					// justification
					ss << data->getJustification() << ";";
					// free text
					ss << data->getTextContent() << "\n";
				}
				std::string tmp = ss.str();
				ss.clear();
				ss.str("");
				ss << "[No of Records]=" << i <<"\n" << "[List of Records]\n";
				ss << tmp;
			}
			catch ( TA_Base_Core::DatabaseException& )
			{
				for (std::vector<TA_IRS_Core::StisTimeScheduleData*>::iterator iter = timeSchedules.begin();
					iter != timeSchedules.end();
					++iter)
				{
					delete (*iter);
				}
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", "");
				throw TA_Base_Core::DatabaseErrorException();
			}
			catch ( TA_Base_Core::DataException& )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", "");
				for (std::vector<TA_IRS_Core::StisTimeScheduleData*>::iterator iter = timeSchedules.begin();
					iter != timeSchedules.end();
					++iter)
				{
					delete (*iter);
				}
				throw TA_Base_Core::DatabaseErrorException();
			}
			for (std::vector<TA_IRS_Core::StisTimeScheduleData*>::iterator iter = timeSchedules.begin();
				iter != timeSchedules.end();
				++iter)
			{
				delete (*iter);
			}

			try
			{
				// Save it to local path
				std::string dstFileLocation = SFTP_HOME + TIME_SCHEDULE_UPLOAD_FOLDER + TIME_SCHEDULE_FILE_NAME;
				std::ofstream of(TIME_SCHEDULE_FILE_NAME.c_str(), std::ofstream::trunc);
				if(false == of.is_open())
				{
					throw TA_Base_Core::TransactiveException("Fail to open file to write");
				}
				of << ss.rdbuf();
				of.close();

				TA_Base_Core::SFTPManager::getInstance().sendFile(m_stisSftpIPAddress, m_stisSftpUserName, m_stisSftpUserPassword, TIME_SCHEDULE_FILE_NAME, dstFileLocation, m_stisSftpPort);
			    submitOCCStatusRequestMessage(true);
				// Timeschedule event not have value
				TA_Base_Core::NameValuePair n1("STISTimeScheduleDownload"," ");
				TA_Base_Core::DescriptionParameters desc;
				desc.push_back(&n1);
                m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISTimeScheduleDownload,
					                                    m_entityKey,
                                                        desc,
                                                        "", // Further description text
                                                        sessionID,
                                                        "", // alarm ID - not required
                                                        "", // incident key - not required
                                                        "" ); // event key - not required
			}
	        catch ( const TA_Base_Core::SFTPException& sftpex )
	        {
	            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, sftpex.what() );
			    throw TA_Base_Bus::ISTISManagerCorbaDef::STISGeneralFailureException( "Caught an SFTPException while attempt to send the timeschedule file to server");
	        }
			catch (const TA_Base_Core::TransactiveException& e)
			{
	            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, e.what());
				throw TA_Base_Bus::ISTISManagerCorbaDef::STISGeneralFailureException( "Caught an file exception while attempt to write local timeschedule file" );
			}
			catch ( ... )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Unknown error");
				throw TA_Base_Bus::ISTISManagerCorbaDef::STISGeneralFailureException( "Caught an unknown exception while attempt to send timeschedule file to server" );
			}
		}
		else
		{
			FUNCTION_EXIT;
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
		}
		FUNCTION_EXIT;
	}

	void STISManager::submitPredefinedDisplayRequest2( const TA_Base_Core::STISDestinationList& destinationList,
												 TA_Base_Core::ELibrarySection librarySection,
												 CORBA::ULong libraryVersion,
												 unsigned short messageTag,
												 CORBA::ULong startTime, CORBA::ULong endTime,
												 CORBA::UShort priority, const char* sessionID )
	{
        FUNCTION_ENTRY( "submitPredefinedDisplayRequest2" );
		time_t s = startTime, e = endTime;
		struct tm* stm = localtime(&s);
		std::string startTimeStr, endTimeStr;
		std::stringstream ss;
		ss << std::setw(4) << std::setfill('0') << (stm->tm_year + 1900);
		ss << std::setw(2) << std::setfill('0') << stm->tm_mon + 1;
		ss << std::setw(2) << std::setfill('0') << stm->tm_mday;
		ss << std::setw(2) << std::setfill('0') << stm->tm_hour;
		ss << std::setw(2) << std::setfill('0') << stm->tm_min;
		ss << std::setw(2) << std::setfill('0') << stm->tm_sec;
		startTimeStr = ss.str();
		struct tm* etm = localtime(&e);
		ss.clear();
		ss.str("");
		ss << std::setw(4) << std::setfill('0') << (etm->tm_year + 1900);
		ss << std::setw(2) << std::setfill('0') << etm->tm_mon + 1;
		ss << std::setw(2) << std::setfill('0') << etm->tm_mday;
		ss << std::setw(2) << std::setfill('0') << etm->tm_hour;
		ss << std::setw(2) << std::setfill('0') << etm->tm_min;
		ss << std::setw(2) << std::setfill('0') << etm->tm_sec;
		endTimeStr = ss.str();
		if(m_isPaTisSyncBroadcastIgnoreVersion)
		{
			libraryVersion = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion();
		}
		submitPredefinedDisplayRequest(destinationList, librarySection, libraryVersion, messageTag, startTimeStr.c_str(), endTimeStr.c_str(), priority, sessionID);
		FUNCTION_EXIT;
	}

    void STISManager::submitPredefinedDisplayRequest( const TA_Base_Core::STISDestinationList& destinationList, TA_Base_Core::ELibrarySection librarySection,
            CORBA::ULong libraryVersion, unsigned short messageTag, const char* startTime, const char* endTime, CORBA::UShort priority, const char* sessionID )
    {
        FUNCTION_ENTRY( "submitPredefinedDisplayRequest" );

        TA_Base_Bus::GenericAgent::ensureControlMode();
		unsigned long currentISCSStationLibraryVersion = 0, currentSTISStationLibraryVersion = 0;
        if( isConnectedToSTISServer() )
        {
            currentISCSStationLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion();

            // Check that the calling client has the same library version to current iscs library verison
            if( libraryVersion == currentISCSStationLibraryVersion )
            {
				currentSTISStationLibraryVersion = StatusMonitor::getInstance()->getCurrentSTISStationLibraryVersion();
				// and current iscs library version equal to current stis station library verison
                if( currentISCSStationLibraryVersion == currentSTISStationLibraryVersion )
                {
                    TA_Base_Core::STISDestinationList occDestinationList;
                    int remoteStationSequenceLength = 0;

                    for( unsigned long i = 0; i < destinationList.length(); i++ )
                    {
                        std::string station = destinationList[i].station.in();
                        // Station agent send other stations' display request to OCC TisAgent
                        if ( ( m_locationType != TA_Base_Core::ILocation::OCC ) &&
                                ( station != m_locationName ) )
                        {
                            occDestinationList.length( ++remoteStationSequenceLength );
                            occDestinationList[remoteStationSequenceLength-1] = destinationList[i];
                        }
                        else
                        {
                            // Create the new display request object
                            STISPredefinedDisplayRequest* displayRequest = new STISPredefinedDisplayRequest();

                            std::string operatorName = getOperatorNameFromSession( std::string( sessionID ) );

                            // Set the data members
                            displayRequest->setData( destinationList[i], librarySection, libraryVersion, messageTag, startTime, endTime, priority, operatorName );

                            displayRequest->setSequenceNumber( getNewSequenceNumber() );

                            // Send the message to the STIS server
                            // This method blocks until the response message has been received
                            // or the response is not received within the configured time period

                            try
                            {
                                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M10 Predefined Display Request message" );
                                sendMessageWithWait( displayRequest );
                            }
                            catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
                            {
                                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                                delete displayRequest;

                                FUNCTION_EXIT;
                                throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
                            }
                            catch ( const TA_IRS_Core::TISAgentException& tae )
                            {
                                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                                delete displayRequest;

                                FUNCTION_EXIT;
                                throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( tae.what() );
                            }

                            // Clean up and throw an exception if the response was not received
                            if( false == displayRequest->hasReceivedResponse() )
                            {
                                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                                delete displayRequest;

                                FUNCTION_EXIT;
                                throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException();
                            }

                            // No need to process the response for this type of message
                            // the fact that a response was received is satisfactory

                            if( true == displayRequest->receivedNackResponse() )
                            {
                                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                                delete displayRequest;

                                FUNCTION_EXIT;
                                throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Invalid message sent to STIS server" );
                            }
                            else // Received response
                            {
                                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                                     "Received A10 response message" );

                                //Defect ID (9981) - PISF/TST/446
                                timeb loggedTime;
                                StringHelper::isConvertToTimeb( displayRequest->getTimestamp(), loggedTime );

                                std::string locationStr( destinationList[i].station.in() );

                                TA_Base_Core::IPredefinedMessageLibrary* STISMessageLibrary =
                                    TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary( currentISCSStationLibraryVersion,
                                            TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS );

                                TA_Base_Core::IPredefinedMessageLibrary::PredefinedMessageList predefinedMessages;
                                TA_Base_Core::DisplayAttributes defaultSTISAttributes;
								defaultSTISAttributes = STISMessageLibrary->getDefaultSTISAttributes();
                                predefinedMessages = STISMessageLibrary->getMessages();

                                std::vector <TA_Base_Core::PredefinedMessage*>::iterator iter = predefinedMessages.begin();
                                TA_Base_Core::PredefinedMessage* selectedPredefinedMessage;

                                selectedPredefinedMessage = NULL;
                                for ( iter = predefinedMessages.begin(); iter < predefinedMessages.end(); iter ++ )
                                {
                                    if ( ( *iter )->librarySection == librarySection && ( *iter )->messageTag == messageTag )
                                    {
                                        selectedPredefinedMessage = ( *iter );
                                        break;
                                    }
                                }

                                TA_ASSERT( NULL != selectedPredefinedMessage, "Selected Predefined Message cannot be found." );

								// Title id [message description]
                                TA_Base_Core::DescriptionParameters desc;
								// value should be blank
								TA_Base_Core::NameValuePair blank("blank", " ");
								desc.push_back(&blank);

                                TA_Base_Core::NameValuePair titleId( EVENT_PARAMETER_TITLE_ID, selectedPredefinedMessage->description );
                                desc.push_back( &titleId);

								// Start time and end time
                                std::string startTimeStr( startTime );
                                std::string endTimeStr( endTime );
								TA_Base_Core::NameValuePair startTimeValue( EVENT_PARAMETER_START_DATE_TIME, StringHelper::convertTimeFormat(startTime));
								TA_Base_Core::NameValuePair endTimeValue( EVENT_PARAMETER_END_DATE_TIME, StringHelper::convertTimeFormat(endTimeStr));
                                desc.push_back( &startTimeValue );
                                desc.push_back( &endTimeValue );

								// Priority
                                std::stringstream ss;
                                ss << selectedPredefinedMessage->priority;
                                TA_Base_Core::NameValuePair priorityValue( EVENT_PARAMETER_PRIORITY, ss.str() );
                                desc.push_back( &priorityValue );

								// STN PID pair sequence, eg. DT17 LED1, DT17 LED2
								ss.clear();
								ss.str("");
								ss << StringHelper::generateStnPidPairsString(getLocationDisplayName(locationStr), destinationList[i].pids);
                                TA_Base_Core::NameValuePair stnPidPairs( EVENT_PARAMETER_STN_PID_INFO, ss.str() );
                                desc.push_back( &stnPidPairs);

                                m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISPredefinedMessage, m_entityKey,
                                                                        desc,
                                                                        "", // Further description text
                                                                        sessionID,
                                                                        "", // alarm ID - not required
                                                                        "", // incident key - not required
                                                                        "", // event key - not required
                                                                        NULL, // data filter - not required
                                                                        loggedTime );   //same time as been logged

                                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                                delete displayRequest;
                            }
                        }
                    }

                    if ( remoteStationSequenceLength > 0 )
                    {
                        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                                     "Sending other station Predefined display request to OCC agent." );
                        try
                        {
                            CORBA_CALL( TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent(),
                                        submitPredefinedDisplayRequest, (
                                            occDestinationList,
                                            librarySection,
                                            libraryVersion,
                                            messageTag,
                                            startTime,
                                            endTime,
                                            priority,
                                            sessionID ) );//limin
                        }
                        // catch non-corba exceptions
                        // exceptions specd in this IDL method are allowed through
                        catch( TA_Base_Core::TransactiveException& te )
                        {
                            // error resolving OCC TIS agent
                            LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
                            throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Unable to send remote station display requests to OCC TIS agent." );
                        }
                    }

                    // TODO - make one mother audit message with all stations in it
                }
                else
                {
				    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Agent use version: %d, different to STIS version: %d", currentISCSStationLibraryVersion, currentSTISStationLibraryVersion);
                    throw TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException( "STIS server has a different predefined message library version to the TISAgent" );
                }

            }
            else
            {
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Client use version: %d, different to agent version: %d", libraryVersion, currentISCSStationLibraryVersion);
                throw TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException( "The display request does not use the current message library." );
            }

        }
        else
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Predefined display request message not sent - not connected to STIS server" );
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
        }

        FUNCTION_EXIT;
    }

	void STISManager::submitAdHocDisplayRequest(const ::TA_Base_Core::STISDestinationList& destinationList,
												const char* messageContent,
												const char* startTime,
												const char* endTime,
												::CORBA::UShort priority,
												const ::TA_Base_Core::DisplayAttributes& displayAttributes,
												const ::TA_Base_Core::LCDAttributesSTIS& lcdAttributes,
												const ::TA_Base_Core::LEDAttributes& ledAttributes,
												const char* sessionID)
    {
        FUNCTION_ENTRY( "submitAdHocDisplayRequest" );

        TA_Base_Bus::GenericAgent::ensureControlMode();
        if( isConnectedToSTISServer() )
        {
            TA_Base_Core::STISDestinationList occDestinationList;
            int remoteStationSequenceLength = 0;

            for( unsigned long i = 0; i < destinationList.length(); i++ )
            {
                std::string station = destinationList[i].station.in();
                // Station agent send other stations' display request to OCC TisAgent
                if ( ( m_locationType != TA_Base_Core::ILocation::OCC ) &&
                        ( station != m_locationName ) )
                {
                    occDestinationList.length( ++remoteStationSequenceLength );
                    occDestinationList[remoteStationSequenceLength-1] = destinationList[i];
                }
                else
                {
					// Create display request
                    STISAdHocDisplayRequest* displayRequest = new STISAdHocDisplayRequest();
                    // Set the data members
                    std::string operatorName = getOperatorNameFromSession( std::string( sessionID ) );
                    displayRequest->setData( destinationList[i], messageContent, startTime, endTime, priority, displayAttributes, lcdAttributes, ledAttributes, operatorName );
                    displayRequest->setSequenceNumber( getNewSequenceNumber() );

                    // Send the message to the STIS server
                    // This method blocks until the response message has been received
                    // or the response is not received within the configured time period
                    try
                    {
                        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M11 Adhoc Display Request message" );
                        sendMessageWithWait( displayRequest );
                    }
                    catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
                    {
                        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                        delete displayRequest;

                        FUNCTION_EXIT;
                        throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
                    }
                    catch ( const TA_IRS_Core::TISAgentException& tae )
                    {
                        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                        delete displayRequest;

                        FUNCTION_EXIT;
                        throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( tae.what() );
                    }
                    // Clean up and throw an exception if the response was not received
                    if( !displayRequest->hasReceivedResponse() )
                    {
                        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                        delete displayRequest;

                        FUNCTION_EXIT;
                        throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException();
                    }

                    // No need to process the response for this type of message
                    // the fact that a response was received is satisfactory

                    if( displayRequest->receivedNackResponse() )
                    {
                        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                        delete displayRequest;

                        FUNCTION_EXIT;
                        throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Invalid message sent to STIS server" );
                    }
                    else
                    {
                        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                             "Received A10 response message" );
                        timeb loggedTime;
                        StringHelper::isConvertToTimeb( displayRequest->getTimestamp(), loggedTime );

                        std::string messageContentStr( messageContent );
                        std::string locationStr( destinationList[i].station.in() );

                        TA_Base_Core::DescriptionParameters desc;
						// Value should be blank, add a not existed name value pair
						TA_Base_Core::NameValuePair blank("blank", " ");
						desc.push_back(&blank);

						// Message content
                        TA_Base_Core::NameValuePair content( EVENT_PARAMETER_MESSAGE_CONTENT, messageContentStr);
                        desc.push_back( &content);

						// Start time and end time
                        std::string startTimeStr( startTime );
                        std::string endTimeStr( endTime );
						TA_Base_Core::NameValuePair startTimeValue( EVENT_PARAMETER_START_DATE_TIME, StringHelper::convertTimeFormat(startTime));
						TA_Base_Core::NameValuePair endTimeValue( EVENT_PARAMETER_END_DATE_TIME, StringHelper::convertTimeFormat(endTimeStr));
                        desc.push_back( &startTimeValue );
                        desc.push_back( &endTimeValue );

						// Priority
                        std::stringstream ss;
                        ss << priority;
                        TA_Base_Core::NameValuePair priorityValue( EVENT_PARAMETER_PRIORITY, ss.str() );
                        desc.push_back( &priorityValue );

						// STN PID pair sequence, eg. DT17 LED1, DT17 LED2
						ss.clear();
						ss.str("");
						ss << StringHelper::generateStnPidPairsString(getLocationDisplayName(locationStr), destinationList[i].pids);
                        TA_Base_Core::NameValuePair stnPidPairs( EVENT_PARAMETER_STN_PID_INFO, ss.str() );
                        desc.push_back( &stnPidPairs);

                        m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISFreeTextMessage, m_entityKey,
                                                                desc,
                                                                "", // Further description text
                                                                sessionID,
                                                                "", // alarm ID - not required
                                                                "", // incident key - not required
                                                                "", // event key - not required
                                                                NULL, // data filter - not required
                                                                loggedTime ); // same time as been logged

                        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                        delete displayRequest;
                    }
                }
            } // For end

            if ( remoteStationSequenceLength > 0 )
            {
                try
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                                 "Sending other station Adhoc display request to OCC agent." );
                    CORBA_CALL( TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent(), submitAdHocDisplayRequest, (
                                    occDestinationList,
                                    messageContent,
                                    startTime,
                                    endTime,
                                    priority,
                                    displayAttributes,
                                    lcdAttributes,
                                    ledAttributes,
                                    sessionID ) );//limin
                }
                // catch non-corba exceptions
                // exceptions specd in this IDL method are allowed through
                catch( TA_Base_Core::TransactiveException& te )
                {
                    // error resolving OCC TIS agent
                    LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
                    throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Unable to send remote station display requests to OCC TIS agent." );
                }
            }

            // TODO - make one mother audit message with all stations in it
        }
        else
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Adhoc display request message not sent - not connected to STIS server" );
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
        }

        FUNCTION_EXIT;
    }


    void STISManager::submitClearRequest( const TA_Base_Core::STISDestinationList& destinationList,
                                          CORBA::UShort highPriority, CORBA::UShort lowPriority, const char* sessionID )
    {
        FUNCTION_ENTRY( "submitClearRequest" );

        TA_Base_Bus::GenericAgent::ensureControlMode();
        if( isConnectedToSTISServer() )
        {
            TA_Base_Core::STISDestinationList occDestinationList;
            int remoteStationSequenceLength = 0;

            for( unsigned long i = 0; i < destinationList.length(); i++ )
            {
                std::string station = destinationList[i].station.in();

                // if the destination station isnt this one
                // and this isnt the occ
                if ( ( m_locationType != TA_Base_Core::ILocation::OCC ) &&
                        ( station != m_locationName ) )
                {
                    // add this to the sequence that will be sent
                    // to the occ tis agent

                    // increase the sequence length
                    remoteStationSequenceLength++;
                    occDestinationList.length( remoteStationSequenceLength );

                    // add the destination to the sequence
                    occDestinationList[remoteStationSequenceLength-1] = destinationList[i];
                }
                else
                {
                    // request for this station

                    // Create the new display request object
                    STISClearRequest* displayRequest = new STISClearRequest();

                    std::string operatorName = getOperatorNameFromSession( std::string( sessionID ) );

                    // Set the data members
                    displayRequest->setData( destinationList[i], highPriority, lowPriority);

                    displayRequest->setSequenceNumber( getNewSequenceNumber() );

                    // Send the message to the STIS server
                    // This method blocks until the response message has been received
                    // or the response is not received within the configured time period
                    try
                    {
                        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M20 Clear Request message" );
                        sendMessageWithWait( displayRequest );
                    }
                    catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
                    {
                        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                        delete displayRequest;

                        FUNCTION_EXIT;
                        throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
                    }
                    catch ( const TA_IRS_Core::TISAgentException& tae )
                    {
                        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                        delete displayRequest;

                        FUNCTION_EXIT;
                        throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( tae.what() );
                    }

                    // Clean up and throw an exception if the response was not received
                    if( !displayRequest->hasReceivedResponse() )
                    {
                        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                        delete displayRequest;

                        FUNCTION_EXIT;
                        throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException();
                    }

                    // No need to process the response for this type of message
                    // the fact that a response was received is satisfactory

                    if( displayRequest->receivedNackResponse() )
                    {
                        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                        delete displayRequest;

                        FUNCTION_EXIT;
                        throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Invalid message sent to STIS server" );
                    }
                    else
                    {
                        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                             "Received A20 response message" );

                        std::string locationStr( destinationList[i].station.in() );

                        // Generate an audit message to capture the operator action
                        TA_Base_Core::DescriptionParameters desc;
						// Message type
						std::stringstream ss;
                        if ( lowPriority == 6 && highPriority == 1 )
                        {
                            ss << "All Message";
                        }
                        else if ( lowPriority == 3 && highPriority == 1 )
                        {
                            ss << "Emergency Message";
                        }
                        else if ( lowPriority == 6 && highPriority == 4 )
                        {
                            ss << "Normal Message";
                        }
						else
						{
							ss << "Unknown Message";
						}

						TA_Base_Core::NameValuePair blank("blank", " ");
						desc.push_back(&blank);

                        TA_Base_Core::NameValuePair clearType( EVENT_PARAMETER_CLEAR_TYPE, ss.str());
                        desc.push_back( &clearType);

						TA_Base_Core::NameValuePair stnPidPairs( EVENT_PARAMETER_STN_PID_INFO,
                            StringHelper::generateStnPidPairsString(getLocationDisplayName(locationStr), destinationList[i].pids));
                        desc.push_back( &stnPidPairs);


                        m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISResetMessage, m_entityKey,
                                                                desc,
                                                                "", // Further description text
                                                                sessionID,
                                                                "", // alarm ID - not required
                                                                "", // incident key - not required
                                                                "" ); // event key - not required

                        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                        delete displayRequest;
                    }
                }
            }

            if ( remoteStationSequenceLength > 0 )
            {
                try
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                                 "Sending other station clear request to OCC agent." );

                    // this clear request is for a different station
                    // send it to the occ agent
                    CORBA_CALL( TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent(), submitClearRequest, (
                                    occDestinationList,
                                    highPriority,
                                    lowPriority,
                                    sessionID ) );//limin
                }
                // catch non-corba exceptions
                // exceptions specd in this IDL method are allowed through
                catch( TA_Base_Core::TransactiveException& te )
                {
                    // error resolving OCC TIS agent
                    LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
                    throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Unable to send remote station clear request to OCC TIS agent." );
                }
            }
        }
        else
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Clear request message not sent - not connected to STIS server" );
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
        }

        FUNCTION_EXIT;
    }

    void STISManager::submitUpgradePredefinedMessageRequest( unsigned long libraryVersion )
    {
        FUNCTION_ENTRY( "submitUpgradePredefinedMessageRequest" );

        TA_Base_Bus::GenericAgent::ensureControlMode();
        if( isConnectedToSTISServer() )
        {
            // Create the new display request object
            STISUpgradePredefinedMessageLibraryRequest* displayRequest = new STISUpgradePredefinedMessageLibraryRequest();

            // Set the data members
            displayRequest->setData( libraryVersion );

            displayRequest->setSequenceNumber( getNewSequenceNumber() );

            // Send the message to the STIS server
            // This method blocks until the response message has been received
            // or the response is not received within the configured time period
            try
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M70 Upgrade Predefined Message Request" );
                sendMessageWithWait( displayRequest );
            }
            catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
            {
                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                delete displayRequest;

                FUNCTION_EXIT;
                throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
            }
            catch ( const TA_IRS_Core::TISAgentException& tae )
            {
                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                delete displayRequest;

                FUNCTION_EXIT;
                throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( tae.what() );
            }

            // Clean up and throw an exception if the response was not received
            if( !displayRequest->hasReceivedResponse() )
            {
                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                delete displayRequest;

                FUNCTION_EXIT;
                throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException();
            }

            // No need to process the response for this type of message
            // the fact that a response was received is satisfactory

            if( displayRequest->receivedNackResponse() )
            {
                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                delete displayRequest;

                FUNCTION_EXIT;
                throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Invalid message sent to STIS server" );
            }
            else
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                     "Received A20 response message" );

                // No need to log this type of message

                // or process the response

                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                delete displayRequest;
            }
        }
        else
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Clear request message not sent - not connected to STIS server" );
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
        }

        FUNCTION_EXIT;
    }

    void STISManager::upgradePredefinedStationMessageLibrary( CORBA::ULong newLibraryVersion, const char* sessionID )
    {
        FUNCTION_ENTRY( "upgradePredefinedStationMessageLibrary" );


        TA_Base_Bus::GenericAgent::ensureControlMode();
        try
        {
            if( m_locationType == TA_Base_Core::ILocation::OCC )
            {
                //TD16479, hongzhi, submit event upon begin STIS Library Update
                std::stringstream versionStream;
                std::stringstream sessionIDStream( sessionID );
                TA_Base_Core::DescriptionParameters desc;
                TA_Base_Core::NameValuePair versionVP( LIBRARY_VERSION_AUDIT_MESSAGE_PARAMETER_NAME, "" );
                versionStream << newLibraryVersion;
                versionVP.value = versionStream.str();
                desc.push_back( &versionVP );

                //hongran++ TD17500
                // upgrade the occ local library version first
                upgradeOCCStationLibraryVersion( newLibraryVersion );
                //++hongran TD17500
                if( stationLibraryUpgradeInProgress() )
                {
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                         "A station library upgrade is already in progress, client request to upgrade ignored" );
                    return;
                }
                else
                {
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                         "OCC Agent attempting to upgrade station libraries" );

                    // initiate the library upgrade - tells the upgrader thread to do its thang
                    upgradeStationLibrary( sessionID );

                    STISManager::getInstance()->closeAlarm( TA_Base_Core::TISAlarms::StationLibraryPendingUpgrade );
                }
            }
            else
            {
                unsigned long currentLocalISCSStationLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion();
                unsigned long nextLocalISCSStationLibraryVersion = StatusMonitor::getInstance()->getNextISCSStationLibraryVersion();

                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                     "TD17500 Station recieved the station lib upgrade request. current version is : [%d], next version is [%d]", currentLocalISCSStationLibraryVersion, nextLocalISCSStationLibraryVersion );

                if( currentLocalISCSStationLibraryVersion != nextLocalISCSStationLibraryVersion )
                {
                    StatusMonitor::getInstance()->setCurrentISCSStationLibraryVersion( nextLocalISCSStationLibraryVersion );

                    //removed by hongran. This table can not be operated on station because it is a mview
                    // delete the previous message library and messages from the LOCAL STATION database
                    //                TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().deleteAllMessageLibrariesOfTypeNotMatching(
                    //                    "STIS", nextISCSStationLibraryVersion, nextISCSStationLibraryVersion );
                }
                else
                {
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                         "OCC has requested a predefined STIS message library upgrade. Current and next library version are the same - no upgrade required" );
                }
            }
        }
        catch ( const TA_Base_Core::DatabaseException& ex )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "DatabaseException in STISManager::upgradePredefinedStationMessageLibrary: %s", ex.what() );
        }
        catch ( const TA_Base_Core::DataException& ex )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "DataException in STISManager::upgradePredefinedStationMessageLibrary: %s", ex.what() );
        }
        catch ( const TA_Base_Core::TransactiveException& ex )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "TransactiveException in STISManager::upgradePredefinedStationMessageLibrary:%s", ex.what() );
        }
        catch ( const CORBA::Exception&)
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "CORBA::Exception in  STISManager::upgradePredefinedStationMessageLibrary:" );
        }
        catch ( ... )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "Unknow exception in STISManager::upgradePredefinedStationMessageLibrary." );
        }

        // Generate an audit message to capture the operator action
        // this method can only be invoked as a result of direct operator action
        // at the OCC


        FUNCTION_EXIT;
    }

    void STISManager::upgradePredefinedTrainMessageLibrary( CORBA::ULong newLibraryVersion, const char* sessionID  )
    {
        FUNCTION_ENTRY( "upgradePredefinedTrainMessageLibrary" );

        TA_Base_Bus::GenericAgent::ensureControlMode();
        StisNamedObject *stisNamedObject = NULL;

        TA_Base_Core::CorbaNameList stisNameList;

        try
        {

            if( m_locationType == TA_Base_Core::ILocation::OCC )
            {
                // upgrade the occ local library version first
                upgradeOCCTrainLibraryVersion( newLibraryVersion );

                if( trainLibraryUpgradeInProgress() )
                {
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                         "A station library upgrade is already in progress, client request to upgrade ignored" );
                    return;
                }
                else
                {
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                         "OCC Agent attempting to upgrade train libraries" );

                    // initiate the library upgrade - tells the upgrader thread to do its thing
                    upgradeTrainLibrary( sessionID );

                    STISManager::getInstance()->closeAlarm( TA_Base_Core::TISAlarms::TrainLibraryPendingUpgrade );
                }
            }
            else
            {
                unsigned long currentISCSTrainLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion();
                unsigned long nextISCSTrainLibraryVersion = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion();

                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                     "The Upgrade station train lib is called. the current is [%d], next is [%d]", currentISCSTrainLibraryVersion, nextISCSTrainLibraryVersion );
                if( currentISCSTrainLibraryVersion != nextISCSTrainLibraryVersion )
                {
                    StatusMonitor::getInstance()->setCurrentISCSTrainLibraryVersion( nextISCSTrainLibraryVersion );

                    //removed by hongran. This table can not be operated on station because it is a mview
                    // delete the previous message library and messages from the LOCAL STATION database
					/*
					TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().deleteAllMessageLibrariesOfTypeNotMatching(
						"TTIS", nextISCSTrainLibraryVersion, nextISCSTrainLibraryVersion );
					*/
                }
                else
                {
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                         "OCC has requested a predefined TTIS message library upgrade. Current and next library version are the same - no upgrade required" );
                }
            }
        }
        catch ( const TA_Base_Core::DatabaseException& ex )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "DatabaseException in STISManager::upgradePredefinedTrainMessageLibrary: %s", ex.what() );
        }
        catch ( const TA_Base_Core::DataException& ex )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "DataException in STISManager::upgradePredefinedTrainMessageLibrary: %s", ex.what() );
        }
        catch ( const TA_Base_Core::TransactiveException& ex )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "TransactiveException in STISManager::upgradePredefinedTrainMessageLibrary:%s", ex.what() );
        }
        catch ( const CORBA::Exception& )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "CORBA::Exception in  STISManager::upgradePredefinedTrainMessageLibrary:" );
        }
        catch ( ... )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "Unknow exception in STISManager::upgradePredefinedTrainMessageLibrary." );
        }

        FUNCTION_EXIT;
    }

    TA_Base_Core::CurrentDisplayingMessage* STISManager::getCurrentDisplayingMessage( const TA_Base_Core::STISDestination& destination )
    {
        FUNCTION_ENTRY( "getCurrentDisplayMessage" );

        TA_Base_Bus::GenericAgent::ensureControlMode();
        TA_Base_Core::CurrentDisplayingMessage* currentDisplayingMessage;

        if( isConnectedToSTISServer() )
        {
            std::string station = destination.station.in();

            if ( ( m_locationType != TA_Base_Core::ILocation::OCC ) &&
                    ( station != m_locationName ) )
            {
                try
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                                 "Sending other station current displaying message request to OCC agent." );

                    // this request is for a different station
                    // send it to the occ agent
                    TA_Base_Core::CurrentDisplayingMessage_var returnedValue;
                    CORBA_CALL_RETURN( returnedValue, TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent(),
                                       getCurrentDisplayingMessage, ( destination ) );//limin

                }
                // catch non-corba exceptions
                // exceptions specd in this IDL method are allowed through
                catch( TA_Base_Core::TransactiveException& te )
                {
                    // error resolving OCC TIS agent
                    LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
                    throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Unable to send other station message request to OCC TIS agent." );
                }
            }
            else
            {
                // Create the new display request object
                STISCurrentDisplayingMessageRequest displayRequest;

                // Ensure that the station name is not empty
                if( ( destination.station != "" ) && ( destination.pids.length() == 1 ) )
                {
                    int pidAddress = getPIDAddress( std::string( destination.pids[0] ), std::string( destination.station ) );

                    // Set the data members
                    displayRequest.setData( std::string( destination.station ), pidAddress );

                    displayRequest.setSequenceNumber( getNewSequenceNumber() );

                    // Send the message to the STIS server
                    // This method blocks until the response message has been received
                    // or the response is not received within the configured time period
                    try
                    {
                        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M50 Current Displaying Message Request message" );
                        sendMessageWithWait( &displayRequest );
                    }
                    catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
                    {
                        deleteMessageFromPendingList( displayRequest.getSequenceNumber() );

                        FUNCTION_EXIT;
                        throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
                    }
                    catch ( const TA_IRS_Core::TISAgentException& tae )
                    {
                        deleteMessageFromPendingList( displayRequest.getSequenceNumber() );

                        FUNCTION_EXIT;
                        throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( tae.what() );
                    }

                    // Clean up and throw an exception if the response was not received
                    if( !displayRequest.hasReceivedResponse() )
                    {
                        deleteMessageFromPendingList( displayRequest.getSequenceNumber() );

                        FUNCTION_EXIT;
                        throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException();
                    }

                    if( displayRequest.receivedNackResponse() )
                    {
                        deleteMessageFromPendingList( displayRequest.getSequenceNumber() );

                        FUNCTION_EXIT;
                        throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Invalid message sent to STIS server" );
                    }
                    else
                    {
                        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                             "Received A50 response message" );

                        displayRequest.processResponse();

                        TA_Base_Core::CurrentDisplayingMessage temp = displayRequest.getMessage();

                        currentDisplayingMessage = new TA_Base_Core::CurrentDisplayingMessage();
                        currentDisplayingMessage->messageContent = temp.messageContent;
                        currentDisplayingMessage->startTime = temp.startTime;
                        currentDisplayingMessage->endTime = temp.endTime;
                        currentDisplayingMessage->priority = temp.priority;

                        // TODO assign temp to return struct

                        deleteMessageFromPendingList( displayRequest.getSequenceNumber() );
                    }
                }
                else
                {
                    // TODO - throw an exception - invalid parameters
                }
            }

        }
        else
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Current Displaying message request not sent - not connected to STIS server" );
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
        }

        FUNCTION_EXIT;

        return currentDisplayingMessage;
    }


    bool STISManager::isValid()
    {
        FUNCTION_ENTRY( "isValid" );

        FUNCTION_EXIT;
        return true;
    }

    void STISManager::start()
    {
        FUNCTION_ENTRY( "start" );


        if( m_locationType == TA_Base_Core::ILocation::OCC )
        {
            // Create this object - the thread will be run elsewhere
            m_stationMessageLibrarySynchroniser = new MessageLibrarySynchroniser( m_stationLibrarySynchronisationTimeout, TA_IRS_App::STATION_LIBRARY );

            m_trainMessageLibrarySynchroniser = new MessageLibrarySynchroniser( m_stationLibrarySynchronisationTimeout, TA_IRS_App::TRAIN_LIBRARY );

            m_stationMessageLibraryUpgrader = new MessageLibraryUpgrader( m_stationLibrarySynchronisationTimeout, TA_IRS_App::STATION_LIBRARY );

            m_trainMessageLibraryUpgrader = new MessageLibraryUpgrader( m_stationLibrarySynchronisationTimeout, TA_IRS_App::TRAIN_LIBRARY );

            m_stationMessageLibrarySynchroniser->start();
            m_trainMessageLibrarySynchroniser->start();

            m_stationMessageLibraryUpgrader->start();
            m_trainMessageLibraryUpgrader->start();

            // Initialise the message library manager worker thread
            m_messageLibraryManager = new MessageLibraryManager( m_stationMessageLibraryFilename,
                    m_stationMessageLibraryPath,
                    m_trainMessageLibraryFilename,
                    m_trainMessageLibraryPath,
					m_stisSftpIPAddress,
					m_stisSftpPort,
					m_stisSftpUserName,
					m_stisSftpUserPassword,
					m_cctSftpIPAddress,
					m_cctSftpPort,
					m_cctSftpUserName,
					m_cctSftpUserPassword );
        }

        // always runs - it will just sit on the semaphore until it receives a message
        m_statusRequestQueue.start();
		m_statusProcessQueue.start();

        FUNCTION_EXIT;

    }

    void STISManager::stop()
    {
        FUNCTION_ENTRY( "stop" );

        m_statusRequestQueue.terminateAndWait();
		m_statusProcessQueue.terminateAndWait();

        if( m_stationMessageLibrarySynchroniser != NULL )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager asking the station message library synchronisation thread to terminate..." );
            m_stationMessageLibrarySynchroniser->terminateAndWait();

            delete m_stationMessageLibrarySynchroniser;
            m_stationMessageLibrarySynchroniser = NULL;
        }

        if( m_trainMessageLibrarySynchroniser != NULL )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager asking the train message library synchronisation thread to terminate..." );
            m_trainMessageLibrarySynchroniser->terminateAndWait();

            delete m_trainMessageLibrarySynchroniser;
            m_trainMessageLibrarySynchroniser = NULL;
        }

        if( m_stationMessageLibraryUpgrader != NULL )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager asking the station message library upgrade thread to terminate..." );
            m_stationMessageLibraryUpgrader->terminateAndWait();

            delete m_stationMessageLibraryUpgrader;
            m_stationMessageLibraryUpgrader = NULL;
        }

        if( m_trainMessageLibraryUpgrader != NULL )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager asking the train message library upgrade thread to terminate..." );
            m_trainMessageLibraryUpgrader->terminateAndWait();

            delete m_trainMessageLibraryUpgrader;
            m_trainMessageLibraryUpgrader = NULL;
        }


        delete m_messageLibraryManager;
        m_messageLibraryManager = NULL;

        // Disconnect from the STIS server
        m_communicationsHandler.disconnectFromServer();
		// Disconnect from the CCT server
		m_cctCommunicationsHandler.disconnectFromServer();

        deactivateServant();
        FUNCTION_EXIT;
    }


    void STISManager::update( const TA_Base_Core::ConfigUpdateDetails& configUpdateDetails )
    {
        FUNCTION_ENTRY( "update" );

        FUNCTION_EXIT;
    }


    void STISManager::remove()
    {
        FUNCTION_ENTRY( "remove" );

        FUNCTION_EXIT;
    }


    void STISManager::setToControlMode()
    {
        FUNCTION_ENTRY("setToControlMode");
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager switching to Control Mode");
		StatusMonitor::getInstance()->setDataPointsToControlMode();

		m_librarySynFailedManager->startAllProcessors();
        // check and process if any virtual DataPoints have been changed
        //processDataPointStateManuallyUpdate();

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager asking the Communications Handler to connect to the STIS Server" );

        // Attempt to connect to the STIS server
        // If we're not at the depot
        if( m_locationType != TA_Base_Core::ILocation::DPT )
        {
            m_communicationsHandler.connectToServer( m_serverIPAddress, m_serverPort );
        }

		if( m_locationType == TA_Base_Core::ILocation::OCC )
		{
			m_cctCommunicationsHandler.connectToServer(m_cctIPAddress, m_cctPort );
		}

        if( m_entityName.compare( "" ) )
            activateServantWithName( m_entityName );


        //chenlei-+ moved this procedure into a new thread
        STISFailoverThread* failoverloadThread = new STISFailoverThread;
        failoverloadThread->start();
        // Start the thread that polls the hardware
        //StatusMonitor::getInstance()->setDataPointsToControlMode();

        //StatusMonitor::getInstance()->setStationLibrariesSynchronised(true);
        //StatusMonitor::getInstance()->setTrainLibrariesSynchronised(true);

        FUNCTION_EXIT;
    }

    void STISManager::setToMonitorMode()
    {
        FUNCTION_ENTRY( "setToMonitorMode" );

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager switching to Monitor Mode" );

		m_librarySynFailedManager->stopAllProcessors();

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Setting datapoints to Monitor Mode");
        StatusMonitor::getInstance()->setDataPointsToMonitorMode();

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager Asking the Communications Handler to disconnect from the STIS Server" );

        m_communicationsHandler.disconnectFromServer();
		m_cctCommunicationsHandler.disconnectFromServer();

		m_statusRequestQueue.clearQueue();
		m_statusProcessQueue.clearQueue();
        // only need to deactivate the servant - dont need to delete it
        deactivateServant();

        FUNCTION_EXIT;
    }

    void STISManager::initialise( TA_Base_Core::IEntityDataPtr EntityData )
    {
        FUNCTION_ENTRY( "initialise" );

        m_stisEntityData = boost::dynamic_pointer_cast<TA_Base_Core::STISEntityData> ( EntityData );

        TA_ASSERT( m_stisEntityData.get() != NULL, "STISManager entity data is incorrect" );

        m_entityKey = m_stisEntityData->getKey();
        m_entityTypeKey = m_stisEntityData->getTypeKey();
        m_locationKey = m_stisEntityData->getLocation();
        m_locationName = m_stisEntityData->getLocationName();
        m_subsystemKey = m_stisEntityData->getSubsystem();
        m_serverIPAddress = m_stisEntityData->getServerIPAddress();
        m_serverPort = m_stisEntityData->getServerPort();
		// set CCT ip address and port
		m_cctIPAddress = m_stisEntityData->getCCTIPAddress();
		m_cctPort = m_stisEntityData->getCCTPort();
		// stis sftp related parameter value
		m_stisSftpIPAddress = m_stisEntityData->getStisSftpIPAddress();
		m_stisSftpPort = m_stisEntityData->getStisSftpPort();
		m_stisSftpUserName = m_stisEntityData->getStisSftpUserName();
		m_stisSftpUserPassword = m_stisEntityData->getStisSftpPassword();
		// cct sftp related parameter value
		m_cctSftpIPAddress = m_stisEntityData->getCCTSftpIPAddress();
		m_cctSftpPort = m_stisEntityData->getCCTSftpPort();
		m_cctSftpUserName = m_stisEntityData->getCCTSftpUserName();
		m_cctSftpUserPassword = m_stisEntityData->getCCTSftpPassword();

        m_messageTimeout = m_stisEntityData->getMessageTimeout();
        m_messageRetries = m_stisEntityData->getMessageRetries();
        m_entityName = m_stisEntityData->getName();
        m_stationLibrarySynchronisationTimeout = m_stisEntityData->getStationLibrarySynchronisationTimeout();
        m_localDBString = m_stisEntityData->getLocalDBConnectionString();

		unsigned short repeatInterval = atoi(m_stisEntityData->getHelper()->getParameter("RepeatInterval").c_str());

        // Init m_pidShortNameToFullNameMap data
        try
        {
            std::vector<TA_Base_Core::IEntityData*> entityData;
            std::vector<std::string> nameParts;
			// Need also init m_pidShortNameToAddressMap, so get datapoint entity
            entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken(
                             TA_Base_Core::DataPointEntityData::getStaticType(),LCD_STATUS_DP_LIKE_NAME);
            for(std::vector<TA_Base_Core::IEntityData*>::iterator it = entityData.begin(); it != entityData.end(); ++it)
            {
                std::string pidName = (*it)->getName();
                tokenizePIDName(pidName, nameParts);
                if(nameParts.size() == 5)
                {
    				std::string pidShortName = nameParts[0] + nameParts[3];
    				std::string address = (*it)->getAddress();
    				m_pidShortNameToAddressMap.insert(std::make_pair(pidShortName, atoi(address.c_str())));
                }
                delete *it;
            }
            entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken(
                             TA_Base_Core::DataPointEntityData::getStaticType(),LED_STATUS_DP_LIKE_NAME);
            for(std::vector<TA_Base_Core::IEntityData*>::iterator it = entityData.begin(); it != entityData.end(); ++it)
            {
                std::string pidName = (*it)->getName();
                tokenizePIDName(pidName, nameParts);
                if(nameParts.size() == 5)
                {
    				std::string pidShortName = nameParts[0] + nameParts[3];
    				std::string address = (*it)->getAddress();
    				m_pidShortNameToAddressMap.insert(std::make_pair(pidShortName, atoi(address.c_str())));
                }
                delete *it;
            }
        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed get pid name data from database");
        }
        m_locationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType( m_locationName );
        if( m_locationType == TA_Base_Core::ILocation::OCC )
        {
			m_librarySynFailedManager = new OccLibrarySyncFailedManager(repeatInterval);
        }
		else
		{
			m_librarySynFailedManager = new LibrarySyncFailedManager;
		}

        activateServantWithName( m_entityName );
        FUNCTION_EXIT;
    }

    unsigned long STISManager::getNewSequenceNumber()
    {
        FUNCTION_ENTRY( "getNewSequenceNumber" );

        TA_Base_Core::ThreadGuard guard( m_sequenceNumberLock );

        // Ensure that the sequence number is in the acceptable range
        if( m_sequenceNumber < 9999 )
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

    void STISManager::activate()
    {
        FUNCTION_ENTRY( "activate" );

        if( m_entityName != "" )
        {
            activateServantWithName( m_entityName );
        }

        // TODO - throw an exception otherwise
        FUNCTION_EXIT;

    }

    std::string STISManager::getOperatorNameFromSession( const std::string& currentSessionID )
    {
        FUNCTION_ENTRY( "getOperatorNameFromSession" );

        std::string returnName( "Not available" );

        TA_Base_Core::ISession* operatorSession = NULL;

        try
        {
            operatorSession = TA_Base_Core::SessionAccessFactory::getInstance().getSession( currentSessionID );

            if( NULL != operatorSession )
            {
                returnName = operatorSession->getOperatorName();

                delete operatorSession;
            }
        }
        catch( const TA_Base_Core::DatabaseException& dbe )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", dbe.what() );
        }
        catch( const TA_Base_Core::DataException& de )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", de.what() );
        }
        FUNCTION_EXIT;
        return returnName;

    }

    void STISManager::processResponseMessage( const std::vector<unsigned char>& message )
    {
        FUNCTION_ENTRY( "processResponseMessage" );

        // Establish the sequence number in the response message
        int sequenceNumber;

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
             "Received a response message of some form" );

        std::stringstream responseStream;
        std::string sequenceNumberString;

        // Read the sequence number characters
        // The 'start of packet' byte is the first byte,
        // the following four are the sequence number (0000 - 9999)
        for( int i = 1 ; i <= 4 ; i++ )
        {
            responseStream << message[i];
        }
        sequenceNumberString = responseStream.str();
        sequenceNumber = atoi( sequenceNumberString.c_str() );

        // TODO - dont think I need to lock this
        TA_Base_Core::ThreadGuard guard( m_incomingMessageLock );
        bool foundAnOwner = false;

		for ( std::vector<STISMessage*>::iterator iter = m_pendingMessages.begin() ; iter < m_pendingMessages.end() ; iter++ )
        {
            if( ( *iter )->getSequenceNumber() == sequenceNumber )
            {
				// Found!
                foundAnOwner = true;

                // Read the Message ID to ensure this not a NACK message
                std::stringstream responseMessageIDStream;
                std::string responseMessageIDString;
                for( int i = 19 ; i <= 21 ; i++ )
                {
                    responseMessageIDStream << message[i];
                }
                responseMessageIDString = responseMessageIDStream.str();

				// If A99 set nack & break
                if( 0 == responseMessageIDString.compare( "A99" ) )
                {
                    std::string nackMessage;
                    nackMessage += "Received a NACK message for sequence number: ";
                    nackMessage += sequenceNumberString;
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
                         nackMessage.c_str() );
                    // this WAS a NACK message
                    ( *iter )->setResponse( message, true );
                    break;
                }
                else
                {
					// Following comment code check if the response message digit number
					// equal to message original digit number, and throw exception(should be)
					// But this method not called by STISManager, it can not catch the exception,
					// and do something, so it comment

	                // Grab the last two digits of the message ID
	                // of the original message, AND the response
					/*
	                std::stringstream responseMessageIDNumberPortion;
	                for( int i = 1 ; i <= 2 ; i++ )
	                {
	                    responseMessageIDNumberPortion << responseMessageIDString[i];
	                }
	                std::string responseIDNumber = responseMessageIDNumberPortion.str();

	                std::string messageIDString;
	                std::stringstream messageIDStream;
	                messageIDString = ( *iter )->getMessageID();
	                for( int i = 1 ; i <= 2 ; i++ )
	                {
	                    messageIDStream << messageIDString[i];
	                }
	                std::string messageIDNumber = messageIDStream.str();
					if(responseIDNumber != messageIDNumber)
					{
						//TODO throw exception.
					}
					*/

                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                         "Found matching message for response message" );

                    // Give the message request its response message
                    // this is NOT a NACK message
                    ( *iter )->setResponse( message, false );
                    break;
                }

            }
        }
        // If no matching message was found - log for debug purposes
        if( false == foundAnOwner )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "Received an unexpected response - discarding" );
        }

        FUNCTION_EXIT;
    }


    void STISManager::submitStationStatusRequestMessage()
    {
        if( isConnectedToSTISServer() )
        {
            // Create the new display request object
			STISStationStatusRequest* displayRequest = new STISStationStatusRequest(getLocationName());
            displayRequest->setSequenceNumber( getNewSequenceNumber() );
            m_statusRequestQueue.addMessageToQueue( displayRequest );
        }
    }


	void STISManager::submitCCTStatusRequest()
	{
		if(isConnectedToCCT())
		{
			CCTOCCStatusRequest* cctStatusRequest = new CCTOCCStatusRequest;
			cctStatusRequest->setSequenceNumber(getNewSequenceNumber());
			m_statusRequestQueue.addMessageToQueue( cctStatusRequest);
		}
	}

	void STISManager::submitCCTLibraryReceivedNotiyRequest(unsigned long libraryVersion)
	{
		if(isConnectedToCCT())
		{
			CCTLibraryReceivedNotifyRequest* cctLibraryReceived = new CCTLibraryReceivedNotifyRequest(libraryVersion);
			cctLibraryReceived->setSequenceNumber(getNewSequenceNumber());
			m_statusRequestQueue.addMessageToQueue( cctLibraryReceived);
		}
	}

    void STISManager::submitOCCStatusRequestMessage(bool isTimeScheduleValid)
    {
        if( isConnectedToSTISServer() )
        {
            // Create the new display request object
            STISOCCStatusRequest* displayRequest = new STISOCCStatusRequest(isTimeScheduleValid);
            displayRequest->setSequenceNumber( getNewSequenceNumber() );
            m_statusRequestQueue.addMessageToQueue( displayRequest );
        }
    }

    void STISManager::handleStatusRequestMessage( STISStatusRequest* displayRequest )
    {
        FUNCTION_ENTRY( "handleStatusRequestMessage" );
		bool isCCTConnected = isConnectedToCCT();
		bool isSTISConnected = isConnectedToSTISServer();
		bool isSentToSTIS = displayRequest->isMessageSentToSTIS();

        if(( isCCTConnected && (false == isSentToSTIS) ) || ( isSTISConnected && ( true == isSentToSTIS )))
        {
            // Send the message to the STIS server
            // This method blocks until the response message has been received
            // or the response is not received within the configured time period
            try
            {
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending %s Status Request message", isSentToSTIS?"STIS":"CCT" );
                sendMessageWithWait( displayRequest );
            }
            catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "STISServerNotConnectedException",
                                     "Status request message not sent - not connected to the STIS server" );
                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                delete displayRequest;
                FUNCTION_EXIT;
                return;
            }
			catch( const TA_Base_Bus::ISTISManagerCorbaDef::CCTNotConnectedException& )
			{
                LOG_EXCEPTION_CATCH( SourceInfo, "CCTServerNotConnectedException",
                                     "Status request message not sent - not connected to the CCT server" );
                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                delete displayRequest;
                FUNCTION_EXIT;
                return;
			}
            catch ( const TA_IRS_Core::TISAgentException& tae )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "TISAgentException", tae.what() );
                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                delete displayRequest;
                FUNCTION_EXIT;
                return;
            }

            // Log the message if it was sent and processed successfully
            if( !displayRequest->hasReceivedResponse() )
            {
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Timed out while waiting for status response from %s Server", isSentToSTIS?"STIS":"CCT" );
                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                delete displayRequest;
                FUNCTION_EXIT;
                return;
            }

            if( displayRequest->receivedNackResponse() )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid message sent to %s server", isSentToSTIS?"STIS":"CCT" );
                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
                delete displayRequest;
                FUNCTION_EXIT;
                return;
            }
            else
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Received response message" );
                // No need to log this type of message
                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
				m_statusProcessQueue.addMessageToQueue(displayRequest);
            }
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                         "Status request message not sent - not connected to the %s server", isSentToSTIS?"STIS":"CCT" );
        }

        FUNCTION_EXIT;
    }

    void STISManager::submitPIDControlRequest( const char* pidDestination, TA_Base_Core::EPIDControl controlCommand, const char* sessionID )
    {
        FUNCTION_ENTRY( "submitPIDControlRequest" );

        TA_Base_Bus::GenericAgent::ensureControlMode();//limin
        std::string destination = StringHelper::replaceStr<char>(pidDestination, "OCC_", "");

        // break the PID name up into parts
        std::vector<std::string> parts = tokenizePIDName( destination );

        if ( parts.size() != 4 )
        {
            // this is not a PID name
            std::stringstream error;
            error << "The PID name " << destination << " is not valid.";
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException( error.str().c_str() );
        }

        // the first part is the station name
		if(parts[0] != m_locationName && m_locationType != TA_Base_Core::ILocation::OCC)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                         "The PID is not on this station.  Ignoring." );
            FUNCTION_EXIT;
            return;
        }

        // the PID belongs to this station
        if( !isConnectedToSTISServer() )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "OCC status request message not sent - not connected to STIS server" );
            FUNCTION_EXIT;
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
        }

        // Create the new display request object
        STISPIDControlRequest* displayRequest = new STISPIDControlRequest();

        displayRequest->setData( destination, controlCommand );

        displayRequest->setSequenceNumber( getNewSequenceNumber() );

        // Send the message to the STIS server
        // This blocks until the message is sent (it will retry a number of times)
        // or until an exception is thrown
        try
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M21 PID Ctrl Request message" );
            sendMessageWithWait( displayRequest );
        }
        catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
        {
            deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
            delete displayRequest;

            FUNCTION_EXIT;
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
        }
        catch ( const TA_IRS_Core::TISAgentException& tae )
        {
            deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
            delete displayRequest;

            FUNCTION_EXIT;
            TA_THROW( TA_IRS_Core::TISAgentException( tae.what() ) );
        }

        // Log the message if it was sent and processed successfully
        if( !displayRequest->hasReceivedResponse() )
        {
            deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
            delete displayRequest;

            FUNCTION_EXIT;
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException();
        }

        if( displayRequest->receivedNackResponse() )
        {
            deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
            delete displayRequest;

            FUNCTION_EXIT;
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Invalid message sent to STIS server" );
        }
        else
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "Received A21 response message" );

            // No need to log this type of message

            // or process the response

            deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
            delete displayRequest;
        }

        sendPIDStatusChangedAuditMessage( destination, controlCommand ? TA_Base_Core::TURN_ON : TA_Base_Core::TURN_OFF, sessionID );

        FUNCTION_EXIT;
    }


    void STISManager::newMessageLibraryReady( unsigned long newVersion, TA_IRS_App::EMessageLibraryType type )
    {
        FUNCTION_ENTRY( "newMessageLibraryReady" );

        TA_ASSERT( m_locationType == TA_Base_Core::ILocation::OCC, "newMessageLibraryReady is only available at the OCC" );

        switch( type )
        {
            case TA_IRS_App::STATION_LIBRARY:

                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "New STIS Predefined message library ready. STISManager starting the STIS message library synchroniser thread..." );

                if( m_stationLibrarySynchronisationInProgress == false )
                {
                    m_stationMessageLibrarySynchroniser->initiateSynchronisation();
                }

                m_stationLibrarySynchronisationInProgress = true;

                break;

            case TA_IRS_App::TRAIN_LIBRARY:

                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "New TTIS Predefined message library ready. STISManager starting the TTIS message library synchroniser thread..." );

                if( m_trainLibrarySynchronisationInProgress == false )
                {
                    m_trainMessageLibrarySynchroniser->initiateSynchronisation();
                }

                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                     "hongran : m_trainLibrarySynchronisationInProgress is set to be null in newMessageLibraryReady" );
                m_trainLibrarySynchronisationInProgress = true;

                break;

        }

        FUNCTION_EXIT;
    }

    void STISManager::sendMessageWithWait( STISMessage* message )
    {
        FUNCTION_ENTRY( "sendMessageWithWait" );

        {
            TA_Base_Core::ThreadGuard guard( m_messageLock );

            // Add it to the list of pending messages
            m_pendingMessages.push_back( message );
        }

		bool isSentToSTIS = message->isMessageSentToSTIS();
		std::string serverStr = isSentToSTIS ? "STIS" : "CCT";
        // Create a TIS protocol message from the object's data members
        // the message will differ depending on the object
        std::vector<unsigned char> protocolMessage = message->createProtocolMessage();
		// Log the message data
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,  message->createLogMessage(protocolMessage).c_str());
        //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending message to STIS server...");

        int retries = 0;
        while( ( false == message->hasReceivedResponse() ) && ( retries < m_messageRetries ) )
        {
            if( retries > 0 )
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                     "Timed out while waiting for response, retrying..." );
            }

            try
            {
				if( true == isSentToSTIS )
				{
                    m_communicationsHandler.sendMessage( protocolMessage );
				}
				else
				{
					m_cctCommunicationsHandler.sendMessage( protocolMessage );
				}
            }
            catch( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Not connected to STIS server" );
                throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
            }
			catch( const TA_Base_Bus::ISTISManagerCorbaDef::CCTNotConnectedException& )
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Not connected to CCT server" );
                throw TA_Base_Bus::ISTISManagerCorbaDef::CCTNotConnectedException();
            }
            catch( ... )
            {
				std::ostringstream errorMsg;
				errorMsg << "Error sending message to ";
				errorMsg << serverStr;
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, errorMsg.str().c_str());
                TA_THROW( TA_IRS_Core::TISAgentException( errorMsg.str().c_str()));
            }

            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                 "Message sent to %s , waiting for response...", serverStr.c_str() );

            int timer = 0;
            while( ( timer < m_messageTimeout ) && ( false == message->hasReceivedResponse() ) )
            {
                TA_Base_Core::Thread::sleep( 250 );
                timer += 250;
            }

            retries++;
        }

        if( false == message->hasReceivedResponse() )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "Timed out while waiting for response, reached maximum retries - giving up." );
			if(true == message->isMessageSentToSTIS())
			{
                m_communicationsHandler.reconnect();
			}
			else //message sent to CCT
			{
				m_cctCommunicationsHandler.reconnect();
			}
        }

        FUNCTION_EXIT;
    }

    void STISManager::sendMessageWithoutWait( STISMessage* message )
    {
        FUNCTION_ENTRY( "sendMessageWithoutWait" );

        // Create a TIS protocol message from the object's data members
        // the message will differ depending on the object
        std::vector<unsigned char> protocolMessage = message->createProtocolMessage();

        //LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending message to STIS server...");

        int retries = 0;
        bool messageSent = false;

        while( ( retries < m_messageRetries ) && ( false == messageSent ) )
        {
            if( retries > 0 )
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                     "Could not send message, retrying..." );
            }

            try
            {
                m_communicationsHandler.sendMessage( protocolMessage );

                messageSent = true;

                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                     "Message sent to STIS server." );
            }
            catch( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Not connected to STIS server" );
                throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
            }
            catch( ... )
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Error sending message to STIS server" );
                TA_THROW( TA_IRS_Core::TISAgentException( "Error sending message to STIS server" ) );
            }

            retries++;
        }

        FUNCTION_EXIT;
    }

    void STISManager::deleteMessageFromPendingList( int sequenceNumber )
    {
        FUNCTION_ENTRY( "deleteMessageFromPendingList" );

        TA_Base_Core::ThreadGuard guard( m_messageLock );

        std::vector<STISMessage*>::iterator i;
        i = m_pendingMessages.begin();

        for ( ; i < m_pendingMessages.end() ; i++ )
        {
            if( ( *i )->getSequenceNumber() == sequenceNumber )
            {
                m_pendingMessages.erase( i );
                FUNCTION_EXIT;
                return;
            }

        }

        TA_ASSERT( false, "Message with specified sequence number not in pending list" );
        FUNCTION_EXIT;
    }

    bool STISManager::isConnectedToSTISServer()
    {

        return m_communicationsHandler.isConnected();
    }

	bool STISManager::isConnectedToCCT()
	{
		return m_cctCommunicationsHandler.isConnected();
	}

    CORBA::Boolean STISManager::reportStationLibraryVersionReceived( CORBA::ULong newVersion )
    {
        FUNCTION_ENTRY( "reportStationLibraryVersionReceived" );

        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        bool received = false;

        try
        {
            TA_Base_Core::IPredefinedMessageLibrary* stisMessageLibrary = NULL;

            // if on occ, then check the version in occ DB.
            if ( m_locationType == TA_Base_Core::ILocation::OCC )
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                     "@ Attempt to check station lib version [%d] in occ DB ",  newVersion );
                stisMessageLibrary = TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getOCCLocalPredefinedMessageLibrary(
                                         newVersion,
                                         TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS );
                if ( stisMessageLibrary == NULL )
                {
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                         "@ stisMessageLibrary is NULL " );
                }
                else
                {
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                         "@ stisMessageLibrary not null, size is : %d " , stisMessageLibrary->getKey() );
                }

                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                     "@ New lib version is exist in occ DB " );
            }
            else
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                     "@ Attempt to check station lib version [%d] in station DB ",  newVersion );
                stisMessageLibrary = TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary(
                                         newVersion,
                                         TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS );
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                     "@ New lib version is exist in station DB " );
            }
            //++hongran

            delete stisMessageLibrary;
            stisMessageLibrary = NULL;

            received = true;

            if( StatusMonitor::getInstance()->getNextISCSStationLibraryVersion() != newVersion )
            {
                // This method will update StatusMonitor::m_newStationLibraryVersion
                // when it examines the version in the database
                StatusMonitor::getInstance()->setNextISCSStationLibraryVersion( newVersion );
            }
        }
        catch( const TA_Base_Core::DataException& de )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", de.what() );
        }
        catch( const TA_Base_Core::DatabaseException& dbe )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", dbe.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "while attempting to report Station Library Version Received" );
        }

        FUNCTION_EXIT;
        return received;
    }

    CORBA::Boolean STISManager::reportTrainLibraryVersionReceived( CORBA::ULong newVersion )
    {
        FUNCTION_ENTRY( "reportTrainLibraryVersionReceived" );

        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        bool received = false;

        try
        {
            TA_Base_Core::IPredefinedMessageLibrary* ttisMessageLibrary = NULL;

            // if on occ, then check the version in occ DB.
            if ( m_locationType == TA_Base_Core::ILocation::OCC )
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                     "@ Attempt to check station lib version [%d] in occ DB ",  newVersion );
                ttisMessageLibrary = TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getOCCLocalPredefinedMessageLibrary(
                                         newVersion,
                                         TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_TTIS );
            }
            else
            {
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                     "@ Attempt to check station lib version [%d] in station DB ",  newVersion );
                ttisMessageLibrary = TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary(
                                         newVersion,
                                         TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_TTIS );
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                     "@ New lib version is exist in station DB " );
            }
            delete ttisMessageLibrary;
            ttisMessageLibrary = NULL;

            received = true;

            // This method will update StatusMonitor::m_newTrainLibraryVersion
            // when it examines the version in the database
            StatusMonitor::getInstance()->setNextISCSTrainLibraryVersion( newVersion );
        }
        catch( const TA_Base_Core::DataException& de )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DataException", de.what() );
        }
        catch( const TA_Base_Core::DatabaseException& dbe )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::DatabaseException", dbe.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "while attempting to report Station Library Version Received" );
        }

        FUNCTION_EXIT;
        return received;
    }

    void STISManager::processNewMessageLibrary( TA_IRS_App::EMessageLibraryType type, const std::string& filename, unsigned long indicatedVersion )
    {
        FUNCTION_ENTRY( "processNewMessageLibrary" );

		if(type == STATION_LIBRARY)
		{
			m_librarySynFailedManager->stopSTISSynchronise();
			m_librarySynFailedManager->stopSTISUpgrade();
		}
		else
		{
			m_librarySynFailedManager->stopTTISSynchronise();
			m_librarySynFailedManager->stopTTISUpgrade();
		}
        TA_ASSERT( m_messageLibraryManager != NULL, "m_messageLibraryManager is NULL" )
        m_messageLibraryManager->processNewMessageLibrary( type, filename, indicatedVersion );

        FUNCTION_EXIT;
    }

    CORBA::Boolean STISManager::isStationLibrarySynchronisationComplete()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        bool complete = StatusMonitor::getInstance()->getStationLibrariesSynchronised();

        return CORBA::Boolean( complete );
    }

    CORBA::Boolean STISManager::isTrainLibrarySynchronisationComplete()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        bool complete = StatusMonitor::getInstance()->getTrainLibrariesSynchronised();

        return CORBA::Boolean( complete );
    }

    std::string STISManager::getLocationName()
    {
        return m_locationName;
    }

    TA_Base_Core::ILocation::ELocationType STISManager::getLocationType()
    {
        return m_locationType;
    }

    unsigned long STISManager::getLocationKey()
    {
        return m_locationKey;
    }


    void STISManager::setBadDataPointQuality()
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
            "STIS Manager setting quality of PID datapoints to bad");
		StatusMonitor::getInstance()->setBadDataPointQuality();
    }

    int STISManager::getPIDAddress( const std::string& pidName, const std::string& station )
    {
        FUNCTION_ENTRY("getPIDAddress");
		std::string pidShortName = station + pidName;
		if(m_pidShortNameToAddressMap.find(pidShortName) != m_pidShortNameToAddressMap.end())
		{
			return m_pidShortNameToAddressMap[pidShortName];
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
			"Can't find the the pid address, pid name:%s, station:%s", pidName.c_str(), station.c_str());
		return 0;
    }

    CORBA::ULong STISManager::getCurrentSTISMessageLibraryVersion()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
             "Received client request for STIS Message library version" );
        return StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion();
    }

    CORBA::ULong STISManager::getCurrentTTISMessageLibraryVersion()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
             "Received client request for TTIS Message library version" );
        return StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion();
    }

    CORBA::ULong STISManager::getNextSTISMessageLibraryVersion()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
             "Received client request for next STIS Message library version" );
        return StatusMonitor::getInstance()->getNextISCSStationLibraryVersion();
    }

    CORBA::ULong STISManager::getNextTTISMessageLibraryVersion()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
             "Received client request for next TTIS Message library version" );
        return StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion();
    }

    CORBA::ULong STISManager::getCurrentCDBSTISMessageLibraryVersion()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
             "Received client request for Central DB STIS Message library version" );
        return StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion_central();
    }

    CORBA::ULong STISManager::getCurrentCDBTTISMessageLibraryVersion()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
             "Received client request for Central DB TTIS Message library version" );
        return StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion_central();
    }

	CORBA::ULong STISManager::getNextCDBSTISMessageLibraryVersion()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
             "Received client request for Central DB next STIS Message library version" );
        return StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central();
    }

    CORBA::ULong STISManager::getNextCDBTTISMessageLibraryVersion()
    {
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
             "Received client request for Central DB next TTIS Message library version" );
        return StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();
    }

    void STISManager::connectToServer()
    {
        FUNCTION_ENTRY( "connectToServer" );

        m_communicationsHandler.connectToServer( m_serverIPAddress, m_serverPort );

        FUNCTION_EXIT;
    }

    void STISManager::synchroniseStationLibrary()
    {
        //should do a checking before doing the initiate, since the member pointer may be NULL
        //
        if ( m_stationMessageLibrarySynchroniser != NULL )
        {
            TA_Base_Core::ThreadGuard guard( m_stationLibrarySynchronisationLock );

            m_stationLibrarySynchronisationInProgress = true;
            m_stationMessageLibrarySynchroniser->initiateSynchronisation();
        }
        else
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                 "Mintao(TD12342): Thread that used to examine Station library synchroniser is NULL" );
        }
    }

    void STISManager::synchroniseTrainLibrary()
    {
        //should do a checking before doing the initiate, since the member pointer may be NULL
        //
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
             "hongran : synchroniseTrainLibrary is called" );
        if ( m_trainMessageLibrarySynchroniser != NULL )
        {
            TA_Base_Core::ThreadGuard guard( m_trainLibrarySynchronisationLock );
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "hongran : m_trainLibrarySynchronisationInProgress is set to be null in synchroniseTrainLibrary" );

            m_trainLibrarySynchronisationInProgress = true;

            m_trainMessageLibrarySynchroniser->initiateSynchronisation();
        }
        else
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "Mintao(TD12342): Thread that used to examine Train library synchroniser is NULL" );
        }
    }

    void STISManager::messageLibrarySynchronised( TA_IRS_App::EMessageLibraryType type )
    {
        if( type == TA_IRS_App::STATION_LIBRARY )
        {
            TA_Base_Core::ThreadGuard guard( m_stationLibrarySynchronisationLock );
            m_stationLibrarySynchronisationInProgress = false;
			m_librarySynFailedManager->startSTISSynchronise();
        }
        else if( type == TA_IRS_App::TRAIN_LIBRARY )
        {
            TA_Base_Core::ThreadGuard guard( m_trainLibrarySynchronisationLock );
            m_trainLibrarySynchronisationInProgress = false;
			m_librarySynFailedManager->startTTISSynchronise();
        }
    }

    bool STISManager::stationLibrarySynchInProgress()
    {
        TA_Base_Core::ThreadGuard guard( m_stationLibrarySynchronisationLock );

        return m_stationLibrarySynchronisationInProgress;
    }

    bool STISManager::trainLibrarySynchInProgress()
    {
        TA_Base_Core::ThreadGuard guard( m_trainLibrarySynchronisationLock );

        return m_trainLibrarySynchronisationInProgress;
    }

    void STISManager::upgradeStationLibrary( const std::string& sessionID )
    {
        TA_Base_Core::ThreadGuard guard( m_stationLibraryUpgradeLock );

		m_librarySynFailedManager->stopSTISUpgrade();

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
             "Station library upgrade initiated" );
        m_stationLibraryUpgradeInProgress = true;
        m_stationMessageLibraryUpgrader->initiateUpgrade( sessionID );
		m_librarySynFailedManager->setSessionId(sessionID);
    }

    void STISManager::upgradeTrainLibrary( const std::string& sessionID )
    {
        TA_Base_Core::ThreadGuard guard( m_trainLibraryUpgradeLock );


		m_librarySynFailedManager->stopTTISUpgrade();

        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
             "Train library upgrade initiated" );
        m_trainLibraryUpgradeInProgress = true;
        m_trainMessageLibraryUpgrader->initiateUpgrade( sessionID );
		m_librarySynFailedManager->setSessionId(sessionID);
    }

    void STISManager::messageLibraryUpgraded( TA_IRS_App::EMessageLibraryType type )
    {
        if( type == TA_IRS_App::STATION_LIBRARY )
        {
            TA_Base_Core::ThreadGuard guard( m_stationLibraryUpgradeLock );
            m_stationLibraryUpgradeInProgress = false;
			m_librarySynFailedManager->startSTISUpgrade();
        }
        else if( type == TA_IRS_App::TRAIN_LIBRARY )
        {
            TA_Base_Core::ThreadGuard guard( m_trainLibraryUpgradeLock );
            m_trainLibraryUpgradeInProgress = false;
			m_librarySynFailedManager->startTTISUpgrade();
        }
    }

    bool STISManager::stationLibraryUpgradeInProgress()
    {
        TA_Base_Core::ThreadGuard guard( m_stationLibraryUpgradeLock );

        return m_stationLibraryUpgradeInProgress;
    }

    bool STISManager::trainLibraryUpgradeInProgress()
    {
        TA_Base_Core::ThreadGuard guard( m_trainLibraryUpgradeLock );

        return m_trainLibraryUpgradeInProgress;
    }

   void STISManager::tokenizePIDName(std::string pidName, std::vector<std::string>& parts)
    {
        parts.clear();
        // funky boost tokenizer - break name on '.'
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep(".");
        tokenizer tokens(pidName, sep);
        for(tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
        {
            parts.push_back(*tok_iter);
        }
        // if parts is empty, then this should return the entire string
        if(parts.size() == 0)
        {
            parts.push_back(pidName);
        }
    }

    std::vector<std::string> STISManager::tokenizePIDName( std::string pidName )
    {
        // funky boost tokenizer - break name on '.'
        std::vector<std::string> parts;
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

        boost::char_separator<char> sep( "." );
        tokenizer tokens( pidName, sep );

        for ( tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter )
        {
            parts.push_back( *tok_iter );
        }

        // if parts is empty, then this should return the entire string
        if ( parts.size() == 0 )
        {
            parts.push_back( pidName );
        }

        return parts;
    }

    void STISManager::submitAuditMessage( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& desc, const char* sessionID )
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

    void STISManager::submitAlarm( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& dp )
    {
        FUNCTION_ENTRY( "submitAlarm" );

        if( type.getTypeKey() == TA_Base_Core::TISAlarms::STISServerCommFailure.getTypeKey() )
        {
            TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset( type,
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

            closeAlarm( type );

            TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().submitAlarmWithAsset( type,
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
    }

    void STISManager::closeAlarm( const TA_Base_Core::MessageType& type )
    {
        TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation( type, m_entityKey, m_locationKey );
    }


    void STISManager::sendPIDStatusChangedAuditMessage( const std::string& pid, const bool isOn, const std::string& sessionID )
    {
        if ( m_auditMessageSender == NULL )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Could not send pid status change audit message" );
            return;
        }

        // Audit message content is:
        // The PID [PID] at Station [location] has been switched [on/off]
        TA_Base_Core::DescriptionParameters desc;
		// Blank value
		TA_Base_Core::NameValuePair blank("blank", " ");
		desc.push_back(&blank);

        TA_Base_Core::NameValuePair statusPair( EVENT_PARAMETER_PID_STATUS, isOn?"On":"Off" );
		desc.push_back(&statusPair);

		std::stringstream ss;
		ss << getLocationDisplayName(m_locationName) << " " << pid.substr(pid.find_last_of('.')+1);
		TA_Base_Core::NameValuePair stnPidPairs( EVENT_PARAMETER_STN_PID_INFO, ss.str() );
		desc.push_back(&stnPidPairs);
		// get pid assetname
		std::string pidAssetName;
		try
		{
			std::auto_ptr<TA_Base_Core::IData> parameterData(TA_Base_Core::EntityAccessFactory::getInstance().getParameter(pid, "AssetName"));
            if(parameterData->getNumRows() > 0)
            {
    			pidAssetName = parameterData->getStringData(0, "VALUE");
            }
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed get pid assetname");
		}
        m_auditMessageSender->sendAuditMessageWithAsset( TA_Base_Core::TISAudit::STISPidStatusChanged, m_entityKey,
                desc,
                "", // Further description text
                sessionID,
                "", // alarm ID - not required
                "", // incident key - not required
                "",
                pidAssetName);
    }


    //hongran++ TD17500
    void STISManager::upgradeOCCStationLibraryVersion( unsigned long version )
    {
        unsigned long currentISCSStationLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion();
        unsigned long nextISCSStationLibraryVersion = StatusMonitor::getInstance()->getNextISCSStationLibraryVersion();

        if( currentISCSStationLibraryVersion != nextISCSStationLibraryVersion )
        {
            StatusMonitor::getInstance()->setCurrentISCSStationLibraryVersion( nextISCSStationLibraryVersion );

            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "TD17500 Successfully  upgradeOCCStationLibraryVersion " );
        }
        else
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "OCC has requested a predefined STIS message library upgrade. Current and next library version are the same - no upgrade required" );
        }

    }


    void STISManager::upgradeOCCTrainLibraryVersion( unsigned long version )
    {
        unsigned long currentISCSTrainLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion();
        unsigned long nextISCSTrainLibraryVersion = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion();

        if( currentISCSTrainLibraryVersion != nextISCSTrainLibraryVersion )
        {
            StatusMonitor::getInstance()->setCurrentISCSTrainLibraryVersion( nextISCSTrainLibraryVersion );

            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "TD17500 Successfully  upgradeOCCTrainLibraryVersion " );
        }
        else
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                 "OCC has requested a predefined TTIS message library upgrade. Current and next library version are the same - no upgrade required" );
        }

    }

	CORBA::Boolean STISManager::saveSTISLibraryToLocalDB(::CORBA::ULong newversion, const TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType& content)
	{
		// If content is NULL pointer
		if( 0 == content.length() )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The STIS library content is empty.");
			return false;
		}

		if( m_stisDBSyn->getInicateVersion() != newversion )
		{
			if(m_stisDBSyn->isRunning())
			{
				m_stisDBSyn->terminateAndWait();
			}
			m_stisDBSyn->setLibraryData(newversion, StringConverter::corbaCharSequenceToVectorChar(content));
			m_stisDBSyn->start();
		}
		return true;
	}

	CORBA::Boolean STISManager::saveTTISLibraryToLocalDB(::CORBA::ULong newversion, const TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType& content)
	{
		// If content is NULL pointer
		if( 0 == content.length() )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "The TTIS library content is empty.");
			return false;
		}	

		if ( m_ttisDBSyn->getInicateVersion() != newversion )
		{
			if( m_ttisDBSyn->isRunning() )
			{
				m_ttisDBSyn->terminateAndWait();
			}
			m_ttisDBSyn->setLibraryData(newversion, StringConverter::corbaCharSequenceToVectorChar(content));
			m_ttisDBSyn->start();
		}
		return true;
	}

    void STISFailoverThread::run()
    {
        StatusMonitor::getInstance()->setDataPointsToControlMode();
        StatusMonitor::getInstance()->setStationLibrariesSynchronised( true );
        StatusMonitor::getInstance()->setTrainLibrariesSynchronised( true );
    };


	TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType* STISManager::getSTISLibraryDBContent(CORBA::ULong version)
	{
		FUNCTION_ENTRY("getSTISLibraryDBContent");
        TA_Base_Core::ThreadGuard guard( m_stisDBfilesLock);
		TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType_var 
			tmp(new TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType);
		if(m_stisDBfiles.find(version) == m_stisDBfiles.end())
		{
			//get current library binary content
			std::vector<unsigned char> dbFileContent;
			try
			{
				MessageLibrarySynchroniser::getLibraryFileFromDB( dbFileContent, version, STATION_LIBRARY);
			}
			catch( TA_Base_Core::TransactiveException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
			}
			catch( ... )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "...", "While attempting to retrieve the Station Predefined Message Library from the Database" );
			}
			if( false == dbFileContent.empty() )
			{
				m_stisDBfiles[version] = new TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType(StringConverter::vectorCharToCorbaCharSequence(dbFileContent));
				tmp = m_stisDBfiles[version];
			}
		}
		else
		{
			tmp = m_stisDBfiles[version];
		}
		FUNCTION_EXIT;
		return tmp._retn();
	}

	TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType* STISManager::getTTISLibraryDBContent(CORBA::ULong version)
	{
		FUNCTION_ENTRY("getTTISLibraryDBContent");
        TA_Base_Core::ThreadGuard guard( m_ttisDBfilesLock);
		TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType_var 
			tmp(new TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType);
		if(m_ttisDBfiles.find(version) == m_ttisDBfiles.end())
		{
			//get current library binary content
			std::vector<unsigned char> dbFileContent;
			try
			{
				MessageLibrarySynchroniser::getLibraryFileFromDB( dbFileContent, version, TRAIN_LIBRARY);
			}
			catch( TA_Base_Core::TransactiveException& e )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );
			}
			catch( ... )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "...", "While attempting to retrieve the Train Predefined Message Library from the Database" );
			}
			if( false == dbFileContent.empty() )
			{			
				m_ttisDBfiles[version] = new TA_Base_Bus::ISTISManagerCorbaDef::LibraryContentType(StringConverter::vectorCharToCorbaCharSequence(dbFileContent));
				tmp = m_ttisDBfiles[version];
			}
		}
		else
		{
			tmp = m_ttisDBfiles[version];
		}
		FUNCTION_EXIT;
		return tmp._retn();
	}

    std::string STISManager::getLocationDisplayName( const std::string& locationName )
    {
        std::string displayName;
        try
        {
            displayName = ILocationStore::getInstance()->getLocationByName(locationName)->getDisplayName();
        }
        catch(...)
        {
        }
        return displayName;
    }

    void STISManager::submitAllStationsStatusRequestMessage()
    {
        if(isConnectedToSTISServer())
        {
            // Create the new display request object
            STISAllStationsStatusRequest* request = new STISAllStationsStatusRequest;
            request->setSequenceNumber(getNewSequenceNumber());
            m_statusRequestQueue.addMessageToQueue(request);
        }
    }
}
