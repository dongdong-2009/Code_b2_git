/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/4669_TIP/4669/transactive/app/signs/ttis_manager/src/DisplayRequestManager.cpp $
  * @author:   Adam Radics
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2009/10/02 13:41:49 $
  * Last modified by:  $Author: zhongyi.wu $
  *
  * Manages the display and clear buttons.
  * Submits requests when pressed, and manages enabling/disabling.
  */

#include "StdAfx.h"


#include "DisplayAttributeManager.h"
#include "DisplayRequestManager.h"
#include "TTISCommonConfiguration.h"
#include "TTISMessageManager.h"
#include "TTISPredefinedMessages.h"
#include "PIDSAdhocMessagePage.h"
#include "PIDSManagerDlg.h"

#include "bus/generic_gui/src/TransactiveMessage.h" // TD14164
#include "bus/mfc_extensions/src/progress_window/ProgressWnd.h"
#include "bus/signs_4669/tis_agent_access/src/TISAgentAccessFactory.h"
#include "bus/trains/TrainSelectionComponent/src/MFCTrainSelectorGUI.h"
#include "bus/trains/TrainSelectionComponent/src/TrainSelectorGUI.h"
#include "bus/trains/TrainCorbaProxy/src/ExceptionConversion.h"
#include "bus/trains/TrainCorbaProxy/src/TisTypesConversion.h"

#include "core/data_access_interface/entity_access/src/TTISEntityData.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/message/types/TISBroadcastComms_MessageTypes.h"
#include "core/message/types/TISComms_MessageTypes.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/FunctionEntry.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/message/types/TrainAgentComms_MessageTypes.h"
#include "bus/trains/TrainAgentCorba/src/TrainAgentObjectNames.h"

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


    DisplayRequestManager::DisplayRequestManager( CButton& resetBtn,
                                                  PIDSManagerDlg& dlg )
            : m_resetBtn( &resetBtn ),
            m_pidsManagerDlg( &dlg ),
            m_ttisPredefinedMessages( NULL ),
            m_subscribedToMessages( false ),
            m_predefinedMsgDisplayStatusDialog( NULL ),
            m_adhocMsgDisplayStatusDialog( NULL )
            //m_clearMsgDialog( NULL )
    {
        FUNCTION_ENTRY( "DisplayRequestManager" );


        //create status dialogs
        initStatusDialogs();

        subscribeToMessages();


        FUNCTION_EXIT;
    }


    DisplayRequestManager::~DisplayRequestManager()
    {
        FUNCTION_ENTRY( "~DisplayRequestManager" );

        unsubscribeToMessages();
        deleteStatusDialogs();

        if ( NULL != m_ttisPredefinedMessages )
        {
            m_ttisPredefinedMessages = NULL;
            TTISPredefinedMessages::removeInstance();
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestManager::initStatusDialogs()
    {
        FUNCTION_ENTRY( "initStatusDialogs" );

        //m_clearMsgDialog = new DisplayRequestStatusDialog();
        m_predefinedMsgDisplayStatusDialog = new PredefinedMsgDisplayRequestDlg();
        m_adhocMsgDisplayStatusDialog = new AdhocDisplayRequestStatusDlg();

        //m_clearMsgDialog->Create( DisplayRequestStatusDialog::IDD );
        m_predefinedMsgDisplayStatusDialog->Create( PredefinedMsgDisplayRequestDlg::IDD );
        m_adhocMsgDisplayStatusDialog->Create( AdhocDisplayRequestStatusDlg::IDD );

        FUNCTION_EXIT;
    }

    void DisplayRequestManager::deleteStatusDialogs()
    {
        FUNCTION_ENTRY( "initStatusDialogs" );
        /*
        if ( m_clearMsgDialog )
        {
            delete m_clearMsgDialog;
            m_clearMsgDialog = NULL;
        }
		*/

        if ( m_predefinedMsgDisplayStatusDialog )
        {
            delete m_predefinedMsgDisplayStatusDialog;
            m_predefinedMsgDisplayStatusDialog = NULL;
        }

        if ( m_adhocMsgDisplayStatusDialog )
        {
            delete m_adhocMsgDisplayStatusDialog;
            m_adhocMsgDisplayStatusDialog = NULL;
        }

        FUNCTION_EXIT;
    }

    void DisplayRequestManager::enableButtons( bool trainsSelected,
                                               bool messageSelected,
                                               bool hasRightsToDisplay,
                                               bool hasRightsToClear )
    {
        FUNCTION_ENTRY( "enableButtons" );
        // enable clear buttons
        bool enableClear = hasRightsToClear && trainsSelected;

        m_resetBtn->EnableWindow( enableClear );

        //2008-08-08
        if ( trainsSelected &&
                messageSelected &&
                hasRightsToDisplay &&
                hasRightsToClear )
        {
            m_pidsManagerDlg->TB_CanApply( true );
        }
        else
        {
            m_pidsManagerDlg->TB_CanApply( false );
        }
        FUNCTION_EXIT;
    }


    void DisplayRequestManager::onDisplay( const TA_IRS_Bus::TrainSelectorGUI& trainSelectorGUI,
                                           const DisplayAttributeManager& displayAttributeManager,
                                           const TA_Base_Core::PredefinedMessage& selectedMessage )
    {
        FUNCTION_ENTRY( "onDisplay" );

        //
        // Collect all the required information
        //

        //save current message
        m_currentPredefinedMessage = selectedMessage;
        // get selected trains
        TA_Base_Core::TrainList trainList   = getSelectedTrains( trainSelectorGUI );

        TA_Base_Core::TTISPredefinedMessageParameters displayParameters;

        //set pid list size to 1
        displayParameters.pidList.length( 1 );
        displayParameters.pidList[0] = TA_Base_Core::ENTIRE_TRAIN;

        // selected message details
        if ( NULL == m_ttisPredefinedMessages )
        {
            m_ttisPredefinedMessages = &( TTISPredefinedMessages::getInstance() );
        }

        displayParameters.libraryVersion = m_ttisPredefinedMessages->getCurrentMessageLibraryVersion();
        displayParameters.messageId = selectedMessage.messageTag;
        if ( displayAttributeManager.getMessageType() == DisplayAttributeManager::MESSAGE_CONTINUOUS )
        {
            displayParameters.perRun = 0;
        }
        else
        {
            displayParameters.perRun = static_cast< CORBA::Octet >( displayAttributeManager.getPerrun() );
        }

		//fill up m_currentPredefinedMessage
		m_currentPredefinedMessage.perRun = displayParameters.perRun;

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "Request to display a pre-defined message: \n%s",
                     selectedMessage.message.c_str() );

        // get the session ID and timestamp
        time_t currentTime      = ACE_OS::time( NULL );
        std::string sessionId   = TTISCommonConfiguration::getInstance().getSessionId();

        //
        // send display request
        //

        submitPredefinedDisplayRequest( trainList, trainSelectorGUI.getSelectedTrainData(), displayParameters, currentTime, sessionId );

        FUNCTION_EXIT;
    }


    void DisplayRequestManager::onDisplay( const TA_IRS_Bus::TrainSelectorGUI& trainSelectorGUI,
                                           const DisplayAttributeManager& displayAttributeManager,
                                           const PIDSAdhocMessagePage::AdHocMessage& selectedMessage )
    {
        FUNCTION_ENTRY( "onDisplay" );

        //
        // Collect all the required information
        //

        //save current message
        m_currentAdhocMessage = selectedMessage;

        // get selected trains
        TA_Base_Core::TrainList trainList   = getSelectedTrains( trainSelectorGUI );

        TA_Base_Core::TTISFreeTextMessageParameters displayParameters;

        //set pid list size to 1
        displayParameters.pidList.length( 1 );
        displayParameters.pidList[0] = TA_Base_Core::ENTIRE_TRAIN;


        // get per-run
        if ( displayAttributeManager.getMessageType() == DisplayAttributeManager::MESSAGE_CONTINUOUS )
        {
            displayParameters.perRun = 0;
        }
        else
        {
            displayParameters.perRun = static_cast< CORBA::Octet >( displayAttributeManager.getPerrun() );
        }


		displayParameters.fontSize = selectedMessage.fontSize;
        displayParameters.justification = selectedMessage.justification;
        displayParameters.intensity = selectedMessage.intensity;
        displayParameters.displayMode = selectedMessage.displayMode;

        // actual message
        displayParameters.message = selectedMessage.messageContent.c_str();

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                     "Request to display an freetext message: \n%s",
                     selectedMessage.messageContent.c_str() );

        // get the session ID and timestamp
        time_t currentTime      = ACE_OS::time( NULL );
        std::string sessionId   = TTISCommonConfiguration::getInstance().getSessionId();

        //
        // send display request(s)
        //

        submitFreeTextDisplayRequest( trainList, trainSelectorGUI.getSelectedTrainData(), displayParameters, currentTime, sessionId );

        FUNCTION_EXIT;
    }


    void DisplayRequestManager::onClear( TA_Base_Core::ETTISClearType clearType,
                                         TA_IRS_Bus::TrainSelectorGUI& trainSelectorGUI )
    {
        FUNCTION_ENTRY( "onClear" );

        //
        // Collect all the required information
        //

        // get selected trains
        TA_Base_Core::TrainList trainList   = getSelectedTrains( trainSelectorGUI );

        TA_Base_Core::TTISMessageResetParameters displayParameters;
        displayParameters.pidList.length( 1 );
        displayParameters.pidList[0] = TA_Base_Core::ENTIRE_TRAIN;
        displayParameters.messageType = TA_Base_Core::CLEAR_ALL;

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Request to clear" );


        // get the session ID and timestamp
        time_t currentTime      = ACE_OS::time( NULL );
        std::string sessionId   = TTISCommonConfiguration::getInstance().getSessionId();

        //
        // send display request(s)
        //

        submitClearRequest( trainList, trainSelectorGUI.getSelectedTrainData(), displayParameters, currentTime, sessionId );

        FUNCTION_EXIT;
    }


    void DisplayRequestManager::receiveSpecialisedMessage( const TA_Base_Core::CommsMessageCorbaDef& payload )
    {
        FUNCTION_ENTRY( "receiveSpecialisedMessage" );
        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Receive displayresult message" );

        if ( TA_Base_Core::TISComms::TrainDisplayResult.getTypeKey() == payload.messageTypeKey.in() )
        {
            TA_Base_Core::TTISDisplayResult* displayResult;
            if ( payload.messageState >>= displayResult )
            {

                switch ( displayResult->originalCommand )
                {
                    case TA_Base_Core::TisFreeTextMessageCommand:
                        m_adhocMsgDisplayStatusDialog->handleDisplayResult( *displayResult );
                        break;
                    case TA_Base_Core::TisPredefinedMessageCommand:
                        m_predefinedMsgDisplayStatusDialog->handleDisplayResult( *displayResult );
                        break;
						/*
                    case TA_Base_Core::TisClearCommand:
                        m_clearMsgDialog->handleDisplayResult( *displayResult );
                        break;
						*/
                }
            }
            else
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugError, "Could not extract TTISDisplayResult from CommsMessageCorbaDef" );
            }
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestManager::unsubscribeToMessages()
    {
        FUNCTION_ENTRY( "unsubscribeToMessages" );
        if ( true == m_subscribedToMessages )
        {
            TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );
            m_subscribedToMessages = false;
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
            if ( false == m_subscribedToMessages )
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
                m_subscribedToMessages = true;
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

        FUNCTION_EXIT;
    }


    void DisplayRequestManager::submitPredefinedDisplayRequest( const TA_Base_Core::TrainList& trainList,
                                                                const TA_IRS_Bus::TrainInformationTypes::TrainDetailsList& trainDetailsList,
                                                                const TA_Base_Core::TTISPredefinedMessageParameters& displayParameters,
                                                                time_t timestamp,
                                                                const std::string& sessionId )
    {
        FUNCTION_ENTRY( "submitPredefinedDisplayRequest" );

        try
        {
            unsigned long nFlexTimeOut = 0;
            try
            {
                CORBA_CALL_RETURN( nFlexTimeOut, TISAgentAccessFactory::getInstance().getTTISAgent(), getFlexibleTimeOutValue, () );
            }
            catch ( TA_Base_Core::ObjectResolutionException& ex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", ex.what() );
                CORBA_CALL_RETURN( nFlexTimeOut, TISAgentAccessFactory::getInstance().getTTISAgent(), getFlexibleTimeOutValue, () );
            }

            m_predefinedMsgDisplayStatusDialog->setDisplayRequest( trainDetailsList, m_currentPredefinedMessage, timestamp, sessionId, TA_Base_Core::TisPredefinedMessageCommand, nFlexTimeOut * 1000 );

            try
            {
                CORBA_CALL( TISAgentAccessFactory::getInstance().getTTISAgent(), submitPredefinedDisplayRequest, (  trainList,
                            displayParameters,
                            timestamp,
                            sessionId.c_str() ) ) ;
            }
            catch ( TA_Base_Core::ObjectResolutionException& ex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", ex.what() );

                CORBA_CALL( TISAgentAccessFactory::getInstance().getTTISAgent(), submitPredefinedDisplayRequest, (  trainList,
                            displayParameters,
                            timestamp,
                            sessionId.c_str() ) ) ;
            }



            // now show the status dialog
            // this will return as soon as the dialog is opened
            // however the dialog will remain open and modal until the statuses are all received
            m_predefinedMsgDisplayStatusDialog->displayModalWindow();

        }
        catch ( const TA_Base_Core::OperationModeException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException",
                                 "while calling submitPredefinedDisplayRequest" );

            // show the error to the operator
            TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
        }
        catch ( const TA_Base_Core::AccessDeniedException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException",
                                 "while calling submitPredefinedDisplayRequest" );

			// show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_DISPLAY_TRAIN << ":Access Denied";
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
			*/
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter", e.details.in() );
           
			// show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_DISPLAY_TRAIN << " : " << std::endl << e.details.in();
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
			*/
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", e.details.in() );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_DISPLAY_TRAIN << ": " << e.details.in();
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
			*/
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( " Exception thrown while calling submitPredefinedDisplayRequest: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", exceptionMsg.c_str() );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERRORPREDEFINE );
			*/
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERRORPREDEFINE );
			*/
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "while calling submitPredefinedDisplayRequest" );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERRORPREDEFINE );
			*/
        }



        FUNCTION_EXIT;
    }


    void DisplayRequestManager::submitFreeTextDisplayRequest( const TA_Base_Core::TrainList& trainList,
                                                              const TA_IRS_Bus::TrainInformationTypes::TrainDetailsList& trainDetailsList,
                                                              const TA_Base_Core::TTISFreeTextMessageParameters& displayParameters,
                                                              time_t timestamp,
                                                              const std::string& sessionId )
    {
        FUNCTION_ENTRY( "submitFreeTextDisplayRequest" );

        try
        {
            // first populate the request status dialog with the details of this request
            unsigned long nFlexTimeOut = 0;
            try
            {
                CORBA_CALL_RETURN( nFlexTimeOut, TISAgentAccessFactory::getInstance().getTTISAgent(), getFlexibleTimeOutValue, () );
            }
            catch ( TA_Base_Core::ObjectResolutionException& ex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", ex.what() );
                CORBA_CALL_RETURN( nFlexTimeOut, TISAgentAccessFactory::getInstance().getTTISAgent(), getFlexibleTimeOutValue, () );
            }

            m_adhocMsgDisplayStatusDialog->setDisplayRequest( trainDetailsList, m_currentAdhocMessage,
                                                              displayParameters, timestamp, sessionId,
                                                              TA_Base_Core::TisFreeTextMessageCommand,
                                                              nFlexTimeOut * 1000 );

            //covert argument

            try
            {
                CORBA_CALL( TISAgentAccessFactory::getInstance().getTTISAgent(), submitAdHocDisplayRequest, ( trainList,
                            displayParameters,
                            timestamp,
                            sessionId.c_str() ) );
            }
            catch ( TA_Base_Core::ObjectResolutionException& ex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", ex.what() );
                CORBA_CALL( TISAgentAccessFactory::getInstance().getTTISAgent(), submitAdHocDisplayRequest, ( trainList,
                            displayParameters,
                            timestamp,
                            sessionId.c_str() ) );
            }
            // now show the status dialog
            // this will return as soon as the dialog is opened
            // however the dialog will remain open and modal until the statuses are all received
            m_adhocMsgDisplayStatusDialog->displayModalWindow();
        }
        catch ( const TA_Base_Core::OperationModeException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException",
                                 "while calling submitAdHocDisplayRequest" );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
        }
        catch ( const TA_Base_Core::AccessDeniedException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException",
                                 "while calling submitAdHocDisplayRequest" );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_DISPLAY_TRAIN << ":Access Denied";
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
			*/
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter", e.details.in() );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_DISPLAY_TRAIN << " : " << std::endl << e.details.in();
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
			*/
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", e.details.in() );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_DISPLAY_TRAIN << ": " << e.details.in();
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
			*/
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while calling submitFreeTextDisplayRequest: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", exceptionMsg.c_str() );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERRORFREETEXT );
			*/
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERRORFREETEXT );
			*/
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "while calling submitFreeTextDisplayRequest" );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_DISPLAY_ERRORFREETEXT );
			*/
        }

        FUNCTION_EXIT;
    }


    void DisplayRequestManager::submitClearRequest( const TA_Base_Core::TrainList& trainList,
                                                    const TA_IRS_Bus::TrainInformationTypes::TrainDetailsList& trainDetailsList,
                                                    const TA_Base_Core::TTISMessageResetParameters& displayParameters,
                                                    time_t timestamp,
                                                    const std::string& sessionId )
    {
        FUNCTION_ENTRY( "submitClearRequest" );

        try
        {
            // first populate the request status dialog with the details of this request

            unsigned long nFlexTimeOut = 0; // 20s is the default corba timeout
            try
            {
                CORBA_CALL_RETURN( nFlexTimeOut, TISAgentAccessFactory::getInstance().getTTISAgent(), getFlexibleTimeOutValue, () );
            }
            catch ( TA_Base_Core::ObjectResolutionException& ex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", ex.what() );
                CORBA_CALL_RETURN( nFlexTimeOut, TISAgentAccessFactory::getInstance().getTTISAgent(), getFlexibleTimeOutValue, () );
            }

            //m_clearMsgDialog->setDisplayRequest( trainDetailsList, timestamp, sessionId, TA_Base_Core::TisClearCommand, nFlexTimeOut * 1000 );


            try
            {
                CORBA_CALL( TISAgentAccessFactory::getInstance().getTTISAgent(), submitClearRequest, (  trainList,
                            displayParameters,
                            timestamp,
                            sessionId.c_str()  ) );
            }
            catch ( TA_Base_Core::ObjectResolutionException& ex )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "ObjectResolutionException", ex.what() );

                CORBA_CALL( TISAgentAccessFactory::getInstance().getTTISAgent(), submitClearRequest, (  trainList,
                            displayParameters,
                            timestamp,
                            sessionId.c_str()  ) );
            }


            // now show the status dialog
            // this will return as soon as the dialog is opened
            // however the dialog will remain open and modal until the statuses are all received
            //m_clearMsgDialog->displayModalWindow();
        }
        catch ( const TA_Base_Core::OperationModeException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::OperationModeException",
                                 "while calling submitClearRequest" );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_CLEAR_ERROR );
			*/
        }
        catch ( const TA_Base_Core::AccessDeniedException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::AccessDeniedException",
                                 "while calling submitClearRequest" );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_CLEAR_TRAIN << ":Access Denied";
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
			*/
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISInvalidParameter", e.details.in() );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_CLEAR_TRAIN << " : " << std::endl << e.details.in();
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
			*/
        }
        catch ( const TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Bus::ITTISManagerCorbaDef::TTISGeneralFailure", e.details.in() );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            std::ostringstream error;
            error << TTISMessageManager::ERROR_SENDING_CLEAR_TRAIN << ": " << e.details.in();
            TTISMessageManager::getInstance().displayError( error.str().c_str() );
			*/
        }
        catch ( const CORBA::Exception& cex )
        {
            std::string exceptionMsg( "Exception thrown while calling submitClearRequest: " );
            exceptionMsg.append( TA_Base_Core::CorbaUtil::getInstance().exceptionToString( cex ) );

            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::Exception", exceptionMsg.c_str() );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_CLEAR_ERROR );
			*/
        }
        catch ( const TA_Base_Core::TransactiveException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "TA_Base_Core::TransactiveException", e.what() );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_CLEAR_ERROR );
			*/
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "...", "while calling submitClearRequest" );

            // show the error to the operator
			TTISMessageManager::getInstance().displayError( TTISMessageManager::ERROR_CONTACTED_AGENT );
			/*
            TTISMessageManager::getInstance().displayError( TTISMessageManager::NO_TIS_AGENT_CLEAR_ERROR );
			*/
        }

        FUNCTION_EXIT;
    }


    TA_Base_Core::TrainList DisplayRequestManager::getSelectedTrains( const TA_IRS_Bus::TrainSelectorGUI& trainSelectorGUI )
    {
        FUNCTION_ENTRY( "getSelectedTrains" );

        TA_IRS_Bus::CommonTypes::TrainIdList selectedTrains  = trainSelectorGUI.getSelectedTrains();
//         TA_ASSERT( selectedTrains.size() > 0,
//                    "The display button should not be enabled if no trains are selected" );


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


} // TA_IRS_App

