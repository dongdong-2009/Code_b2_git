/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/signs/tis_log_viewer/src/DisplayRequestSender.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  *
  */

#include "app/signs/tis_log_viewer/src/stdafx.h"

#include "app/signs/tis_log_viewer/src/DisplayRequestCondition.h"
#include "app/signs/tis_log_viewer/src/DisplayRequestSender.h"
#include "app/signs/tis_log_viewer/src/PredefinedMessageHelper.h"
#include "app/signs/tis_log_viewer/src/UserMessages.h"

#include "bus/mfc_extensions/src/progress_window/ProgressWnd.h"
#include "bus/signs_4669/TISManagerCorbaDef/src/ITTISManagerCorbaDef.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/trains/TrainCommonLibrary/src/AgentCommunicationException.h"
#include "bus/trains/TrainCommonLibrary/src/InvalidParameterException.h"

#include "core/data_access_interface/entity_access/src/TTISEntityData.h"
#include "core/data_access_interface/tis_agent/src/IDisplayRequest.h"
#include "core/data_access_interface/tis_agent_4669/IDL/src/TISMessageDataDefinitions.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/TISComms_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#include <boost/lexical_cast.hpp>

#include <sstream>


using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_IRS_Core::IDisplayRequest;
using TA_Base_Bus::TISAgentAccessFactory;
using TA_Base_Core::DataException;


#define REASONABLE_TIMEOUT_VALUE 500

namespace TA_IRS_App
{

    DisplayRequestSender::DisplayRequestSender( unsigned long locationKey )
        : m_locationKey( locationKey )
    {
        FUNCTION_ENTRY( "DisplayRequestSender" );

        subscribeToMessages();

        FUNCTION_EXIT;
    }


    DisplayRequestSender::~DisplayRequestSender()
    {
        FUNCTION_ENTRY( "~DisplayRequestSender" );

        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );

        FUNCTION_EXIT;
    }


    void DisplayRequestSender::sendRequest( IDisplayRequest* request )
    {
        FUNCTION_ENTRY( "sendRequest" );

        CWaitCursor wait;

        // if its null, nothing was selected. This button shouldnt even be enabled
        TA_ASSERT( request != NULL, "Given a null request" );

        try
        {
            if ( request->getDestinationSystem() == "TTIS" )
            {
                resubmitTtisDisplayRequest( request );
                UserMessages::getInstance().displayInfo( "Request sent successfully" );
            }
            else if ( request->getDestinationSystem() == "STIS" )
            {
                resubmitStisDisplayRequest( request );
                UserMessages::getInstance().displayInfo( "Request sent successfully" );
            }
            else if ( request->getDestinationSystem() == "RATIS" )
            {
                resubmitStisDisplayRequest( request );
                UserMessages::getInstance().displayInfo( "Request sent successfully" );
            }
            else
            {
                resubmitStisDisplayRequest( request );
                UserMessages::getInstance().displayInfo( "Request sent successfully" );
            }
        }
        //
        // TTIS exceptions
        //
        catch ( TA_Base_Core::OperationModeException& e )
        {
            // log it
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::OperationModeException", e.reason.in() );

            UserMessages::getInstance().displayError( "Train error while re-submitting request. Request not submitted." );
        }
        catch ( TA_Base_Core::AccessDeniedException& e )
        {
            // log it
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::AccessDeniedException", e.reason.in() );

            UserMessages::getInstance().displayError( "Train error while re-submitting request. Request not submitted." );
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter& ex )
        {
            // log it
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TTISInvalidParameter", ex.details );

            std::stringstream msg;
            msg << "Error re-submitting request." << std::string( ex.details );
            UserMessages::getInstance().displayError( msg.str().c_str() );
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure& ex )
        {
            // log it
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", ex.details );

            UserMessages::getInstance().displayError( "Train error while re-submitting request. Request not submitted." );
        }
        //
        // STIS exceptions
        //
        catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISServerNotConnectedException& )
        {
            // log it
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "STISServerNotConnectedException", "STISServerNotConnectedException" );

            UserMessages::getInstance().displayError( "STIS server connection error while re-submitting request. Request not submitted." );
        }
        catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISCommunicationTimeoutException& )
        {
            // log it
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "STISCommunicationTimeoutException", "STISCommunicationTimeoutException" );

            UserMessages::getInstance().displayError( "STIS server connection timed out while re-submitting request. Request not submitted." );
        }
        catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISFunctionalityNotAvailableException& )
        {
            // log it
	        LOG( SourceInfo, DebugUtil::ExceptionCatch, "STISFunctionalityNotAvailableException", "STISFunctionalityNotAvailableException" );

            UserMessages::getInstance().displayError( "The STIS server does not support that request. Request not submitted." );
        }
        catch ( const TA_Base_Bus::ISTISManagerCorbaDef::STISInvalidParameterException& ivp )
        {
            // log it
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "STISInvalidParameterException", ivp.details.in() );

            std::string error( "Error while re-submitting request. " );
            error += ivp.details.in();
            UserMessages::getInstance().displayError( error.c_str() );
        }
        //
        // Common exceptions
        //
        catch ( const TA_Base_Core::DatabaseException& dbe )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DatabaseException", dbe.what() );

            UserMessages::getInstance().displayError( "Error while connecting to the TIS Agent. Request not submitted." );
        }
        catch ( const TA_Base_Core::DataException& de )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::DataException", de.what() );

            UserMessages::getInstance().displayError( "Error while connecting to the TIS Agent. Request not submitted." );
        }
        catch ( const TA_Base_Core::ObjectResolutionException& ore )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "TA_Base_Core::ObjectResolutionException", ore.what() );

            UserMessages::getInstance().displayError( "Error while connecting to the TIS Agent. Request not submitted." );
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while calling submitClearRequest: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG( SourceInfo, DebugUtil::ExceptionCatch, "CORBA", exceptionMsg.c_str() );

            UserMessages::getInstance().displayError( "Error while connecting to the TIS Agent. Request not submitted." );
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestSender::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        if ( TA_Base_Core::TISComms::TisTrainDownloadUpdate.getTypeKey() == payload.messageTypeKey.in() )
        {
            TA_Base_Core::TTISDisplayResult* displayResult;

            if ( payload.messageState >>= displayResult )
            {
                TA_THREADGUARD( m_displayRequestsLock );

                // pass the result onto the object waiting for it
                for ( DisplayRequests::iterator iter = m_displayRequests.begin();
                      m_displayRequests.end() != iter; ++iter )
                {
                    if ( true == ( *iter )->handleDisplayResult( *displayResult ) )
                    {
                        // the display request is complete
                        m_displayRequests.erase( iter );

                        // exit the loop
                        break;
                    }
                }
            }
            else
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                             "Could not extract TTISDisplayResult from CommsMessageCorbaDef" );
            }
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestSender::subscribeToMessages()
    {
        FUNCTION_ENTRY( "subscribeToMessages" );

        //
        // Get the entity, subsystem and location key.
        // There should only be a single entity at a given location.
        //
        try
        {
            TA_Base_Core::IEntityDataList entityData =
                TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation( TA_Base_Core::TTISEntityData::getStaticType(),
                                                                                              m_locationKey );


            if ( entityData.size() != 1 )
            {
                const char* msg = "There is no TIS agent at this location.";
                const char* fld = "TIS Agent";
                TA_THROW( DataException( msg, DataException::MISSING_MANDATORY, fld ) );
            }

            unsigned long entityKey = entityData[0]->getKey();

            delete entityData[0];

            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
                TA_Base_Core::TISComms::TrainDisplayResult,
                this,
                entityKey,
                0,
                m_locationKey );
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


    void DisplayRequestSender::addPendingDisplayRequest( DisplayRequestCondition* requestCondition )
    {
        FUNCTION_ENTRY( "addPendingDisplayRequest" );

        DisplayRequests::iterator findIter = std::find( m_displayRequests.begin(),
                                                             m_displayRequests.end(),
                                                             requestCondition );

        if ( m_displayRequests.end() == findIter )
        {
            m_displayRequests.push_back( requestCondition );
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestSender::removeCompletedDisplayRequest( DisplayRequestCondition* requestCondition )
    {
        FUNCTION_ENTRY( "removeCompletedDisplayRequest" );

        DisplayRequests::iterator findIter = std::find( m_displayRequests.begin(),
                                                             m_displayRequests.end(),
                                                             requestCondition );

        if ( m_displayRequests.end() != findIter )
        {
            m_displayRequests.erase( findIter );
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestSender::resubmitTtisDisplayRequest( IDisplayRequest* request )
    {
        FUNCTION_ENTRY( "resubmitTtisDisplayRequest" );

        TA_ASSERT( request != NULL, "Null request given to resubmitTtisDisplayRequest" );

        // check whether its predefined or ad-hoc
        switch ( request->getMessageType() )
        {
            case TA_IRS_Core::IDisplayRequest::PREDEFINED:
                submitTtisPreDefinedDisplayRequest( request );
                break;

            case TA_IRS_Core::IDisplayRequest::ADHOC:
                submitTtisAdHocDisplayRequest( request );
                break;

            case TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW:
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW:
            case TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE:
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE:
            default:
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                         "Received resubmit for invalid TTIS message type: %d", request->getMessageType());
            UserMessages::getInstance().displayError("Re-submit failed. The request type was invalid for a TTIS display request.");
            break;
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestSender::resubmitStisDisplayRequest( IDisplayRequest* request )
    {
        FUNCTION_ENTRY( "resubmitStisDisplayRequest" );

        TA_ASSERT( request != NULL, "Null request given to resubmitStisDisplayRequest" );

        // check whether its predefined or ad-hoc
        switch ( request->getMessageType() )
        {
            case TA_IRS_Core::IDisplayRequest::PREDEFINED:
                submitStisPreDefinedDisplayRequest( request );
                break;

            case TA_IRS_Core::IDisplayRequest::ADHOC:
                submitStisAdHocDisplayRequest( request );
                break;

            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE:
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW:
            case TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR:
                submitStisRatisApprovalRequest( request );
                break;

            default:
            LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                         "Received resubmit for invalid TTIS message type: %d", request->getMessageType());
            UserMessages::getInstance().displayError("Re-submit failed. The request type was invalid for an STIS display request.");
            break;
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestSender::submitTtisPreDefinedDisplayRequest( IDisplayRequest* request )
    {
        FUNCTION_ENTRY( "submitTtisPreDefinedDisplayRequest" );

        TA_ASSERT( request != NULL, "Null request given" );
        TA_ASSERT( RunParams::getInstance().isSet( RPARAM_SESSIONID ), "Session ID is not set" );

        // get the appropriate data

		//for reinstate
        //TA_Base_Core::TrainList trainList( 1 ); 
		TA_Base_Core::TrainList trainList;
		trainList.length(1);
			
		trainList[0] = boost::lexical_cast<unsigned int>( request->getTtisDestinationTrainId() );

        TA_Base_Core::TTISPredefinedMessageParameters parameters;

        // list of PIDs
        std::vector<TA_Base_Core::EPIDSelection> selectedPIDs = getTtisPidList( request->getTtisDestinationPids() );

        parameters.pidList.length( selectedPIDs.size() );

        //TD13214
        // build sequence
        unsigned int i = 0;

        for ( std::vector<TA_Base_Core::EPIDSelection>::const_iterator pidIter = selectedPIDs.begin();
              pidIter != selectedPIDs.end(); ++pidIter )
        {
            parameters.pidList[i++] = *pidIter;
        }

        parameters.libraryVersion = request->getLibraryVersion();

        parameters.librarySection = static_cast<TA_Base_Core::ELibrarySection>( request->getLibrarySection() );
        parameters.messageId = request->getMessageTag();
        parameters.priority = static_cast<TA_Base_Core::ETTISMessagePriority>( request->getPriority() );
        parameters.startTime = request->getStartTime().c_str();
        parameters.endTime = request->getEndTime().c_str();

        time_t timestamp;
        time( &timestamp );

        std::string sessionId = RunParams::getInstance().get( RPARAM_SESSIONID ).c_str();

        // set up the object to receive the response
        DisplayRequestCondition displayRequestCondition( trainList[0],
                                                         timestamp,
                                                         sessionId,
                                                         TA_Base_Core::TisPredefinedMessageCommand,
														 REASONABLE_TIMEOUT_VALUE);
                                                         /*TISAgentAccessFactory::getInstance().getTTISAgent()->getFlexibleTimeOutValue() * 1000 );*/
        addPendingDisplayRequest( &displayRequestCondition );

        LOG_GENERIC( SourceInfo,
                     DebugUtil::DebugInfo,
                     "Resubmitting TTIS predefined display request with key %s",
                     request->getKey().c_str() );

        CORBA_CALL( TISAgentAccessFactory::getInstance().getTTISAgent(), submitPredefinedDisplayRequest, ( trainList,
                                                                                             parameters,
                                                                                             timestamp,
                                                                                             sessionId.c_str() ) );//limin

        // wait for the response
        displayRequestCondition.waitForCompletion();

//         if ( false == displayRequestCondition.wasSuccessful() )
//         {
//             std::ostringstream error;
//             error << "Re-submit failed. " << displayRequestCondition.getErrorDetails();
// 
//             UserMessages::getInstance().displayError( error.str().c_str() );
//         }

        // remove the display request from the list
        removeCompletedDisplayRequest( &displayRequestCondition );

        FUNCTION_EXIT;
    }


    void DisplayRequestSender::submitTtisAdHocDisplayRequest( IDisplayRequest* request )
    {
        FUNCTION_ENTRY( "submitTtisAdHocDisplayRequest" );

        TA_ASSERT( request != NULL, "Null request given" );
        TA_ASSERT( RunParams::getInstance().isSet( RPARAM_SESSIONID ), "Session ID is not set" );

        // get the appropriate data

		//for reinstate
        // train id
        //TA_Base_Core::TrainList trainList( 1 );
		TA_Base_Core::TrainList trainList;
		trainList.length(1);

		trainList[0] = boost::lexical_cast<unsigned int>( request->getTtisDestinationTrainId() );		

		TA_Base_Core::TTISFreeTextMessageParameters parameters;

        // PIDs
        std::vector<TA_Base_Core::EPIDSelection> selectedPIDs = getTtisPidList( request->getTtisDestinationPids() );
        parameters.pidList.length( selectedPIDs.size() );

        unsigned int i = 0;

        for ( std::vector<TA_Base_Core::EPIDSelection>::const_iterator pidIter = selectedPIDs.begin();
              pidIter != selectedPIDs.end(); ++pidIter )
        {
            parameters.pidList[i++] = *pidIter;
        }
		
        switch ( request->getLEDAttributes().fontSize )
        {
            case TA_Base_Core::FONT_SIZE_SMALL:
                parameters.fontSize = TA_Base_Core::TTIS_FONT_SIZE_SMALL;
                break;

            case TA_Base_Core::FONT_SIZE_LARGE:
                //fall through

            case TA_Base_Core::FONT_SIZE_MEDIUM:
                //fall through

            case TA_Base_Core::UNUSED_LED_FONT_SIZE:
                //fall through

            default:
                parameters.fontSize = TA_Base_Core::TTIS_FONT_SIZE_LARGE;
                break;
        }

        parameters.justification = request->getDisplayAttributes().justification;

        parameters.intensity = static_cast<TA_Base_Core::ETTISLEDIntensity>( request->getLEDAttributes().intensity );
        parameters.displayMode = request->getDisplayAttributes().displayMode;
        parameters.priority = static_cast<TA_Base_Core::ETTISMessagePriority>( request->getPriority() );
        parameters.startTime = request->getStartTime().c_str();
        parameters.endTime = request->getEndTime().c_str();
        parameters.repeatInterval = static_cast<CORBA::Octet>( request->getDisplayAttributes().repeatInterval );
        parameters.message = request->getAdhocMessageContent().c_str();

        time_t timestamp;
        time( &timestamp );

        std::string sessionId = RunParams::getInstance().get( RPARAM_SESSIONID ).c_str();

        LOG_GENERIC( SourceInfo,
                     DebugUtil::DebugInfo,
                     "Resubmitting TTIS ad-hoc display request with key %s",
                     request->getKey().c_str() );
			// set up the object to receive the response
		DisplayRequestCondition displayRequestCondition( trainList[0],
														 timestamp,
														 sessionId,
														 TA_Base_Core::TisFreeTextMessageCommand,
														 REASONABLE_TIMEOUT_VALUE);
														 /*TISAgentAccessFactory::getInstance().getTTISAgent()->getFlexibleTimeOutValue() * 1000);*/
		        
        addPendingDisplayRequest( &displayRequestCondition );

        CORBA_CALL( TISAgentAccessFactory::getInstance().getTTISAgent(), submitAdHocDisplayRequest, ( trainList,
                                                                                        parameters,
                                                                                        timestamp,
                                                                                        sessionId.c_str() ) );//limin

        // wait for the response
        displayRequestCondition.waitForCompletion();

// 		if ( false == displayRequestCondition.wasSuccessful() )
// 		{
// 			std::ostringstream error;
// 			error << "Re-submit failed. " << displayRequestCondition.getErrorDetails();
// 			
// 			UserMessages::getInstance().displayError( error.str().c_str() );
// 		}

        // remove the display request from the list
        removeCompletedDisplayRequest( &displayRequestCondition );

        FUNCTION_EXIT;
    }


    void DisplayRequestSender::submitStisPreDefinedDisplayRequest( IDisplayRequest* request )
    {
        FUNCTION_ENTRY( "submitStisPreDefinedDisplayRequest" );

        TA_ASSERT( request != NULL, "Null request given" );
        TA_ASSERT( RunParams::getInstance().isSet( RPARAM_SESSIONID ), "Session ID is not set" );

        // build destination structure
        TA_Base_Core::STISDestinationList destination = getStisDestination( request->getStisDestinationStation(),
                                                                            request->getStisDestinationLevels(),
                                                                            request->getStisDestinationPids() );


        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "Resubmitting STIS predefined display request with key %s", request->getKey().c_str() );

        CORBA_CALL( TISAgentAccessFactory::getInstance().getSTISAgent(), 
        submitPredefinedDisplayRequest, ( destination,
                                        ( TA_Base_Core::ELibrarySection ) request->getLibrarySection(),
                                        request->getLibraryVersion(),
                                        request->getMessageTag(),
                                        request->getStartTime().c_str(),
                                        request->getEndTime().c_str(),
                                        request->getPriority(),
                                        RunParams::getInstance().get( RPARAM_SESSIONID ).c_str() ) );//limin

        FUNCTION_EXIT;
    }


    void DisplayRequestSender::submitStisAdHocDisplayRequest( IDisplayRequest* request )
    {
        FUNCTION_ENTRY( "submitStisAdHocDisplayRequest" );

        TA_ASSERT( request != NULL, "Null request given" );
        TA_ASSERT( RunParams::getInstance().isSet( RPARAM_SESSIONID ), "Session ID is not set" );

        // build destination structure
        TA_Base_Core::STISDestinationList destination = getStisDestination( request->getStisDestinationStation(),
                                                                            request->getStisDestinationLevels(),
                                                                            request->getStisDestinationPids() );

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "Resubmitting STIS ad-hoc display request with key %s", request->getKey().c_str() );

        CORBA_CALL( TISAgentAccessFactory::getInstance().getSTISAgent(), 
        submitAdHocDisplayRequest, ( destination,
                                   request->getAdhocMessageContent().c_str(),
                                   request->getStartTime().c_str(),
                                   request->getEndTime().c_str(),
                                   request->getPriority(),
                                   request->getDisplayAttributes(),
                                   request->getPlasmaAttributes(),
                                   request->getLEDAttributes(),
                                   RunParams::getInstance().get( RPARAM_SESSIONID ).c_str() ) );//limin

        FUNCTION_EXIT;
    }


    void DisplayRequestSender::submitStisRatisApprovalRequest( IDisplayRequest* request )
    {
        FUNCTION_ENTRY( "submitStisRatisApprovalRequest" );

        TA_ASSERT( request != NULL, "Null request given" );
        TA_ASSERT( RunParams::getInstance().isSet( RPARAM_SESSIONID ), "Session ID is not set" );

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "Resubmitting STIS RATIS approval request with key %s", request->getKey().c_str());

        // build the RATIS structure
        TA_Base_Core::ERATISMessageType messageType;
        if ( request->getMessageType() == TA_IRS_Core::IDisplayRequest::RATIS_IN_NEW )
        {
            messageType = TA_Base_Core::RATIS_IN_NEW;
        }
        else if ( request->getMessageType() == TA_IRS_Core::IDisplayRequest::RATIS_OUT_NEW )
        {
            messageType = TA_Base_Core::RATIS_OUT_NEW;
        }
        else if ( request->getMessageType() == TA_IRS_Core::IDisplayRequest::RATIS_OUT_UPDATE )
        {
            // must be an update
            messageType = TA_Base_Core::RATIS_OUT_UPDATE;
        }
        else if ( request->getMessageType() == TA_IRS_Core::IDisplayRequest::RATIS_IN_UPDATE )
        {
            // must be an update
            messageType = TA_Base_Core::RATIS_IN_UPDATE;
        }
        else if ( request->getMessageType() == TA_IRS_Core::IDisplayRequest::RATIS_OUT_CLEAR )
        {
            // must be an update
            messageType = TA_Base_Core::RATIS_OUT_CLEAR;
        }
        else if ( request->getMessageType() == TA_IRS_Core::IDisplayRequest::RATIS_IN_CLEAR )
        {
            // must be an update
            messageType = TA_Base_Core::RATIS_IN_CLEAR;
        }

        // send to the STIS Agent (delegates up if not OCC)
        CORBA_CALL( TISAgentAccessFactory::getInstance().getSTISAgent(), 
        submitRATISDisplayRequest, ( request->getAdhocMessageContent().c_str(),
                                   request->getPriority(),
                                   request->getRatisTag().c_str(),
                                   request->getRatisDestination().c_str(),
                                   request->getStartTime().c_str(),
                                   request->getEndTime().c_str(),
                                   messageType,
                                   request->getRatisOverridable(),
                                   true,
                                   RunParams::getInstance().get( RPARAM_SESSIONID ).c_str() ) );//limin

        FUNCTION_EXIT;
    }


    std::vector<TA_Base_Core::EPIDSelection> DisplayRequestSender::getTtisPidList( const std::string& destinationPidList ) const
    {
        FUNCTION_ENTRY( "getTtisPidList" );

        std::stringstream delimiterStream;
        delimiterStream << destinationPidList << " ";

        int temp = -1;
        std::vector<TA_Base_Core::EPIDSelection> selectedPIDs;

        delimiterStream >> std::skipws >> temp;

        while ( ( temp != -1 ) && ( !delimiterStream.eof() ) )
        {
            selectedPIDs.push_back( ( TA_Base_Core::EPIDSelection ) temp );

            temp = -1;
            delimiterStream >> std::skipws >> temp;
        }

        FUNCTION_EXIT;
        return selectedPIDs;
    }


    TA_Base_Core::STISDestinationList DisplayRequestSender::getStisDestination( const std::string& station,
                                                                                const std::string& levels,
                                                                                const std::string& pids ) const
    {
        FUNCTION_ENTRY( "getStisDestination" );

        TA_Base_Core::STISDestination destination;

        // station
        destination.station = station.c_str();


        // levels
        std::stringstream delimiterStream;
        delimiterStream << levels << " ";

        std::vector<std::string> tempVector;

        std::string temp( "" );

        delimiterStream >> std::skipws >> temp;
        while ( (temp.length() > 0) && (!delimiterStream.eof()) )
        {
            tempVector.push_back(temp);

            temp = "";
            delimiterStream >> std::skipws >> temp;
        }

        destination.levels.length(tempVector.size());
        for ( unsigned int i = 0; i < tempVector.size(); i++)
        {
            destination.levels[i] = tempVector[i].c_str();
        }

        // pids
        delimiterStream.str("");
        delimiterStream.clear();
        tempVector.clear();
        temp = "";

        delimiterStream << pids << " ";

        delimiterStream >> std::skipws >> temp;
        while ( (temp.length() > 0) && (!delimiterStream.eof()) )
        {
            tempVector.push_back(temp);

            temp = "";
            delimiterStream >> std::skipws >> temp;
        }

        destination.pids.length(tempVector.size());
        for ( size_t i = 0; i < tempVector.size(); i++)
        {
            destination.pids[i] = tempVector[i].c_str();
        }

        TA_Base_Core::STISDestinationList destinationList;

        destinationList.length( 1 );
        destinationList[0] = destination;

        FUNCTION_EXIT;
        return destinationList;
    }

} // TA_IRS_App

