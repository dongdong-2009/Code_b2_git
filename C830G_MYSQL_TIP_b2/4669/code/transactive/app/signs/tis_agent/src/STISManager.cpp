/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File$
  * @author:   Robin Ashcroft 
  * @version:  $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
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
#include "app/signs/tis_agent/src/STISApprovedRATISDisplayRequest.h"
#include "app/signs/tis_agent/src/STISClearRequest.h"
#include "app/signs/tis_agent/src/STISConfigurationDataRequest.h"
#include "app/signs/tis_agent/src/STISManager.h"
#include "app/signs/tis_agent/src/STISOCCStatusRequest.h"
#include "app/signs/tis_agent/src/STISPIDControlRequest.h"
#include "app/signs/tis_agent/src/STISPredefinedDisplayRequest.h"
#include "app/signs/tis_agent/src/STISStationStatusRequest.h"
#include "app/signs/tis_agent/src/STISTrainInformationMessage.h"
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
#include "core/data_access_interface/tis_agent/src/DisplayRequestAccessFactory.h"
#include "core/data_access_interface/tis_agent/src/DisplayRequestData.h"
#include "core/data_access_interface/tis_agent/src/RATISMessageAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/src/IPredefinedMessageLibrary.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryAccessFactory.h"
#include "core/data_access_interface/tis_agent_4669/src/PredefinedMessageLibraryTable.h"
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

#include <ace/OS.h>

#include <boost/tokenizer.hpp>
#include "bus/generic_agent/src/GenericAgent.h"

#include <iomanip>
#include "app/signs/tis_agent/src/ILocationStore.h"


#define STATUS_DP_NAME "eiiTISC-Status"
#define LOCKSTATUS_DP_NAME "diiTISC-LockStatus"

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
#define RATISTAG_AUDIT_MESSAGE_PARAMETER_NAME "Ratis Tag"
#define ENABLE_STATUS_AUDIT_MESSAGE_PARAMETER_NAME "Enabled/Disabled"
#define STATUS_AUDIT_MESSAGE_PARAMETER_NAME "on/off"
#define RATISDESTINATION_AUDIT_MESSAGE_PARAMETER_NAME "Ratis destination"
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
	//TD18093,jianghp
	//TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().( TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper() ), 
	//TD18093,jianghp
	m_stationLibrarySynchronisationInProgress( false ), 
	m_trainLibrarySynchronisationInProgress( false ),
	m_stationLibraryUpgradeInProgress( false ), 
	m_trainLibraryUpgradeInProgress( false ),
	m_tisMessageSender( NULL ), 
	m_needVetting(true),
	m_raisedAtsForwardingFailureAlarm(false), //TD15198 azenitha
	m_nonUniqueAlarmHelper(TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper())
	{
		m_pidVirtualDataPointList.clear();

		TA_Base_Bus::RightsLibraryFactory   rlf;
		m_rightsLibrary = rlf.buildRightsLibrary();
		TA_ASSERT(m_rightsLibrary!=NULL,"Rights library not created.");

		m_auditMessageSender = 
				TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::TISAudit::Context );

		TA_ASSERT(m_auditMessageSender != NULL, "Could not create message publication manager");


		m_tisMessageSender = TA_Base_Core::MessagePublicationManager::getInstance()
            .getCommsMessageSender( TA_Base_Core::TISComms::Context);

        m_ratisMessenger.start();
	}

	STISManager::~STISManager()
	{
		FUNCTION_ENTRY("~STISManager");

		std::vector< TA_IRS_App::DataPointDetails* >::iterator dpIter;
		for ( dpIter = m_pidVirtualDataPointList.begin(); 
			  dpIter != m_pidVirtualDataPointList.end();
			  dpIter++ )
		{
			delete (*dpIter);
		}

		if( m_auditMessageSender )
		{
			delete m_auditMessageSender;
			m_auditMessageSender = NULL;
		}

		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();

		FUNCTION_EXIT;
	}

	TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageList* STISManager::getAllIncomingRATISMessages()
	{
		FUNCTION_ENTRY("getAllIncomingRATISMessages");


		TA_Base_Bus::GenericAgent::ensureControlMode();
		TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageList* ratisMessageSeq = new TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageList();
        ratisMessageSeq->length(0); // initialise it to 0 just in case there's no incoming message

        try
        {
		    TA_Base_Core::ThreadGuard guard( m_incomingRATISLock );
		    
		    std::vector<TA_IRS_Core::IRATISMessage*> messagesVec = 
			    TA_IRS_Core::RATISMessageAccessFactory::getInstance().getVettingRATISMessages();

            int size = messagesVec.size();
            if (size > 0)
            {
                ratisMessageSeq->length(size);

		        for( int i=0; i < size ; i++ )
		        {
			        (*ratisMessageSeq)[i].messageID = messagesVec[i]->getKey();
			        (*ratisMessageSeq)[i].sessionRef = messagesVec[i]->getSessionRef();
			        (*ratisMessageSeq)[i].startTime = CORBA::string_dup( messagesVec[i]->getStartTime().c_str() );
			        (*ratisMessageSeq)[i].endTime = CORBA::string_dup( messagesVec[i]->getEndTime().c_str() );
			        (*ratisMessageSeq)[i].requiresVetting = messagesVec[i]->getRequiresVetting();
			        (*ratisMessageSeq)[i].overridable = messagesVec[i]->getOverridable();
			        (*ratisMessageSeq)[i].type = ( TA_Base_Core::ERATISMessageType )messagesVec[i]->getType();
			        (*ratisMessageSeq)[i].status = ( TA_Base_Core::ERATISMessageStatus )messagesVec[i]->getStatus();
			        (*ratisMessageSeq)[i].timeCreated = messagesVec[i]->getCreationDate();
			        (*ratisMessageSeq)[i].destination = CORBA::string_dup( getFullDestinationString(messagesVec[i]->getDestination()).c_str() );
			        (*ratisMessageSeq)[i].tag = CORBA::string_dup( messagesVec[i]->getTag().c_str() );
			        (*ratisMessageSeq)[i].priority = messagesVec[i]->getPriority();
			        (*ratisMessageSeq)[i].messageText = CORBA::string_dup( messagesVec[i]->getMessageText().c_str() );
			        (*ratisMessageSeq)[i].isTTIS = messagesVec[i]->isForTTIS();
		        }

		        for( unsigned int j = 0; j < messagesVec.size() ; j++ )
		        {
			        delete messagesVec[j];
		        }
            }
        }
        catch (TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            // don't throw an exception if it didn't fetch any value
            if (ex.getFailType() != TA_Base_Core::DataException::NO_VALUE)
            {
    			throw TA_Base_Bus::ISTISManagerCorbaDef::STISGeneralFailureException("Failed to fetch all incoming RATIS messages from database");
            }
        }
        catch (...)
        {
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISGeneralFailureException("Failed to fetch all incoming RATIS messages from database");
        }

        return ratisMessageSeq; 

		FUNCTION_EXIT;
	}


	TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails* STISManager::getIncomingRATISMessage( CORBA::Long messageID ) 
	{
		FUNCTION_ENTRY("getIncomingRATISMessage");

		TA_Base_Bus::GenericAgent::ensureControlMode();
        try
        {
		    TA_Base_Core::ThreadGuard guard( m_incomingRATISLock );

		    TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails* messageDetails = new TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails();

		    TA_IRS_Core::IRATISMessage* message = 
			    TA_IRS_Core::RATISMessageAccessFactory::getInstance().getRATISMessage( messageID );

		    messageDetails->messageID = messageID;
		    messageDetails->sessionRef = message->getSessionRef();
		    messageDetails->startTime = CORBA::string_dup( message->getStartTime().c_str() );
		    messageDetails->endTime = CORBA::string_dup( message->getEndTime().c_str() );
		    messageDetails->requiresVetting = message->getRequiresVetting();
		    messageDetails->overridable = message->getOverridable();
		    messageDetails->type = (TA_Base_Core::ERATISMessageType)message->getType();
		    messageDetails->status = (TA_Base_Core::ERATISMessageStatus)message->getStatus();
		    messageDetails->timeCreated = message->getCreationDate();
		    messageDetails->destination = CORBA::string_dup( getFullDestinationString(message->getDestination()).c_str() );
		    messageDetails->tag = CORBA::string_dup( message->getTag().c_str() );
		    messageDetails->priority = message->getPriority();
			// TD17564
			if (message->getMessageText().empty())
			{
				std::string emptyString(" ");
				messageDetails->messageText = CORBA::string_dup(emptyString.c_str());
			}
			else
			{
				messageDetails->messageText = CORBA::string_dup( message->getMessageText().c_str() );
			}
			// TD17564
            messageDetails->isTTIS = message->isForTTIS();
		    delete message;

		    return messageDetails;
        }
        catch (...)
        {
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISGeneralFailureException("Failed to the incoming RATIS messages from database");
        }

		FUNCTION_EXIT;
	}

	void STISManager::submitPredefinedDisplayRequest( const TA_Base_Core::STISDestinationList& destinationList, TA_Base_Core::ELibrarySection librarySection,
		unsigned short libraryVersion, unsigned short messageTag, const char* startTime, const char* endTime, CORBA::UShort priority, const char* sessionID )
	{
		FUNCTION_ENTRY("submitPredefinedDisplayRequest");

		TA_Base_Bus::GenericAgent::ensureControlMode();
		if( isConnectedToSTISServer() )
		{
			// TD 17408
			int currentISCSStationLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion();
			// TD 17408

			// Check that the calling client has the same library version
			if( libraryVersion == currentISCSStationLibraryVersion )
			{
				if( currentISCSStationLibraryVersion == StatusMonitor::getInstance()->getCurrentSTISStationLibraryVersion() )
				{
                    TA_Base_Core::STISDestinationList occDestinationList;
                    int remoteStationSequenceLength = 0;

                    for( unsigned int i = 0; i < destinationList.length(); i++ )
                    {
                        std::string station = destinationList[i].station.in();

                        // if the destination station isnt this one
                        // and this isnt the occ
                        if ( (m_locationType != TA_Base_Core::ILocation::OCC) &&
                             (station != m_locationName) )
                        {
                            // add this to the sequence that will be sent
                            // to the occ tis agent

                            // increase the sequence length
                            remoteStationSequenceLength++;
                            occDestinationList.length(remoteStationSequenceLength);

                            // add the destination to the sequence
                            occDestinationList[remoteStationSequenceLength-1] = destinationList[i];
                        }
                        else
                        {
					        // Create the new display request object
					        STISPredefinedDisplayRequest* displayRequest = new STISPredefinedDisplayRequest();

							std::string operatorName = getOperatorNameFromSession( std::string(sessionID) );

					        // Set the data members
					        displayRequest->setData( destinationList[i], librarySection, libraryVersion, messageTag, startTime, endTime, priority, operatorName );

					        displayRequest->setSequenceNumber( getNewSequenceNumber() );

					        // Send the message to the STIS server
					        // This method blocks until the response message has been received
					        // or the response is not received within the configured time period

					        try
					        {
						        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M10 Predefined Display Request message");
						        sendMessageWithWait( displayRequest );
					        }
					        catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
					        {
								//hongran++ TD17502
								displayRequest->log(false);
								//++hongran TD17502
						        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
						        delete displayRequest;

						        FUNCTION_EXIT;
						        throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
					        }
					        catch ( const TA_IRS_Core::TISAgentException& tae )
					        {
								//hongran++ TD17502
								displayRequest->log(false);
								//++hongran TD17502
						        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
						        delete displayRequest;

						        FUNCTION_EXIT;
						        throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( tae.what() );
					        }

					        // Clean up and throw an exception if the response was not received
					        if( !displayRequest->hasReceivedResponse() )
					        {
								//hongran++ TD17502
								displayRequest->log(false);
								//++hongran TD17502
						        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
						        delete displayRequest;

						        FUNCTION_EXIT;
						        throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException();
					        }

					        // No need to process the response for this type of message
					        // the fact that a response was received is satisfactory

					        if( displayRequest->receivedNackResponse() )
					        {
								//hongran++ TD17502
								displayRequest->log(false);
								//++hongran TD17502
						        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
						        delete displayRequest;

						        FUNCTION_EXIT;
						        throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Invalid message sent to STIS server" );
					        }
					        else
					        {
						        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
							        "Received A10 response message");

								//hongran++ TD17502
						        // Add it to the display request log
						        displayRequest->log(true);
								//++hongran TD17502

                                //Defect ID (9981) - PISF/TST/446
                                timeb loggedTime;
                                StringHelper::isConvertToTimeb(displayRequest->getTimestamp(), loggedTime);

						        // Generate an audit message to capture the operator action
						        std::string messageIDStr;
						        std::stringstream messageIDStream;

                                //TD 9983
						        std::string locationStr( destinationList[i].station.in() );
								// std::string messageDesc(StatusMonitor::getInstance()->getStationPredefinedMessageDescription(librarySection, messageTag));
								
								TA_Base_Core::IPredefinedMessageLibrary* STISMessageLibrary =  
									TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary(currentISCSStationLibraryVersion,
									TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS);
								
                                TA_Base_Core::IPredefinedMessageLibrary::PredefinedMessageList predefinedMessages;
                                TA_Base_Core::STISLedAttributes defaultSTISLedAttributes;
                                TA_Base_Core::STISPlasmaAttributes defaultSTISPlasmaAttributes;
								
                                defaultSTISLedAttributes = STISMessageLibrary->getDefaultSTISLedAttributes();
                                defaultSTISPlasmaAttributes = STISMessageLibrary->getDefaultSTISPlasmaAttributes();
                                predefinedMessages = STISMessageLibrary->getMessages();
                                
                                std::vector <TA_Base_Core::PredefinedMessage*>::iterator iter = predefinedMessages.begin();
                                TA_Base_Core::PredefinedMessage* selectedPredefinedMessage;
								
                                selectedPredefinedMessage = NULL;
                                for (iter = predefinedMessages.begin(); iter < predefinedMessages.end(); iter ++)
                                {
                                    if ((*iter)->librarySection == librarySection && (*iter)->messageTag == messageTag) 
                                    {
                                        selectedPredefinedMessage = (*iter);
                                        break;
                                    }
                                }
                                
                                TA_ASSERT(NULL != selectedPredefinedMessage, "Selected Predefined Message cannot be found.");


						        messageIDStream << messageTag;
						        messageIDStr = messageIDStream.str();
						        
						        TA_Base_Core::DescriptionParameters desc;
						        TA_Base_Core::NameValuePair messageID( ADHOC_MESSAGE_AUDIT_MESSAGE_PARAMETER_NAME, selectedPredefinedMessage->description );
						        TA_Base_Core::NameValuePair location( LOCATION_AUDIT_MESSAGE_PARAMETER_NAME, locationStr );

						        desc.push_back(&messageID);
						        desc.push_back(&location);

								//TES831 Events not logged!
								const TA_Base_Core::PIDList& pidList = destinationList[i].pids;

								std::stringstream pidListStream;

								for( unsigned long k = 0; k < pidList.length(); k++ )
								{
									if( k != 0 )
									{
										pidListStream << ", ";
									}

									pidListStream << pidList[k];
								}

								TA_Base_Core::NameValuePair pidListDP("PID list", pidListStream.str());

								desc.push_back(&pidListDP);

                                //TD 9983
                                std::string startTimeStr(startTime);
                                std::string endTimeStr(endTime);

								TA_Base_Core::NameValuePair startTimeValue(STARTTIME_AUDIT_MESSAGE_PARAMETER_NAME, 
									TISMessageDataDefinitionsHelper::convertTimeStringToFormattedTimeString(startTimeStr));
								TA_Base_Core::NameValuePair endTimeValue(ENDTIME_AUDIT_MESSAGE_PARAMETER_NAME, 
									TISMessageDataDefinitionsHelper::convertTimeStringToFormattedTimeString(endTimeStr));
                       
                                if (priority > 3 || selectedPredefinedMessage->priority > 3)
                                {
									desc.push_back(&startTimeValue);
									desc.push_back(&endTimeValue);
								}

                                std::stringstream displayAttributesStr;
                                std::stringstream repeatIntervalStr;
                                std::stringstream priorityStr;
                                
                                repeatIntervalStr << defaultSTISLedAttributes.repeatInterval << " Second";
                                displayAttributesStr << TISMessageDataDefinitionsHelper::convertSTISLedAttributesToString(defaultSTISLedAttributes)
                                       << ", " << TISMessageDataDefinitionsHelper::convertSTISPlasmaAttributesToString(defaultSTISPlasmaAttributes);
                                priorityStr << selectedPredefinedMessage->priority;

                                TA_Base_Core::NameValuePair repeatIntervalValue(REPEAT_INTERVAL_AUDIT_MESSAGE_PARAMETER_NAME, repeatIntervalStr.str().c_str());

                                if (priority > 3 || selectedPredefinedMessage->priority > 3)
                                {
									desc.push_back(&repeatIntervalValue);
                                }

                                TA_Base_Core::NameValuePair displayAttributesValue(DISPLAY_ATTRIBUTE_AUDIT_MESSAGE_PARAMETER_NAME, displayAttributesStr.str().c_str());
                                TA_Base_Core::NameValuePair priorityValue(PRIORITY_AUDIT_MESSAGE_PARAMETER_NAME, priorityStr.str().c_str());

                                desc.push_back(&displayAttributesValue);
                                desc.push_back(&priorityValue);

								//TES831 Events not logged!
                                
                                //TD 9981
						        m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISPredefinedMessage, m_entityKey, 
							        desc, 
							        "", // Further description text
							        sessionID,
							        "", // alarm ID - not required
							        "", // incident key - not required
							        "", // event key - not required 
                                    NULL, // data filter - not required
                                    loggedTime);    //same time as been logged

						        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
						        delete displayRequest;
					        }
                        }
                    }

                    if (remoteStationSequenceLength > 0)
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                            "Sending other station Predefined display request to OCC agent.");
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
                            LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", te.what());
                            throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Unable to send remote station display requests to OCC TIS agent." );
                        }
                    }

                    // TODO - make one mother audit message with all stations in it
				}
				else
				{
					throw TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException( "STIS server has a different predefined message library version to the TISAgent" );
				}

			}
			else
			{
				throw TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException( "The display request does not use the current message library." );
			}

		}
		else
		{
			int currentISCSStationLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion();

			// Check that the calling client has the same library version
			if( currentISCSStationLibraryVersion == StatusMonitor::getInstance()->getCurrentSTISStationLibraryVersion() )
			{
				TA_Base_Core::STISDestinationList occDestinationList;
				int remoteStationSequenceLength = 0;

				for( unsigned int i = 0; i < destinationList.length(); i++ )
				{
					std::string station = destinationList[i].station.in();

					// if the destination station isnt this one
					// and this isnt the occ
					if ( (m_locationType != TA_Base_Core::ILocation::OCC) &&
						 (station != m_locationName) )
					{
						// add this to the sequence that will be sent
						// to the occ tis agent

						// increase the sequence length
						remoteStationSequenceLength++;
						occDestinationList.length(remoteStationSequenceLength);

						// add the destination to the sequence
						occDestinationList[remoteStationSequenceLength-1] = destinationList[i];
					}
					else
					{
						// Create the new display request object
						STISPredefinedDisplayRequest* displayRequest = new STISPredefinedDisplayRequest();

						std::string operatorName = getOperatorNameFromSession( std::string(sessionID) );

						// Set the data members
						displayRequest->setData( destinationList[i], librarySection, libraryVersion, messageTag, startTime, endTime, priority, operatorName );

						displayRequest->setSequenceNumber( getNewSequenceNumber() );

						displayRequest->log(false);

					}
				}
			}

			throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
		}

		FUNCTION_EXIT;
	}

	void STISManager::submitAdHocDisplayRequest( const TA_Base_Core::STISDestinationList& destinationList, 
											const char* messageContent,
											const char* startTime, const char* endTime,
											CORBA::UShort priority, const TA_Base_Core::DisplayAttributes& displayAttributes,
											const TA_Base_Core::PlasmaAttributes& plasmaAttributes, const TA_Base_Core::LEDAttributes& ledAttributes,
											const char* sessionID )
	{
		FUNCTION_ENTRY("submitAdHocDisplayRequest");

		TA_Base_Bus::GenericAgent::ensureControlMode();
		if( isConnectedToSTISServer() )
		{
            TA_Base_Core::STISDestinationList occDestinationList;
            int remoteStationSequenceLength = 0;

            for( unsigned int i = 0; i < destinationList.length(); i++ )
            {
                std::string station = destinationList[i].station.in();

                // if the destination station isnt this one
                // and this isnt the occ
                if ( (m_locationType != TA_Base_Core::ILocation::OCC) &&
                     (station != m_locationName) )
                {
                    // add this to the sequence that will be sent
                    // to the occ tis agent

                    // increase the sequence length
                    remoteStationSequenceLength++;
                    occDestinationList.length(remoteStationSequenceLength);

                    // add the destination to the sequence
                    occDestinationList[remoteStationSequenceLength-1] = destinationList[i];
                }
                else
                {
                    // request for this station

                    // Create the new display request object
			        STISAdHocDisplayRequest* displayRequest = new STISAdHocDisplayRequest();

					std::string operatorName = getOperatorNameFromSession( std::string(sessionID) );

			        // Set the data members
			        displayRequest->setData( destinationList[i], messageContent, startTime, endTime, priority, displayAttributes,
									        plasmaAttributes, ledAttributes, operatorName );

			        displayRequest->setSequenceNumber( getNewSequenceNumber() );

			        // Send the message to the STIS server
			        // This method blocks until the response message has been received
			        // or the response is not received within the configured time period
			        try
			        {
				        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M11 Adhoc Display Request message");
				        sendMessageWithWait( displayRequest );
			        }
			        catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
			        {
						//hongran++ TD17502
						displayRequest->log(false);
						//++hongran TD17502
				        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
				        delete displayRequest;

				        FUNCTION_EXIT;
				        throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
			        }
			        catch ( const TA_IRS_Core::TISAgentException& tae )
			        {
						//hongran++ TD17502
						displayRequest->log(false);
						//++hongran TD17502
				        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
				        delete displayRequest;

				        FUNCTION_EXIT;
				        throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( tae.what() );
			        }

			        // Clean up and throw an exception if the response was not received
			        if( !displayRequest->hasReceivedResponse() )
			        {
						//hongran++ TD17502
						displayRequest->log(false);
						//++hongran TD17502
				        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
				        delete displayRequest;

				        FUNCTION_EXIT;
				        throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException();
			        }

			        // No need to process the response for this type of message
			        // the fact that a response was received is satisfactory

			        if( displayRequest->receivedNackResponse() )
			        {
						//hongran++ TD17502
						displayRequest->log(false);
						//++hongran TD17502
				        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
				        delete displayRequest;

				        FUNCTION_EXIT;
				        throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Invalid message sent to STIS server" );
			        }
			        else
			        {
				        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
					        "Received A10 response message");

				        // Add it to the display request log
						//hongran++ TD17502
						displayRequest->log(true);
						//++hongran TD17502

                        //Defect ID (9981) - PISF/TST/446
                        timeb loggedTime;
                        StringHelper::isConvertToTimeb(displayRequest->getTimestamp(), loggedTime);

				        std::string messageContentStr( messageContent );

                        //TD 9983
				        std::string locationStr( destinationList[i].station.in() );

				        TA_Base_Core::DescriptionParameters desc;
				        TA_Base_Core::NameValuePair adhocMessage( ADHOC_MESSAGE_AUDIT_MESSAGE_PARAMETER_NAME, messageContentStr );
				        TA_Base_Core::NameValuePair location( LOCATION_AUDIT_MESSAGE_PARAMETER_NAME, locationStr );

				        desc.push_back(&adhocMessage);
				        desc.push_back(&location);

						//TES831 Events not logged!

						const TA_Base_Core::PIDList& pidList = destinationList[i].pids;

						std::stringstream pidListStream;

						for( unsigned long k = 0; k < pidList.length(); k++ )
						{
							if( k != 0 )
							{
								pidListStream << ", ";
							}

							pidListStream << pidList[k];
						}

						TA_Base_Core::NameValuePair pidListDP("PID list", pidListStream.str());

						desc.push_back(&pidListDP);

                        //TD 9983
                        std::string startTimeStr(startTime);
                        std::string endTimeStr(endTime);
                        std::stringstream displayAttributesStr;
                        std::stringstream repeatIntervalStr;
                        std::stringstream priorityStr;

                        repeatIntervalStr << displayAttributes.repeatInterval << " Second";
                        displayAttributesStr << "(Display Attributes: " << TISMessageDataDefinitionsHelper::convertDisplayAttributeToString(displayAttributes) << "), "
                            << "(Plasma Attributes: " << TISMessageDataDefinitionsHelper::convertPlasmaAttributeToString(plasmaAttributes) << "), "
                            << "(LED Attributes: " << TISMessageDataDefinitionsHelper::convertLEDAttributesToString(ledAttributes) << ")";
                        priorityStr << priority;
						
						TA_Base_Core::NameValuePair startTimeValue(STARTTIME_AUDIT_MESSAGE_PARAMETER_NAME, 
							TISMessageDataDefinitionsHelper::convertTimeStringToFormattedTimeString(startTimeStr));
						TA_Base_Core::NameValuePair endTimeValue(ENDTIME_AUDIT_MESSAGE_PARAMETER_NAME, 
							TISMessageDataDefinitionsHelper::convertTimeStringToFormattedTimeString(endTimeStr));
						TA_Base_Core::NameValuePair repeatIntervalValue(REPEAT_INTERVAL_AUDIT_MESSAGE_PARAMETER_NAME, repeatIntervalStr.str().c_str());

                        if (static_cast<unsigned long>(priority) > 3)
                        {							
							desc.push_back(&startTimeValue);
							desc.push_back(&endTimeValue);
							desc.push_back(&repeatIntervalValue);
                        }

                        TA_Base_Core::NameValuePair displayAttributesValue(DISPLAY_ATTRIBUTE_AUDIT_MESSAGE_PARAMETER_NAME, displayAttributesStr.str().c_str());
                        TA_Base_Core::NameValuePair priorityValue(PRIORITY_AUDIT_MESSAGE_PARAMETER_NAME, priorityStr.str().c_str());

                        desc.push_back(&displayAttributesValue);
                        desc.push_back(&priorityValue);

						//TES831 Events not logged!

                        //TD 9981
				        m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISFreeTextMessage, m_entityKey, 
					        desc, 
					        "", // Further description text 
					        sessionID,
					        "", // alarm ID - not required
					        "", // incident key - not required
					        "", // event key - not required 
                            NULL, // data filter - not required
                            loggedTime); // same time as been logged

				        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
				        delete displayRequest;
			        }
                }
            }

            if (remoteStationSequenceLength > 0)
            {
                try
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                        "Sending other station Adhoc display request to OCC agent.");
                    CORBA_CALL( TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent(), submitAdHocDisplayRequest, (
                        occDestinationList,
                        messageContent,
                        startTime, 
                        endTime,
                        priority,
                        displayAttributes,
                        plasmaAttributes,
                        ledAttributes,
                        sessionID ) );//limin
                }
                // catch non-corba exceptions
                // exceptions specd in this IDL method are allowed through
                catch( TA_Base_Core::TransactiveException& te )
                {
                    // error resolving OCC TIS agent
                    LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", te.what());
                    throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Unable to send remote station display requests to OCC TIS agent." );
                }
            }

            // TODO - make one mother audit message with all stations in it
		}
		else
		{
			//hongran++ TD17502
            TA_Base_Core::STISDestinationList occDestinationList;
            int remoteStationSequenceLength = 0;

            for( unsigned int i = 0; i < destinationList.length(); i++ )
            {
                std::string station = destinationList[i].station.in();

                // if the destination station isnt this one
                // and this isnt the occ
                if ( (m_locationType != TA_Base_Core::ILocation::OCC) &&
                     (station != m_locationName) )
                {
                    // add this to the sequence that will be sent
                    // to the occ tis agent

                    // increase the sequence length
                    remoteStationSequenceLength++;
                    occDestinationList.length(remoteStationSequenceLength);

                    // add the destination to the sequence
                    occDestinationList[remoteStationSequenceLength-1] = destinationList[i];
                }
                else
                {
                    // request for this station

                    // Create the new display request object
			        STISAdHocDisplayRequest* displayRequest = new STISAdHocDisplayRequest();

					std::string operatorName = getOperatorNameFromSession( std::string(sessionID) );

			        // Set the data members
			        displayRequest->setData( destinationList[i], messageContent, startTime, endTime, priority, displayAttributes,
									        plasmaAttributes, ledAttributes, operatorName );

			        displayRequest->setSequenceNumber( getNewSequenceNumber() );

					displayRequest->log(false);
				}
			}
			//++hongran TD17502

            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Adhoc display request message not sent - not connected to STIS server");
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
		}

		FUNCTION_EXIT;
	}


	void STISManager::submitClearRequest( const TA_Base_Core::STISDestinationList& destinationList, 
			CORBA::UShort upperPriority, CORBA::UShort lowerPriority, const char* sessionID )
	{
		FUNCTION_ENTRY("submitClearRequest");

		TA_Base_Bus::GenericAgent::ensureControlMode();
		if( isConnectedToSTISServer() )
		{
            TA_Base_Core::STISDestinationList occDestinationList;
            int remoteStationSequenceLength = 0;

            for( unsigned int i = 0; i < destinationList.length(); i++ )
            {
                std::string station = destinationList[i].station.in();

                // if the destination station isnt this one
                // and this isnt the occ
                if ( (m_locationType != TA_Base_Core::ILocation::OCC) &&
                     (station != m_locationName) )
                {
                    // add this to the sequence that will be sent
                    // to the occ tis agent

                    // increase the sequence length
                    remoteStationSequenceLength++;
                    occDestinationList.length(remoteStationSequenceLength);

                    // add the destination to the sequence
                    occDestinationList[remoteStationSequenceLength-1] = destinationList[i];
                }
                else
                {
                    // request for this station

                    // Create the new display request object
			        STISClearRequest* displayRequest = new STISClearRequest();

					std::string operatorName = getOperatorNameFromSession( std::string(sessionID) );

			        // Set the data members
					displayRequest->setData( destinationList[i], upperPriority, lowerPriority, TA_Base_Core::RATIS_OUT_CLEAR, false, operatorName );

			        displayRequest->setSequenceNumber( getNewSequenceNumber() );

			        // Send the message to the STIS server
			        // This method blocks until the response message has been received
			        // or the response is not received within the configured time period
			        try
			        {
				        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M20 Clear Request message");
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
				        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
					        "Received A20 response message");

				        // No need to log this type of message

				        // or process the response

                        //TD 9983
				        std::string locationStr( destinationList[i].station.in() );

				        // Generate an audit message to capture the operator action

				        TA_Base_Core::DescriptionParameters desc;
				        TA_Base_Core::NameValuePair location( LOCATION_AUDIT_MESSAGE_PARAMETER_NAME, locationStr );

						//TES831 Events not logged!
						std::stringstream priorityStream;

                        //TD 9983
                        if (lowerPriority == 8 && upperPriority == 1)
                        {
                           priorityStream << "Clear All";
                        }
                        else if (lowerPriority == 3 && upperPriority == 1)
                        {
                            priorityStream << "Clear Emergency";
                        }
                        else if (lowerPriority == 6 && upperPriority == 6)
                        {
                            priorityStream << "Clear Priority 6";
                        }
                        else if (lowerPriority == 8 && upperPriority == 4)
                        {
                            priorityStream << "Clear Normal";
                        }
                        else
                        {
                            priorityStream << "Priority " << upperPriority << "/" << lowerPriority;
                        }
                        
						TA_Base_Core::NameValuePair priorityDV(PRIORITY_AUDIT_MESSAGE_PARAMETER_NAME, priorityStream.str());
						desc.push_back(&priorityDV);
						//TES831 Events not logged!

						desc.push_back(&location);

						//TES831 Events not logged!
						const TA_Base_Core::PIDList& pidList = destinationList[i].pids;

						std::stringstream pidListStream;

						for( unsigned long k = 0; k < pidList.length(); k++ )
						{
							if( k != 0 )
							{
								pidListStream << ", ";
							}

							pidListStream << pidList[k];
						}

						TA_Base_Core::NameValuePair pidListDP("PID list", pidListStream.str());
						desc.push_back(&pidListDP);
						//TES831 Events not logged!


				        m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISResetMessage, m_entityKey, 
					        desc, 
					        "", // Further description text 
					        sessionID,
					        "", // alarm ID - not required
					        "", // incident key - not required
					        ""); // event key - not required 

				        deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
				        delete displayRequest;
			        }
                }
            }

            if (remoteStationSequenceLength > 0)
            {
                try
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                            "Sending other station clear request to OCC agent.");

                    // this clear request is for a different station
                    // send it to the occ agent
                    CORBA_CALL( TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent(), submitClearRequest, (
                        occDestinationList,
                        upperPriority,
                        lowerPriority,
                        sessionID ) );//limin
                }
                // catch non-corba exceptions
                // exceptions specd in this IDL method are allowed through
                catch( TA_Base_Core::TransactiveException& te )
                {
                    // error resolving OCC TIS agent
                    LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", te.what());
                    throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Unable to send remote station clear request to OCC TIS agent." );
                }
            }
		}
		else
		{
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Clear request message not sent - not connected to STIS server");
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
		}

		FUNCTION_EXIT;
	}

	void STISManager::submitUpgradePredefinedMessageRequest( int libraryVersion )
	{
		FUNCTION_ENTRY("submitUpgradePredefinedMessageRequest");

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
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M70 Upgrade Predefined Message Request");
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
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
					"Received A20 response message");

				// No need to log this type of message

				// or process the response

				deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
				delete displayRequest;
			}
		}
		else
		{
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Clear request message not sent - not connected to STIS server");
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
		}

		FUNCTION_EXIT;
	}

	void STISManager::upgradePredefinedStationMessageLibrary( CORBA::UShort newLibraryVersion, const char* sessionID )
	{
		FUNCTION_ENTRY("upgradePredefinedStationMessageLibrary");


		TA_Base_Bus::GenericAgent::ensureControlMode();
		try
		{
			if( m_locationType == TA_Base_Core::ILocation::OCC )
			{
				//TD16479, hongzhi, submit event upon begin STIS Library Update
				std::stringstream versionStream;
				std::stringstream sessionIDStream(sessionID);
				TA_Base_Core::DescriptionParameters desc;
		        TA_Base_Core::NameValuePair versionVP(LIBRARY_VERSION_AUDIT_MESSAGE_PARAMETER_NAME, "" );
				versionStream << newLibraryVersion;			
				versionVP.value = versionStream.str();
				desc.push_back(&versionVP);
				submitAuditMessage( TA_Base_Core::TISAudit::STISISCSVersionUpgradeBegin,
                desc, sessionIDStream.str().c_str());
				
				//hongran++ TD17500
				// upgrade the occ local library version first
				upgradeOCCStationLibraryVersion(newLibraryVersion);
				//++hongran TD17500
				if( stationLibraryUpgradeInProgress() )
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
									"A station library upgrade is already in progress, client request to upgrade ignored");
					return;
				}
				else
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
									"OCC Agent attempting to upgrade station libraries");
		
					// initiate the library upgrade - tells the upgrader thread to do its thang
					upgradeStationLibrary(sessionID);

					STISManager::getInstance()->closeAlarm( TA_Base_Core::TISAlarms::StationLibraryPendingUpgrade );
				}
			}
			else
			{
				int currentLocalISCSStationLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion();
				int nextLocalISCSStationLibraryVersion = StatusMonitor::getInstance()->getNextISCSStationLibraryVersion();
				
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"TD17500 Station recieved the station lib upgrade request. current version is : [%d], next version is [%d]", currentLocalISCSStationLibraryVersion, nextLocalISCSStationLibraryVersion);

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
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"OCC has requested a predefined STIS message library upgrade. Current and next library version are the same - no upgrade required");
				}
			}
		}
		catch (const TA_Base_Core::DatabaseException& ex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"DatabaseException in STISManager::upgradePredefinedStationMessageLibrary: %s", ex.what());
		}
		catch (const TA_Base_Core::DataException& ex) {
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"DataException in STISManager::upgradePredefinedStationMessageLibrary: %s", ex.what());
		}
		catch (const TA_Base_Core::TransactiveException& ex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"TransactiveException in STISManager::upgradePredefinedStationMessageLibrary:%s", ex.what());
		}
		catch (const CORBA::Exception& ex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"CORBA::Exception in  STISManager::upgradePredefinedStationMessageLibrary:");
		}
		catch (...) 
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Unknow exception in STISManager::upgradePredefinedStationMessageLibrary.");
		}

		// Generate an audit message to capture the operator action
		// this method can only be invoked as a result of direct operator action 
		// at the OCC


		FUNCTION_EXIT;
	}

    void STISManager::upgradePredefinedTrainMessageLibrary( CORBA::UShort newLibraryVersion, const char* sessionID  )
	{
		FUNCTION_ENTRY("upgradePredefinedTrainMessageLibrary");

		TA_Base_Bus::GenericAgent::ensureControlMode();
		StisNamedObject *stisNamedObject = NULL;

		TA_Base_Core::CorbaNameList stisNameList;

		try
		{

			if( m_locationType == TA_Base_Core::ILocation::OCC )
			{
				//hongran++ TD17500
				// upgrade the occ local library version first
				upgradeOCCTrainLibraryVersion(newLibraryVersion);
				//++hongran TD17500

				if( trainLibraryUpgradeInProgress() )
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
									"A station library upgrade is already in progress, client request to upgrade ignored");
					return;
				}
				else
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
									"OCC Agent attempting to upgrade train libraries");
					
					// initiate the library upgrade - tells the upgrader thread to do its thing
					upgradeTrainLibrary(sessionID);

					STISManager::getInstance()->closeAlarm( TA_Base_Core::TISAlarms::TrainLibraryPendingUpgrade );
				}
			}
			else
			{
				int currentISCSTrainLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion();
				int nextISCSTrainLibraryVersion = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion();

				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"The Upgrade station train lib is called. the current is [%d], next is [%d]", currentISCSTrainLibraryVersion, nextISCSTrainLibraryVersion);
				if( currentISCSTrainLibraryVersion != nextISCSTrainLibraryVersion )
				{
					StatusMonitor::getInstance()->setCurrentISCSTrainLibraryVersion( nextISCSTrainLibraryVersion );

					//removed by hongran. This table can not be operated on station because it is a mview
					// delete the previous message library and messages from the LOCAL STATION database
//					TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().deleteAllMessageLibrariesOfTypeNotMatching(
//						"TTIS", nextISCSTrainLibraryVersion, nextISCSTrainLibraryVersion );
				}
				else
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"OCC has requested a predefined TTIS message library upgrade. Current and next library version are the same - no upgrade required");
				}
			}
		}
		catch (const TA_Base_Core::DatabaseException& ex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"DatabaseException in STISManager::upgradePredefinedTrainMessageLibrary: %s", ex.what());
		}
		catch (const TA_Base_Core::DataException& ex) {
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"DataException in STISManager::upgradePredefinedTrainMessageLibrary: %s", ex.what());
		}
		catch (const TA_Base_Core::TransactiveException& ex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"TransactiveException in STISManager::upgradePredefinedTrainMessageLibrary:%s", ex.what());
		}
		catch (const CORBA::Exception& ex)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"CORBA::Exception in  STISManager::upgradePredefinedTrainMessageLibrary:");
		}
		catch (...) 
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Unknow exception in STISManager::upgradePredefinedTrainMessageLibrary.");
		}

		FUNCTION_EXIT;
	}

	TA_Base_Core::CurrentDisplayingMessage* STISManager::getCurrentDisplayingMessage( const TA_Base_Core::STISDestination& destination )
	{
		FUNCTION_ENTRY("getCurrentDisplayMessage");
		
		TA_Base_Bus::GenericAgent::ensureControlMode();
		TA_Base_Core::CurrentDisplayingMessage* currentDisplayingMessage;

		if( isConnectedToSTISServer() )
		{
            std::string station = destination.station.in();

            if ( (m_locationType != TA_Base_Core::ILocation::OCC) &&
                 (station != m_locationName) )
            {
                try
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                        "Sending other station current displaying message request to OCC agent.");

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
                    LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", te.what());
                    throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Unable to send other station message request to OCC TIS agent." );
                }
            }
            else
            {
			    // Create the new display request object
			    STISCurrentDisplayingMessageRequest displayRequest;

				// Ensure that the station name is not empty 
				if( (destination.station != "" ) && ( destination.pids.length() == 1 ))
				{
					int pidAddress = getPIDAddress( std::string(destination.pids[0]), std::string(destination.station) );

					// Set the data members
					displayRequest.setData( std::string(destination.station), pidAddress );

					displayRequest.setSequenceNumber( getNewSequenceNumber() );

					// Send the message to the STIS server
					// This method blocks until the response message has been received
					// or the response is not received within the configured time period
					try
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M50 Current Displaying Message Request message");
						sendMessageWithWait( &displayRequest );
					}
					catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
					{
						//hongran++ TD17502
						displayRequest.log(false);
						//++hongran TD17502

						deleteMessageFromPendingList( displayRequest.getSequenceNumber() );

						FUNCTION_EXIT;
						throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
					}
					catch ( const TA_IRS_Core::TISAgentException& tae )
					{
						//hongran++ TD17502
						displayRequest.log(false);
						//++hongran TD17502

						deleteMessageFromPendingList( displayRequest.getSequenceNumber() );

						FUNCTION_EXIT;
						throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( tae.what() );
					}

					// Clean up and throw an exception if the response was not received
					if( !displayRequest.hasReceivedResponse() )
					{
						//hongran++ TD17502
						displayRequest.log(false);
						//++hongran TD17502

						deleteMessageFromPendingList( displayRequest.getSequenceNumber() );

						FUNCTION_EXIT;
						throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException();
					}

					if( displayRequest.receivedNackResponse() )
					{
						//hongran++ TD17502
						displayRequest.log(false);
						//++hongran TD17502

						deleteMessageFromPendingList( displayRequest.getSequenceNumber() );

						FUNCTION_EXIT;
						throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Invalid message sent to STIS server" );
					}
					else
					{
						LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
							"Received A50 response message");

						// Add it to the display request log
						//hongran++ TD17502
						displayRequest.log(true);
						//++hongran TD17502

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
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Current Displaying message request not sent - not connected to STIS server");
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
		}

        FUNCTION_EXIT;

		return currentDisplayingMessage;
	}

	void STISManager::submitRATISVettingResponse( CORBA::Long key, CORBA::Boolean approved, CORBA::UShort priority, const char* content, const char* sessionID )
	{
		FUNCTION_ENTRY("submitRATISVettingResponse");

		TA_Base_Bus::GenericAgent::ensureControlMode();
        // We'll just send this to a queue and return straight away because if there are many trains (each clear message has to be sent to the train agent 
        // individually) the method will time out each time.
        m_ratisMessenger.addRATISVettingResponse(key, approved, priority, content, sessionID);

        FUNCTION_EXIT;
    }


    void STISManager::reApproveVettedRATISMessage( const TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails& ratisMessage,
                                                   const char* sessionID )
    {
        FUNCTION_ENTRY("reApproveVettedRATISMessage");

		TA_Base_Bus::GenericAgent::ensureControlMode();
        // check occ
        if (m_locationType != TA_Base_Core::ILocation::OCC)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Sending RATIS reapprove request to OCC agent.");

            try
            {
                CORBA_CALL( TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent(), reApproveVettedRATISMessage, (
                    ratisMessage,
                    sessionID ) );//limin
            }
            // catch non-corba exceptions
            // exceptions specd in this IDL method are allowed through
            catch( TA_Base_Core::TransactiveException& te )
            {
                // error resolving OCC TIS agent
                LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", te.what());
                throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Unable to resubmit RATIS approval to OCC TIS agent." );
            }

    		FUNCTION_EXIT;
            return;
        }

        // must be at the OCC


        // check stis server connected
        if( !isConnectedToSTISServer() )
		{
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "RATIS reapproval message not sent - not connected to STIS server");
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
		}

        // must be conected to STIS server

        // set the flags in the message structure
        TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails copyOfRatisMessage(ratisMessage);
        copyOfRatisMessage.requiresVetting = false;
        copyOfRatisMessage.overridable = false;
        copyOfRatisMessage.timeCreated = time(NULL);

        // call process new ratis message
        try
        {
            processIncomingRATISMessage( copyOfRatisMessage, std::string(sessionID) );
        }
        // wrap any exceptions in a STISGeneralFailureException
        catch (TA_Base_Core::TransactiveException& te)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::TransactiveException", te.what());
            
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISGeneralFailureException(te.what());
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "...", "While resubmitting ratis message");
            
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISGeneralFailureException("Error resubmitting RATIS Message");
        }

        FUNCTION_EXIT;
    }


    void STISManager::submitRATISVettingResponse( long key, bool approved, short priority, const std::string& content, const std::string& sessionID )
	{
		FUNCTION_ENTRY("submitRATISVettingResponse");

		TA_Base_Bus::GenericAgent::ensureControlMode();
		TA_IRS_Core::IRATISMessage* message = NULL;
        try
        {
    		message = TA_IRS_Core::RATISMessageAccessFactory::getInstance().getRATISMessage( key );
        }
        catch (...)
        {
        }
        // Could not find the message.  It's possible that this request is sent just after another operator
        // has approved it (before the client receive RATISStatusUpdate), so it's ok to just ignore it.
        if (NULL == message) 
        {
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
						"Received vetting response for a RATIS message that does not exist in the database");
            return;
		}

        TA_Base_Core::ERATISMessageType type = message->getType();
        std::string text = message->getMessageText();
        std::string destination = getFullDestinationString(message->getDestination());
        std::string tag = message->getTag();
        std::string startTime = message->getStartTime();
        std::string endTime = message->getEndTime();
        bool override = message->getOverridable();
        if (priority == 0)
        {
            priority = message->getPriority();
        }

		if( !approved ) // disapprove message
		{
            // send a notification if we've previously told banner to display it so that it can remove it
            if (message->getRequiresVetting() || ((message->getType() == TA_Base_Core::RATIS_OUT_CLEAR ) && message->isForTTIS())) // if it was a message that required vetting
            {
			    TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageApprovalDetails approvalMessage;
			    approvalMessage.messageID = message->getKey();
			    approvalMessage.sessionRef = message->getSessionRef();
                approvalMessage.status = TA_Base_Core::REJECTED;
			    CORBA::Any data;
			    data <<= approvalMessage;

                // Tell the banners that we've successfully rejected the message
			    m_tisMessageSender->sendCommsMessage(
				    TA_Base_Core::TISComms::RATISStatusUpdate,
				    m_entityKey,
				    data,
				    m_subsystemKey,
				    m_locationKey,
				    NULL); // filter
            }

            // delete the message from the database
            message->deleteThisRATISMessage();

            sendRatisProcessedAuditMessage(type, text, approved, destination, tag, priority, 
                                            startTime, endTime, override, sessionID);

            delete message;
            message = NULL;
        	FUNCTION_EXIT;
            return;
		}

        bool success = false;
        // approved message, see if we're connected to the server
		if( !isConnectedToSTISServer() )
		{
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "RATIS vetting response message not sent - not connected to STIS server");
		}
        else
        {
            try
            {
                // if it is an update or new display request, then send the approval along to the STIS server
				if (type == TA_Base_Core::RATIS_IN_CLEAR || type == TA_Base_Core::RATIS_OUT_CLEAR)
                {
                    approveRATISClearMessage(message, priority, sessionID);
                }
                else
                {
                    approveRATISDisplayMessage(message, priority, content, sessionID);

				// TD17572
				// As required, only submit ProcessAudit message when the message is vetting,
				// and don't care about it is a clear type or not
					
//					sendRatisProcessedAuditMessage(type, content, approved, destination, tag, priority, 
//                                                startTime, endTime, override, sessionID);
                }
                
				if (message->getRequiresVetting() && m_needVetting)
				{
					sendRatisProcessedAuditMessage(type, text, approved, destination, tag, priority, 
                                                startTime, endTime, override, sessionID);
				}
				// TD17572
				
				// delete the message from the database
                message->deleteThisRATISMessage();

                

                success = true;
            }
            catch (...)
            {
                // send a notification if the message is not displayed
                if (!(message->getRequiresVetting() || ((message->getType() == TA_Base_Core::RATIS_OUT_CLEAR) && message->isForTTIS())))
                {
			        TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageApprovalDetails incomingMessage;

			        incomingMessage.messageID = message->getKey();
			        incomingMessage.sessionRef = message->getSessionRef();
			        incomingMessage.status = TA_Base_Core::APPROVE_FAILED;

			        CORBA::Any data;
			        data <<= incomingMessage;

				    m_tisMessageSender->sendCommsMessage(
					    TA_Base_Core::TISComms::IncomingRATISMessage,
					    m_entityKey,
					    data,
				        m_subsystemKey,
				        m_locationKey,
					    NULL); // filter
                }

            }
        }

        // send a notification if we've previously told banner to display it so that it can remove it
        if (message->getRequiresVetting() || ((message->getType() == TA_Base_Core::RATIS_OUT_CLEAR) && message->isForTTIS())) // if it was a message that required vetting
        {
		    TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageApprovalDetails approvalMessage;
		    approvalMessage.messageID = message->getKey();
		    approvalMessage.sessionRef = message->getSessionRef();
            approvalMessage.status = success ? TA_Base_Core::APPROVED : TA_Base_Core::APPROVE_FAILED;
		    CORBA::Any data;
		    data <<= approvalMessage;

            // Tell the banners that we've successfully rejected the message
		    m_tisMessageSender->sendCommsMessage(
			    TA_Base_Core::TISComms::RATISStatusUpdate,
			    m_entityKey,
			    data,
			    m_subsystemKey,
			    m_locationKey,
			    NULL); // filter

            delete message;
            message = NULL;
        }
		FUNCTION_EXIT;
	}


    void STISManager::approveRATISClearMessage(TA_IRS_Core::IRATISMessage* message, const short priority, const std::string& sessionID)
    {
        FUNCTION_ENTRY("approveRATISClearMessage");
		TA_Base_Bus::GenericAgent::ensureControlMode();



            // check if the message is overridable, and if so override the priority if it differs
            if ( true == message->getOverridable() )
            {
                // if the priority is specified, and differs from the message then we'll reset the priority
                if ( ( message->getPriority() != priority ) &&
                    ( priority > 0 ) &&
                    (priority < 9) )
        {
                    message->setPriority( priority );
                }
            }


            if ( true == message->isForTTIS() )
            {
                TTISManager::getInstance()->submitRatisClearRequest( message, sessionID );
		}
        else
        {
                std::string operatorName = getOperatorNameFromSession( std::string(sessionID) );

                submitClearRequest( message->getDestination(), priority, message->getTag(), message->getType(), false, operatorName );

			logRatisClearMessage(message, priority, sessionID);
        }

        FUNCTION_EXIT;
    }

	//TD9978
	//zhou yuan++
	void STISManager::logRatisClearMessage(TA_IRS_Core::IRATISMessage* message, 
										   const short priority, 
										   const std::string& sessionID)
	{
		TA_Base_Bus::GenericAgent::ensureControlMode();
		TA_IRS_Core::IDisplayRequest* displayRequest = TA_IRS_Core::DisplayRequestAccessFactory::getInstance().createNewDisplayRequest();
		
		if( displayRequest )
		{
			// TD17572
			TA_Base_Core::ERATISMessageType ratisType = message->getType();
			TA_IRS_Core::IDisplayRequest::EMessageType type;
			
			switch (ratisType)
			{
			case TA_Base_Core::RATIS_IN_CLEAR:
				type = TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR;
				break;
			case TA_Base_Core::RATIS_OUT_CLEAR:
				type = TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR;
				break;
			case TA_Base_Core::RATIS_IN_NEW:
				TA_ASSERT(false, "Attempting to log a  no RATIS approved");
				break;
			case TA_Base_Core::RATIS_OUT_NEW:
				TA_ASSERT(false, "Attempting to log a no RATIS approved");
				break;
			case TA_Base_Core::RATIS_IN_UPDATE:
				TA_ASSERT(false, "Attempting to log a no RATIS approved");
				break;
			case TA_Base_Core::RATIS_OUT_UPDATE:
				TA_ASSERT(false, "Attempting to log a no RATIS approved");
				break;
			}
			// TD17572

			displayRequest->setDestinationSystem( "STIS" );
			displayRequest->setStartTime( message->getStartTime() );
			displayRequest->setEndTime( message->getEndTime() );
			displayRequest->setPriority( priority );
			displayRequest->setTimestamp( getNewTimestampString() );
			// TD17572
            displayRequest->setMessageType( type /* TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR*/ );
			// TD17572
			displayRequest->setRatisTag(message->getTag());
			//hongran++ TD17502
			displayRequest->setIfSendSuccess(TA_Base_Core::Pass);
			//++hongran TD17502

			std::string operatorName = getOperatorNameFromSession( sessionID );
            if (operatorName == "Not available")
            {
                // An operator name of "RXH" shouild be logged instead of "Not available"
                operatorName = "RXH";
            }
			displayRequest->setSubmittedBy( operatorName );
			displayRequest->setRatisDestination(message->getDestination());
			//defectID(17927) AVteam++
			//displayRequest->setAdhocMessageContent("Ratis Clear Message");
			displayRequest->setAdhocMessageContent(" ");
            //defectID(17927) ++AVteam
			// log the destination as an STIS destination for displaying in the log viewer
			std::string stisStation;
			std::string stisLevels;
			std::string stisPids;
			
			getSTISDestinationFromRATISDestination(	message->getDestination(),
													stisStation,
													stisLevels,
													stisPids );
			displayRequest->setStisDestinationStation( stisStation );
			displayRequest->setStisDestinationLevels( stisLevels );
			displayRequest->setStisDestinationPids( stisPids );

			displayRequest->applyChanges();

			// TD17572
			if (message->getType() == TA_Base_Core::RATIS_OUT_CLEAR)
			{
				sendRatisOutgoingAuditMessage(message->getType(), message->getMessageText(), message->getDestination(), message->getTag(),
					message->getPriority(), message->getStartTime(), message->getEndTime(), message->getOverridable(),
					sessionID.c_str());
			}
			// TD17572
			
			delete displayRequest;
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Could not create new display request Database object");
		}
	}

	std::string STISManager::getNewTimestampString()
	{
		std::stringstream timeStamp;
		
		time_t currentTime;
		tm localTime;
		
		currentTime = time(NULL);
        ACE_OS::localtime_r(&currentTime, &localTime);
		
        // the timestamp is in the format
        // YYYYMMDDHHmmSS
		
        // YYYY
		timeStamp << (localTime.tm_year + 1900);
		
        // MM
        // tm_mon is months since january, so 0 = january
        timeStamp << std::setw(2) << std::setfill('0') << ( localTime.tm_mon + 1 );
		
        // DD
        // tm_mday is the day of the month (1-31)
        timeStamp << std::setw(2) << std::setfill('0') << localTime.tm_mday;
		
        // HH
        // tm_hour is the hours past the day (0-23)
        timeStamp << std::setw(2) << std::setfill('0') << localTime.tm_hour;
		
        // MM
        // tm_min is the minutes past the hour (0-59)
		timeStamp << std::setw(2) << std::setfill('0') << localTime.tm_min;
		
        // SS
        // tm_sec is the seconds past the minute (0-60) (61 for leap seconds)
		timeStamp << std::setw(2) << std::setfill('0') << localTime.tm_sec;
		
		return timeStamp.str();
	}

	//++zhou yuan

    void STISManager::submitClearRequest(const std::string& destination, const short priority, const std::string& tag, const TA_Base_Core::ERATISMessageType type, const bool overridable, const std::string& operatorName)
    {
        FUNCTION_ENTRY("submitClearRequest");
		TA_Base_Bus::GenericAgent::ensureControlMode();
        // It's STIS, so just clear the message similar to how we've done in submitClearRequest
		STISClearRequest* displayRequest = new STISClearRequest();

		// Set the data members.  The rest of this is just copied from submitClearMessage
		displayRequest->setData( destination, priority, tag, type, overridable, operatorName);

		displayRequest->setSequenceNumber( getNewSequenceNumber() );

		// Send the message to the STIS server
		// This method blocks until the response message has been received
		// or the response is not received within the configured time period
		try
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M20 Clear Request message");
			sendMessageWithWait( displayRequest );
		}
		catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
		{
			displayRequest->log(false);
			deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			delete displayRequest;

			FUNCTION_EXIT;
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
		}
		catch ( const TA_IRS_Core::TISAgentException& tae )
		{
			displayRequest->log(false);
			deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			delete displayRequest;

			FUNCTION_EXIT;
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( tae.what() );
		}

		// Clean up and throw an exception if the response was not received
		if( !displayRequest->hasReceivedResponse() )
		{
			displayRequest->log(false);
			deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			delete displayRequest;

			FUNCTION_EXIT;
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException();
		}

		// No need to process the response for this type of message
		// the fact that a response was received is satisfactory

		if( displayRequest->receivedNackResponse() )
		{
			displayRequest->log(false);
			deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			delete displayRequest;

			FUNCTION_EXIT;
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Invalid message sent to STIS server" );
		}
		else
		{
			// TD17572
			// Since in clear message is log by the function logRatisClearMessage()
			// don't need log twice, this modification is quite ugly, but cannot find
			// a good way to solve this problem
			if (type == TA_Base_Core::RATIS_OUT_CLEAR)
			{
				displayRequest->log(true);
			}
			// TD17572
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
				"Received A20 response message");

			deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			delete displayRequest;
		}
    }


    void STISManager::approveRATISDisplayMessage(TA_IRS_Core::IRATISMessage* message, const short priority, const std::string& content, const std::string& sessionID)
    {
        FUNCTION_ENTRY("approveRATISDisplayMessage");
		TA_Base_Bus::GenericAgent::ensureControlMode();



            // check if the message is overridable,
            // and if so override the priority and message text if it differs
            if ( true == message->getOverridable() )
        {
                // if the priority is specified, and differs from the message then we'll reset the priority
                if ( ( message->getPriority() != priority ) &&
                     ( priority > 0 ) &&
                     (priority < 9) )
            {
                    message->setPriority( priority );
            }

                if ( ( false == content.empty() ) &&
                     ( message->getMessageText() != content ) )
            {
                    message->setMessageText( content );
            }
        }

            if ( true == message->isForTTIS() )
        {
                TTISManager::getInstance()->submitRatisDisplayRequest( message, sessionID );
        }
        else
        {
		    // Create the new display request object
		    STISApprovedRATISDisplayRequest* displayRequest = new STISApprovedRATISDisplayRequest();

            // set the operator name
            std::string operatorName = getOperatorNameFromSession( sessionID );
            if (operatorName == "Not available")
            {
                // An operator name of "RXH" shouild be logged instead of "Not available"
                operatorName = "RXH";
            }

		    // Set the data members

		    displayRequest->setData( 
			    message->getMessageText(),
			    message->getPriority(),
			    message->getTag(),
			    message->getDestination(),  
			    message->getStartTime(), 
			    message->getEndTime(),
			    message->getType(),
                    message->getOverridable(),
			    message->getRequiresVetting(),
                operatorName ); // This is an incoming message

		    displayRequest->setSequenceNumber( getNewSequenceNumber() );

		    // Send the message to the STIS server
		    // This method blocks until the response message has been received
		    // or the response is not received within the configured time period
		    try
		    {
			    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M12 Approved RATIS Display Request message");
			    sendMessageWithWait( displayRequest );
		    }
		    catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
		    {
				//hongran++ TD17502
				displayRequest->log(false);
				//++hongran TD17502

			    deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			    delete displayRequest;

			    FUNCTION_EXIT;
			    throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
		    }
		    catch ( const TA_IRS_Core::TISAgentException& tae )
		    {
				//hongran++ TD17502
				displayRequest->log(false);
				//++hongran TD17502

			    deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			    delete displayRequest;

			    FUNCTION_EXIT;
			    throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( tae.what() );
		    }

		    // Clean up and throw an exception if the response was not received
		    if( !displayRequest->hasReceivedResponse() )
		    {
				//hongran++ TD17502
				displayRequest->log(false);
				//++hongran TD17502

			    deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			    delete displayRequest;

			    FUNCTION_EXIT;
			    throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException();
		    }

		    // No need to process the response for this type of message
		    // the fact that a response was received is satisfactory

		    if( displayRequest->receivedNackResponse() )
		    {
				//hongran++ TD17502
				displayRequest->log(false);
				//++hongran TD17502

			    deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			    delete displayRequest;

			    FUNCTION_EXIT;
			    throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Invalid message sent to STIS server" );
		    }
		    else
		    {
			    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
				    "Received A10 response message");

			    // Add it to the display request log
				//hongran++ TD17502
				displayRequest->log(true);
				//++hongran TD17502

			    deleteMessageFromPendingList( displayRequest->getSequenceNumber() );

			    delete displayRequest;
		    }
        }
        FUNCTION_EXIT;
    }            


	void STISManager::submitRATISDisplayRequest(const char* messageContent, CORBA::UShort priority, 
		const char* RATISTag, const char* RATISDestination, const char* startTime, const char* endTime, 
		TA_Base_Core::ERATISMessageType type, CORBA::Boolean overridable, CORBA::Boolean vetting, 
		const char* sessionID)
	{
		FUNCTION_ENTRY("submitRATISDisplayRequest");

		TA_Base_Bus::GenericAgent::ensureControlMode();
        // only the OCC can submit RATIS
        // delegate to the OCC if this is not the OCC agent
        if (m_locationType != TA_Base_Core::ILocation::OCC)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                "Sending RATIS display request to OCC agent.");

            try
            {
                CORBA_CALL( TA_Base_Bus::TISAgentAccessFactory::getInstance().getOccSTISAgent(), submitRATISDisplayRequest, (
                    messageContent,
                    priority,
                    RATISTag,
                    RATISDestination,
                    startTime,
                    endTime,
                    type,
                    overridable,
					vetting,
                    sessionID ) );//limin
            }
            // catch non-corba exceptions
            // exceptions specd in this IDL method are allowed through
            catch( TA_Base_Core::TransactiveException& te )
            {
                // error resolving OCC TIS agent
                LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", te.what());
                throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Unable to send RATIS request to OCC TIS agent." );
            }
    		FUNCTION_EXIT;
            return;
        }

        // must be the OCC

        if( !isConnectedToSTISServer() )
		{
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "RATIS Display request message not sent - not connected to STIS server");
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
		}

        if (type == TA_Base_Core::RATIS_OUT_CLEAR)
        {
			std::string operatorName = getOperatorNameFromSession( std::string(sessionID) );
            submitClearRequest(RATISDestination, priority, RATISTag, TA_Base_Core::RATIS_OUT_CLEAR, overridable, operatorName);
			sendRatisOutgoingAuditMessage(type, messageContent, RATISDestination, RATISTag, priority, 
                                            startTime, endTime, overridable, sessionID);
            FUNCTION_EXIT;
            return;
        }

		// Create the new display request object
		// Note - the object used is the same as approved RATIS messages
		// this is due to the fact that the protocol uses the M12 message
		// to fullfil both requirements
		// i.e Approved messages to STIS PIDs, and ISCS -> RXH messages
		STISApprovedRATISDisplayRequest* displayRequest = new STISApprovedRATISDisplayRequest();

        std::string operatorName = getOperatorNameFromSession( std::string(sessionID) );
		
		// Set the data members
		displayRequest->setData( messageContent, priority, RATISTag, RATISDestination,
			startTime, endTime, type, overridable, vetting, operatorName );  

		displayRequest->setSequenceNumber( getNewSequenceNumber() );

		// Send the message to the STIS server
		// This method blocks until the response message has been received
		// or the response is not received within the configured time period
		try
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M12 RATIS Display Request Message");
			sendMessageWithWait( displayRequest );
		}
		catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
		{
			//hongran++ TD17502
			displayRequest->log(false);
			//++hongran TD17502

			LOG_EXCEPTION_CATCH(SourceInfo, "STISServerNotConnectedException", "While submitting Ratis request" );
			deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			delete displayRequest;

			FUNCTION_EXIT;
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
		}
		catch ( const TA_IRS_Core::TISAgentException& tae )
		{
			//hongran++ TD17502
			displayRequest->log(false);
			//++hongran TD17502
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Core::TISAgentException", tae.what() );
			deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			delete displayRequest;

			FUNCTION_EXIT;
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( tae.what() );
		}

		// Clean up and throw an exception if the response was not received
		if( !displayRequest->hasReceivedResponse() )
		{
			//hongran++ TD17502
			displayRequest->log(false);
			//++hongran TD17502
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Received No response for Ratis Request (timed out)");
			deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			delete displayRequest;

			FUNCTION_EXIT;
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException();
		}

		if( displayRequest->receivedNackResponse() )
		{
			//hongran++ TD17502
			displayRequest->log(false);
			//++hongran TD17502
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Received Nack response for Ratis Request");
			deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			delete displayRequest;

			FUNCTION_EXIT;
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationFailureException( "Invalid message sent to STIS server" );
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
				"Received A10 response message");

			// Add it to the display request log
			//hongran++ TD17502
			displayRequest->log(true);
			//++hongran TD17502

			// TD 9981
			std::string timeStamp = displayRequest->getTimestamp();
			// TD 9981

			deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			delete displayRequest;

//            sendRatisSubmittedAuditMessage(false, type, messageContent, RATISdestination,
//                RATISTag, priority, startTime, endTime, overridable, sessionID, timeStamp); // TD 9981 add timestamp

			sendRatisOutgoingAuditMessage(type, messageContent, RATISDestination, RATISTag, priority, 
                                            startTime, endTime, overridable, sessionID, timeStamp);

		}

		FUNCTION_EXIT;
	}


	bool STISManager::isValid()
	{
		FUNCTION_ENTRY("isValid");

		FUNCTION_EXIT;
		return true;
	}

	void STISManager::start()
	{
		FUNCTION_ENTRY("start");
			

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

				// Start it
                                                                     m_trainMessageLibraryPath,
                                                                     m_serverIPAddress );


        }

        // always runs - it will just sit on the semaphore until it receives a message
        m_statusRequestQueue.start();

		FUNCTION_EXIT;

	}
	
	void STISManager::stop()
	{
		FUNCTION_ENTRY("stop");
	
        m_statusRequestQueue.terminateAndWait();

		    if( m_stationMessageLibrarySynchroniser != NULL )
		    {
				    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager asking the station message library synchronisation thread to terminate...");
				    m_stationMessageLibrarySynchroniser->terminateAndWait();

			    delete m_stationMessageLibrarySynchroniser;
				m_stationMessageLibrarySynchroniser = NULL;
		    }

			if( m_trainMessageLibrarySynchroniser != NULL )
		    {
				    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager asking the train message library synchronisation thread to terminate...");
				    m_trainMessageLibrarySynchroniser->terminateAndWait();

			    delete m_trainMessageLibrarySynchroniser;
				m_trainMessageLibrarySynchroniser = NULL;
		    }

		    if( m_stationMessageLibraryUpgrader != NULL )
		    {
				    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager asking the station message library upgrade thread to terminate...");
				    m_stationMessageLibraryUpgrader->terminateAndWait();

			    delete m_stationMessageLibraryUpgrader;
				m_stationMessageLibraryUpgrader = NULL;
		    }

			if( m_trainMessageLibraryUpgrader != NULL )
		    {
				    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager asking the train message library upgrade thread to terminate...");
				    m_trainMessageLibraryUpgrader->terminateAndWait();

			    delete m_trainMessageLibraryUpgrader;
				m_trainMessageLibraryUpgrader = NULL;
		    }


			    delete m_messageLibraryManager;
				m_messageLibraryManager = NULL;

			// Disconnect from the STIS server 
			m_communicationsHandler.disconnectFromServer();

		    deactivateServant();

		// for each of the data points created for this agent, deactive
		// the data point's servant

		// Dont deactivate the PID datapoints if this is the OCC agent
		// the PIDs are not real datapoints and are used to establish the address
		// of the station PIDs when sending display requests	
		if( m_locationType == TA_Base_Core::ILocation::OCC)
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

		FUNCTION_EXIT;
	}


	void STISManager::update( const TA_Base_Core::ConfigUpdateDetails& configUpdateDetails )
	{
		FUNCTION_ENTRY("update");

		FUNCTION_EXIT;
	}


	void STISManager::remove()
	{
		FUNCTION_ENTRY("remove");
		
		FUNCTION_EXIT;
	}


	void STISManager::setToControlMode()
	{
		FUNCTION_ENTRY("setToControlMode");

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager switching to Control Mode");

		// Dont set the PID datapoints to Control mode if this is the OCC agent
		// the PIDs are not real datapoints and are used to establish the address
		// of the station PIDs when sending display requests
		if( m_locationType != TA_Base_Core::ILocation::OCC)
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
		
		// check and process if any virtual DataPoints have been changed
		//processDataPointStateManuallyUpdate();

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager asking the Communications Handler to connect to the STIS Server");

		// Attempt to connect to the STIS server
		// If we're not at the depot
		if( m_locationType != TA_Base_Core::ILocation::DPT )
		{
			m_communicationsHandler.connectToServer( m_serverIPAddress, m_serverPort );
		}

		if( m_entityName.compare("") )
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
		FUNCTION_ENTRY("setToMonitorMode");

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager switching to Monitor Mode");


		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Setting datapoints to Monitor Mode");
				// set the data point to monitor mode

		// Dont set the PID datapoints to Monitor mode if this is the OCC agent
		// the PIDs are not real datapoints and are used to establish the address
		// of the station PIDs when sending display requests
		if( m_locationType != TA_Base_Core::ILocation::OCC )
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
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "The STISManager caught an exception while attempting to switch a datapoint to Monitor Mode");
				}
			}
		}

		StatusMonitor::getInstance()->setDataPointsToMonitorMode();

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STISManager Asking the Communications Handler to disconnect from the STIS Server");

		m_communicationsHandler.disconnectFromServer();
		
		// only need to deactivate the servant - dont need to delete it
		deactivateServant();

		FUNCTION_EXIT;
	}

	void STISManager::initialise( TA_Base_Core::IEntityDataPtr EntityData )
	{
		FUNCTION_ENTRY("initialise");

        m_stisEntityData = boost::dynamic_pointer_cast<TA_Base_Core::STISEntityData> (EntityData);
	
		TA_ASSERT( m_stisEntityData.get() != NULL, "STISManager entity data is incorrect" );

		m_entityKey = m_stisEntityData->getKey();
		m_entityTypeKey = m_stisEntityData->getTypeKey();
		m_locationKey = m_stisEntityData->getLocation();
		m_locationName = m_stisEntityData->getLocationName();
		m_subsystemKey = m_stisEntityData->getSubsystem();
		m_serverIPAddress = m_stisEntityData->getServerIPAddress();
		m_serverPort = m_stisEntityData->getServerPort();
		m_messageTimeout = m_stisEntityData->getMessageTimeout();
		m_messageRetries = m_stisEntityData->getMessageRetries();
		m_entityName = m_stisEntityData->getName();
		m_stationLibrarySynchronisationTimeout = m_stisEntityData->getStationLibrarySynchronisationTimeout();
		m_localDBString = m_stisEntityData->getLocalDBConnectionString();
        m_needVetting = m_stisEntityData->getVetting();

        try
        {
            m_locationType = TA_Base_Core::LocationAccessFactory::getInstance().getLocationType(m_locationName);
        }
        catch (...)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                "Error retrieving location type from database");

            // do a name based check
            if (m_locationName == "OCC")
            {
                m_locationType = TA_Base_Core::ILocation::OCC;
            }
            else
            {
                m_locationType = TA_Base_Core::ILocation::STATION;
            }
        }

            if( m_locationType == TA_Base_Core::ILocation::OCC )
            {
                std::vector<TA_Base_Core::IEntityData*> entityData;

                // Load all PID datapoints for all locations
                entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeWithNameLikeToken("DataPoint","%TIS%");

                std::string theName;
                std::vector<TA_Base_Core::IEntityData*>::iterator i;
                for ( i = entityData.begin(); i != entityData.end(); i++)
                {
                    theName = (*i)->getName();

                    DataPointDetails* pid = new DataPointDetails();

                    // Extract the station - this is the first three characters, up until the first "." character
                    std::string::size_type position = theName.find(".");

                    pid->station = theName.substr(0,position);

                    std::string::size_type position2 = theName.find(".",position+1);
                    std::string temp = theName.substr(position2+1,theName.size());
                    std::string::size_type position3 = temp.find(".");
                    std::string temp2 = temp.substr(position3+1,temp.size());
                    std::string::size_type position4 = temp2.find(".");

                    pid->shortPIDName = temp2.substr(0,position4);

                    pid->address = (*i)->getAddress();

                    // Ignore Data nodes at this level other than PIDs
                    if( ( pid->shortPIDName != "ISCS" ) && ( pid->shortPIDName != "SEV" ) && ( pid->shortPIDName != "TIS01" ) && ( pid->shortPIDName != "TIS02" ))
                    {
                        addDataPoint( pid );

                        // if this datapoint is the lock status, then initialise it to false since all pids
                        // are not locked initially
                        if (pid->dataPointName.compare(LOCKSTATUS_DP_NAME) == 0)
                        {
                            DpValue dpValue(pid->dataPoint->getDataPointDataType(),
                                            pid->dataPoint->getEnumLabelsMap(),
                                            pid->dataPoint->getBooleanLabels());
                            dpValue.setBoolean(false);
                            DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
                            pid->dataPoint->updateFieldState( newState );
                        }
                    }
                    else
                    {
                        delete pid;
                    }
                }

                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "PID datapoints loaded");

                for ( i = entityData.begin(); i != entityData.end(); i++)
                {
                    delete (*i);
                }
            }

            activateServantWithName( m_entityName );
		FUNCTION_EXIT;
	}

	unsigned long STISManager::getNewSequenceNumber()
	{
		FUNCTION_ENTRY("getNewSequenceNumber");

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
		FUNCTION_ENTRY("activate");

		if( m_entityName != "" )
		{
			activateServantWithName( m_entityName );
		}

		// TODO - throw an exception otherwise
		FUNCTION_EXIT;

	}

	std::string STISManager::getOperatorNameFromSession( const std::string& currentSessionID )
	{
		FUNCTION_ENTRY("getOperatorNameFromSession");

		std::string returnName("Not available");

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
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", dbe.what());
		}
		catch( const TA_Base_Core::DataException& de )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", de.what());
		}

		return returnName;

		FUNCTION_EXIT;
	}

	void STISManager::processResponseMessage( const std::vector<unsigned char>& message )
	{
		FUNCTION_ENTRY("processResponseMessage");

        // Establish the sequence number in the response message
        int sequenceNumber;

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
                "Received a response message of some form");

        std::stringstream responseStream;
        std::string sequenceNumberString;

        // Read the sequence number characters
		// The 'start of packet' byte is the first byte,
		// the following four are the sequence number (0000 - 9999)
		int i;

        for(i=1 ; i <= 4 ; i++)
        {
            responseStream << message[i];
        }
        sequenceNumberString = responseStream.str();

        sequenceNumber = atoi( sequenceNumberString.c_str() );

        std::vector<STISMessage*>::iterator iter;
		iter = m_pendingMessages.begin();

        // TODO - dont think I need to lock this
		TA_Base_Core::ThreadGuard guard( m_incomingMessageLock );

		bool foundAnOwner = false;

		for ( iter=m_pendingMessages.begin() ; iter < m_pendingMessages.end() ; iter++ )
		{
			if( (*iter)->getSequenceNumber() == sequenceNumber )
			{
				std::stringstream responseMessageIDStream;
				std::string responseMessageIDString;

				foundAnOwner = true;

				// Read the Message ID to ensure this not a NACK message
				for( i=19 ; i <= 21 ; i++)
				{
					responseMessageIDStream << message[i];
				}

				responseMessageIDString = responseMessageIDStream.str();


				// Grab the last two digits of the message ID 
				// of the original message, AND the response
				std::stringstream responseMessageIDNumberPortion;
				int i;
				for(i=1 ; i <= 2 ; i++)
				{
					responseMessageIDNumberPortion << responseMessageIDString[i];
				}

				std::string responseIDNumber = responseMessageIDNumberPortion.str();

				std::string messageIDString;
				std::stringstream messageIDStream;

				messageIDString = (*iter)->getMessageID();

				for( i=1 ; i <= 2 ; i++)
				{
					messageIDStream << messageIDString[i];
				}

				std::string messageIDNumber = messageIDStream.str();

		//		if( !messageIDString.compare("A99") )
				if( !responseMessageIDString.compare("A99"))
				{
					std::string nackMessage;

					nackMessage += "Received a NACK message for sequence number: ";
					nackMessage += sequenceNumberString;

					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError,
						nackMessage.c_str() );	
					
					// this WAS a NACK message
					(*iter)->setResponse( message, true );

					break;
				}
				else 
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
					"Found matching message for response message");

					// Give the message request its response message
					// this is NOT a NACK message
					(*iter)->setResponse( message, false );

					break;
				}
			}
		}

        // If no matching message was found - log for debug purposes
 		if( !foundAnOwner )
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
                "Received an unexpected response - discarding");
        }

		FUNCTION_EXIT;
	}


    void STISManager::submitStationStatusRequestMessage()
    {
		if( isConnectedToSTISServer() )
        {
		    // Create the new display request object
		    // Note - the object used is the same as approved RATIS messages
		    // this is due to the fact that the protocol uses the M12 message
		    // to fullfil both requirements
		    // i.e Approved messages to STIS PIDs, and ISCS -> RXH messages
		    STISStationStatusRequest* displayRequest = new STISStationStatusRequest();

		    displayRequest->setSequenceNumber( getNewSequenceNumber() );

            m_statusRequestQueue.addMessageToQueue(displayRequest);
        }
    }


    void STISManager::submitOCCStatusRequestMessage()
    {
		if( isConnectedToSTISServer() )
        {
		    // Create the new display request object
		    // Note - the object used is the same as approved RATIS messages
		    // this is due to the fact that the protocol uses the M12 message
		    // to fulfill both requirements
		    // i.e Approved messages to STIS PIDs, and ISCS -> RXH messages
		    STISOCCStatusRequest* displayRequest = new STISOCCStatusRequest();
		    displayRequest->setSequenceNumber( getNewSequenceNumber() );

            m_statusRequestQueue.addMessageToQueue(displayRequest);
        }
    }


	void STISManager::handleStatusRequestMessage(STISStatusRequest* displayRequest)
	{
		FUNCTION_ENTRY("handleStatusRequestMessage");

		if( isConnectedToSTISServer() )
		{
			// Send the message to the STIS server
			// This method blocks until the response message has been received
			// or the response is not received within the configured time period
			try
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Sending Status Request message");
				sendMessageWithWait( displayRequest );
			}
			catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
			{
                LOG_EXCEPTION_CATCH(SourceInfo, "STISServerNotConnectedException", 
                    "Status request message not sent - not connected to the STIS server");
				
                deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
				delete displayRequest;

				FUNCTION_EXIT;
                return;
            }    
			catch ( const TA_IRS_Core::TISAgentException& tae )
			{
                LOG_EXCEPTION_CATCH(SourceInfo, "TISAgentException", tae.what());

				deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
				delete displayRequest;

				FUNCTION_EXIT;
				return;
			}

			// Log the message if it was sent and processed successfully
			if( !displayRequest->hasReceivedResponse() )
			{
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Timed out while waiting for status response from STIS Server");
				deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
				delete displayRequest;

				FUNCTION_EXIT;
				return;
			}

			if( displayRequest->receivedNackResponse() )
			{
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid message sent to STIS server");
				deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
				delete displayRequest;

				FUNCTION_EXIT;
				return;
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Received response message");

				// No need to log this type of message

				displayRequest->processResponse();

				deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
				delete displayRequest;
			}
		}
		else
		{
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "Status request message not sent - not connected to the STIS server");
		}

		FUNCTION_EXIT;
	}

	void STISManager::submitTrainInformationMessage( 
            TA_IRS_Bus::StationIdType stationId,
            TA_IRS_Bus::TisPlatformIdType platformId, 
			const TA_IRS_Bus::IAtsTisCorbaDef::PlatformInfo& platformInfo, 
			const TA_IRS_Bus::IAtsTisCorbaDef::TrainInfoPair& trainInfoPair)
	{
		FUNCTION_ENTRY("submitTrainInformationMessage");

		if( isConnectedToSTISServer() )
		{
			//TD15198 azenitha++
			//TD10007
			//zhou yuan++
			if (m_raisedAtsForwardingFailureAlarm == true)
			{
				closeAlarm(TA_Base_Core::TISAlarms::AtsForwardingFailure);
				m_raisedAtsForwardingFailureAlarm = false;
			}
			//++zhou yuan
			//TD15198 ++azenitha

			STISTrainInformationMessage* displayRequest = new STISTrainInformationMessage();

			displayRequest->setSequenceNumber( getNewSequenceNumber() );

			// Use this agent's location name here (field 6 in the M09 message)
			// Each train in the train information message will supply its
			// destination station 
            std::string locationName = m_locationName;

            // if this is the OCC agent the location name will be OCC
            // this is not good for station platforms.
            if (stationId != getLocationKey())
            {
                // load the location name for the station id
                TA_Base_Core::ILocation* location = NULL;
                
                //wenbo++,CL-21314
                location = ILocationStore::getInstance()->getLocationByKey(stationId);
                //++wenbo               
               
                if ( location != NULL )               
                    locationName = location->getName();           
                
            }

			displayRequest->setData( platformId, locationName, platformInfo, trainInfoPair );

			// Send the message to the STIS server
			// This message is sent without waiting for an ACK from the STIS server
			try
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M09 Train Information message");
				sendMessageWithoutWait( displayRequest );

				//TD15198 azenitha++
				//TD10007
				//zhou yuan++
				if (m_raisedAtsForwardingFailureAlarm == true)
				{
					closeAlarm(TA_Base_Core::TISAlarms::AtsForwardingFailure);
					m_raisedAtsForwardingFailureAlarm = false;
				}
				//++zhou yuan
				//TD15198 ++azenitha
			}
			catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
			{
				delete displayRequest;

                //TD9180
				//TD10007
				//zhou yuan++
				if(!m_raisedAtsForwardingFailureAlarm)
				{
					TA_Base_Core::DescriptionParameters dp;
					submitAlarm(TA_Base_Core::TISAlarms::AtsForwardingFailure, dp);
					m_raisedAtsForwardingFailureAlarm = true; //TD15198 azenitha
				}
				//++zhou yuan

				FUNCTION_EXIT;
                throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
			}
			catch ( const TA_IRS_Core::TISAgentException& tae )
			{
				delete displayRequest;

                //TD9180
				//TD10007
				//zhou yuan++
				if(!m_raisedAtsForwardingFailureAlarm)
				{
					TA_Base_Core::DescriptionParameters dp;
					submitAlarm(TA_Base_Core::TISAlarms::AtsForwardingFailure, dp);
					m_raisedAtsForwardingFailureAlarm = true; //TD15198 azenitha
				}
				//++zhou yuan
				FUNCTION_EXIT;
				TA_THROW( TA_IRS_Core::TISAgentException( tae.what() ) );
			}


			// No need to log this type of message

			// or process the response
		
			delete displayRequest;

		}
		else
		{
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "OCC status request message not sent - not connected to STIS server");
			FUNCTION_EXIT;

            //TD9180
			//TD10007
			//zhou yuan++
			if(!m_raisedAtsForwardingFailureAlarm)
			{
				TA_Base_Core::DescriptionParameters dp;
				submitAlarm(TA_Base_Core::TISAlarms::AtsForwardingFailure, dp);
				m_raisedAtsForwardingFailureAlarm = true; //TD15198 azenitha
			}
			//++zhou yuan

			throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
		}
		FUNCTION_EXIT;
	}

	void STISManager::submitConfigurationDataRequestMessage()
	{
		FUNCTION_ENTRY("submitConfigurationDataRequestMessage");

		if( isConnectedToSTISServer() )
		{
			// Create the new display request object
			STISConfigurationDataRequest* displayRequest = new STISConfigurationDataRequest();

			displayRequest->setSequenceNumber( getNewSequenceNumber() );

			// Send the message to the STIS server
			// This method blocks until the response message has been received
			// or the response is not received within the configured time period
			try
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M00 Configuration Data Request message");
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

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
					"Received A00 response message");

			// No need to log this type of message

			displayRequest->processResponse();

			deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			delete displayRequest;

		}
		else
		{
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "OCC status request message not sent - not connected to STIS server");
			FUNCTION_EXIT;

			throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
		}
		FUNCTION_EXIT;
	}


	void STISManager::submitPIDControlRequest( const char* destination, TA_Base_Core::EPIDControl controlCommand, const char* sessionID )
	{
		FUNCTION_ENTRY("submitPIDControlRequest");

        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        // break the PID name up into parts
        std::vector<std::string> parts = tokenizePIDName( destination );

        if (parts.size() != 4)
        {
            // this is not a PID name
            std::stringstream error;
            error << "The PID name "<< destination << " is not valid.";
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException( error.str().c_str() );
        }

        // the first part is the station name
        if (parts[0] != m_locationName)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                        "The PID is not on this station.  Ignoring.");
			FUNCTION_EXIT;
            return;
        }

        // the PID belongs to this station
        if( !isConnectedToSTISServer() )
		{
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "OCC status request message not sent - not connected to STIS server");
			FUNCTION_EXIT;
			throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
		}

		// Create the new display request object
		// Note - the object used is the same as approved RATIS messages
		// this is due to the fact that the protocol uses the M12 message
		// to fullfil both requirements
		// i.e Approved messages to STIS PIDs, and ISCS -> RXH messages
		STISPIDControlRequest* displayRequest = new STISPIDControlRequest();

		displayRequest->setData( destination, controlCommand );

		displayRequest->setSequenceNumber( getNewSequenceNumber() );

		// Send the message to the STIS server
		// This blocks until the message is sent (it will retry a number of times)
		// or until an exception is thrown
		try
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending M31 OCC Status Request message");
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
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo,
				"Received A21 response message");

			// No need to log this type of message

			// or process the response

			deleteMessageFromPendingList( displayRequest->getSequenceNumber() );
			delete displayRequest;
		}
        
		// TD 9983
		sendPIDStatusChangedAuditMessage(destination, controlCommand ? TA_Base_Core::TURN_ON : TA_Base_Core::TURN_OFF, sessionID);
		// TD 9983

		FUNCTION_EXIT;
	}


	void STISManager::processIncomingRATISMessage( const TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageDetails& incomingMessage,
                                                   std::string sessionId /*= ""*/ )
	{
		FUNCTION_ENTRY("processIncomingRATISMessage");

		TA_ASSERT( m_locationType == TA_Base_Core::ILocation::OCC, "processIncomingRATISMessage is only available at the OCC");

		TA_Base_Core::ThreadGuard guard( m_incomingRATISLock );

        bool isClear = (incomingMessage.type==TA_Base_Core::RATIS_IN_CLEAR);
		
        std::vector<TA_IRS_Core::IRATISMessage*> existingMessages;
		try
		{
			existingMessages = TA_IRS_Core::RATISMessageAccessFactory::getInstance().getRATISMessages();
		}
		catch( const TA_Base_Core::DataException& de )
		{
			std::string msg("Caught a DataException while attempting to retreive existing RATIS messages");
			msg += de.what();

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, msg.c_str() );
		}
		catch( const TA_Base_Core::DatabaseException& dbe )
		{
			std::string msg("Caught a DatabaseException while attempting to retreive existing RATIS messages");
			msg += dbe.what();

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, msg.c_str() );
		}

        TA_IRS_Core::IRATISMessage* theMessage = NULL;
		// Store the incoming message to the database regardless of whether it requires operator vetting/approval
            theMessage = TA_IRS_Core::RATISMessageAccessFactory::getInstance().createNewRATISMessage();
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
				"Creating a new RATIS message");

		//theMessage->setSessionRef( /* TODO */ );
		theMessage->setStartTime( std::string(incomingMessage.startTime) );		
		theMessage->setEndTime( std::string(incomingMessage.endTime) );		
		theMessage->setRequiresVetting( incomingMessage.requiresVetting );		
		theMessage->setOverridable( incomingMessage.overridable );
        if (isClear)
        {
            theMessage->setType(TA_Base_Core::RATIS_IN_CLEAR);
        }
        else
        {   // we've initialised the message as new before
            theMessage->setType(TA_Base_Core::RATIS_IN_NEW);
        }
		theMessage->setStatus( incomingMessage.status );
		theMessage->setCreationDate( incomingMessage.timeCreated );
		theMessage->setPriority( incomingMessage.priority );
		theMessage->setTag( std::string(incomingMessage.tag) );
		theMessage->setDestination( std::string(incomingMessage.destination) );
		theMessage->setMessageText( std::string(incomingMessage.messageText) );

		theMessage->applyChanges();

        // Submit audit message saying RATIS message was submitted
        sendRatisSubmittedAuditMessage(incomingMessage.requiresVetting, theMessage->getType(),
            incomingMessage.messageText.in(), getFullDestinationString(incomingMessage.destination.in()), 
            incomingMessage.tag.in(), incomingMessage.priority, incomingMessage.startTime.in(), 
            incomingMessage.endTime.in(), incomingMessage.overridable, sessionId);
            
        // If number of message in call banner exceeds 100, message will be rejected
        // Only let the banner know about it if it's a TTIS clear request or requires vetting
        TA_Base_Bus::ISTISManagerCorbaDef::RATISMessageList* currentRatisList = getAllIncomingRATISMessages();
        if (currentRatisList->length() > 100)
        {
            submitRATISVettingResponse( theMessage->getKey(), false, 0, "", sessionId.c_str() );           
        } 
		// TD17572 
		// Cannot find the requirement why should add "isClear && theMessage->isForTTIS()"
		else if ((incomingMessage.requiresVetting && m_needVetting) /*|| (isClear && theMessage->isForTTIS())*/)
		// TD17572
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
				"Message requires vetting, going to notify MFT");
			TA_Base_Bus::ISTISManagerCorbaDef::IncomingRATISEvent message;

			message.messageID = theMessage->getKey();
			message.sessionRef = theMessage->getSessionRef();
			message.requiresVetting = theMessage->getRequiresVetting();
			message.type = theMessage->getType();

			CORBA::Any data;
			data <<= message;

			if( NULL != m_tisMessageSender )
			{
			    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
				    "Sending IncomingRATISMessage");
				m_tisMessageSender->sendCommsMessage(
					TA_Base_Core::TISComms::IncomingRATISMessage,
					m_entityKey,
					data,
				    m_subsystemKey,
				    m_locationKey,
					NULL); // filter
			}
			else
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Could not broadcast message to indicate incoming RATIS message");
			}
        }
		else // This message does not require vetting, or ISCS vetting is off - approve it straight away
		{
			submitRATISVettingResponse( theMessage->getKey(), true, 0, "", sessionId.c_str() );
			// perhaps just update its status in the database?

            // IF REMOVED then ensure that the resubmitting of vetting responses also works
		}

        delete currentRatisList;
        currentRatisList = NULL;

        {
            delete theMessage;
            theMessage = NULL;
        }

        unsigned int i = 0;
		for (i=0; i < existingMessages.size() ; i++ )
		{
			delete existingMessages[i];
            existingMessages[i] = NULL;
		}

		FUNCTION_EXIT;
	}

	void STISManager::newMessageLibraryReady( int newVersion, TA_IRS_App::EMessageLibraryType type )
	{
		FUNCTION_ENTRY("newMessageLibraryReady");

		TA_ASSERT( m_locationType == TA_Base_Core::ILocation::OCC, "newMessageLibraryReady is only available at the OCC");
			
		switch( type )
		{
		case TA_IRS_App::STATION_LIBRARY:

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "New STIS Predefined message library ready. STISManager starting the STIS message library synchroniser thread...");

			if( m_stationLibrarySynchronisationInProgress == false )
			{
				m_stationMessageLibrarySynchroniser->initiateSynchronisation();
			}

			m_stationLibrarySynchronisationInProgress = true;

			break;

		case TA_IRS_App::TRAIN_LIBRARY:

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "New TTIS Predefined message library ready. STISManager starting the TTIS message library synchroniser thread...");

			if( m_trainLibrarySynchronisationInProgress == false )
			{
				m_trainMessageLibrarySynchroniser->initiateSynchronisation();
			}

            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"hongran : m_trainLibrarySynchronisationInProgress is set to be null in newMessageLibraryReady");
			m_trainLibrarySynchronisationInProgress = true;

			break;

		}

		FUNCTION_EXIT;
	}

	void STISManager::updateATSTrainInformation()
	{
		FUNCTION_ENTRY("updateATSTrainInformation");

		FUNCTION_EXIT;
	}

	void STISManager::sendMessageWithWait( STISMessage* message )
	{
		FUNCTION_ENTRY("sendMessageWithWait");

		{
			TA_Base_Core::ThreadGuard guard( m_messageLock );

			// Add it to the list of pending messages
			m_pendingMessages.push_back( message );
		}
	
		// Create a TIS protocol message from the object's data members
        // the message will differ depending on the object
		std::vector<unsigned char> protocolMessage = message->createProtocolMessage();

		// Log the message data
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,  message->createLogMessage(protocolMessage).c_str());

		int retries = 0;
		while( ( !message->hasReceivedResponse() ) && ( retries < m_messageRetries ) )
		{
			if( retries > 0 )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Timed out while waiting for response, retrying...");
			}

			try
			{
				m_communicationsHandler.sendMessage( protocolMessage );
			}
			catch( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Not connected to STIS server");
				throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
			}
			catch( ... )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Error sending message to STIS server");
				TA_THROW( TA_IRS_Core::TISAgentException( "Error sending message to STIS server" ) );
			}

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"Message sent to STIS server, waiting for response...");

			int timer = 0;
			while( ( timer < m_messageTimeout ) && ( !message->hasReceivedResponse()) )
			{
				TA_Base_Core::Thread::sleep(250);
				timer += 250;
			}

			retries++;
		}

		if( !message->hasReceivedResponse() )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Timed out while waiting for response, reached maximum retries - giving up.");
			//TD10645
			m_communicationsHandler.reconnect();
		}

		FUNCTION_EXIT;
	}

	void STISManager::sendMessageWithoutWait( STISMessage* message )
	{
		FUNCTION_ENTRY("sendMessageWithoutWait");

		// Create a TIS protocol message from the object's data members
        // the message will differ depending on the object
		std::vector<unsigned char> protocolMessage = message->createProtocolMessage();

		//LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "Sending message to STIS server...");

		int retries = 0;
		bool messageSent = false;

		while( ( retries < m_messageRetries ) && ( !messageSent ) )
		{
			if( retries > 0 )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Could not send message, retrying...");
			}

			try
			{
				m_communicationsHandler.sendMessage( protocolMessage );

				messageSent = true;

				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"Message sent to STIS server.");
			}
			catch( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Not connected to STIS server");
				throw TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException();
			}
			catch( ... )
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Error sending message to STIS server");
				TA_THROW( TA_IRS_Core::TISAgentException( "Error sending message to STIS server" ) );
			}

			retries++;
		}

		FUNCTION_EXIT;
	}

	void STISManager::deleteMessageFromPendingList( int sequenceNumber )
	{
		FUNCTION_ENTRY("deleteMessageFromPendingList");

		TA_Base_Core::ThreadGuard guard( m_messageLock );

		std::vector<STISMessage*>::iterator i;
		i = m_pendingMessages.begin();

		for ( ; i < m_pendingMessages.end() ; i++ )
		{
			if( (*i)->getSequenceNumber() == sequenceNumber )
			{
				m_pendingMessages.erase(i);
				FUNCTION_EXIT;
				return;
			}

		}

		TA_ASSERT(false,"Message with specified sequence number not in pending list");
		FUNCTION_EXIT;
	}

	bool STISManager::isConnectedToSTISServer()
	{

		return m_communicationsHandler.isConnected();
	}

    CORBA::Boolean STISManager::reportStationLibraryVersionReceived( CORBA::UShort newVersion )
    {
		FUNCTION_ENTRY("reportStationLibraryVersionReceived");

        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        bool received = false;

        try
        {
            TA_Base_Core::IPredefinedMessageLibrary* stisMessageLibrary = NULL;

			//hongran++ TD17500
			// if on occ, then check the version in occ DB.
			if (m_locationType == TA_Base_Core::ILocation::OCC)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"@ Attempt to check station lib version [%d] in occ DB ",  newVersion);
				stisMessageLibrary = TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getOCCLocalPredefinedMessageLibrary(
					newVersion,
					TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS);
				if (stisMessageLibrary == NULL)
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"@ stisMessageLibrary is NULL " );
				}
				else
				{
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
						"@ stisMessageLibrary not null, size is : %d " , stisMessageLibrary->getKey());
				}
				
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"@ New lib version is exist in occ DB " );
			}
			else
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"@ Attempt to check station lib version [%d] in station DB ",  newVersion);
				stisMessageLibrary = TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary(
					newVersion,
					TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_STIS);
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
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
        catch( const TA_Base_Core::DataException& de)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", de.what());
        }
        catch( const TA_Base_Core::DatabaseException& dbe)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", dbe.what());
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "...", "while attempting to report Station Library Version Received");
		}

		FUNCTION_EXIT;
        return received;
    }

    CORBA::Boolean STISManager::reportTrainLibraryVersionReceived( CORBA::UShort newVersion )
    {
		FUNCTION_ENTRY("reportTrainLibraryVersionReceived");

        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        bool received = false;

        try
        {
            TA_Base_Core::IPredefinedMessageLibrary* ttisMessageLibrary = NULL;
            
			//hongran++ TD17500
			// if on occ, then check the version in occ DB.
			if (m_locationType == TA_Base_Core::ILocation::OCC)
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"@ Attempt to check station lib version [%d] in occ DB ",  newVersion);
				ttisMessageLibrary = TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getOCCLocalPredefinedMessageLibrary(
					newVersion,
					TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_TTIS);
			}
			else
			{
				LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"@ Attempt to check station lib version [%d] in station DB ",  newVersion);
				ttisMessageLibrary = TA_Base_Core::PredefinedMessageLibraryAccessFactory::getInstance().getPredefinedMessageLibrary(
					newVersion,
					TA_Base_Core::TA_TISAgentDAI::LIBRARY_TYPE_TTIS);
					LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"@ New lib version is exist in station DB " );
			}
            delete ttisMessageLibrary;
            ttisMessageLibrary = NULL;

            received = true;

            // This method will update StatusMonitor::m_newTrainLibraryVersion
            // when it examines the version in the database
            StatusMonitor::getInstance()->setNextISCSTrainLibraryVersion( newVersion );
        }
        catch( const TA_Base_Core::DataException& de)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DataException", de.what());
        }
        catch( const TA_Base_Core::DatabaseException& dbe)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::DatabaseException", dbe.what());
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "...", "while attempting to report Station Library Version Received");
		}

		FUNCTION_EXIT;
        return received;
    }

	void STISManager::processNewMessageLibrary( TA_IRS_App::EMessageLibraryType type, const std::string& filename, int indicatedVersion )
	{
            FUNCTION_ENTRY( "processNewMessageLibrary" );

            TA_ASSERT( m_messageLibraryManager != NULL, "m_messageLibraryManager is NULL" )
		
		switch( type )
		{
		case STATION_LIBRARY:
			//m_messageLibraryManager->processNewMessageLibrary( type, StatusMonitor::getInstance()->getNextSTISLibraryFilename() );
			m_messageLibraryManager->processNewMessageLibrary( type, filename, indicatedVersion );
			break;

		case TRAIN_LIBRARY:
			//m_messageLibraryManager->processNewMessageLibrary( type, StatusMonitor::getInstance()->getNextTTISLibraryFilename() );
			m_messageLibraryManager->processNewMessageLibrary( type, filename, indicatedVersion );
			break;
		}
		
            FUNCTION_EXIT;
	}

	CORBA::Boolean STISManager::isStationLibrarySynchronisationComplete()
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		bool complete = StatusMonitor::getInstance()->getStationLibrariesSynchronised();

		return CORBA::Boolean(complete);
	}

	CORBA::Boolean STISManager::isTrainLibrarySynchronisationComplete()
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		bool complete = StatusMonitor::getInstance()->getTrainLibrariesSynchronised();

		return CORBA::Boolean(complete);
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


	void STISManager::updatePIDStatus( int pidAddress, int status )
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"Updating PID status");
		// Find the PID by the supplied address
		
		std::vector< DataPointDetails* >::iterator iter;
		for( iter = m_pidVirtualDataPointList.begin() ; iter != m_pidVirtualDataPointList.end() ; iter++ )
		{
			//TD 14395
			//zhou yuan++ 
			try
			{
				int addr = atoi((*iter)->address.c_str());

				// Ensure that we find a Status datapoint with the correct address
				if ( (addr == pidAddress ) && ( (*iter)->dataPointName.compare(STATUS_DP_NAME) == 0 ))
				{
					DpValue dpValue((*iter)->dataPoint->getDataPointDataType(),
									(*iter)->dataPoint->getEnumLabelsMap(),
									(*iter)->dataPoint->getBooleanLabels());
					//TD 14669
					//zhou yuan ++
					dpValue.setEnum(status);

					DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
					(*iter)->dataPoint->updateFieldState( newState );
				}

				// TD8333 - Set Lock Status Quality Status to GOOD
				if ( (addr == pidAddress ) && ( (*iter)->dataPointName.compare(LOCKSTATUS_DP_NAME) == 0 ))
				{
					DpValue dpValue = (*iter)->dataPoint->getCurrentValue();
                
					DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
                
					(*iter)->dataPoint->updateFieldState( newState );
				}
			}
			catch(...)
			{
				//TD 14669
				//zhou yuan ++
				LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "set data point failure, while update PID's status.");
			}
        }
	}

	void STISManager::addDataPoint( DataPointDetails* theDataPoint )
	{
		LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Added datapoint to STISManager");
		// 8333-Start
		// Initialize the LockStatus DataPoint to Good Quality.When the STISManager 
		// swithces to Monitor mode the status should be retrived automatically.
		// we are interested only when it's in control mode and datapoint creation.
		if(theDataPoint->dataPointName.compare(LOCKSTATUS_DP_NAME) == 0)
		{
			DpValue dpValue(theDataPoint->dataPoint->getDataPointDataType(),
							theDataPoint->dataPoint->getEnumLabelsMap(),
							theDataPoint->dataPoint->getBooleanLabels());
			DataPointState dataPointState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
			theDataPoint->dataPoint->updateFieldState( dataPointState );
		}
		// 8333-End
		m_pidVirtualDataPointList.push_back( theDataPoint );
	}

	void STISManager::setBadDataPointQuality()
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "STIS Manager setting quality of PID datapoints to bad");

        time_t now = time( NULL );

        std::vector< DataPointDetails* >::iterator iter;
        for( iter = m_pidVirtualDataPointList.begin() ; iter != m_pidVirtualDataPointList.end() ; iter++ )
        {
            DpValue dpValue = (*iter)->dataPoint->getCurrentValue();
            DataPointState dataPointState ( dpValue, now, TA_Base_Bus::QUALITY_BAD_LAST_KNOWN_VALUE );
            
			const_cast<DataPoint*>((*iter)->dataPoint)->updateFieldState( dataPointState );
		}

	}

	int STISManager::getPIDAddress( const std::string& pidName, const std::string& station )
	{
            FUNCTION_ENTRY( "getPIDAddress" );
		std::vector< TA_IRS_App::DataPointDetails* >::iterator iter;

		for( iter = m_pidVirtualDataPointList.begin() ; iter != m_pidVirtualDataPointList.end() ; iter++ )
		{
			// Check STATION at OCC only
			if( m_locationType == TA_Base_Core::ILocation::OCC )
			{
				// CL-20439
				if( ((*iter)->shortPIDName.compare( pidName ) == 0 ) && ( (*iter)->station.compare( station ) == 0 ) && ( (*iter)->address.compare( VIRTUAL_DATAPOINT_ADDRESS_NAME ) != 0) )
				{
                        FUNCTION_EXIT;
					return atoi( (*iter)->address.c_str() );
				}
			}
			else
			{
				// CL-20439
				if( (*iter)->shortPIDName.compare( pidName ) == 0 && ( (*iter)->address.compare( VIRTUAL_DATAPOINT_ADDRESS_NAME ) != 0) )
				{
                        FUNCTION_EXIT;
					return atoi( (*iter)->address.c_str() );
				}
			}
		}

            FUNCTION_EXIT;
		return 0;
	}

	unsigned short STISManager::getCurrentSTISMessageLibraryVersion()
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Received client request for STIS Message library version");
		return StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion();
	}

	unsigned short STISManager::getCurrentTTISMessageLibraryVersion()
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Received client request for TTIS Message library version");
		return StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion();
	}

	unsigned short STISManager::getNextSTISMessageLibraryVersion()
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Received client request for next STIS Message library version");
		return StatusMonitor::getInstance()->getNextISCSStationLibraryVersion();
	}

	unsigned short STISManager::getNextTTISMessageLibraryVersion()
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Received client request for next TTIS Message library version");
		return StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion();
	}

	//hongran++ TD17500
	unsigned short STISManager::getCurrentCDBSTISMessageLibraryVersion()
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Received client request for Central DB STIS Message library version");
		return StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion_central();
	}

	unsigned short STISManager::getCurrentCDBTTISMessageLibraryVersion()
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Received client request for Central DB TTIS Message library version");
		return StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion_central();
	}

	unsigned short STISManager::getNextCDBSTISMessageLibraryVersion()
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Received client request for Central DB next STIS Message library version");
		return StatusMonitor::getInstance()->getNextISCSStationLibraryVersion_central();
	}

	unsigned short STISManager::getNextCDBTTISMessageLibraryVersion()
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Received client request for Central DB next TTIS Message library version");
		return StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion_central();
	}
	//++hongran TD17500


    TA_Base_Core::TimeScheduleVersion STISManager::getCurrentTrainTimeScheduleVersion()
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Received client request for Train Time Schedule version");

        TA_Base_Core::TimeScheduleVersion timeScheduleInfo;

		timeScheduleInfo.version = StatusMonitor::getInstance()->getCurrentTrainTimeScheduleVersion();

		timeScheduleInfo.timeScheduleKey = StatusMonitor::getInstance()->getCurrentTrainTimeScheduleKey();

		return timeScheduleInfo;
	}

    void STISManager::setCurrentTrainTimeScheduleVersion( const TA_Base_Core::TimeScheduleVersion& scheduleVersion, const char* sessionID )
	{
        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		TA_Base_Core::ThreadGuard guard( m_scheduleVersionLock );

		if( m_locationType == TA_Base_Core::ILocation::OCC )
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Client attempted to set Current Train Time Schedule version");


			int version = StatusMonitor::getInstance()->getCurrentTrainTimeScheduleVersion();

			if( version == 999 )
			{
				version = 1;
			}
			else
			{
				version++;
			}

			StatusMonitor::getInstance()->setCurrentTrainTimeScheduleKey( scheduleVersion.timeScheduleKey );

			StatusMonitor::getInstance()->setCurrentTrainTimeScheduleVersion( version );
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Client attempted to set Current Train Time Schedule version - not the OCC agent, so no action taken");
		}
	}

	void STISManager::connectToServer()
	{		
            FUNCTION_ENTRY( "connectToServer" );

		m_communicationsHandler.connectToServer( m_serverIPAddress, m_serverPort );

            // Send a Configuration data message on startup
            try
            {
                submitConfigurationDataRequestMessage();
            }
            catch( TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException& )
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Timed out while waiting for status response from STIS Server");
	}
            catch( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "Status message not sent - not connected to the STIS server");
            }

            FUNCTION_EXIT;
        }
	void STISManager::synchroniseStationLibrary()
	{
        //TD12342
        //Mintao++
        //should do a checking before doing the initiate, since the member pointer may be NULL
        //
        if (m_stationMessageLibrarySynchroniser != NULL)
        {
		    TA_Base_Core::ThreadGuard guard( m_stationLibrarySynchronisationLock );

		    m_stationLibrarySynchronisationInProgress = true;
		    m_stationMessageLibrarySynchroniser->initiateSynchronisation();
        }
        else
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
					"Mintao(TD12342): Thread that used to examine Station library synchroniser is NULL");
        }
        //TD12342
        //Mintao++
	}

	void STISManager::synchroniseTrainLibrary()
	{
        //TD12342
        //Mintao++
        //should do a checking before doing the initiate, since the member pointer may be NULL
        //
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"hongran : synchroniseTrainLibrary is called");
        if (m_trainMessageLibrarySynchroniser != NULL)
        {
		    TA_Base_Core::ThreadGuard guard( m_trainLibrarySynchronisationLock );
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"hongran : m_trainLibrarySynchronisationInProgress is set to be null in synchroniseTrainLibrary");

		    m_trainLibrarySynchronisationInProgress = true;

		    m_trainMessageLibrarySynchroniser->initiateSynchronisation();
        }
        else
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
					"Mintao(TD12342): Thread that used to examine Train library synchroniser is NULL");
        }
        //TD12342
        //Mintao++
	}

	void STISManager::messageLibrarySynchronised( TA_IRS_App::EMessageLibraryType type )
	{
		if(type == TA_IRS_App::STATION_LIBRARY)
		{
			TA_Base_Core::ThreadGuard guard( m_stationLibrarySynchronisationLock );

			m_stationLibrarySynchronisationInProgress = false;
		}
		else if( type == TA_IRS_App::TRAIN_LIBRARY )
		{
			TA_Base_Core::ThreadGuard guard( m_trainLibrarySynchronisationLock );

			m_trainLibrarySynchronisationInProgress = false;
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

	void STISManager::upgradeStationLibrary(const std::string& sessionID)
	{
		TA_Base_Core::ThreadGuard guard( m_stationLibraryUpgradeLock );

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Station library upgrade initiated");

		m_stationLibraryUpgradeInProgress = true;

		m_stationMessageLibraryUpgrader->initiateUpgrade(sessionID);
	}

	void STISManager::upgradeTrainLibrary(const std::string& sessionID)
	{
		TA_Base_Core::ThreadGuard guard( m_trainLibraryUpgradeLock );

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				"Train library upgrade initiated");

		m_trainLibraryUpgradeInProgress = true;

		m_trainMessageLibraryUpgrader->initiateUpgrade(sessionID);
	}

	void STISManager::messageLibraryUpgraded( TA_IRS_App::EMessageLibraryType type )
	{
		if( type == TA_IRS_App::STATION_LIBRARY )
		{
			TA_Base_Core::ThreadGuard guard( m_stationLibraryUpgradeLock );

			m_stationLibraryUpgradeInProgress = false;
		}
		else if( type == TA_IRS_App::TRAIN_LIBRARY )
		{
			TA_Base_Core::ThreadGuard guard( m_trainLibraryUpgradeLock );

			m_trainLibraryUpgradeInProgress = false;
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

    std::vector<std::string> STISManager::tokenizePIDName(std::string pidName)
    {
        // funky boost tokenizer - break name on '.'
        std::vector<std::string> parts;
        typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

        boost::char_separator<char> sep(".");
        tokenizer tokens(pidName, sep);
        
        for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter)
        {
            parts.push_back(*tok_iter);
        }

        // if parts is empty, then this should return the entire string
        if (parts.size() == 0)
        {
            parts.push_back(pidName);
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
			""); // event key - not required 
	}

	void STISManager::submitAlarm( const TA_Base_Core::MessageType& type, const TA_Base_Core::DescriptionParameters& dp )
	{
            FUNCTION_ENTRY( "submitAlarm" );
			
            if( type.getTypeKey() == TA_Base_Core::TISAlarms::STISServerCommFailure.getTypeKey() )
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
	}
	
	void STISManager::closeAlarm( const TA_Base_Core::MessageType& type)
	{
		TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().closeAlarmAtLocation( type, m_entityKey, m_locationKey);
	}


    void STISManager::setLockStatus(const char* name, bool lock, const char* sessionID )
    {
        FUNCTION_ENTRY("setLockStatus");

        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"Setting PID lock status for %s to %d", name, lock);

        // TODO: use sessionId to determine rights

        // Find the PID by the supplied name.  The name is in the form of DBG.TIS.Platform.PDP01
        // we want to set pidName = PDP01 and station = DBG
        std::vector<std::string> parts = tokenizePIDName(name);

        if (parts.size() != 4)
        {
            // this is not a PID name
            std::stringstream error;
            error << "The PID name "<< name << " is not valid.";
            throw TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException( error.str().c_str() );
        }

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"Try to get the pid address");

        std::string& station = parts[0];
        std::string& pidName = parts[3];

        if (station != m_locationName)
        {
		    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				    "Pid is not on this station.  Ignoring.");
            FUNCTION_EXIT;
            return;
        }

    	int pidAddress = getPIDAddress(pidName, station);
		
		std::vector< DataPointDetails* >::iterator iter;
		for( iter = m_pidVirtualDataPointList.begin() ; iter != m_pidVirtualDataPointList.end() ; iter++ )
		{
			int addr = atoi((*iter)->address.c_str());

			// Ensure that we find a Status datapoint with the correct address
			if ( (addr == pidAddress ) && ( (*iter)->dataPointName.compare(LOCKSTATUS_DP_NAME) == 0 ))
			{
		        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				        "Found the datapoint, need to set the value now");

				DpValue dpValue((*iter)->dataPoint->getDataPointDataType(),
								(*iter)->dataPoint->getEnumLabelsMap(),
								(*iter)->dataPoint->getBooleanLabels());
				dpValue.setBoolean(lock);

				DataPointState newState ( dpValue, time( NULL ), TA_Base_Bus::QUALITY_GOOD_NO_SPECIFIC_REASON );
				(*iter)->dataPoint->updateFieldState( newState );
		        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				        "Successfully setting the PID lock status for %s to %d", name, lock);
                break;
			}
		}
        FUNCTION_EXIT;
    }


    bool STISManager::isRATISVettingOn()
    {
        FUNCTION_ENTRY("isRATISVettingOn");

        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        if (m_locationType != TA_Base_Core::ILocation::OCC)
        {
            // if we're not on OCC, then we can't set this
		    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"Can only get RATIS vetting status from OCC");
    		throw TA_Base_Bus::ISTISManagerCorbaDef::STISFunctionalityNotAvailableException();
        }

        return m_needVetting;
    }


    void STISManager::setRATISVetting(bool vettingStatus, const char* sessionID)
    {
        FUNCTION_ENTRY("setRATISVetting");

        TA_Base_Bus::GenericAgent::ensureControlMode();//limin

        if (m_locationType != TA_Base_Core::ILocation::OCC)
        {
            // if we're not on OCC, then we can't set this
		    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"Only OCC can set RATIS vetting");
    		throw TA_Base_Bus::ISTISManagerCorbaDef::STISFunctionalityNotAvailableException();
        }

        if (m_needVetting == vettingStatus)
        {
            // already got the right status, just return
		    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				"Already got the right vettting status. Returning");
            return;
        }

        try
        {
            m_stisEntityData->setVetting(vettingStatus);
            m_needVetting = vettingStatus;

		    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				    "Successfully set vetting to %d", vettingStatus);

            CORBA::Boolean vetting = vettingStatus ? 1 : 0;

		    CORBA::Any data;

            data <<= CORBA::Any::from_boolean (vetting);

            if( NULL != m_tisMessageSender )
		    {
			    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug,
				    "Sending RATISVetting to subsystem = %d, location = %d, entity = %d", m_subsystemKey, m_locationKey, m_entityKey);
			    m_tisMessageSender->sendCommsMessage(
				    TA_Base_Core::TISComms::RATISVetting,
				    m_entityKey,
				    data,
				    m_subsystemKey,
				    m_locationKey,
				    NULL); // filter
		    }
		    else
		    {
			    LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Could not broadcast message to indicate incoming RATIS message");
		    }

            sendRatisVettingAuditMessage(vettingStatus, sessionID);
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Exception", "Failed to set RATIS Vetting, got either Data Exception or Database Exception");
    		throw TA_Base_Bus::ISTISManagerCorbaDef::STISGeneralFailureException("Failed to set the RATIS vetting status");
        }
        FUNCTION_EXIT;
    }



    void STISManager::sendRatisSubmittedAuditMessage(bool vetted, TA_Base_Core::ERATISMessageType type,
            const std::string& message, const std::string& destination, const std::string& tag,
            int priority, const std::string& startTime, const std::string& endTime, bool override,
            const std::string& sessionID, std::string timeStamp) // TD 9981 add timestamp
    {
        if (m_auditMessageSender == NULL)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Could not broadcast message to indicate incoming RATIS message");
            return;
		}

        // Audit message content is:
        // Ratis [Vetted] [type] Message [Message] Submitted. Destination: [Destination] Tag: [Ratis Tag] 
        // Priority: [Priority] Start time: [Start time] End time: [End time] Override: [Override], 

		TA_Base_Core::DescriptionParameters desc;
        std::string typeStr;
        switch (type)
        {
        case TA_Base_Core::RATIS_IN_NEW:
            typeStr = "New IN";
            break;
        case TA_Base_Core::RATIS_OUT_NEW:
            typeStr = "New Out";
            break;
        case TA_Base_Core::RATIS_IN_UPDATE:
            typeStr = "Update In";
            break;
        case TA_Base_Core::RATIS_OUT_UPDATE:
            typeStr = "Update Out";
            break;
        case TA_Base_Core::RATIS_IN_CLEAR:
            typeStr = "Clear In";
            break;
        case TA_Base_Core::RATIS_OUT_CLEAR:
            typeStr = "Clear Out";
            break;
        default:
            break;
        }

        std::ostringstream priorityStream;
        priorityStream << priority;

        TA_Base_Core::NameValuePair vettedPair(VETTED_AUDIT_MESSAGE_PARAMETER_NAME, (vetted ? "Vetting" : "Non-vetting"));
		TA_Base_Core::NameValuePair typePair(TYPE_AUDIT_MESSAGE_PARAMETER_NAME, typeStr);
		TA_Base_Core::NameValuePair messagePair(MESSAGE_AUDIT_MESSAGE_PARAMETER_NAME, message);
		TA_Base_Core::NameValuePair destinationPair(DESTINATION_AUDIT_MESSAGE_PARAMETER_NAME, destination);
		TA_Base_Core::NameValuePair tagPair(RATISTAG_AUDIT_MESSAGE_PARAMETER_NAME, tag);
        TA_Base_Core::NameValuePair priorityPair(PRIORITY_AUDIT_MESSAGE_PARAMETER_NAME, priorityStream.str());
        TA_Base_Core::NameValuePair startTimePair(STARTTIME_AUDIT_MESSAGE_PARAMETER_NAME, 
            TA_IRS_Core::DisplayRequestData::startEndTimeToString(startTime));
        TA_Base_Core::NameValuePair endTimePair(ENDTIME_AUDIT_MESSAGE_PARAMETER_NAME, 
            TA_IRS_Core::DisplayRequestData::startEndTimeToString(endTime));
        TA_Base_Core::NameValuePair overridePair(OVERRIDE_AUDIT_MESSAGE_PARAMETER_NAME, override ? "Yes" : "No");

        desc.push_back(&vettedPair);
		desc.push_back(&typePair);
		desc.push_back(&messagePair);
		desc.push_back(&destinationPair);
		desc.push_back(&tagPair);
        desc.push_back(&priorityPair);
        desc.push_back(&startTimePair);
        desc.push_back(&endTimePair);
        desc.push_back(&overridePair);

		// TD 9981
		if (timeStamp == "")
		{
			m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISRatisSubmitted, m_entityKey, 
				desc, 
				"", // Further description text 
				sessionID,
				"", // alarm ID - not required
				"", // incident key - not required
				""); // event key - not required 
		}
		else
		{
			timeb loggedTime;
			StringHelper::isConvertToTimeb(timeStamp, loggedTime);

			m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISRatisSubmitted, m_entityKey, 
				desc, 
				"", // Further description text 
				sessionID,
				"", // alarm ID - not required
				"", // incident key - not required
				"",
				NULL,
				loggedTime); // event key - not required 
		}
		// TD 9981
    }


    void STISManager::sendRatisVettingAuditMessage(const bool vettingOn, const std::string& sessionID)
    {
        if (m_auditMessageSender == NULL)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Could not broadcast message to indicate incoming RATIS message");
            return;
		}

        // Audit message content is:
        // Ratis Vetting [Enabled/Disabled]
		TA_Base_Core::DescriptionParameters desc;
        TA_Base_Core::NameValuePair status(ENABLE_STATUS_AUDIT_MESSAGE_PARAMETER_NAME, vettingOn ? "Enabled" : "Disabled");

        desc.push_back(&status);

		m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISRatisVetting, m_entityKey, 
			desc, 
			"", // Further description text 
			sessionID,
			"", // alarm ID - not required
			"", // incident key - not required
			""); // event key - not required 
    }


    void STISManager::sendRatisProcessedAuditMessage(TA_Base_Core::ERATISMessageType type, const std::string& message,
            bool approved, const std::string& destination, const std::string& tag, int priority,
            const std::string& startTime, const std::string& endTime, bool override, const std::string& sessionID,
			std::string timestamp)
    {
        if (m_auditMessageSender == NULL)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Could not broadcast message to indicate incoming RATIS message");
            return;
		}

        // Audit message content is:
        // Incoming Ratis [type] Message [Message] [Approved/Rejected]. Destination: [Destination] 
        // Tag: [Ratis Tag] Priority: [Priority] Start time: [Start time] End time: [End time] Override: [Override]

        TA_Base_Core::DescriptionParameters desc;
        std::string typeStr;
        switch (type)
        {
        case TA_Base_Core::RATIS_IN_NEW:
            typeStr = "New IN";
            break;
        case TA_Base_Core::RATIS_OUT_NEW:
            typeStr = "New Out";
            break;
        case TA_Base_Core::RATIS_IN_UPDATE:
            typeStr = "Update In";
            break;
        case TA_Base_Core::RATIS_OUT_UPDATE:
            typeStr = "Update Out";
            break;
        case TA_Base_Core::RATIS_IN_CLEAR:
            typeStr = "Clear In";
            break;
        case TA_Base_Core::RATIS_OUT_CLEAR:
            typeStr = "Clear Out";
            break;
        default:
            break;
        }

        std::ostringstream priorityStream;
        priorityStream << priority;

		TA_Base_Core::NameValuePair typePair(TYPE_AUDIT_MESSAGE_PARAMETER_NAME, typeStr);
		TA_Base_Core::NameValuePair messagePair(MESSAGE_AUDIT_MESSAGE_PARAMETER_NAME, message);
        TA_Base_Core::NameValuePair approvedPair(APPROVAL_STATUS_AUDIT_MESSAGE_PARAMETER_NAME, approved ? "Approved" : "Rejected");
		TA_Base_Core::NameValuePair destinationPair(DESTINATION_AUDIT_MESSAGE_PARAMETER_NAME, destination);
		TA_Base_Core::NameValuePair tagPair(RATISTAG_AUDIT_MESSAGE_PARAMETER_NAME, tag);
        TA_Base_Core::NameValuePair priorityPair(PRIORITY_AUDIT_MESSAGE_PARAMETER_NAME, priorityStream.str());
        TA_Base_Core::NameValuePair startTimePair(STARTTIME_AUDIT_MESSAGE_PARAMETER_NAME, 
            TA_IRS_Core::DisplayRequestData::startEndTimeToString(startTime));
        TA_Base_Core::NameValuePair endTimePair(ENDTIME_AUDIT_MESSAGE_PARAMETER_NAME, 
            TA_IRS_Core::DisplayRequestData::startEndTimeToString(endTime));
        TA_Base_Core::NameValuePair overridePair(OVERRIDE_AUDIT_MESSAGE_PARAMETER_NAME, override ? "Yes" : "No");
        
		desc.push_back(&typePair);
		desc.push_back(&messagePair);
		desc.push_back(&approvedPair);
        desc.push_back(&destinationPair);
		desc.push_back(&tagPair);
		desc.push_back(&priorityPair);
        desc.push_back(&startTimePair);
        desc.push_back(&endTimePair);
        desc.push_back(&overridePair);

		if(timestamp == "")
		{
			m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISRatisProcessed, m_entityKey, 
				desc, 
				"", // Further description text 
				sessionID,
				"", // alarm ID - not required
				"", // incident key - not required
				""); // event key - not required 
		}
		else
		{
			timeb curtime;
			StringHelper::isConvertToTimeb(timestamp, curtime);
			m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISRatisProcessed, 
				m_entityKey, 
				desc, 
				"", // Further description text 
				sessionID,
				"", // alarm ID - not required
				"", // incident key - not required
				"", // event key - not required 
				NULL,
				curtime);
		}
    }

    // TD 16367
	void STISManager::sendRatisOutgoingAuditMessage(TA_Base_Core::ERATISMessageType type, const std::string& message,
            const std::string& destination, const std::string& tag, int priority,
            const std::string& startTime, const std::string& endTime, bool override, const std::string& sessionID,
			std::string timeStamp)
    {
        if (m_auditMessageSender == NULL)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Could not broadcast message to indicate outgoing RATIS message");
            return;
		}

        // Audit message content is:
        // Outgoing Ratis [type] Message [Message] Destination: [Destination] 
        // Tag: [Ratis Tag] Priority: [Priority] Start time: [Start time] End time: [End time] Override: [Override]

        TA_Base_Core::DescriptionParameters desc;
        std::string typeStr;
        switch (type)
        {
        case TA_Base_Core::RATIS_IN_NEW:
            typeStr = "New IN";
            break;
        case TA_Base_Core::RATIS_OUT_NEW:
            typeStr = "New Out";
            break;
        case TA_Base_Core::RATIS_IN_UPDATE:
            typeStr = "Update In";
            break;
        case TA_Base_Core::RATIS_OUT_UPDATE:
            typeStr = "Update Out";
            break;
        case TA_Base_Core::RATIS_IN_CLEAR:
            typeStr = "Clear In";
            break;
        case TA_Base_Core::RATIS_OUT_CLEAR:
            typeStr = "Clear Out";
            break;
        default:
            break;
        }

        std::ostringstream priorityStream;
        priorityStream << priority;

		TA_Base_Core::NameValuePair typePair(TYPE_AUDIT_MESSAGE_PARAMETER_NAME, typeStr);
		desc.push_back(&typePair);

		TA_Base_Core::NameValuePair messagePair(MESSAGE_AUDIT_MESSAGE_PARAMETER_NAME, message);
		desc.push_back(&messagePair);


		TA_Base_Core::NameValuePair destinationPair(DESTINATION_AUDIT_MESSAGE_PARAMETER_NAME, destination);
		TA_Base_Core::NameValuePair tagPair(RATISTAG_AUDIT_MESSAGE_PARAMETER_NAME, tag);
        TA_Base_Core::NameValuePair priorityPair(PRIORITY_AUDIT_MESSAGE_PARAMETER_NAME, priorityStream.str());

        desc.push_back(&destinationPair);
		desc.push_back(&tagPair);
		desc.push_back(&priorityPair);
		
		TA_Base_Core::NameValuePair startTimePair(STARTTIME_AUDIT_MESSAGE_PARAMETER_NAME, 
			TA_IRS_Core::DisplayRequestData::startEndTimeToString(startTime));
		TA_Base_Core::NameValuePair endTimePair(ENDTIME_AUDIT_MESSAGE_PARAMETER_NAME, 
			TA_IRS_Core::DisplayRequestData::startEndTimeToString(endTime));

		std::string overrideString("_");
		if ((type != TA_Base_Core::RATIS_IN_CLEAR) || (type != TA_Base_Core::RATIS_OUT_CLEAR))
		{
			overrideString = override ? "Yes" : "No";
		}

		TA_Base_Core::NameValuePair overridePair(OVERRIDE_AUDIT_MESSAGE_PARAMETER_NAME, overrideString);
			
		desc.push_back(&startTimePair);
		desc.push_back(&endTimePair);
		desc.push_back(&overridePair);
	
		if (timeStamp == "")
		{
			m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISRatisOutgoing, m_entityKey, 
													desc, 
													"", // Further description text 
													sessionID,
													"", // alarm ID - not required
													"", // incident key - not required
													""); // event key - not required 
		}
		else
		{
			timeb loggedTime;
			StringHelper::isConvertToTimeb(timeStamp, loggedTime);

			m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISRatisOutgoing, m_entityKey, 
													desc, 
													"", // Further description text 
													sessionID,
													"", // alarm ID - not required
													"", // incident key - not required
													"",
													NULL,
													loggedTime); // event key - not required 
		}


    }
    // TD 16367

    void STISManager::sendPIDStatusChangedAuditMessage(const std::string& pid, const bool isOn, const std::string& sessionID)
    {
        if (m_auditMessageSender == NULL)
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, "Could not broadcast message to indicate incoming RATIS message");
            return;
		}

        // Audit message content is:
        // The PID [PID] at Station [location] has been switched [on/off]
		TA_Base_Core::DescriptionParameters desc;
        TA_Base_Core::NameValuePair pidPair(PID_AUDIT_MESSAGE_PARAMETER_NAME, pid);
		// TD 9983
        TA_Base_Core::NameValuePair locationPair(PIDCONTROL_LOCATION_AUDIT_MESSAGE_PARAMETER_NAME, m_locationName);
		// TD 9983
        TA_Base_Core::NameValuePair statusPair(STATUS_AUDIT_MESSAGE_PARAMETER_NAME, isOn ? "on" : "off");

        desc.push_back(&pidPair);
        desc.push_back(&locationPair);
        desc.push_back(&statusPair);

		/*
		m_auditMessageSender->sendAuditMessage( TA_Base_Core::TISAudit::STISPidStatusChanged, m_entityKey, 
			desc, 
			"", // Further description text 
			sessionID,
			"", // alarm ID - not required
			"", // incident key - not required
			""); // event key - not required 
		*/
		m_auditMessageSender->sendAuditMessageWithAsset( TA_Base_Core::TISAudit::STISPidStatusChanged, m_entityKey, 
			desc, 
			"", // Further description text 
			sessionID,
			"", // alarm ID - not required
			"", // incident key - not required
			"",
			StatusMonitor::getInstance()->getAgentAssetName()); // event key - not required 
		
    }


    std::string STISManager::getFullDestinationString(const std::string destination) const
    {
        // e.g. the destination strings to expect:
        //  - CCLCVC000 (all station PIDS at CVC), 
        //  - CCLDBG002001002 (2 station PIDs at DBG)
        //  - CCLAST (All PIDs at all Stations)
        //  - CCLATR (All Trains)
        //  - CCLTRN001002 (1 train)

        // the first 3 characters is the system Id, next 3 is the station
        static const int beginStationIndex = 3;
        static const int stationSize = 3;
        static const int numIdSize = 3;
        static const int idSize = 3;
        static const int endStationIndex = beginStationIndex + stationSize;
        static const int endNumIdIndex = endStationIndex + numIdSize;

        std::string station = destination.substr(beginStationIndex, stationSize);

        if (station.compare("sys") == 0)
        {
            // return the rest of the string if a destination we're going to send to RXH
            return destination.substr(endStationIndex, destination.size());
        }
        else if (station.compare("ATR") == 0)
        {
            return "All Trains";
        }
        else if (station.compare("AST") == 0)
        {
            return "All PIDs at all Stations";
        }
        else if (station.compare("TRN") == 0)
        {
            // TODO: what to do for train?
            std::string destStr = "Train ID: ";

            int numIds = atol(destination.substr(endStationIndex, numIdSize).c_str());
            // check if size is right
            int num = (destination.size() - endNumIdIndex) / idSize;
            if (num < numIds)
            {
                // the string is somehow incomplete, we'll just display what we can
                numIds = num;
            }

            for (int i = 0; i < numIds; i++)
            {
                destStr += " " + destination.substr(endNumIdIndex + i * idSize, idSize);
            }
            return destStr;
        }
        else
        {
            // Stations.  see how many items there are
            std::string destStr = station + ": ";

            // if the number of id is 0, then it means all pids are involved
            int numIds = atol(destination.substr(endStationIndex, numIdSize).c_str());
            if (numIds == 0)
            {
                destStr += "All PIDs";
                return destStr;
            }

            // check if size is right
            int num = (destination.size() - endNumIdIndex) / idSize;
            if (num < numIds)
            {
                // the string is somehow incomplete, we'll just display what we can
                numIds = num;
            }

        	std::vector<TA_IRS_App::DataPointDetails*>::const_iterator iter;
            for (int i = 0; i < numIds; i++)
            {
                std::stringstream temp;
                temp << atol(destination.substr(endNumIdIndex + i * idSize, idSize).c_str());
                std::string address = temp.str();

		        for (iter = m_pidVirtualDataPointList.begin() ; iter != m_pidVirtualDataPointList.end() ; iter++)
		        {
				    if (((*iter)->address.compare(address) == 0) && ((*iter)->station.compare(station) == 0))
				    {
                        destStr += " " + (*iter)->shortPIDName;
                        break;
				    }
                }
            }

            return destStr;
        }
        return destination;
    }


    void STISManager::getSTISDestinationFromRATISDestination( const std::string& destination,
                                                              std::string& destinationStation,
                                                              std::string& destinationLevels,
                                                              std::string& destinationPIDs ) const
    {
        // e.g. the destination strings to expect:
        //  - CCLCVC000 (all station PIDS at CVC), 
        //  - CCLDBG002001002 (2 station PIDs at DBG)
        //  - CCLAST (All PIDs at all Stations)
        //  - CCLATR (All Trains)
        //  - CCLTRN001002 (1 train)

        // this is not valid for train destinations - as this is only used to log incoming RATIS
        // messages destined to CCL

        // the first 3 characters is the system Id, next 3 is the station
        static const int beginStationIndex = 3;
        static const int stationSize = 3;
        static const int numIdSize = 3;
        static const int idSize = 3;
        static const int endStationIndex = beginStationIndex + stationSize;
        static const int endNumIdIndex = endStationIndex + numIdSize;

        if (destination.size() < 6)
        {
            // stop a crash in case this is called on the wrong type of string
            return;
        }

        std::string station = destination.substr(beginStationIndex, stationSize);

        if (station.compare("AST") == 0)
        {
            destinationStation = "AST";
            destinationPIDs = "All PIDs";
        }
        else
        {
            // Station.  see how many items there are
            destinationStation = station;

            if (destination.size() < endNumIdIndex )
            {
                // stop a crash in case this is called on the wrong type of string
                return;
            }

            // if the number of id is 0, then it means all pids are involved
            int numIds = atol(destination.substr(endStationIndex, numIdSize).c_str());
            if (numIds == 0)
            {
                destinationPIDs = "All PIDs";
                return;
            }

            // check if size is right
            int num = (destination.size() - endNumIdIndex) / idSize;
            if (num < numIds)
            {
                // the string is somehow incomplete, we'll just display what we can
                numIds = num;
            }

        	std::vector<TA_IRS_App::DataPointDetails*>::const_iterator iter;
            for (int i = 0; i < numIds; i++)
            {
                std::stringstream temp;
                temp << atol(destination.substr(endNumIdIndex + i * idSize, idSize).c_str());
                std::string address = temp.str();

		        for (iter = m_pidVirtualDataPointList.begin() ; iter != m_pidVirtualDataPointList.end() ; iter++)
		        {
				    if (((*iter)->address.compare(address) == 0) && ((*iter)->station.compare(station) == 0))
				    {
                        destinationPIDs += (*iter)->shortPIDName + " ";
                        break;
				    }
                }
            }
        }

	
	}

		

	//hongran++ TD17500
	void STISManager::upgradeOCCStationLibraryVersion(unsigned short version)
	{
		int currentISCSStationLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSStationLibraryVersion();
		int nextISCSStationLibraryVersion = StatusMonitor::getInstance()->getNextISCSStationLibraryVersion();

		if( currentISCSStationLibraryVersion != nextISCSStationLibraryVersion )
		{
			StatusMonitor::getInstance()->setCurrentISCSStationLibraryVersion( nextISCSStationLibraryVersion );

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "TD17500 Successfully  upgradeOCCStationLibraryVersion ");
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"OCC has requested a predefined STIS message library upgrade. Current and next library version are the same - no upgrade required");
		}

	}


	void STISManager::upgradeOCCTrainLibraryVersion(unsigned short version)
	{
		int currentISCSTrainLibraryVersion = StatusMonitor::getInstance()->getCurrentISCSTrainLibraryVersion();
		int nextISCSTrainLibraryVersion = StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion();

		if( currentISCSTrainLibraryVersion != nextISCSTrainLibraryVersion )
		{
			StatusMonitor::getInstance()->setCurrentISCSTrainLibraryVersion( nextISCSTrainLibraryVersion );

			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "TD17500 Successfully  upgradeOCCTrainLibraryVersion ");
		}
		else
		{
			LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
			"OCC has requested a predefined TTIS message library upgrade. Current and next library version are the same - no upgrade required");
		}
		
	}
	//++hongran TD17500
	
	void STISFailoverThread::run()
	{
		StatusMonitor::getInstance()->setDataPointsToControlMode();
			
		StatusMonitor::getInstance()->setStationLibrariesSynchronised(true);
		StatusMonitor::getInstance()->setTrainLibrariesSynchronised(true);
	};
	
}
