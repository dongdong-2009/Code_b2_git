/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_T01271350/4669/transactive/app/signs/ttis_manager/src/DisplayRequestManager.cpp $
  * @author:   Adam Radics
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2011/03/29 11:29:48 $
  * Last modified by:  $Author: builder $
  *
  * Manages the display and clear buttons.
  * Submits requests when pressed, and manages enabling/disabling.
  */

#include "StdAfx.h"

#include "resource.h"

#include "DisplayAttributeManager.h"
#include "DisplayRequestManager.h"
#include "IPidSelectionListener.h"
#include "PidSelectionManager.h"
#include "TTISCommonConfiguration.h"
#include "TTISMessageManager.h"
#include "TTISPredefinedMessages.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "bus/mfc_extensions/src/progress_window/ProgressWnd.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/trains/TrainSelectionComponent/src/MFCTrainSelectorGUI.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h"

#include "core/data_access_interface/entity_access/src/TTISEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/message/types/TISBroadcastComms_MessageTypes.h"
#include "core/message/types/TISComms_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/FunctionEntry.h"
#include "core/utilities/src/TAAssert.h"

#include <ace/os.h>


namespace TA_IRS_App
{

    using TA_Base_Core::DebugUtil;
    using TA_Base_Core::ThreadGuard;
    using TA_Base_Core::DebugUtil;
    using TA_Base_Core::DatabaseException;
    using TA_Base_Core::DataException;
    using TA_Base_Core::MessageSubscriptionManager;
    using TA_Base_Core::IEntityData;
    using TA_Base_Core::EntityAccessFactory;
    using TA_Base_Bus::TISAgentAccessFactory;


    DisplayRequestManager::DisplayRequestManager( CButton& clearAllMessages,
                                                  CButton& clearEmergencyMessages,
                                                  CButton& clearNormalMessages,
                                                  CButton& clearPriority6,
                                                  CButton& display )
        : m_clearAllMessages( &clearAllMessages ),
          m_clearEmergencyMessages( &clearEmergencyMessages ),
          m_clearNormalMessages( &clearNormalMessages ),
          m_clearPriority6( &clearPriority6 ),
          m_display( &display ),
          m_ttisPredefinedMessages( NULL )
    {
        FUNCTION_ENTRY( "DisplayRequestManager" );

        subscribeToMessages();

        FUNCTION_EXIT;
    }


    DisplayRequestManager::~DisplayRequestManager()
    {
        FUNCTION_ENTRY( "~DisplayRequestManager" );

        TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );

        if ( NULL != m_ttisPredefinedMessages )
        {
            m_ttisPredefinedMessages = NULL;
            TTISPredefinedMessages::removeInstance();
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestManager::enableButtons( bool trainsSelected,
                                               bool pidsSelected,
                                               bool messageSelected,
                                               bool hasRightsToDisplay,
                                               bool hasRightsToClear )
    {
        FUNCTION_ENTRY( "enableButtons" );
        // enable clear buttons
        bool enableClear = hasRightsToClear && trainsSelected && pidsSelected;
		
        m_clearAllMessages->EnableWindow(enableClear);
		m_clearEmergencyMessages->EnableWindow(enableClear);
		m_clearNormalMessages->EnableWindow(enableClear);
		m_clearPriority6->EnableWindow(enableClear);

		//2008-08-08
		if (trainsSelected &&
			pidsSelected &&
			messageSelected &&
			hasRightsToDisplay &&
			hasRightsToClear )
		{
			m_display->EnableWindow( true );
		}
		else 
		{
			m_display->EnableWindow( false );
		}
        FUNCTION_EXIT;
    }


    void DisplayRequestManager::onDisplay( const PidSelectionManager& pidSelectionManager,
                                           const TA_IRS_Bus::TrainSelectorGUI& trainSelectorGUI,
                                           const DisplayAttributeManager& displayAttributeManager,
                                           const TA_Base_Core::PredefinedMessage& selectedMessage )
    {
        FUNCTION_ENTRY( "onDisplay" );

        //
        // Collect all the required information
        //

        // get selected trains
        TA_Base_Core::TrainList trainList   = getSelectedTrains( trainSelectorGUI );

        TA_Base_Core::TTISPredefinedMessageParameters displayParameters;

        // get selected PID's
        displayParameters.pidList = getSelectedPids( pidSelectionManager );

        // get start and end time
        std::string startTime;
        std::string endTime;
        if ( false == getTimes( displayAttributeManager, startTime, endTime ) )
        {
            FUNCTION_EXIT;
            return;
        }
        displayParameters.startTime = startTime.c_str();
        displayParameters.endTime = endTime.c_str();

        // get the priority
        displayParameters.priority = getPriority( displayAttributeManager );

        // selected message details
        if ( NULL == m_ttisPredefinedMessages )
        {
            m_ttisPredefinedMessages = &( TTISPredefinedMessages::getInstance() );
        }

        displayParameters.libraryVersion = m_ttisPredefinedMessages->getCurrentMessageLibraryVersion();
        displayParameters.librarySection = static_cast< TA_Base_Core::ELibrarySection >( selectedMessage.librarySection );
        displayParameters.messageId = static_cast< CORBA::Octet >( selectedMessage.messageTag );

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "Request to display a pre-defined message: \n%s\nStart Time: %s, End Time: %s",
                     selectedMessage.message.c_str(), startTime.c_str(), endTime.c_str() );

        // get the session ID and timestamp
        time_t currentTime      = ACE_OS::time( NULL );
        std::string sessionId   = TTISCommonConfiguration::getInstance().getSessionId();

        //
        // send display request
        //

        submitPredefinedDisplayRequest( trainList, displayParameters, currentTime, sessionId );

        FUNCTION_EXIT;
    }


    void DisplayRequestManager::onDisplay( const PidSelectionManager& pidSelectionManager,
                                           const TA_IRS_Bus::TrainSelectorGUI& trainSelectorGUI,
                                           const DisplayAttributeManager& displayAttributeManager,
                                           const CTrainSelectFreeTextPage::AdHocMessage& selectedMessage )
    {
        FUNCTION_ENTRY( "onDisplay" );

        //
        // Collect all the required information
        //

        // get selected trains
        TA_Base_Core::TrainList trainList   = getSelectedTrains( trainSelectorGUI );

        TA_Base_Core::TTISFreeTextMessageParameters displayParameters;

        // get selected PID's
        displayParameters.pidList = getSelectedPids( pidSelectionManager );

        // get start and end time
        std::string startTime;
        std::string endTime;
        if ( false == getTimes( displayAttributeManager, startTime, endTime ) )
        {
            FUNCTION_EXIT;
            return;
        }

        displayParameters.startTime = startTime.c_str();
        displayParameters.endTime = endTime.c_str();

        // get repeat interval
        displayParameters.repeatInterval = static_cast< CORBA::Octet >( displayAttributeManager.getRepeatInterval() );

        // get the priority
        displayParameters.priority = getPriority( displayAttributeManager );

        // display options
        switch ( selectedMessage.fontSize )
        {
        case TA_Base_Core::FONT_SIZE_SMALL:
            displayParameters.fontSize = TA_Base_Core::TTIS_FONT_SIZE_SMALL;
            break;

        case TA_Base_Core::FONT_SIZE_LARGE:
        default:
            displayParameters.fontSize = TA_Base_Core::TTIS_FONT_SIZE_LARGE;
            break;
        }
        displayParameters.justification = selectedMessage.justification;
        displayParameters.intensity = selectedMessage.intensity;
        displayParameters.displayMode = selectedMessage.displayMode;

        // actual message
        displayParameters.message = selectedMessage.messageContent.c_str();

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "Request to display an Ad-Hoc message: \n%s\nStart Time: %s, End Time: %s",
                     selectedMessage.messageContent.c_str(), startTime.c_str(), endTime.c_str() );


        // get the session ID and timestamp
        time_t currentTime      = ACE_OS::time( NULL );
        std::string sessionId   = TTISCommonConfiguration::getInstance().getSessionId();

        //
        // send display request(s)
        //

        submitFreeTextDisplayRequest( trainList, displayParameters, currentTime, sessionId );

        FUNCTION_EXIT;
    }


    void DisplayRequestManager::onClear( TA_Base_Core::ETTISClearType clearType,
                                         PidSelectionManager& pidSelectionManager,
                                         TA_IRS_Bus::TrainSelectorGUI& trainSelectorGUI )
    {
        FUNCTION_ENTRY( "onClear" );

        //
        // Collect all the required information
        //

        // get selected trains
        TA_Base_Core::TrainList trainList   = getSelectedTrains( trainSelectorGUI );

        TA_Base_Core::TTISMessageResetParameters displayParameters;

        // get selected PID's
        displayParameters.pidList = getSelectedPids( pidSelectionManager );

        // clear type
        displayParameters.messageType = clearType;

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Request to clear" );


        // get the session ID and timestamp
        time_t currentTime      = ACE_OS::time( NULL );
        std::string sessionId   = TTISCommonConfiguration::getInstance().getSessionId();

        //
        // send display request(s)
        //

        submitClearRequest( trainList, displayParameters, currentTime, sessionId );

        FUNCTION_EXIT;
    }


    void DisplayRequestManager::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        if ( TA_Base_Core::TISComms::TrainDisplayResult.getTypeKey() == payload.messageTypeKey.in() )
        {
            TA_Base_Core::TTISDisplayResult* displayResult;

            if ( payload.messageState >>= displayResult )
            {
                m_statusDialog.handleDisplayResult( *displayResult );
            }
            else
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
                             "Could not extract TTISDisplayResult from CommsMessageCorbaDef" );
            }
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestManager::subscribeToMessages()
    {
        FUNCTION_ENTRY( "subscribeToMessages" );

        //
        // Get the entity, subsystem and location key.
        // There should only be a single entity at a given location.
        //
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

            TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToCommsMessage( TA_Base_Core::TISComms::TrainDisplayResult,
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


    void DisplayRequestManager::submitPredefinedDisplayRequest( const TA_Base_Core::TrainList& trainList,
                                                                const TA_Base_Core::TTISPredefinedMessageParameters& displayParameters,
                                                                time_t timestamp,
                                                                const std::string& sessionId )
    {
        FUNCTION_ENTRY( "submitPredefinedDisplayRequest" );

        try
        {
            // first populate the request status dialog with the details of this request
            /*m_statusDialog.setDisplayRequest( trainList, timestamp, sessionId, TA_Base_Core::TisPredefinedMessageCommand,
                                              TISAgentAccessFactory::getInstance().getTTISAgent()->getFlexibleTimeOutValue() * 1000 );

            // submit the request
            TISAgentAccessFactory::getInstance().getTTISAgent()->submitPredefinedDisplayRequest( trainList,
                                                                                                 displayParameters,
                                                                                                 timestamp,
                                                                                                 sessionId.c_str() );*/
			long nFlexTimeOut =0;
			try
			{
				CORBA_CALL_RETURN(nFlexTimeOut,TISAgentAccessFactory::getInstance().getTTISAgent(),getFlexibleTimeOutValue,());
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(nFlexTimeOut,TISAgentAccessFactory::getInstance().getTTISAgent(),getFlexibleTimeOutValue,());
			}
			m_statusDialog.setDisplayRequest( trainList, timestamp, sessionId, TA_Base_Core::TisPredefinedMessageCommand,nFlexTimeOut * 1000 );

			//libo++
			try
			{
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),submitPredefinedDisplayRequest,(trainList,
                                                                                                 displayParameters,
                                                                                                 timestamp,
                                                                                                 sessionId.c_str()));
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),submitPredefinedDisplayRequest,(trainList,
                                                                                                 displayParameters,
                                                                                                 timestamp,
                                                                                                 sessionId.c_str()));
			}
			//++libo

            // now show the status dialog
            // this will return as soon as the dialog is opened
            // however the dialog will remain open and modal until the statuses are all received
            m_statusDialog.displayModalWindow();
        }
        catch ( const TA_Base_Core::OperationModeException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException",
                                 "while calling submitPredefinedDisplayRequest" );

            // show the error to the operator
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERROR_PREDEFINE );
        }
        catch ( const TA_Base_Core::AccessDeniedException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException",
                                 "while calling submitPredefinedDisplayRequest" );

            // show the error to the operator
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_DISPLAY_TRAIN << ": Access Denied";
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter", e.details.in() );

            // show the error to the operator
            std::ostringstream error;
			//[[CL-17931
            //error << TTISMessageManager::ERROR_SENDING_DISPLAY_TRAIN << ": " << e.details.in();
			error << TTISMessageManager::ERROR_SENDING_DISPLAY_TRAIN << ": " << std::endl << e.details.in();
			//]]
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", e.details.in() );

            // show the error to the operator
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_DISPLAY_TRAIN << ": " << e.details.in();
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while calling submitPredefinedDisplayRequest: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exceptio", exceptionMsg.c_str() );

            // show the error to the operator
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERROR_PREDEFINE );
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            // show the error to the operator
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERROR_PREDEFINE );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "while calling submitPredefinedDisplayRequest" );

            // show the error to the operator
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERROR_PREDEFINE );
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestManager::submitFreeTextDisplayRequest( const TA_Base_Core::TrainList& trainList,
                                                              const TA_Base_Core::TTISFreeTextMessageParameters& displayParameters,
                                                              time_t timestamp,
                                                              const std::string& sessionId )
    {
        FUNCTION_ENTRY( "submitFreeTextDisplayRequest" );

        try
        {
            // first populate the request status dialog with the details of this request
            /*m_statusDialog.setDisplayRequest( trainList, timestamp, sessionId, TA_Base_Core::TisFreeTextMessageCommand,
                                              TISAgentAccessFactory::getInstance().getTTISAgent()->getFlexibleTimeOutValue() * 1000 );

            // submit the request
            TISAgentAccessFactory::getInstance().getTTISAgent()->submitAdHocDisplayRequest( trainList,
                                                                                            displayParameters,
                                                                                            timestamp,
                                                                                            sessionId.c_str() );*/
			long nFlexTimeOut =0;
			try
			{
				CORBA_CALL_RETURN(nFlexTimeOut,TISAgentAccessFactory::getInstance().getTTISAgent(),getFlexibleTimeOutValue,());
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(nFlexTimeOut,TISAgentAccessFactory::getInstance().getTTISAgent(),getFlexibleTimeOutValue,());
			}
			m_statusDialog.setDisplayRequest( trainList, timestamp, sessionId, TA_Base_Core::TisFreeTextMessageCommand,nFlexTimeOut * 1000 );

			//libo++
			try
			{
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),submitAdHocDisplayRequest,(trainList,
                                                                                            displayParameters,
                                                                                            timestamp,
                                                                                            sessionId.c_str()));
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),submitAdHocDisplayRequest,(trainList,
                                                                                            displayParameters,
                                                                                            timestamp,
                                                                                            sessionId.c_str() ));
			}
			//++libo

            // now show the status dialog
            // this will return as soon as the dialog is opened
            // however the dialog will remain open and modal until the statuses are all received
            m_statusDialog.displayModalWindow();
        }
        catch ( const TA_Base_Core::OperationModeException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException",
                                 "while calling submitAdHocDisplayRequest" );

            // show the error to the operator
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERROR_FREETEXT );
        }
        catch ( const TA_Base_Core::AccessDeniedException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException",
                                 "while calling submitAdHocDisplayRequest" );

            // show the error to the operator
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_DISPLAY_TRAIN << ": Access Denied";
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter", e.details.in() );

            // show the error to the operator
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_DISPLAY_TRAIN << ": " << e.details.in();
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", e.details.in() );

            // show the error to the operator
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_DISPLAY_TRAIN << ": " << e.details.in();
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while calling submitAdHocDisplayRequest: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exceptio", exceptionMsg.c_str() );

            // show the error to the operator
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERROR_FREETEXT );
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            // show the error to the operator
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERROR_FREETEXT );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "while calling submitAdHocDisplayRequest" );

            // show the error to the operator
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERROR_FREETEXT );
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestManager::submitClearRequest( const TA_Base_Core::TrainList& trainList,
                                                    const TA_Base_Core::TTISMessageResetParameters& displayParameters,
                                                    time_t timestamp,
                                                    const std::string& sessionId )
    {
        FUNCTION_ENTRY( "submitClearRequest" );

        try
        {
            // first populate the request status dialog with the details of this request
            //m_statusDialog.setDisplayRequest( trainList, timestamp, sessionId, TA_Base_Core::TisClearCommand,
              //                                TISAgentAccessFactory::getInstance().getTTISAgent()->getFlexibleTimeOutValue() * 1000 );	

			long nFlexTimeOut =0;
			try
			{
				CORBA_CALL_RETURN(nFlexTimeOut,TISAgentAccessFactory::getInstance().getTTISAgent(),getFlexibleTimeOutValue,());
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL_RETURN(nFlexTimeOut,TISAgentAccessFactory::getInstance().getTTISAgent(),getFlexibleTimeOutValue,());
			}
			m_statusDialog.setDisplayRequest(trainList, timestamp, sessionId, TA_Base_Core::TisClearCommand,nFlexTimeOut * 1000 );



            // submit the request
            //TISAgentAccessFactory::getInstance().getTTISAgent()->submitClearRequest( trainList, displayParameters,timestamp, sessionId.c_str() );

			//libo++
			try
			{
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),submitClearRequest,(trainList, displayParameters,timestamp, sessionId.c_str() ));
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
				CORBA_CALL(TISAgentAccessFactory::getInstance().getTTISAgent(),submitClearRequest,(trainList, displayParameters,timestamp, sessionId.c_str() ));
			}
			//++libo

            // now show the status dialog
            // this will return as soon as the dialog is opened
            // however the dialog will remain open and modal until the statuses are all received
            m_statusDialog.displayModalWindow();
        }
        catch ( const TA_Base_Core::OperationModeException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException", "while calling submitClearRequest" );

            // show the error to the operator
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_CLEAR_ERROR );
        }
        catch ( const TA_Base_Core::AccessDeniedException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException", "while calling submitClearRequest" );

            // show the error to the operator
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_CLEAR_TRAIN << ": Access Denied";
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter", e.details.in() );

            // show the error to the operator
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_CLEAR_TRAIN << ": " << e.details.in();
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", e.details.in() );

            // show the error to the operator
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_CLEAR_TRAIN << ": " << e.details.in();
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while calling submitClearRequest: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exceptio", exceptionMsg.c_str() );

            // show the error to the operator
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_CLEAR_ERROR );
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            // show the error to the operator
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_CLEAR_ERROR );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "while calling submitClearRequest" );

            // show the error to the operator
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_CLEAR_ERROR );
        }

        FUNCTION_EXIT;
    }


    TA_Base_Core::TrainList DisplayRequestManager::getSelectedTrains( const TA_IRS_Bus::TrainSelectorGUI& trainSelectorGUI )
    {
        FUNCTION_ENTRY( "getSelectedTrains" );

        TA_IRS_Bus::CommonTypes::TrainIdList selectedTrains  = trainSelectorGUI.getSelectedTrains();
        TA_ASSERT( selectedTrains.size() > 0,
                   "The display button should not be enabled if no trains are selected" );


        // convert to the CORBA type
        TA_Base_Core::TrainList trainList;
        trainList.length( selectedTrains.size() );
        unsigned int i = 0;
		//2008-08-04
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::iterator iter = selectedTrains.begin();
              iter != selectedTrains.end(); iter++, i++ )
        {
            trainList[i] = *iter;
        }

        FUNCTION_EXIT;
        return trainList;
    }


    TA_Base_Core::TrainPIDList DisplayRequestManager::getSelectedPids( const PidSelectionManager& pidSelectionManager )
    {
        FUNCTION_ENTRY( "getSelectedPids" );

        std::vector< TA_Base_Core::EPIDSelection > selectedPIDs = pidSelectionManager.getSelectedPIDs();

        TA_ASSERT( selectedPIDs.size() > 0,
                   "The display button should not be enabled if no PIDs are selected" );

        // convert to the CORBA type
        TA_Base_Core::TrainPIDList pidList;
        pidList.length( selectedPIDs.size() );

        for ( unsigned long i = 0; i < selectedPIDs.size(); ++i )
        {
            pidList[i] = selectedPIDs[i];
        }

        FUNCTION_EXIT;
        return pidList;
    }


    bool DisplayRequestManager::getTimes( const DisplayAttributeManager& displayAttributeManager,
                                          std::string& startTime,
                                          std::string& endTime )
    {
        FUNCTION_ENTRY( "getTimes" );

        DisplayAttributeManager::TimeType timeType  = displayAttributeManager.getMessageType();

        // start and end time
        char startTimeString[5] = {0};
        char endTimeString[5] = {0};

        // set the display type to either cyclic or continuous
        if ( timeType == DisplayAttributeManager::MESSAGE_CYCLIC )
        {
            CTime startTime         = displayAttributeManager.getStartTime();
            CTime endTime           = displayAttributeManager.getEndTime();
            //CTime tempCurrentTime   = CTime::GetCurrentTime();

            // Check if they are valid
            if ( startTime == endTime )
            {
                TTISMessageManager::getInstance().displayError( TTISMessageManager::START_TIME_SAME_AS_END_TIME );

                FUNCTION_EXIT;
                return false;
            }
//             else if ( startTime > endTime )
//             {
//                 TTISMessageManager::getInstance().displayError( TTISMessageManager::START_TIME_AFTER_END_TIME );
// 
//                 FUNCTION_EXIT;
//                 return false;
//             }
			/*
            else if ( startTime < tempCurrentTime )
            {
                TTISMessageManager::getInstance().displayError( TTISMessageManager::START_TIME_BEFORE_CURRENT_TIME );

                FUNCTION_EXIT;
                return false;
            }
            */
            // TD17546
            if ( ( ( startTime.GetHour() == 3 ) && ( startTime.GetMinute() == 0 ) ) ||  // start time is 3am OR
                 ( ( endTime.GetHour() == 3 ) && ( endTime.GetMinute() == 0 ) ) ||  // end time is 3am OR
                 ( ( startTime.GetHour() < 3 ) && ( endTime.GetHour() >= 3 || endTime < startTime ) ) ||				
				 ( (startTime.GetHour() >= 3) && (!(endTime > startTime) && !( 0 <= endTime.GetHour() && endTime.GetHour() <= 2)))
				 )
            {
                TTISMessageManager::getInstance().displayError( TTISMessageManager::OUT_OF_TIME_RANGE );

                FUNCTION_EXIT;
                return false;
            }

            sprintf( startTimeString, "%02d%02d", startTime.GetHour(), startTime.GetMinute() );
            sprintf( endTimeString, "%02d%02d", endTime.GetHour(), endTime.GetMinute() );
        }
        else
        {
            // continuous

            // start time is current time
            CTime currentTime   = CTime::GetCurrentTime();
            sprintf( startTimeString, "%02d%02d", currentTime.GetHour(), currentTime.GetMinute() );


            // set the end time 1 minute before the start time
            // 0 days, 0 hours, 1 minute, 0 seconds
            currentTime -= CTimeSpan( 0, 0, 1, 0 );
            sprintf( endTimeString, "%02d%02d", currentTime.GetHour(), currentTime.GetMinute() );
        }

        // assign the result to the output variables
        startTime = startTimeString;
        endTime = endTimeString;

        FUNCTION_EXIT;
        return true;
    }


    TA_Base_Core::ETTISMessagePriority DisplayRequestManager::getPriority( const DisplayAttributeManager& displayAttributeManager )
    {
        FUNCTION_ENTRY( "getPriority" );

        unsigned short priority = displayAttributeManager.getPriority();

        switch ( priority )
        {
            case 1:
                FUNCTION_EXIT;
                return TA_Base_Core::TTIS_PRIORITY_1;
            case 2:
                FUNCTION_EXIT;
                return TA_Base_Core::TTIS_PRIORITY_2;
            case 3:
                FUNCTION_EXIT;
                return TA_Base_Core::TTIS_PRIORITY_3;
            case 4:
                FUNCTION_EXIT;
                return TA_Base_Core::TTIS_PRIORITY_4;
            case 5:
                FUNCTION_EXIT;
                return TA_Base_Core::TTIS_PRIORITY_5;
            case 6:
                FUNCTION_EXIT;
                return TA_Base_Core::TTIS_PRIORITY_6;
            case 7:
                FUNCTION_EXIT;
                return TA_Base_Core::TTIS_PRIORITY_7;
            case 8:
                FUNCTION_EXIT;
                return TA_Base_Core::TTIS_PRIORITY_8;
            default:
                TA_ASSERT( false, "Invalid priority selected" );
        }

        // should never get here, assertion above
        FUNCTION_EXIT;
        return TA_Base_Core::TTIS_PRIORITY_4;
    }


} // TA_IRS_App

