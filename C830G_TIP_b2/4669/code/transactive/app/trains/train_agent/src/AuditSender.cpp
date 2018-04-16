/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  adamr
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */

#include "app/trains/train_agent/src/AuditSender.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"
#include "app/trains/train_agent/src/TrainCommsState.h"

#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"

#include "core/data_access_interface/src/ILocation.h"
#include "core/message/public_interfaces/ItaAuditing.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/types/TrainAudit_MessageTypes.h"

#include <sys/timeb.h>

#include <sstream>


namespace TA_IRS_App
{

    AuditSender::AuditSender( ITrainAgentConfiguration& configuration,
                              ITrainStatusManager& statusManager,
                              TA_Base_Core::ItaAuditing& taAuditing )
        : m_configuration( configuration ),
          m_statusManager( statusManager ),
          m_taAuditing( taAuditing )
    {
        FUNCTION_ENTRY( "AuditSender" );
        FUNCTION_EXIT;
    }


    AuditSender::~AuditSender()
    {
        FUNCTION_ENTRY( "~AuditSender" );
        FUNCTION_EXIT;
    }


    void AuditSender::auditOperatorTakeTrainControl( const std::string& sessionId,
                                                     TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                     time_t timeStamp )
    {
        FUNCTION_ENTRY( "auditOperatorTakeTrainControl" );

        TA_Base_Core::NameValuePairs emptyPairs;

        auditEvent( TA_Base_Core::TrainAudit::OperatorTakeTrainControl, sessionId, emptyPairs, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditTrainTimeout( const std::string& sessionId,
                                         TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                         const std::string& operation )
    {
        FUNCTION_ENTRY( "auditTrainTimeout" );

        // create a NameValuePairs object, and add the following parameters
        // "Event" = operation

        TA_Base_Core::NameValuePairs descriptionParameters;

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "Event", operation ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainTimeoutAuditEvent, sessionId, descriptionParameters, trainId );

        FUNCTION_EXIT;
    }


    void AuditSender::auditTrainCallStarted( const std::string& sessionId,
                                             TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                             time_t timeStamp )
    {
        FUNCTION_ENTRY( "auditTrainCallStarted" );

        TA_Base_Core::NameValuePairs emptyPairs;

        auditEvent( TA_Base_Core::TrainAudit::TrainCallDriverSetupCall, sessionId, emptyPairs, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditTrainCallEnded( const std::string& sessionId,
                                           TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                           time_t timeStamp )
    {
        FUNCTION_ENTRY( "auditTrainCallEnded" );

        TA_Base_Core::NameValuePairs emptyPairs;

        auditEvent( TA_Base_Core::TrainAudit::TrainCallDriverEndCall, sessionId, emptyPairs, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditCctvCarrier( const std::string& sessionId,
                                        TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                        time_t timeStamp,
                                        bool carrierOn )
    {
        FUNCTION_ENTRY( "auditCctvCarrier" );

        // create a NameValuePairs object, and add the following parameters
        // "On" = On or Off (set by the carrier on parameter)

        TA_Base_Core::NameValuePairs descriptionParameters;

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "On", carrierOn ? "On" : "Off" ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainCCTVCarrier, sessionId, descriptionParameters, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditCctvSingleScreen( const std::string& sessionId,
                                             TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                             time_t timeStamp,
                                             TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera )
    {
        FUNCTION_ENTRY( "auditCctvSingleScreen" );

        // create a NameValuePairs object, and add the following parameters
        // "Screen" = "Single screen " camera

        TA_Base_Core::NameValuePairs descriptionParameters;

        std::stringstream parameterValue;
        parameterValue << "Single screen " << TA_IRS_Bus::ProtocolCctvTypes::cctvCameraToString( camera );
        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "Screen", parameterValue.str() ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainCCTVScreen, sessionId, descriptionParameters, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditCctvSequence( const std::string& sessionId,
                                         TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                         time_t timeStamp,
                                         TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceNumber sequence,
                                         TA_IRS_Bus::ProtocolCctvTypes::ECctvSequenceMode mode )
    {
        FUNCTION_ENTRY( "auditCctvSequence" );

        // create a NameValuePairs object, and add the following parameters
        // "Screen" = "Sequence " sequence number " mode " sequence mode

        TA_Base_Core::NameValuePairs descriptionParameters;

        std::stringstream parameterValue;
        parameterValue << "Sequence " << TA_IRS_Bus::ProtocolCctvTypes::cctvSequenceNumberToString( sequence )
                       << " mode " << TA_IRS_Bus::ProtocolCctvTypes::cctvSequenceModeToString( mode );
        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "Screen", parameterValue.str() ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainCCTVScreen, sessionId, descriptionParameters, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditCctvQuad( const std::string& sessionId,
                                     TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                     time_t timeStamp,
                                     TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraW,
                                     TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraX,
                                     TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraY,
                                     TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera cameraZ )
    {
        FUNCTION_ENTRY( "auditCctvQuad" );

        // create a NameValuePairs object, and add the following parameters
        // "Screen" = "Quad screen " cameraW "," cameraX "," cameraY "," cameraZ

        TA_Base_Core::NameValuePairs descriptionParameters;

        std::stringstream parameterValue;
        parameterValue << "Quad screen " << TA_IRS_Bus::ProtocolCctvTypes::cctvCameraToString( cameraW )
                       << "," << TA_IRS_Bus::ProtocolCctvTypes::cctvCameraToString( cameraX )
                       << "," << TA_IRS_Bus::ProtocolCctvTypes::cctvCameraToString( cameraY )
                       << "," << TA_IRS_Bus::ProtocolCctvTypes::cctvCameraToString( cameraZ );

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "Screen", parameterValue.str() ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainCCTVScreen, sessionId, descriptionParameters, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditCctvConflict( TA_IRS_Bus::CommonTypes::TrainIdType offendingTrainId,
                                         TA_IRS_Bus::CommonTypes::TrainIdType owningTrainId,
                                         time_t timeStamp )
    {
        FUNCTION_ENTRY( "auditCctvConflict" );

        std::string offendingTrainServiceNumber;
        std::string owningTrainServiceNumber;

        // get the train comms state for the offendingTrain and the owningTrain
        // for each, acquire the state lock and get the serviceNumber, then release the state lock

        TrainCommsStatePtr offendingTrainState = m_statusManager.getTrainCommsState( offendingTrainId );
        TrainCommsStatePtr owningTrainState = m_statusManager.getTrainCommsState( owningTrainId );

        {
            // acquire the state lock
            TA_IRS_Bus::TrainStateLock lock( offendingTrainState );
            offendingTrainServiceNumber = offendingTrainState->getServiceNumber();
            // release the state lock
        }

        {
            // acquire the state lock
            TA_IRS_Bus::TrainStateLock lock( owningTrainState );
            owningTrainServiceNumber = owningTrainState->getServiceNumber();
            // release the state lock
        }

        // create a NameValuePairs object, and add the following parameters
        // "OffendingTrain" = offendingTrainServiceNumber
        // "OwningTrain" = owningTrainServiceNumber

        TA_Base_Core::NameValuePairs descriptionParameters;

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "OffendingTrain", offendingTrainServiceNumber ) );
        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "OwningTrain", owningTrainServiceNumber ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainCCTVConflict, "", descriptionParameters, 0, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditDeleteTrainSelection( const std::string& sessionId,
                                                 const std::string& selectionName )
    {
        FUNCTION_ENTRY( "auditDeleteTrainSelection" );

        // create a NameValuePairs object, and add the following parameters
        // "Selection" = selectionName

        TA_Base_Core::NameValuePairs descriptionParameters;

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "Selection", selectionName ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainInfoDeleteSelection, sessionId, descriptionParameters );

        FUNCTION_EXIT;
    }


    void AuditSender::auditSaveTrainSelection( const std::string& sessionId,
                                               const std::string& selectionName,
                                               const TA_IRS_Bus::CommonTypes::TrainIdList& trainList )
    {
        FUNCTION_ENTRY( "auditSaveTrainSelection" );

        std::ostringstream trainListString;

        // for each train ID in the given list
        for ( TA_IRS_Bus::CommonTypes::TrainIdList::const_iterator iter = trainList.begin();
              iter != trainList.end(); ++iter )
        {
            if ( trainList.begin() != iter )
            {
                trainListString << ", ";
            }

            // get its comms state
            TrainCommsStatePtr trainState = m_statusManager.getTrainCommsState( *iter );
            {
                // acquire the state lock
                TA_IRS_Bus::TrainStateLock lock( trainState );

                // get the service number and append to the train list string
                trainListString << trainState->getServiceNumber();

                // release the state lock
            }
        }

        // create a NameValuePairs object, and add the following parameters
        // "Selection" = selectionName
        // "Train list" = train list string

        TA_Base_Core::NameValuePairs descriptionParameters;

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "Selection", selectionName ) );
        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "Train list", trainListString.str() ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainInfoSaveSelection, sessionId, descriptionParameters );

        FUNCTION_EXIT;
    }


    void AuditSender::auditPecActivated( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                         time_t timeStamp,
                                         TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                         TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber )
    {
        FUNCTION_ENTRY( "auditPecActivated" );

        // create a NameValuePairs object, and add the following parameters
        // "PEC" = pecNumber
        // "CAR" = carNumber

        TA_Base_Core::NameValuePairs descriptionParameters;

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "PEC", TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pecNumber ) ) );
        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "CAR", TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( carNumber ) ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainPECActivated, "", descriptionParameters, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditPecAnswer( const std::string& sessionId,
                                      TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                      time_t timeStamp,
                                      TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                      TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber )
    {
        FUNCTION_ENTRY( "auditPecAnswer" );

        // create a NameValuePairs object, and add the following parameters
        // "PEC" = pecNumber
        // "CAR" = carNumber

        TA_Base_Core::NameValuePairs descriptionParameters;

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "PEC", TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pecNumber ) ) );
        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "CAR", TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( carNumber ) ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainPECAnswerCall, sessionId, descriptionParameters, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditPecCallInvalid( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                           time_t timeStamp,
                                           TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                           TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber )
    {
        FUNCTION_ENTRY( "auditPecCallInvalid" );

        // create a NameValuePairs object, and add the following parameters
        // "PEC" = pecNumber
        // "CAR" = carNumber

        TA_Base_Core::NameValuePairs descriptionParameters;

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "PEC", TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pecNumber ) ) );
        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "CAR", TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( carNumber ) ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainPECCallInvalid, "", descriptionParameters, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditPecNotAnswered( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                           time_t timeStamp,
                                           TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                           TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber )
    {
        FUNCTION_ENTRY( "auditPecNotAnswered" );

        // create a NameValuePairs object, and add the following parameters
        // "PEC" = pecNumber
        // "CAR" = carNumber

        TA_Base_Core::NameValuePairs descriptionParameters;

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "PEC", TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pecNumber ) ) );
        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "CAR", TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( carNumber ) ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainPECCallNotAnswered, "", descriptionParameters, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditPecCctvActivated( const std::string& sessionId,
                                             TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                             time_t timeStamp,
                                             TA_IRS_Bus::ProtocolCctvTypes::ECctvCamera camera )
    {
        FUNCTION_ENTRY( "auditPecCctvActivated" );

        // create a NameValuePairs object, and add the following parameters
        // "Screen" = "Camera " camera

        TA_Base_Core::NameValuePairs descriptionParameters;

        std::stringstream parameterValue;
        parameterValue << "Camera " << TA_IRS_Bus::ProtocolCctvTypes::cctvCameraToString( camera );
        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "Screen", parameterValue.str() ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainPECCCTVActivate, sessionId, descriptionParameters, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditPecReset( const std::string& sessionId,
                                     TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                     time_t timeStamp,
                                     TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                     TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber )
    {
        FUNCTION_ENTRY( "auditPecReset" );

        // create a NameValuePairs object, and add the following parameters
        // "PEC" = pecNumber
        // "CAR" = carNumber

        TA_Base_Core::NameValuePairs descriptionParameters;

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "PEC", TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pecNumber ) ) );
        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "CAR", TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( carNumber ) ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainPECResetCall, sessionId, descriptionParameters, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditPecResetWithoutAnswer( const std::string& sessionId,
                                                  TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                  time_t timeStamp,
                                                  TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                                  TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber )
    {
        FUNCTION_ENTRY( "auditPecResetWithoutAnswer" );

        // create a NameValuePairs object, and add the following parameters
        // "PEC" = pecNumber
        // "CAR" = carNumber

        TA_Base_Core::NameValuePairs descriptionParameters;

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "PEC", TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pecNumber ) ) );
        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "CAR", TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( carNumber ) ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainPECResetWithoutAnswer, sessionId, descriptionParameters, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditPecCallLost( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                        time_t timeStamp,
                                        TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                        TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber )
    {
        FUNCTION_ENTRY( "auditPecCallLost" );

        // create a NameValuePairs object, and add the following parameters
        // "PEC" = pecNumber
        // "CAR" = carNumber

        TA_Base_Core::NameValuePairs descriptionParameters;

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "PEC", TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pecNumber ) ) );
        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "CAR", TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( carNumber ) ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainPECEndCall, "", descriptionParameters, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditPecResetAll( const std::string& sessionId,
                                        TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                        time_t timeStamp )
    {
        FUNCTION_ENTRY( "auditPecResetAll" );

        TA_Base_Core::NameValuePairs descriptionParameters;

        auditEvent( TA_Base_Core::TrainAudit::TrainPECAllReset, sessionId, descriptionParameters, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditPecResetByTims( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                           time_t timeStamp,
                                           TA_IRS_Bus::ProtocolPecTypes::EPecCarNumber carNumber,
                                           TA_IRS_Bus::ProtocolPecTypes::EPecNumber pecNumber )
    {
        FUNCTION_ENTRY( "auditPecResetByTims" );

        // create a NameValuePairs object, and add the following parameters
        // "PEC" = pecNumber
        // "CAR" = carNumber

        TA_Base_Core::NameValuePairs descriptionParameters;

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "PEC", TA_IRS_Bus::ProtocolPecTypes::pecNumberToString( pecNumber ) ) );
        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "CAR", TA_IRS_Bus::ProtocolPecTypes::pecCarNumberToString( carNumber ) ) );

        auditEvent( TA_Base_Core::TrainAudit::TrainPECResetByTIMS, "", descriptionParameters, trainId, timeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::auditEvent( const TA_Base_Core::MessageType& messageType,
                                  const std::string& sessionId,
                                  TA_Base_Core::NameValuePairs& descriptionParameters,
                                  TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                  time_t timeStamp )
    {
        FUNCTION_ENTRY( "auditEvent" );

        // If the train ID is not 0, build the common train parameters
        if ( trainId != 0 )
        {
            buildCommonTrainParameters( descriptionParameters, trainId );
        }

        // create a timeb structure
        // if the given timestamp is 0, populate the structure with the current system time
        // otherwise, populate the structure with the given timestamp

        timeb theTimeStamp;

        if ( timeStamp != 0 )
        {
            theTimeStamp.time = timeStamp;
            theTimeStamp.millitm = 0;
        }
        else
        {
            ftime( &theTimeStamp );
        }

        // audit the message
        m_taAuditing.submitAdvancedAuditMessage( messageType,
                                                 m_configuration.getEntityKey(),
                                                 descriptionParameters,
                                                 "",
                                                 sessionId,
                                                 m_configuration.getAssetName(),
                                                 theTimeStamp );

        FUNCTION_EXIT;
    }


    void AuditSender::buildCommonTrainParameters( TA_Base_Core::NameValuePairs& descriptionParameters,
                                                  TA_IRS_Bus::CommonTypes::TrainIdType trainId )
    {
        FUNCTION_ENTRY( "buildCommonTrainParameters" );

        // Get the TrainCommsState from m_statusManager
        TrainCommsStatePtr trainCommsState = m_statusManager.getTrainCommsState( trainId );

        std::string serviceNumber;
        unsigned long trainLocation;
        bool atsValid;
        {
            // acquire the state lock
            TA_IRS_Bus::TrainStateLock lock( trainCommsState );

            // serviceNumber = getServiceNumber (this is already in the format PV/Service)
            serviceNumber = trainCommsState->getServiceNumber();
            atsValid = trainCommsState->isAtsValid();
            trainLocation = trainCommsState->getLocation();

            // release the state lock
        }

        // Add the following name value pairs
        // "PV" = trainId
        // "Train" = serviceNumber
        //
        // "Service" = use string utilities to get the contents of serviceNumber after the '/' character

        std::stringstream trainIdString;
        trainIdString << static_cast< unsigned int >( trainId );
        
        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "PV", trainIdString.str() ) );
        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "Train", serviceNumber ) );

        std::string::size_type slashPos = serviceNumber.find( '/' );
        if ( slashPos != std::string::npos )
        {
            descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "Service",
                                                                                    serviceNumber.substr( ++slashPos, std::string::npos ) ) );
        }
        else
        {
            LOG_GENERIC( SourceInfo,
                         TA_Base_Core::DebugUtil::DebugError,
                         "The serviceNumber[%s] should be formated like PV/Service",
                         serviceNumber.c_str() );
        }

        std::string locationParameter;

        // if isAtsValid
        if ( atsValid )
        {
            // "Location" = trainLocation
            std::stringstream parameterValue;
            parameterValue << TA_IRS_Bus::LocationCache::getInstance().getLocationByKey( trainLocation );
            
            locationParameter = parameterValue.str();
        }
        // otherwise if the trainLocation is not 0
        else if ( trainLocation != 0 )
        {
            // "Location" = "(" trainLocation ")"
            std::stringstream parameterValue;
            parameterValue << "(" << TA_IRS_Bus::LocationCache::getInstance().getLocationByKey( trainLocation ) << ")";
            
            locationParameter = parameterValue.str();
        }
        // otherwise, if this is not the OCC location (from m_configuration)
        else if ( TA_Base_Core::ILocation::OCC != m_configuration.getAgentLocationType() )
        {
            // "Location" = "(" agent location ")"
            std::stringstream parameterValue;
            parameterValue << "(" << TA_IRS_Bus::LocationCache::getInstance().getLocationByKey( m_configuration.getLocationKey() ) << ")";
            
            locationParameter = parameterValue.str();
        }
        // otherwise, this means the location is the OCC, and we do not know the train's location
        else
        {
            // "Location" = "Unknown Mainline Location"
            locationParameter = "Unknown Mainline Location";
        }

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "Location", "Unknown Mainline Location" ) );

        FUNCTION_EXIT;
    }


} // TA_IRS_App
