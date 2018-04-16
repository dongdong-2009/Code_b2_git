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

#include "app/trains/train_agent/src/AlarmUtility.h"
#include "app/trains/train_agent/src/ITrainAgentConfiguration.h"
#include "app/trains/train_agent/src/ITrainStatusManager.h"

#include "bus/trains/TrainCommonLibrary/src/LocationCache.h"
#include "bus/trains/TrainTransactionModel/src/TrainStateLock.h"

#include "core/alarm/public_interfaces/ItaAlarms.h"
#include "core/alarm/src/AlarmConstants.h"
#include "core/message/types/TrainAgentAlarms_MessageTypes.h"


using TA_Base_Core::NameValuePair;
using TA_Base_Core::NameValuePairs;


namespace TA_IRS_App
{

    AlarmUtility::AlarmUtility( ITrainAgentConfiguration& configuration,
                                ITrainStatusManager& statusManager,
                                TA_Base_Core::ItaAlarms& taAlarms )
        : m_configuration( configuration ),
          m_statusManager( statusManager ),
          m_taAlarms( taAlarms )
    {
        FUNCTION_ENTRY( "AlarmUtility" );
        FUNCTION_EXIT;
    }


    AlarmUtility::~AlarmUtility()
    {
        FUNCTION_ENTRY( "~AlarmUtility" );
        FUNCTION_EXIT;
    }


    void AlarmUtility::raiseAtsAgentCommsAlarm( const std::string& atsAgentName )
    {
        FUNCTION_ENTRY( "raiseAtsAgentCommsAlarm" );

        // create a NameValuePairs object, and add the following parameters
        // "ATSAgent" = atsAgentName
        //
        // raise the alarm using raiseUniqueAlarm with the following parameters
        // ATSAgentCommsAlarm, descriptionParameters

        NameValuePairs descriptionParameters;

        descriptionParameters.insert( NameValuePairs::value_type( "ATSAgent", atsAgentName ) );

        raiseUniqueAlarm( TA_Base_Core::TrainAgentAlarms::ATSAgentCommsAlarm, descriptionParameters );

        FUNCTION_EXIT;
    }


    void AlarmUtility::closeAtsAgentCommsAlarm()
    {
        FUNCTION_ENTRY( "closeAtsAgentCommsAlarm" );

        // close the alarm using closeUniqueAlarm with the following parameters
        // ATSAgentCommsAlarm

        closeUniqueAlarm( TA_Base_Core::TrainAgentAlarms::ATSAgentCommsAlarm );

        FUNCTION_EXIT;
    }


    std::string AlarmUtility::raiseCctvPreConflictAlarm( TA_IRS_Bus::CommonTypes::TrainIdType offendingTrainId,
                                                         TA_IRS_Bus::CommonTypes::TrainIdType owningTrainId,
                                                         time_t timeStamp )
    {
        FUNCTION_ENTRY( "raiseCctvPreConflictAlarm" );

        // create a NameValuePairs object using createConflictParameters
        //
        // raise the alarm using raiseNonUniqueAlarm with the following parameters
        // CctvTrainPreConflictAlarm, descriptionParameters, timestamp

        NameValuePairs descriptionParameters = createConflictParameters( offendingTrainId, owningTrainId );

        FUNCTION_EXIT;
        return raiseNonUniqueAlarm( TA_Base_Core::TrainAgentAlarms::CctvTrainPreConflictAlarm, descriptionParameters, timeStamp );
    }


    std::string AlarmUtility::raiseOa1AtcTimerAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                     const std::string& timerType,
                                                     time_t timeStamp )
    {
        FUNCTION_ENTRY( "raiseOa1AtcTimerAlarm" );

        std::string serviceNumber;

        // get the train comms state for the train
        TrainCommsStatePtr trainState = m_statusManager.getTrainCommsState( trainId );
        {
            // acquire the state lock get the serviceNumber, ats validity, and location, then release the state lock

            TA_IRS_Bus::TrainStateLock lock( trainState );

            serviceNumber = trainState->getServiceNumber();
        }

        // create a NameValuePairs object, and add the following parameters
        // "TimerType" = timerType
        // "PhysicalTrainId" = serviceNumber
        //
        // raise the alarm using raiseNonUniqueAlarm with the following parameters
        // Oa1AtcTimerAlarm, descriptionParameters, timestamp

        NameValuePairs descriptionParameters;

        descriptionParameters.insert( NameValuePairs::value_type( "TimerType", timerType ) );
        descriptionParameters.insert( NameValuePairs::value_type( "PhysicalTrainId", serviceNumber ) );

        FUNCTION_EXIT;
        return raiseNonUniqueAlarm( TA_Base_Core::TrainAgentAlarms::Oa1AtcTimerAlarm, descriptionParameters, timeStamp );
    }


    std::string AlarmUtility::raiseRadioModeTimerAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                        time_t timeStamp )
    {
        FUNCTION_ENTRY( "raiseRadioModeTimerAlarm" );

        std::string serviceNumber;
        bool atsValid;
        unsigned long trainLocation;

        // get the train comms state for the train
        TrainCommsStatePtr trainState = m_statusManager.getTrainCommsState( trainId );
        {
            // acquire the state lock get the serviceNumber, ats validity, and location, then release the state lock

            TA_IRS_Bus::TrainStateLock lock( trainState );

            serviceNumber = trainState->getServiceNumber();
            atsValid = trainState->isAtsValid();
            trainLocation = trainState->getLocation();
        }

        // create a NameValuePairs object, and add the following parameters
        // "SERVICE" = serviceNumber

        NameValuePairs descriptionParameters;

        descriptionParameters.insert( NameValuePairs::value_type( "SERVICE", serviceNumber ) );

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

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "LOCATION", locationParameter.c_str() ) );


        // raise the alarm using raiseNonUniqueAlarm with the following parameters
        // RadioModeTimerAlarm, descriptionParameters, timestamp
        FUNCTION_EXIT;
        return raiseNonUniqueAlarm( TA_Base_Core::TrainAgentAlarms::RadioModeTimerAlarm, descriptionParameters, timeStamp );
    }


    std::string AlarmUtility::raiseMpuChangeoverAlarm( TA_IRS_Bus::CommonTypes::TrainIdType trainId,
                                                       time_t timeStamp )
    {
        FUNCTION_ENTRY( "raiseMpuChangeoverAlarm" );

        std::string serviceNumber;
        bool atsValid;
        unsigned long trainLocation;

        // get the train comms state for the train
        TrainCommsStatePtr trainState = m_statusManager.getTrainCommsState( trainId );
        {
            // acquire the state lock get the serviceNumber, ats validity, and location, then release the state lock

            TA_IRS_Bus::TrainStateLock lock( trainState );

            serviceNumber = trainState->getServiceNumber();
            atsValid = trainState->isAtsValid();
            trainLocation = trainState->getLocation();
        }

        // create a NameValuePairs object, and add the following parameters
        // "SERVICE" = serviceNumber

        NameValuePairs descriptionParameters;

        descriptionParameters.insert( NameValuePairs::value_type( "SERVICE", serviceNumber ) );

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

        descriptionParameters.insert( TA_Base_Core::NameValuePairs::value_type( "LOCATION", locationParameter.c_str() ) );

        // raise the alarm using raiseNonUniqueAlarm with the following parameters
        // MpuChangeoverAlarm, descriptionParameters, timestamp
        FUNCTION_EXIT;
        return raiseNonUniqueAlarm( TA_Base_Core::TrainAgentAlarms::MpuChangeoverAlarm, descriptionParameters, timeStamp );
    }


    void AlarmUtility::closeAlarm( const std::string& alarmId )
    {
        FUNCTION_ENTRY( "closeAlarm" );

        // Close the alarm using m_taAlarms.closeNonUniqueAlarmAtLocation
        m_taAlarms.closeNonUniqueAlarmAtLocation( alarmId, m_configuration.getLocationKey() );

        FUNCTION_EXIT;
    }


    void AlarmUtility::closeAllAlarmsForThisAgent()
    {
        FUNCTION_ENTRY( "closeAllAlarmsForThisAgent" );

        // use m_taAlarms.closeEntityUniqueAlarm, once for each of the following alarm types:
        // AtsAgentCommsAlarm
        // CctvPreConflictAlarm
        // CctvConflictAlarm
        // Oa1AtcTimerAlarm
        // RadioModeTimerAlarm
        // MpuChangeoverAlarm


        m_taAlarms.closeEntityUniqueAlarm( TA_Base_Core::TrainAgentAlarms::ATSAgentCommsAlarm,
                                           m_configuration.getEntityKey(),
                                           m_configuration.getLocationKey() );
        m_taAlarms.closeEntityUniqueAlarm( TA_Base_Core::TrainAgentAlarms::CctvTrainPreConflictAlarm,
                                           m_configuration.getEntityKey(),
                                           m_configuration.getLocationKey() );
        m_taAlarms.closeEntityUniqueAlarm( TA_Base_Core::TrainAgentAlarms::CctvTrainConflictAlarm,
                                           m_configuration.getEntityKey(),
                                           m_configuration.getLocationKey() );
        m_taAlarms.closeEntityUniqueAlarm( TA_Base_Core::TrainAgentAlarms::Oa1AtcTimerAlarm,
                                           m_configuration.getEntityKey(),
                                           m_configuration.getLocationKey() );
        m_taAlarms.closeEntityUniqueAlarm( TA_Base_Core::TrainAgentAlarms::RadioModeTimerAlarm,
                                           m_configuration.getEntityKey(),
                                           m_configuration.getLocationKey() );
        m_taAlarms.closeEntityUniqueAlarm( TA_Base_Core::TrainAgentAlarms::MpuChangeoverAlarm,
                                           m_configuration.getEntityKey(),
                                           m_configuration.getLocationKey() );

        FUNCTION_EXIT;
    }


    void AlarmUtility::raiseUniqueAlarm( const TA_Base_Core::MessageType& alarmType,
                                         NameValuePairs& descriptionParameters,
                                         time_t timeStamp )
    {
        FUNCTION_ENTRY( "raiseUniqueAlarm" );

        // Raise an alarm using m_taAlarms.raiseEntityUniqueAlarm
        // sourceTime - from parameters, but if 0, then use AlarmConstants::defaultTime

        timeb sourceTime = TA_Base_Core::AlarmConstants::defaultTime;

        if ( timeStamp != 0 )
        {
            sourceTime.time = timeStamp;
            sourceTime.millitm = 0;
        }

        m_taAlarms.raiseEntityUniqueAlarm( alarmType,
                                           m_configuration.getEntityKey(),
                                           m_configuration.getEntityTypeKey(),
                                           descriptionParameters,
                                           m_configuration.getEntityName(),
                                           m_configuration.getLocationKey(),
                                           m_configuration.getAlarmSubsystem(),
                                           m_configuration.getAssetName(),
                                           sourceTime );

        FUNCTION_EXIT;
    }


    void AlarmUtility::closeUniqueAlarm( const TA_Base_Core::MessageType& alarmType )
    {
        FUNCTION_ENTRY( "closeUniqueAlarm" );

        // Close the alarm using m_taAlarms.closeEntityUniqueAlarm
        m_taAlarms.closeEntityUniqueAlarm( alarmType, m_configuration.getEntityKey() );

        FUNCTION_EXIT;
    }


    std::string AlarmUtility::raiseNonUniqueAlarm( const TA_Base_Core::MessageType& alarmType,
                                                   NameValuePairs& descriptionParameters,
                                                   time_t timeStamp )
    {
        FUNCTION_ENTRY( "raiseNonUniqueAlarm" );

        // Raise an alarm using m_taAlarms.submitNonUniqueAlarmWithAsset
        // sourceTime - from parameters, but if 0, then use AlarmConstants::defaultTime

        timeb sourceTime = TA_Base_Core::AlarmConstants::defaultTime;

        if ( timeStamp != 0 )
        {
            sourceTime.time = timeStamp;
            sourceTime.millitm = 0;
        }

        // use shared_ptr to release temporary newed NameValuePair
        typedef boost::shared_ptr< NameValuePair > NameValuePairPtr;
        typedef std::vector< NameValuePairPtr > NameValuePtrList;

        NameValuePtrList nameValuePtrs;
        nameValuePtrs.reserve( descriptionParameters.size() );

        TA_Base_Core::DescriptionParameters description;

        for ( NameValuePairs::iterator iter = descriptionParameters.begin();
              iter != descriptionParameters.end(); ++iter )
        {
            NameValuePairPtr nameValuePairPtr( new NameValuePair( iter->first, TA_Base_Core::NameValuePair::NOT_EQUALS, iter->second ) );
            nameValuePtrs.push_back( nameValuePairPtr );
            description.push_back( nameValuePairPtr.get() );
        }

        FUNCTION_EXIT;
        return m_taAlarms.submitNonUniqueAlarmWithAsset( alarmType, description, m_configuration.getAssetName(), sourceTime );
    }


    TA_Base_Core::NameValuePairs AlarmUtility::createConflictParameters( TA_IRS_Bus::CommonTypes::TrainIdType offendingTrainId,
                                                                         TA_IRS_Bus::CommonTypes::TrainIdType owningTrainId )
    {
        FUNCTION_ENTRY( "createConflictParameters" );

        // get the train comms state for the offendingTrain and the owningTrain
        // for each, acquire the state lock and get the serviceNumber, then release the state lock

        NameValuePairs descriptionParameters;

        std::string offendingTrainServiceNumber;
        std::string owningTrainServiceNumber;

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

        descriptionParameters.insert( NameValuePairs::value_type( "OffendingTrain", offendingTrainServiceNumber ) );
        descriptionParameters.insert( NameValuePairs::value_type( "OwningTrain", owningTrainServiceNumber ) );

        FUNCTION_EXIT;
        return  descriptionParameters;
    }


} // TA_IRS_App
